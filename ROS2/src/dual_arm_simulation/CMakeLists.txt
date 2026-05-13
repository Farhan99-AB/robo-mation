cmake_minimum_required(VERSION 3.8)
project(dual_arm_simulation)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

# ── Install Python package ────────────────────────────────────────────────────
ament_python_install_package(${PROJECT_NAME})

# ── Install the sim node script as an executable ─────────────────────────────
install(PROGRAMS
  dual_arm_simulation/dual_arm_sim_node.py
  DESTINATION lib/${PROJECT_NAME}
  RENAME dual_arm_sim_node
)

# ── Install MuJoCo scene assets into share/ ───────────────────────────────────
# Any package can now find these with:
#   get_package_share_directory('dual_arm_simulation')
#   + '/mujoco_menagerie/franka_emika_panda/dual_arm_scene.xml'
install(DIRECTORY
  mujoco_menagerie/
  DESTINATION share/${PROJECT_NAME}/mujoco_menagerie
)

# ── Install launch files ──────────────────────────────────────────────────────
install(DIRECTORY
  launch/
  DESTINATION share/${PROJECT_NAME}/launch
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
