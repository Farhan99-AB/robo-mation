import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy
from geometry_msgs.msg import Pose
import sys
import termios
import tty
import select
import numpy as np

# Absolute import based on your package name
from .keyboard_logic import TargetManager

class KeyboardNode(Node):
    def __init__(self):
        super().__init__('keyboard_node')

        # Create a high-speed profile
        fast_qos = QoSProfile(
            reliability=ReliabilityPolicy.BEST_EFFORT,
            history=HistoryPolicy.KEEP_LAST,
            depth=1 # Only keep the single newest message. Throw away the old ones!
        )

        # Define the same q_start you use in the IK/Sim nodes
        q_start = np.zeros(18) # Adjust size to match your model.nq
        q_home = [0, -0.785, 0, -2.356, 0, 1.571, 0.785]
        q_start[:7] = q_home
        q_start[9:16] = q_home

        xml_path = "/root/ROS/Group_3_2026/ros2_lab_ws/src/dual_arm_simulation/mujoco_menagerie/franka_emika_panda/dual_arm_scene.xml"
        
        # Initialize manager with the model to calculate start positions
        self.manager = TargetManager(xml_path, q_start)
        
        self.pub_l = self.create_publisher(Pose, 'target_pose_left', fast_qos)
        self.pub_r = self.create_publisher(Pose, 'target_pose_right', fast_qos)
        
        # Settings to restore terminal later
        self.settings = termios.tcgetattr(sys.stdin)
        
        # Timer for polling keys and publishing (faster rate for responsiveness)
        self.create_timer(0.01, self.run_loop)
        self.get_logger().info("Keyboard Node started using TERMIOS.")
        self.get_logger().info("Control: Arrows/UD (Left), WASD/IK (Right). Press 'q' to quit.")

    def get_key(self):
        """Reads a key from stdin without blocking."""
        fd = sys.stdin.fileno()
        # Set terminal to raw mode
        tty.setraw(fd)
        
        # Check if there is data waiting in the buffer (10ms timeout)
        rlist, _, _ = select.select([sys.stdin], [], [], 0.01)
        key = ''
        if rlist:
            key = sys.stdin.read(1)
            # Handle escape sequences for arrows (which start with \x1b)
            if key == '\x1b':
                # Read the next two characters (e.g., '[A')
                key += sys.stdin.read(2)
        
        # Restore terminal settings immediately
        termios.tcsetattr(fd, termios.TCSADRAIN, self.settings)
        return key

    def run_loop(self):
        key = self.get_key()
        
        if key.lower() == 'q':
            self.get_logger().info("Shutting down...")
            rclpy.shutdown()
            return

        if key:
            self.manager.update_from_input(key)

        # Always publish the current state
        l_pos, r_pos = self.manager.get_targets()
        
        msg_l = Pose()
        msg_l.position.x, msg_l.position.y, msg_l.position.z = l_pos
        self.pub_l.publish(msg_l)

        msg_r = Pose()
        msg_r.position.x, msg_r.position.y, msg_r.position.z = r_pos
        self.pub_r.publish(msg_r)

def main(args=None):
    rclpy.init(args=args)
    node = KeyboardNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        # Crucial: Reset terminal settings so your shell isn't broken
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, node.settings)
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()