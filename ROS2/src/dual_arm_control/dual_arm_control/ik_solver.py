# BEST CODED TILL NOW (LEZGOOOO)

import mujoco
import mink
import numpy as np
from scipy.spatial.transform import Rotation as R


class DualArmIKSolver:
    """
    Dual-arm IK solver.

    Orientation strategy
    ────────────────────
    Both grippers are held pointing STRAIGHT DOWN at all times.
    R_GRIPPER_DOWN is computed ONCE at startup from the home forward kinematics
    and then never changes — it is effectively a hardcoded constant.
    The IK solver enforces this orientation every cycle alongside the
    XZ position target. Joint 7 wrist roll is applied on top via
    from_rotvec(theta * approach_axis) so ONLY joint 7 moves for rotation.

    TUNING KNOBS  (all at the top of the class — easy to find)
    ────────────────────────────────────────────────────────────
    SMOOTH_ALPHA          position tracking speed
    POSTURE_COST          how hard joints resist drifting from home
    ORIENTATION_COST      how hard IK enforces the downward orientation
    ENABLE_COLLISION_AVOIDANCE   toggle OSQP collision solve on/off
    """

    # ── Tuning knobs ──────────────────────────────────────────────────────────
    SMOOTH_ALPHA             = 0.50   # 0.25=smooth/laggy  0.75=snappy/jittery
    POSTURE_COST             = 0.05   # how hard joints snap back to home posture
    ORIENTATION_COST         = 1.5    # how hard IK enforces gripper-down orientation
                                      # raise if gripper tilts; lower if arm is too stiff
    COLLISION_MIN_DIST       = 0.04
    COLLISION_DETECT_DIST    = 0.10
    EE_GUARD_DIST            = 0.20

    def __init__(self, xml_path: str):
        self.model = mujoco.MjModel.from_xml_path(xml_path)
        self.data  = mujoco.MjData(self.model)
        self.configuration = mink.Configuration(self.model)

        # ── Body IDs ──────────────────────────────────────────────────────────
        self.hand_id_left  = mujoco.mj_name2id(self.model, mujoco.mjtObj.mjOBJ_BODY, "hand")
        self.hand_id_right = mujoco.mj_name2id(self.model, mujoco.mjtObj.mjOBJ_BODY, "r_hand")

        base_id_left  = mujoco.mj_name2id(self.model, mujoco.mjtObj.mjOBJ_BODY, "link0")
        base_id_right = mujoco.mj_name2id(self.model, mujoco.mjtObj.mjOBJ_BODY, "r_link0")
        floor_id      = self.model.geom("floor").id
        self.geoms_left  = mink.get_subtree_geom_ids(self.model, base_id_left)
        self.geoms_right = mink.get_subtree_geom_ids(self.model, base_id_right)

        # ── Home configuration ────────────────────────────────────────────────
        q_home_left = np.array([0, -0.785, 0, -2.356, 0, 1.571, 0.785])
        q_home_right = np.array([0, -0.785, 0, -2.356, 0, 1.571, 0.767])
        self.data.qpos[0:7]   = q_home_left
        self.data.qpos[9:16]  = q_home_right
        self.data.qpos[7:9]   = 0.04
        self.data.qpos[16:18] = 0.04
        mujoco.mj_forward(self.model, self.data)
        self.configuration.update(self.data.qpos)

        # ── R_GRIPPER_DOWN — the one constant that matters ────────────────────
        #
        # Both bases share quat="0.707 0 0 -0.707" (identical orientation,
        # only X position differs). Therefore q_home produces the SAME
        # world-frame hand orientation for both arms.
        #
        # We read the LEFT arm's xmat and use it for BOTH arms.
        # This guarantees consistency: if the left gripper points down,
        # the right will too.
        #
        # Printed on startup — copy the values here to fully hardcode if needed:
        # self._R_down_l = np.array([[...], [...], [...]])
        # ↓↓↓ ORIENTATION IS DEFINED HERE ↓↓↓
        self._R_down_l = self.data.xmat[self.hand_id_left].reshape(3, 3).copy()
        # self._R_down_r = self._R_down_l.copy()   # ← same orientation for both arms
        self._R_down_r = self.data.xmat[self.hand_id_right].reshape(3, 3).copy()
        # ↑↑↑ ─────────────────────────────────────────────────────────────────

        # Printed once so you can verify and hardcode manually if needed
        np.set_printoptions(precision=4, suppress=True)
        print(f"[IKSolver] R_down (both arms):\n{self._R_down_l}")
        print(f"[IKSolver] R_down RIGHT (raw, for comparison):\n{self.data.xmat[self.hand_id_right].reshape(3,3)}")
        print("[IKSolver] If both matrices above match, everything is correct.")
        print("[IKSolver] If they differ, the right arm's xmat at q_home is unexpected — check q_home indices.")

        # ── Mink tasks ────────────────────────────────────────────────────────
        self.task_left = mink.FrameTask(
            frame_name="hand", frame_type="body",
            position_cost=1.0,
            orientation_cost=self.ORIENTATION_COST,
            lm_damping=1e-3,
        )
        self.task_right = mink.FrameTask(
            frame_name="r_hand", frame_type="body",
            position_cost=1.0,
            orientation_cost=self.ORIENTATION_COST,
            lm_damping=1e-3,
        )
        self.posture_task = mink.PostureTask(self.model, cost=self.POSTURE_COST)
        self.posture_task.set_target_from_configuration(self.configuration)

        # ── Limits ────────────────────────────────────────────────────────────
        # ↓↓↓ TOGGLE COLLISION AVOIDANCE HERE ↓↓↓
        ENABLE_COLLISION_AVOIDANCE = True
        # ↑↑↑ ──────────────────────────────────

        if ENABLE_COLLISION_AVOIDANCE:
            self.limits = [
                mink.ConfigurationLimit(model=self.model),
                mink.CollisionAvoidanceLimit(
                    model=self.model,
                    geom_pairs=[
                        (self.geoms_left,  [floor_id]),
                        (self.geoms_right, [floor_id]),
                        (self.geoms_left,  self.geoms_right),
                    ],
                    minimum_distance_from_collisions=self.COLLISION_MIN_DIST,
                    collision_detection_distance=self.COLLISION_DETECT_DIST,
                ),
            ]
        else:
            self.limits = [mink.ConfigurationLimit(model=self.model)]

        # ── Smoothed position targets ─────────────────────────────────────────
        self.smoothed_l = self.data.xpos[self.hand_id_left].copy()
        self.smoothed_r = self.data.xpos[self.hand_id_right].copy()

    # ─────────────────────────────────────────────────────────────────────────

    # ── Wrist roll sign ───────────────────────────────────────────────────────
    # Both arms share the same R_down so the approach_axis is identical.
    # However joint7's physical spin axis may be inverted for one arm
    # depending on how the kinematic chain accumulates rotations.
    # If the right arm's wrist spins the wrong way, flip this to -1.
    # Left arm is always +1 (reference).
    THETA_SIGN_L = +1   # ← change to -1 if left wrist spins wrong way
    THETA_SIGN_R = +1   # ← change to +1 if right wrist spins wrong way

    # def _wrist_rotation(self, theta: float, R_down: np.ndarray, sign: int = 1) -> np.ndarray:
    #     """
    #     Apply wrist roll (theta) on top of the fixed downward orientation.

    #     Rotates around the gripper approach axis (third column of R_down).
    #     The sign parameter handles the case where joint7's physical rotation
    #     axis is inverted relative to the approach axis for one arm.
    #     """
    #     approach_axis = R_down[:, 2]
    #     R_roll = R.from_rotvec((sign * theta) * approach_axis).as_matrix()
    #     return R_roll @ R_down
    
    # trial function
    def _wrist_rotation(self, theta: float, R_down: np.ndarray, sign: int = 1) -> np.ndarray:
        """
        Apply wrist roll (theta) as a PURE LOCAL rotation.
        This guarantees the rotation is perfectly aligned with the hand's Z-axis,
        preventing the IK solver from moving the elbow/shoulder to compensate.
        """
        # Create a rotation strictly around the local Z-axis
        local_roll = R.from_euler('z', sign * theta).as_matrix()
        
        # Post-multiply to apply the roll locally to the downward orientation
        return R_down @ local_roll

    def _apply_target_guard(self, tgt_l, tgt_r):
        """Block EE targets from advancing toward each other when arms are close."""
        ee_l = self.data.xpos[self.hand_id_left].copy()
        ee_r = self.data.xpos[self.hand_id_right].copy()
        if np.linalg.norm(ee_l - ee_r) < self.EE_GUARD_DIST:
            sep = ee_r - ee_l
            if sep[0] > 0 and (tgt_l[0] - ee_l[0]) > 0:
                tgt_l[0] = ee_l[0]
            if sep[0] > 0 and (tgt_r[0] - ee_r[0]) < 0:
                tgt_r[0] = ee_r[0]
        return tgt_l, tgt_r

    # ─────────────────────────────────────────────────────────────────────────

    def solve(self, raw_target_l, theta_l, raw_target_r, theta_r, dt):
        """
        Solve IK for both arms.

        The orientation target is always R_down (gripper facing straight down)
        with wrist roll theta applied on top. Position tracks hand XZ.
        """
        self.configuration.update(self.data.qpos)

        # Position targets
        tgt_l = np.array(raw_target_l, dtype=float)
        tgt_r = np.array(raw_target_r, dtype=float)
        tgt_l, tgt_r = self._apply_target_guard(tgt_l, tgt_r)

        self.smoothed_l += (tgt_l - self.smoothed_l) * self.SMOOTH_ALPHA
        self.smoothed_r += (tgt_r - self.smoothed_r) * self.SMOOTH_ALPHA
        self.smoothed_l[1] = tgt_l[1]   # pin Y (depth)
        self.smoothed_r[1] = tgt_r[1]

        # Orientation targets — fixed downward + wrist roll
        # THETA_SIGN_L/R defined above — flip if wrist spins wrong direction
        rot_l = self._wrist_rotation(theta_l, self._R_down_l, self.THETA_SIGN_L)
        rot_r = self._wrist_rotation(theta_r, self._R_down_r, self.THETA_SIGN_R)

        self.task_left.set_target(mink.SE3.from_rotation_and_translation(
            mink.SO3.from_matrix(rot_l), self.smoothed_l,
        ))
        self.task_right.set_target(mink.SE3.from_rotation_and_translation(
            mink.SO3.from_matrix(rot_r), self.smoothed_r,
        ))

        # Solve
        velocity = mink.solve_ik(
            self.configuration,
            [self.task_left, self.task_right, self.posture_task],
            limits=self.limits,
            dt=dt,
            solver="osqp",
        )
        self.configuration.integrate_inplace(velocity, dt)
        self.data.qpos[:] = self.configuration.q
        mujoco.mj_forward(self.model, self.data)

        return self.data.qpos.copy()
