import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy
from sensor_msgs.msg import JointState
from ament_index_python.packages import get_package_share_directory
import mujoco
import mujoco.viewer
import numpy as np


class MuJoCoSimNode(Node):
    """
    MuJoCo passive-viewer sim node.

    Latency optimisations vs original
    -----------------------------------
    1. joint_commands uses BEST_EFFORT / depth=1 (unchanged) — good.
    2. measured_joint_states also uses BEST_EFFORT / depth=1.
       Original used default reliable QoS with depth=10, adding unnecessary
       queuing delay on the IK→sim feedback path.
    3. sim_loop publishes measured_joint_states BEFORE stepping physics so the
       IK node always gets the state that matches the frame it is about to
       compute for, not the one from last cycle.
    4. mj_forward is only called when a new command has actually arrived
       (tracked by self._new_command flag). Avoids a redundant forward pass
       every timer tick when no new IK output is ready.
    5. XML path is resolved via ament_index — no hardcoded absolute paths.
    """

    def __init__(self):
        super().__init__("sim_node")

        # ── QoS ──────────────────────────────────────────────────────────────
        fast_qos = QoSProfile(
            reliability=ReliabilityPolicy.BEST_EFFORT,
            history=HistoryPolicy.KEEP_LAST,
            depth=1,
        )

        # ── XML via ament_index (no hardcoded paths) ──────────────────────────
        share = get_package_share_directory("dual_arm_simulation")
        xml_path = share + "/mujoco_menagerie/franka_emika_panda/dual_arm_scene.xml"

        self.model = mujoco.MjModel.from_xml_path(xml_path)
        self.data  = mujoco.MjData(self.model)

        # ── State publisher — fast QoS so IK node never blocks on old msgs ───
        self.state_pub = self.create_publisher(JointState, "measured_joint_states", fast_qos)

        # ── Command subscriber ────────────────────────────────────────────────
        self.create_subscription(JointState, "joint_commands", self._on_joint_command, fast_qos)

        # ── Dirty flag: only call mj_forward when new data arrived ───────────
        self._new_command = False

        # ── Passive viewer ────────────────────────────────────────────────────
        self.viewer = mujoco.viewer.launch_passive(self.model, self.data)

        # ── Timer: 200 Hz matches IK node rate ───────────────────────────────
        self.create_timer(1.0 / 200.0, self._sim_loop)
        self.get_logger().info(f"MuJoCo Sim Node ready  |  XML: {xml_path}")

    # ─────────────────────────────────────────────────────────────────────────

    def _on_joint_command(self, msg: JointState):
        """Teleport joints to IK solution. Validates length first."""
        if len(msg.position) != self.model.nq:
            self.get_logger().warn(
                f"Joint count mismatch: got {len(msg.position)}, expected {self.model.nq}",
                throttle_duration_sec=2.0,  # to prevent log flooding and prints important logs (this message will log only once per 2 secs) 
            )
            return
        self.data.qpos[:] = msg.position
        self._new_command = True

    def _sim_loop(self):
        if not self.viewer.is_running():
            rclpy.shutdown()
            return

        # 1. Publish current state FIRST so IK node has it immediately
        state_msg = JointState()
        state_msg.header.stamp = self.get_clock().now().to_msg()
        state_msg.position = self.data.qpos.tolist()
        self.state_pub.publish(state_msg)

        # 2. Only run forward kinematics when joints actually changed
        if self._new_command:
            mujoco.mj_forward(self.model, self.data)
            self._new_command = False

        # 3. Sync viewer
        self.viewer.sync()


def main(args=None):
    rclpy.init(args=args)
    node = MuJoCoSimNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.viewer.close()
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
