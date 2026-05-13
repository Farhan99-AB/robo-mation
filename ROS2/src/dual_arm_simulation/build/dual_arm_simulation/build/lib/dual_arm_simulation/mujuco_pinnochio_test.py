### WORKING CODE BUT USES NO PRE DEFINED LIBRARIES
# import mujoco
# import mujoco.viewer
# import numpy as np
# import time

# # --- 1. INITIALIZE ---
# mj_path = "ros2_lab_ws/src/dual_arm_simulation/mujoco_menagerie/franka_emika_panda/scene.xml"
# mj_model = mujoco.MjModel.from_xml_path(mj_path)
# mj_data = mujoco.MjData(mj_model)

# # Find the end-effector ID (usually the last body or a site)
# # In Menagerie Panda, 'hand' is the body name
# EE_ID = mujoco.mj_name2id(mj_model, mujoco.mjtObj.mjOBJ_BODY, "hand")

# # --- 2. INITIAL STATE (Avoid All Zeros) ---
# # Using a slightly bent elbow to avoid singularities
# q_start = np.array([0, -0.785, 0, -2.356, 0, 1.571, 0.785])
# mj_data.qpos[:7] = q_start
# mujoco.mj_forward(mj_model, mj_data)

# # --- 3. TARGET ---
# # Target is 20cm forward in the World X-axis
# target_pos = mj_data.xpos[EE_ID].copy() + np.array([0.2, -0.3, -0.3])

# # --- 4. NATIVE IK LOOP ---
# with mujoco.viewer.launch_passive(mj_model, mj_data) as viewer:
#     print("Native MuJoCo IK Loop Started...")
    
#     while viewer.is_running():
#         step_start = time.time()

#         # Get current hand position from MuJoCo
#         current_pos = mj_data.xpos[EE_ID]
#         error = target_pos - current_pos
        
#         if np.linalg.norm(error) > 0.001:
#             # Initialize Jacobian matrices (3 rows for XYZ, 6 rows for full 6D)
#             # nv is the number of degrees of freedom
#             jacp = np.zeros((3, mj_model.nv)) 
            
#             # Get the Translational Jacobian for the 'hand' body
#             mujoco.mj_jacBody(mj_model, mj_data, jacp, None, EE_ID)
            
#             # Slice for only the first 7 joints (the arm)
#             J_arm = jacp[:, :7]
            
#             # Damped Least Squares
#             damp = 1e-4
#             # Compute joint velocity: dq = J^T * inv(J*J^T + damp) * error
#             # Or use the simpler: dq = inv(J^T*J + damp) * J^T * error
#             dq = np.linalg.solve(J_arm.T @ J_arm + damp * np.eye(7), J_arm.T @ error)
            
#             # Update position
#             mj_data.qpos[:7] += dq * 0.2
            
#             # DEBUG: Print if moving
#             if int(time.time() * 10) % 10 == 0:
#                 print(f"Distance: {np.linalg.norm(error):.4f} | dq_norm: {np.linalg.norm(dq):.4f}")

#         # Sync and visualize
#         mujoco.mj_forward(mj_model, mj_data)
#         viewer.sync()

#         # Maintain 60Hz
#         elapsed = time.time() - step_start
#         if elapsed < 1/60:
#             time.sleep(1/60 - elapsed)


# ### SECOND CODE TRY (Mink works)
# import mujoco
# import mujoco.viewer
# import numpy as np
# import time
# import mink
# from mink import CollisionAvoidanceLimit
# import math

# # --- 1. INITIALIZE ---
# # mj_path = "ros2_lab_ws/src/dual_arm_simulation/mujoco_menagerie/franka_emika_panda/scene.xml"
# mj_path = "ros2_lab_ws/src/dual_arm_simulation/mujoco_menagerie/franka_emika_panda/dual_arm_scene.xml"
# mj_model = mujoco.MjModel.from_xml_path(mj_path)
# mj_data = mujoco.MjData(mj_model)

