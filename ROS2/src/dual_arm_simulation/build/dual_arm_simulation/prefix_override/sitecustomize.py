import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/root/ROS/Group_3_2026/ros2_lab_ws/src/dual_arm_simulation/install/dual_arm_simulation'
