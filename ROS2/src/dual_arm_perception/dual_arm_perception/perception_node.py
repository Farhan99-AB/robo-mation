import os
import time
import threading
import cv2
import rclpy
import numpy as np
import mediapipe as mp
import mujoco
import math
from rclpy.node import Node
from geometry_msgs.msg import Pose
from std_msgs.msg import Bool
from mediapipe.tasks import python
from mediapipe.tasks.python import vision
from ament_index_python.packages import get_package_share_directory

from .camera_capture import HandProcessor


LEFT_COLOR  = (255, 180,  40)
RIGHT_COLOR = ( 40, 220, 120)
FONT        = cv2.FONT_HERSHEY_SIMPLEX


class HandAssigner:
    """Stable left/right assignment by screen-X position."""
    def assign(self, detections: list) -> list:
        if not detections:
            return detections
        sorted_det = sorted(detections, key=lambda d: d["wrist_x"])
        if len(sorted_det) == 1:
            sorted_det[0]["label"] = sorted_det[0]["mediapipe_label"]
            return sorted_det
        sorted_det[0]["label"] = "Left"
        sorted_det[1]["label"] = "Right"
        return sorted_det


class PerceptionNode(Node):
    """
    Performance-first perception node.

    Key latency fixes vs previous version
    ───────────────────────────────────────
    1.  Detection on 640x360 instead of 1280x720 — MediaPipe is ~4x faster,
        accuracy is identical for hand detection at arm's length.
    2.  HUD rendering in a SEPARATE THREAD — never blocks detect→publish path.
    3.  Detection loop runs in its own thread, decoupled from rclpy.spin().
    4.  Minimal HUD: centre dot + one text line per hand. No skeleton, no
        dials, no addWeighted blending — all measurably slowed the main loop.
    5.  Camera buffer flushed every cycle so we always get the newest frame.
    6.  IK solver SMOOTH_ALPHA raised to 0.5 (set in ik_solver.py) so the
        arm moves toward targets twice as fast.
    """

    def __init__(self):
        super().__init__("perception_node")
        self.processor = HandProcessor()
        self.assigner  = HandAssigner()
        self.calibrate = [True, True]

        # MuJoCo warm-start
        _sim_share = get_package_share_directory("dual_arm_simulation")
        scene_xml  = _sim_share + "/mujoco_menagerie/franka_emika_panda/dual_arm_scene.xml"
        mj_model   = mujoco.MjModel.from_xml_path(scene_xml)
        mj_data    = mujoco.MjData(mj_model)
        q_home_left     = np.array([0, -0.785, 0, -2.356, 0, 1.571, 0.785])
        q_home_right     = np.array([0, -0.785, 0, -2.356, 0, 1.571, 0.767])
        mj_data.qpos[0:7]  = q_home_left
        mj_data.qpos[9:16] = q_home_right
        mujoco.mj_forward(mj_model, mj_data)
        init_l = mj_data.xpos[mujoco.mj_name2id(mj_model, mujoco.mjtObj.mjOBJ_BODY, "hand")]
        init_r = mj_data.xpos[mujoco.mj_name2id(mj_model, mujoco.mjtObj.mjOBJ_BODY, "r_hand")]

        # primitive code commented 
        # self.normalized_left = [self.map_to_range(init_l[0], self.processor.x_limit_left[0], self.processor.x_limit_left[1], 0, 0.5) , self.map_to_range(init_l[2], self.processor.z_limit_left[0], self.processor.z_limit_left[1], 0, 1)]
        # self.normalized_right = [self.map_to_range(init_r[0], self.processor.x_limit_right[0], self.processor.x_limit_right[1], 0.5, 1) , self.map_to_range(init_r[2], self.processor.z_limit_right[0], self.processor.z_limit_right[1], 0, 1)]

        self.normalized_left  = self.robot_to_camera(init_l, "Left")
        self.normalized_right = self.robot_to_camera(init_r, "Right")

        print(f"left_x norm:{self.normalized_left[0]}, left_z norm:{self.normalized_left[1]}")
        print(f"right_x norm:{self.normalized_right[0]}, right_z norm:{self.normalized_right[1]}")

        self.processor.sync_to_mujoco_coords("Left",  *init_l)
        self.processor.sync_to_mujoco_coords("Right", *init_r)

        # Publishers
        self.left_pub       = self.create_publisher(Pose, "target_pose_left",  10)
        self.right_pub      = self.create_publisher(Pose, "target_pose_right", 10)
        self.left_grip_pub  = self.create_publisher(Bool, "left_gripper",       10)
        self.right_grip_pub = self.create_publisher(Bool, "right_gripper",      10)

        # MediaPipe VIDEO mode
        pkg_share  = get_package_share_directory("dual_arm_perception")
        model_path = os.path.join(pkg_share, "hand_landmarker.task")
        base_opts  = python.BaseOptions(model_asset_path=model_path)
        options    = vision.HandLandmarkerOptions(
            base_options=base_opts,
            num_hands=2,
            min_hand_detection_confidence=0.55,
            min_hand_presence_confidence=0.60,
            min_tracking_confidence=0.65,
            running_mode=vision.RunningMode.VIDEO,
        )
        self.detector    = vision.HandLandmarker.create_from_options(options)
        self._start_time = time.monotonic()

        # Camera
        self.cap = cv2.VideoCapture("http://192.168.1.97:9090/video")
        self.cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)

        # Shared state for HUD thread
        self._hud_frame = None
        self._hud_lock  = threading.Lock()
        self._fps       = 0.0
        self._fps_t     = time.monotonic()
        self._fps_count = 0
        self._running   = True

        # HUD display thread (~30 Hz, completely decoupled from detection)
        cv2.namedWindow("Teleop HUD", cv2.WINDOW_NORMAL)
        cv2.resizeWindow("Teleop HUD", 640, 360)

        # 2. START DETECTION THREAD (This stays background)
        self.det_thread = threading.Thread(target=self._detection_loop, daemon=True)
        self.det_thread.start()

        # 3. GUI TIMER (Runs in main thread via rclpy.spin)
        # 0.03s is ~33 FPS for the display
        self.hud_timer = self.create_timer(0.03, self._render_hud_callback)

        self.get_logger().info("PerceptionNode ready — HUD on Main Thread, Detection on Background Thread.")

    # premitive function commented
    # def map_to_range(self, val, from_low, from_high, to_low, to_high):
    #     input_range = from_high - from_low
    #     output_range = to_high - to_low
    #     norm_val = val - from_low
    #     y = (output_range/input_range) * norm_val
    #     mapped_value = to_low + y
    #     return mapped_value
    
    def robot_to_camera(self, pos, label: str) -> tuple[float, float]:
        """
        Exact inverse of map_to_robot().
        Converts a robot world position [x, y, z] to normalised camera coords (mx, my).
        LEFT:  mx ∈ [0.0, 0.5],  RIGHT: mx ∈ [0.5, 1.0],  my ∈ [0.0, 1.0]
        """
        p = self.processor
        if label == "Left":
            nx = (pos[0] - p.x_limit_left[0]) / (p.x_limit_left[1] - p.x_limit_left[0])
            mx = float(np.clip(nx * 0.5, 0.0, 0.5))
            my = float(np.clip((p.z_limit_left[1] - pos[2]) / (p.z_limit_left[1] - p.z_limit_left[0]), 0.0, 1.0))
        else:
            nx = (pos[0] - p.x_limit_right[0]) / (p.x_limit_right[1] - p.x_limit_right[0])
            mx = float(np.clip(nx * 0.5 + 0.5, 0.5, 1.0))
            my = float(np.clip((p.z_limit_right[1] - pos[2]) / (p.z_limit_right[1] - p.z_limit_right[0]), 0.0, 1.0))
        return mx, my

    # ── Main Thread HUD Callback ──────────────────────────────────────────────
    def _render_hud_callback(self):
        """This runs in the main thread, safe for OpenCV GUI calls."""
        with self._hud_lock:
            frame = self._hud_frame

        if frame is not None:
            cv2.imshow("Teleop HUD", frame)
            # waitKey(1) is essential to process the GUI event queue
            if cv2.waitKey(1) & 0xFF == ord('q'):
                self._running = False
                rclpy.shutdown()

    # ── Detection loop ────────────────────────────────────────────────────────
    def _detection_loop(self):
        while self._running:
            # Flush camera buffer — always get the newest frame from IP cam
            for _ in range(2):
                self.cap.grab()
            ret, frame = self.cap.retrieve()
            if not ret:
                time.sleep(0.005)
                continue

            # FPS counter
            self._fps_count += 1
            now = time.monotonic()
            if now - self._fps_t >= 1.0:
                self._fps   = self._fps_count / (now - self._fps_t)
                self._fps_t = now
                self._fps_count = 0

            # Resize SMALL for detection — biggest single latency fix
            det_frame = cv2.resize(frame, (640, 360))
            det_frame = cv2.flip(det_frame, 1)

            # MediaPipe
            ts_ms  = int((time.monotonic() - self._start_time) * 1000)
            rgb    = cv2.cvtColor(det_frame, cv2.COLOR_BGR2RGB)
            mp_img = mp.Image(image_format=mp.ImageFormat.SRGB, data=rgb)
            res    = self.detector.detect_for_video(mp_img, ts_ms)

            # Parse detections
            raw_dets = []
            if res.hand_landmarks:
                for i, lm in enumerate(res.hand_landmarks):
                    det_label = res.handedness[i][0].category_name
                    raw_dets.append({
                        "mediapipe_label": "Right" if det_label == "Left" else "Left",
                        "landmarks":       lm,
                        "wrist_x":         lm[0].x,
                    })

            assigned   = self.assigner.assign(raw_dets)
            hud_states = {}

            for det in assigned:
                label = det["label"]
                lm    = det["landmarks"]

                mx, my     = self.processor.calculate_midpoint(lm)
                rx, ry, rz = self.processor.map_to_robot(mx, my, label)
                raw_roll   = self.processor.calculate_wrist_roll(lm)
                roll       = self.processor.map_theta(raw_roll, label)

                dist      = np.hypot(lm[4].x - lm[8].x, lm[4].y - lm[8].y)
                is_closed = dist < 0.05

                pub   = self.left_pub   if label == "Left" else self.right_pub
                g_pub = self.left_grip_pub if label == "Left" else self.right_grip_pub

                if any(self.calibrate):
                    if label == "Left":
                        if math.isclose(mx, self.normalized_left[0], abs_tol=0.05) and math.isclose(my, self.normalized_left[1], abs_tol=0.05):
                            self.calibrate[0] = False
                    else :
                        if math.isclose(mx, self.normalized_right[0], abs_tol=0.05) and math.isclose(my, self.normalized_right[1], abs_tol=0.05):
                            self.calibrate[1] = False         
                    print("Calibrated", self.calibrate)
                else:
                    p_msg = Pose()
                    p_msg.position.x    = float(rx)
                    p_msg.position.y    = float(ry)
                    p_msg.position.z    = float(rz)
                    p_msg.orientation.z = float(roll)
                    pub.publish(p_msg)

                    g_msg      = Bool()
                    g_msg.data = not bool(is_closed)
                    g_pub.publish(g_msg)

                hud_states[label] = {
                    "mx": mx, "my": my, "rx": rx, "rz": rz,
                    "roll": roll, "is_closed": is_closed, "lm": lm,
                }

            # Hand annotated frame to HUD thread — don't block here
            annotated = self._annotate(det_frame, hud_states)
            with self._hud_lock:
                self._hud_frame = annotated

            # Short sleep to prevent CPU pegged at 100% if camera is too fast
            time.sleep(0.001)

    # ── Minimal annotation (fast — no blending, no skeleton, no dials) ────────
    def _annotate(self, frame, states: dict) -> np.ndarray:
        h, w = frame.shape[:2]
        cv2.line(frame, (w // 2, 0), (w // 2, h), (60, 60, 60), 1)
        cv2.putText(frame, f"FPS:{self._fps:.0f}", (w - 65, 16),
                    FONT, 0.42, (0, 220, 80), 1, cv2.LINE_AA)
        
        if any(self.calibrate):
            self.calibration(frame)

        for label, st in states.items():
            color     = LEFT_COLOR if label == "Left" else RIGHT_COLOR
            cx        = int(st["mx"] * w)
            cy        = int(st["my"] * h)
            lm        = st["lm"]
            is_closed = st["is_closed"]

            # Centre dot (thumb-index midpoint)
            cv2.circle(frame, (cx, cy), 8, color, -1, cv2.LINE_AA)
            cv2.circle(frame, (cx, cy), 8, (255, 255, 255), 1, cv2.LINE_AA)

            # Wrist dot + line to midpoint
            wx = int(lm[0].x * w)
            wy = int(lm[0].y * h)
            cv2.circle(frame, (wx, wy), 4, color, -1, cv2.LINE_AA)
            cv2.line(frame, (wx, wy), (cx, cy), color, 1, cv2.LINE_AA)

            # One compact text line
            grip = "CLOSED" if is_closed else "OPEN"
            txt  = f"{label[0]} X:{st['rx']:+.2f} Z:{st['rz']:.2f} R:{np.degrees(st['roll']):+.0f}d {grip}"
            ty   = cy - 14 if cy > 20 else cy + 22
            cv2.putText(frame, txt, (max(cx - 80, 2), ty),
                        FONT, 0.36, color, 1, cv2.LINE_AA)
        return frame

    # ── HUD display thread ────────────────────────────────────────────────────
    def _hud_loop(self):
        while self._running:
            with self._hud_lock:
                frame = self._hud_frame
            if frame is not None:
                cv2.imshow("Teleop HUD", frame)
            cv2.waitKey(1)
            time.sleep(0.033)   # 30 Hz display — don't burn CPU on rendering

    def calibration(self, frame) -> np.ndarray:
        h, w = frame.shape[:2]

        # Left
        cv2.circle(frame, (int(self.normalized_left[0] * w), int(self.normalized_left[1] * h)), 8, (255, 180,  40), 2, cv2.LINE_AA)
        #Right
        cv2.circle(frame, (int(self.normalized_right[0] * w), int(self.normalized_right[1] * h)), 8, (40, 220, 120), 2, cv2.LINE_AA)

        print(f"left x,y:{self.normalized_left[0], self.normalized_left[1]} and right x, y: {self.normalized_right[0], self.normalized_right[1]}")

        cv2.putText(frame, "Align palm with dot",(w // 2 - 120, 30),FONT, 0.7, (0, 0, 0), 3, cv2.LINE_AA)       # black outline
        cv2.putText(frame, "Align palm with dot",(w // 2 - 120, 30),FONT, 0.7, (77, 238, 234), 1, cv2.LINE_AA)  # cyan text on top
        
        return frame
    



def main(args=None):
    rclpy.init(args=args)
    node = PerceptionNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node._running = False
        node.cap.release()
        cv2.destroyAllWindows()
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