# # FIX: Initialize mink with the model and sync with current qpos
# # Configuration It stores the current state of the robot 
# # (angles, velocities, and geometry) and performs the heavy calculations 
# # needed for Inverse Kinematics.
# configuration = mink.Configuration(mj_model)
# configuration.update(mj_data.qpos)

# # Define the rotation matrix for "facing down" (180 deg around X)
# # This keeps the gripper pointing at the floor
# R_down = np.array([
#     [1, 0, 0],
#     [0, -1, 0],
#     [0, 0, -1]
# ])

# # --- 2. DEFINE THE TASK ---
# # Targeting the 'hand' body left
# task_left = mink.FrameTask(
#     frame_name="hand",
#     frame_type="body",
#     position_cost=1.0,
#     orientation_cost=1.0,
#     lm_damping=1e-4
# )
# # Right hand
# task_right = mink.FrameTask(
#     frame_name="r_hand",
#     frame_type="body",
#     position_cost=1.0,
#     orientation_cost=1.0,
#     lm_damping=1e-4
# )

# # ---Define the Collision Constraint---

# # Define the geom pairs to check for collision

# # 1. Get the ID of the floor
# # to print all body names
# # print([mj_model.body(i).name for i in range(mj_model.nbody)])

# floor_id = mj_model.geom("floor").id

# # 2. Get the ID of the robot's root body
# # Replace "link0" with the actual name of your robot's base body
# robot_base_id_left = mj_model.body("link0").id
# robot_base_id_right = mj_model.body("r_link0").id

# # 3. Automatically grab every geom belonging to the robot
# robot_geoms_left = mink.get_subtree_geom_ids(mj_model, robot_base_id_left)
# robot_geoms_right = mink.get_subtree_geom_ids(mj_model, robot_base_id_right)

# # create pairs for geom pairs to avoid collision
# collision_pairs = [
#     (robot_geoms_left, [floor_id]),  # Robot vs Floor
#     (robot_geoms_right, [floor_id]),  # Robot vs Floor
#     (robot_geoms_right, robot_geoms_right),   # Robot vs Itself (Self-collision)
#     (robot_geoms_left, robot_geoms_left),   # Robot vs Itself (Self-collision)
#     (robot_geoms_left, robot_geoms_right),   # Robot left vs Robot right
# ]


# # Initialize collision avoidance limit
# limits = [
#     mink.ConfigurationLimit(model=mj_model),
#     mink.CollisionAvoidanceLimit(
#         model=mj_model,
#         geom_pairs=collision_pairs,
#         minimum_distance_from_collisions=0.02, # 2cm buffer
#         collision_detection_distance=0.05,    # Start braking at 5cm (earlier was 0.05)
#     ),
# ]

# # --- 3. SET INITIAL STATE ---
# q_start = np.array([0, -0.785, 0, -2.356, 0, 1.571, 0.785])
# # q_start = np.array([0, 0, 0, 0, 0, 0, 0])
# mj_data.qpos[:7] = q_start
# mj_data.qpos[9:16] = q_start
# # Fingers (if present in the model)
# # if len(mj_data.qpos) > 7:
# #     mj_data.qpos[7:] = 0.0
# mj_data.qpos[7:9] = mj_data.qpos[16:] = 0.0

# mujoco.mj_forward(mj_model, mj_data)

# # Define target relative to current hand position
# hand_id_left = mujoco.mj_name2id(mj_model, mujoco.mjtObj.mjOBJ_BODY, "hand")
# hand_id_right = mujoco.mj_name2id(mj_model, mujoco.mjtObj.mjOBJ_BODY, "r_hand")

# target_pos_left = mj_data.xpos[hand_id_left].copy() + np.array([0.0, 0.0, 0.0])
# target_pos_right = mj_data.xpos[hand_id_right].copy() + np.array([0.0, 0.0, 0.0])

# # to include keyboard inputs 
# current_target_left = target_pos_left.copy()
# def key_callback(keycode):
#     global current_target_left
#     delta = 0.05  # Move 5cm per keypress
    
