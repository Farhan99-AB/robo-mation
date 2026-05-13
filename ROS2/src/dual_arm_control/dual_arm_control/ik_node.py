import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy
from geometry_msgs.msg import Pose
from std_msgs.msg import Bool
from sensor_msgs.msg import JointState
from ament_index_python.packages import get_package_share_directory
from dual_arm_control.ik_solver import DualArmIKSolver
import numpy as np


class DualArmIKNode(Node):
    def __init__(self):
        super().__init__("dual_arm_ik_node")

        # ── QoS: drop stale messages, keep only the latest ───────────────────
        fast_qos = QoSProfile(
            reliability=ReliabilityPolicy.BEST_EFFORT,
            history=HistoryPolicy.KEEP_LAST,
            depth=1,
        )

        _share = get_package_share_directory("dual_arm_simulation")
        scene_xml_path = (
            _share + "/mujoco_menagerie/franka_emika_panda/dual_arm_scene.xml"
        )
        self.solver = DualArmIKSolver(scene_xml_path)

        # ── Initial targets from home state ──────────────────────────────────
        # Y is ignored by the solver (depth-locked), but we store the full
        # position to keep the data flow uniform.
        initial_l = self.solver.data.xpos[self.solver.hand_id_left].tolist()
        initial_r = self.solver.data.xpos[self.solver.hand_id_right].tolist()

        self.target_l = {"pos": initial_l, "theta": 0.0}
        self.target_r = {"pos": initial_r, "theta": 0.0}
        self.grip_l   = False
        self.grip_r   = False

        # ── Subscriptions ─────────────────────────────────────────────────────
        self.create_subscription(Pose,       "target_pose_left",      self.left_pose_callback,  fast_qos)
        self.create_subscription(Pose,       "target_pose_right",     self.right_pose_callback, fast_qos)
        self.create_subscription(Bool,       "left_gripper",          self.left_grip_callback,  fast_qos)
        self.create_subscription(Bool,       "right_gripper",         self.right_grip_callback, fast_qos)
        self.create_subscription(JointState, "measured_joint_states", self.state_cb,            10)

        # ── Publisher ─────────────────────────────────────────────────────────
        self.joint_publisher = self.create_publisher(JointState, "joint_commands", fast_qos)

        # ── State ─────────────────────────────────────────────────────────────
        self.actual_q = None

        # Throttle collision-warning logs to avoid spamming
        self._last_collision_warn = 0.0

        self.dt = 1.0 / 200.0
        self.create_timer(self.dt, self.execution_cycle)
        self.get_logger().info(
            "IK Node ready — Y-axis locked, collision avoidance active.\n"
            f"  Left  home EE: {np.round(initial_l, 3)}\n"
            f"  Right home EE: {np.round(initial_r, 3)}"
        )

    # ── Callbacks ─────────────────────────────────────────────────────────────

    def state_cb(self, msg: JointState):
        self.actual_q = np.array(msg.position)

    def left_pose_callback(self, msg: Pose):
        self.target_l["pos"]   = [msg.position.x, msg.position.y, msg.position.z]
        self.target_l["theta"] = msg.orientation.z   # wrist roll from perception

    def right_pose_callback(self, msg: Pose):
        self.target_r["pos"]   = [msg.position.x, msg.position.y, msg.position.z]
        self.target_r["theta"] = msg.orientation.z

    def left_grip_callback(self, msg: Bool):
        self.grip_l = msg.data

    def right_grip_callback(self, msg: Bool):
        self.grip_r = msg.data

    # ── Main execution cycle ──────────────────────────────────────────────────

    def execution_cycle(self):
        # Sync solver's internal configuration to actual measured joint state
        # when available — prevents drift between sim and IK model.
        if self.actual_q is not None:
            self.solver.configuration.update(self.actual_q)

        # Run IK — solver enforces Y lock + collision avoidance internally
        updated_qpos = self.solver.solve(
            self.target_l["pos"], self.target_l["theta"],
            self.target_r["pos"], self.target_r["theta"],
            self.dt,
        )

        # ── Gripper joints (Panda: indices 7:9 = left, 16:18 = right) ────────
        # Each finger joint goes to 0.04 m (open) or 0.0 m (closed).
        gripper_open_pos = 0.04
        updated_qpos[7:9]   = gripper_open_pos if self.grip_l else 0.0
        updated_qpos[16:18] = gripper_open_pos if self.grip_r else 0.0

        # ── Optional: log EE proximity for collision debugging ─────────────────
        ee_l = self.solver.data.xpos[self.solver.hand_id_left]
        ee_r = self.solver.data.xpos[self.solver.hand_id_right]
        ee_dist = float(np.linalg.norm(ee_l - ee_r))

        now = self.get_clock().now().nanoseconds * 1e-9
        if ee_dist < self.solver.EE_GUARD_DIST and (now - self._last_collision_warn) > 1.0:
            self.get_logger().warn(
                f"EE proximity: {ee_dist:.3f} m  "
                f"(guard active below {self.solver.EE_GUARD_DIST} m)"
            )
            self._last_collision_warn = now

        # ── Publish ───────────────────────────────────────────────────────────
        cmd = JointState()
        cmd.header.stamp = self.get_clock().now().to_msg()
        cmd.position     = updated_qpos.tolist()
        self.joint_publisher.publish(cmd)


def main(args=None):
    rclpy.init(args=args)
    node = DualArmIKNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
