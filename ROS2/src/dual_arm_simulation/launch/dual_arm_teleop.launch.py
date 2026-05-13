"""
dual_arm_teleop.launch.py

Launches all three nodes for the dual-arm hand-tracking teleop system.
Each node lives in its own ROS2 package:

  Package                 Executable              Node name
  ─────────────────────── ─────────────────────── ───────────────────
  dual_arm_simulation     dual_arm_sim_node        sim_node
  dual_arm_perception     perception_node          perception_node
  dual_arm_control        ik_node                  dual_arm_ik_node

Start order matters:
  sim_node must be up before the IK node starts publishing joint commands,
  and the IK node should be up before the perception node starts publishing
  targets. launch_ros handles this via the order in LaunchDescription —
  nodes are started top-to-bottom with a small stagger via TimerAction.
"""

from launch import LaunchDescription
from launch.actions import TimerAction
from launch_ros.actions import Node


def generate_launch_description():

    # 1. Sim first — it owns the MuJoCo model and publishes measured_joint_states
    sim_node = Node(
        package="dual_arm_simulation",
        executable="dual_arm_sim_node",
        name="sim_node",
        output="screen",
    )

    # 2. IK node second — needs sim to be ready before it starts solving
    ik_node = TimerAction(
        period=2.0,   # seconds — gives sim time to load the XML and open the viewer
        actions=[
            Node(
                package="dual_arm_control",
                executable="ik_node",
                name="dual_arm_ik_node",
                output="screen",
            )
        ],
    )

    # 3. Perception last — starts publishing targets only after IK is ready
    perception_node = TimerAction(
        period=4.0,   # seconds — gives IK node time to warm-start from home pose
        actions=[
            Node(
                package="dual_arm_perception",
                executable="perception_node",
                name="perception_node",
                output="screen",
            )
        ],
    )

    return LaunchDescription([
        sim_node,
        ik_node,
        perception_node,
    ])