#     # Use ASCII or MuJoCo constants
#     if chr(keycode) == ' ': # Space to reset
#         print("Resetting Target")
#         current_target_left = mj_data.xpos[hand_id_left].copy()
#     elif keycode == 262: # Right Arrow
#         current_target_left[0] += delta
#     elif keycode == 263: # Left Arrow
#         current_target_left[0] -= delta
#     elif keycode == 265: # Up Arrow
#         current_target_left[1] += delta
#     elif keycode == 264: # Down Arrow
#         current_target_left[1] -= delta
#     elif chr(keycode).lower() == 'u': # U for Up (Z-axis)
#         current_target_left[2] += delta
#     elif chr(keycode).lower() == 'd': # D for Down (Z-axis)
#         current_target_left[2] -= delta

#     print(f"New Target: {current_target_left}")



# mid_point = np.array([0.3, 0.0, 0.3])   #for testing collision avoidance

# target_pose_left = mink.SE3.from_rotation_and_translation(
#     rotation=mink.SO3.from_matrix(R_down), 
#     translation=target_pos_left
# )
# target_pose_right = mink.SE3.from_rotation_and_translation(
#     rotation=mink.SO3.from_matrix(R_down), 
#     translation=target_pos_right
# )

# task_left.set_target(target_pose_left)
# task_right.set_target(target_pose_right)

# # --- 4. EXECUTION LOOP ---
# with mujoco.viewer.launch_passive(mj_model, mj_data, key_callback=key_callback) as viewer:
#     print("Mink Solver Fixed and Active.")
#     dt = 1/60
    
#     while viewer.is_running():
#         step_start = time.time()

#         # UPDATE THE TASK WITH THE NEW KEYBOARD POSITION
#         new_pose_left = mink.SE3.from_rotation_and_translation(
#             rotation=mink.SO3.from_matrix(R_down), 
#             translation=current_target_left
#         )
#         task_left.set_target(new_pose_left)

#         # Update mink's internal math with the current positions
#         configuration.update(mj_data.qpos)

#         # Solve for joint velocities
#         # to find the vector v (joint velocities) that minimizes the error while respecting 
#         # all limits".
#         # Other solver proxqp, clarabel, daqp
#         # try:
#         velocity = mink.solve_ik(
#             configuration,
#             tasks=[task_left, task_right],
#             limits=limits,
#             dt=dt,
#             solver="osqp",
#             damping=1e-4,             # Increased slightly for better stability (try later)
#             safety_break=True
#         )
#         # velocity_right = mink.solve_ik(
#         #     configuration,
#         #     tasks=[task_right],
#         #     limits=limits,
#         #     dt=dt,
#         #     solver="osqp",
#         #     damping=1e-4,             # Increased slightly for better stability (try later)
#         #     safety_break=True
#         # )

#         # Integrate: new_pos = old_pos + velocity * time_step
#         # mj_data.qpos += velocity_left * dt
#         # mj_data.qpos += velocity_right * dt

#         # total_velocity = velocity_left + velocity_right
#         mj_data.qpos += velocity * dt
#         # except Exception as e:
#         #     # If everything fails, stay still to prevent Segfault
#         #     pass

#         # 3. ENFORCE HARD LIMITS (Prevents the 'NotWithinConfigurationLimits' error)
#         low, high = mj_model.jnt_range[:, 0], mj_model.jnt_range[:, 1]
#         mj_data.qpos = np.clip(mj_data.qpos, low, high)

#         # Physics update
#         mujoco.mj_forward(mj_model, mj_data)
#         viewer.sync()

#         # Loop timing
#         elapsed = time.time() - step_start
#         if elapsed < dt:
#             time.sleep(dt - elapsed)


# import mujoco
# import mujoco.viewer
# import numpy as np
# import time
# import mink
# from mink import CollisionAvoidanceLimit
# import math

# # --- 1. INITIALIZE ---
# mj_path = "ros2_lab_ws/src/dual_arm_simulation/mujoco_menagerie/franka_emika_panda/dual_arm_scene.xml"
# mj_model = mujoco.MjModel.from_xml_path(mj_path)
# mj_data = mujoco.MjData(mj_model)

# # Configuration for manifold integration (more stable than manual qpos addition)
# configuration = mink.Configuration(mj_model)
# configuration.update(mj_data.qpos)

# R_down = np.array([[1, 0, 0], [0, -1, 0], [0, 0, -1]])

# # --- 2. DEFINE THE TASKS ---
# task_left = mink.FrameTask(
#     frame_name="hand", frame_type="body",
#     position_cost=1.0, orientation_cost=1.0, lm_damping=1e-3
# )
# task_right = mink.FrameTask(
#     frame_name="r_hand", frame_type="body",
#     position_cost=1.0, orientation_cost=1.0, lm_damping=1e-3
# )

# # Regularization task to prevent nullspace jitter
# posture_task = mink.PostureTask(mj_model, cost=0.1)

# floor_id = mj_model.geom("floor").id
# robot_base_id_left = mj_model.body("link0").id
# robot_base_id_right = mj_model.body("r_link0").id

# robot_geoms_left = mink.get_subtree_geom_ids(mj_model, robot_base_id_left)
# robot_geoms_right = mink.get_subtree_geom_ids(mj_model, robot_base_id_right)

# collision_pairs = [
#     (robot_geoms_left, [floor_id]),
#     (robot_geoms_right, [floor_id]),
#     (robot_geoms_left, robot_geoms_right),
# ]

# limits = [
#     mink.ConfigurationLimit(model=mj_model),
#     mink.CollisionAvoidanceLimit(
#         model=mj_model,
#         geom_pairs=collision_pairs,
#         minimum_distance_from_collisions=0.02,
#         collision_detection_distance=0.05,
#     ),
# ]

# # --- 3. SET INITIAL STATE ---
# q_start = np.array([0, -0.785, 0, -2.356, 0, 1.571, 0.785])
# mj_data.qpos[:7] = q_start
# mj_data.qpos[9:16] = q_start
# mj_data.qpos[7:9] = mj_data.qpos[16:] = 0.0

# mujoco.mj_forward(mj_model, mj_data)
# configuration.update(mj_data.qpos)
# posture_task.set_target_from_configuration(configuration)

# hand_id_left = mujoco.mj_name2id(mj_model, mujoco.mjtObj.mjOBJ_BODY, "hand")
# current_target_left = mj_data.xpos[hand_id_left].copy()

# # ADDED: Track last valid position for rejection logic
# last_valid_target_left = current_target_left.copy()
# # ADDED: Smoothed target to prevent "teleportation" jitter
# smoothed_target_left = current_target_left.copy()

# def key_callback(keycode):
#     global current_target_left
#     delta = 0.05
#     if keycode == 262: current_target_left[0] += delta 
#     elif keycode == 263: current_target_left[0] -= delta 
#     elif keycode == 265: current_target_left[1] += delta 
#     elif keycode == 264: current_target_left[1] -= delta 
#     elif chr(keycode).lower() == 'w': current_target_left[2] += delta 
#     elif chr(keycode).lower() == 's': current_target_left[2] -= delta

# # --- 4. EXECUTION LOOP ---
# with mujoco.viewer.launch_passive(mj_model, mj_data, key_callback=key_callback) as viewer:
#     dt = 1/60  # Recommended for interactive keyboard use
    
#     while viewer.is_running():
#         step_start = time.time()
#         configuration.update(mj_data.qpos)

#         # A. COLLISION CHECK
#         collision_imminent = False
#         for g_id in robot_geoms_left:
#             d = mujoco.mj_geomDistance(mj_model, mj_data, g_id, floor_id, 0.5, None)
#             if 0 <= d < 0.012: # Slightly tighter 1.2cm buffer
#                 collision_imminent = True
#                 break
        
#         # B. DIRECTIONAL REJECTION
#         # Compare target Z to physical hand Z
#         if collision_imminent and current_target_left[2] < mj_data.xpos[hand_id_left][2]:
#             current_target_left[2] = mj_data.xpos[hand_id_left][2]

#         # C. SMOOTHING (LERP)
#         smoothed_target_left += (current_target_left - smoothed_target_left) * 0.1

#         # D. TASK UPDATE
#         new_pose_left = mink.SE3.from_rotation_and_translation(
#             rotation=mink.SO3.from_matrix(R_down), 
#             translation=smoothed_target_left
#         )
#         task_left.set_target(new_pose_left)

#         # E. SOLVE & INTEGRATE
#         velocity = mink.solve_ik(configuration, [task_left, posture_task], limits=limits, dt=dt, solver="osqp")
#         configuration.integrate_inplace(velocity, dt)

#         # F. SYNC
#         mj_data.qpos[:] = configuration.q
#         mujoco.mj_forward(mj_model, mj_data)
#         viewer.sync()

#         elapsed = time.time() - step_start
#         if elapsed < dt:
#             time.sleep(dt - elapsed)



import mujoco
import mujoco.viewer
import numpy as np
import time
import mink

# --- 1. INITIALIZE MODEL & DATA ---
mj_path = "ros2_lab_ws/src/dual_arm_simulation/mujoco_menagerie/franka_emika_panda/dual_arm_scene.xml"
mj_model = mujoco.MjModel.from_xml_path(mj_path)
mj_data = mujoco.MjData(mj_model)
configuration = mink.Configuration(mj_model)

# --- 2. DEFINE TASKS ---
task_left = mink.FrameTask(
    frame_name="hand", 
    frame_type="body",
    position_cost=1.0, 
    orientation_cost=1.0, 
    lm_damping=1e-3
)
task_right = mink.FrameTask(
    frame_name="r_hand", 
    frame_type="body",
    position_cost=1.0, 
    orientation_cost=1.0, 
    lm_damping=1e-3
)

posture_task = mink.PostureTask(mj_model, cost=0.1)

# --- 3. COLLISION SETUP ---
floor_id = mj_model.geom("floor").id
robot_base_id_left = mj_model.body("link0").id
robot_base_id_right = mj_model.body("r_link0").id

robot_geoms_left = mink.get_subtree_geom_ids(mj_model, robot_base_id_left)
robot_geoms_right = mink.get_subtree_geom_ids(mj_model, robot_base_id_right)

# Mink internal collision limits (The "Physical Guard")
collision_pairs = [
    (robot_geoms_left, [floor_id]),
    (robot_geoms_right, [floor_id]),
    (robot_geoms_left, robot_geoms_right), # Left arm vs Right arm
    (robot_geoms_left, robot_geoms_left), # Left arm vs Right arm
    (robot_geoms_right, robot_geoms_right), # Left arm vs Right arm
]

limits = [
    mink.ConfigurationLimit(model=mj_model),
    mink.CollisionAvoidanceLimit(
        model=mj_model,
        geom_pairs=collision_pairs,
        minimum_distance_from_collisions=0.02, # 2cm buffer
        collision_detection_distance=0.05, 
    ),
]

# --- 4. INITIAL STATE ---
q_start = np.array([0, -0.785, 0, -2.356, 0, 1.571, 0.785])
mj_data.qpos[:7] = q_start
mj_data.qpos[9:16] = q_start
mj_data.qpos[7:9] = mj_data.qpos[16:] = 0.0

mujoco.mj_forward(mj_model, mj_data)
configuration.update(mj_data.qpos)
posture_task.set_target_from_configuration(configuration)

# --- 5. KEYBOARD INTERACTION ---
hand_id_left = mujoco.mj_name2id(mj_model, mujoco.mjtObj.mjOBJ_BODY, "hand")
hand_id_right = mujoco.mj_name2id(mj_model, mujoco.mjtObj.mjOBJ_BODY, "r_hand")

current_target_left = mj_data.xpos[hand_id_left].copy()
current_target_right = mj_data.xpos[hand_id_right].copy()

smoothed_target_left = current_target_left.copy()
smoothed_target_right = current_target_right.copy()

R_down = np.array([[1, 0, 0], 
                   [0, -1, 0], 
                   [0, 0, -1]])

def key_callback(keycode):
    global current_target_left, current_target_right
    delta = 0.05
    # Left Arm: Arrows + U/D
    if keycode == 262: current_target_left[0] += delta 
    elif keycode == 263: current_target_left[0] -= delta 
    elif keycode == 265: current_target_left[1] += delta 
    elif keycode == 264: current_target_left[1] -= delta 
    elif chr(keycode).lower() == 'u': current_target_left[2] += delta 
    elif chr(keycode).lower() == 'd': current_target_left[2] -= delta
    # Right Arm: W/A/S/D + I/K
    elif chr(keycode).lower() == 'w': current_target_right[1] += delta
    elif chr(keycode).lower() == 's': current_target_right[1] -= delta
    elif chr(keycode).lower() == 'a': current_target_right[0] -= delta
    elif chr(keycode).lower() == 'd': current_target_right[0] += delta
    elif chr(keycode).lower() == 'i': current_target_right[2] += delta
    elif chr(keycode).lower() == 'k': current_target_right[2] -= delta

# --- 6. EXECUTION LOOP ---
with mujoco.viewer.launch_passive(mj_model, mj_data, key_callback=key_callback) as viewer:
    dt = 1/60
    while viewer.is_running():
        step_start = time.time()
        configuration.update(mj_data.qpos)

        # A. FLOOR DISTANCE CHECK (Directional Rejection)
        dist_max = 0.2  # For use in geom dist as Maximum distance to check for collision.
        
        # Check Left floor
        left_near_floor = any(mujoco.mj_geomDistance(mj_model, mj_data, g, floor_id, dist_max, None) < 0.015 for g in robot_geoms_left)
        if left_near_floor and current_target_left[2] < mj_data.xpos[hand_id_left][2]:
            current_target_left[2] = mj_data.xpos[hand_id_left][2]

        # Check Right floor
        # mj_geomDistance : Return ValueReturns the minimum distance as a mjtNum (usually double precision)
        # If no collision/distance smaller than distmax is found, it returns distmax, and fromto is filled with zeros (here is None).
        right_near_floor = any(mujoco.mj_geomDistance(mj_model, mj_data, g, floor_id, dist_max, None) < 0.015 for g in robot_geoms_right)
        if right_near_floor and current_target_right[2] < mj_data.xpos[hand_id_right][2]:
            current_target_right[2] = mj_data.xpos[hand_id_right][2]

        # B. ROBOT-TO-ROBOT DISTANCE CHECK (Software Guard)
        # We check distance between the two hands/wrists to prevent targets crossing
        robot_dist_collision = False
        for g_l in robot_geoms_left:
            for g_r in robot_geoms_right:
                d_robots = mujoco.mj_geomDistance(mj_model, mj_data, g_l, g_r, dist_max, None)
                if 0 <= d_robots < 0.03: # 3cm safety gap
                    robot_dist_collision = True
                    break
            if robot_dist_collision: break
        
        if robot_dist_collision:
            # Revert both targets to current physical positions to stop them moving closer
            current_target_left = mj_data.xpos[hand_id_left].copy()
            current_target_right = mj_data.xpos[hand_id_right].copy()

        # C. SMOOTHING
        smoothed_target_left += (current_target_left - smoothed_target_left) * 0.1
        smoothed_target_right += (current_target_right - smoothed_target_right) * 0.1

        # D. TASK UPDATE
        task_left.set_target(mink.SE3.from_rotation_and_translation(mink.SO3.from_matrix(R_down), smoothed_target_left))
        task_right.set_target(mink.SE3.from_rotation_and_translation(mink.SO3.from_matrix(R_down), smoothed_target_right))

        # E. SOLVE & INTEGRATE
        velocity = mink.solve_ik(
            configuration, 
            [task_left, task_right, posture_task], 
            limits=limits, 
            dt=dt, 
            solver="osqp")
        configuration.integrate_inplace(velocity, dt)

        # F. SYNC
        mj_data.qpos[:] = configuration.q
        mujoco.mj_forward(mj_model, mj_data)
        viewer.sync()

        elapsed = time.time() - step_start
        if elapsed < dt: time.sleep(dt - elapsed)