### initial basic code without loop to calcualte distance remaining for each joint
# import pinocchio as pin
# import numpy as np
# import os

# '''
# ###
# # 1. Getting the current pose  
# ###
# '''
# # 1. Path to your URDF file
# # Ensure you have downloaded the franka urdf to your workspace
# urdf_filename = "ros2_lab_ws/src/dual_arm_control/dual_arm_control/panda.urdf" 

# # 2. Build the Model and Data
# # 'model' stores the constant structure (links, joint types)
# # 'data' is used for the results of specific calculations
# model = pin.buildModelFromUrdf(urdf_filename)
# data = model.createData()

# print(f"Model name: {model.name}")
# print(f"Number of joints: {model.nq}") # nq = number of configuration variables

# # 3. Define a Joint Configuration (q)
# # Let's start with a 'neutral' position (all joints at 0)
# q = np.array([1.7, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])

# # 4. Perform Forward Kinematics (FK)
# # This calculates the 3D position of every joint/link based on 'q'
# pin.forwardKinematics(model, data, q)

# # 5. Extract the Pose of a Specific Link
# # We usually want the 'End Effector' (the hand)
# # In most Franka URDFs, the last link is named 'panda_link8' or 'panda_hand'
# # Let's find the ID of that frame:
# frame_name = "panda_link8" 
# if model.existFrame(frame_name):
#     frame_id = model.getFrameId(frame_name)
    
#     # Update the frames based on the FK we just ran (since the end effector or hand is a frame and not a joint)
#     # if we don't run this step hand coordinates won't be updated
#     pin.updateFramePlacements(model, data)
    
#     # Get the pose (Translation + Rotation)
#     pose = data.oMf[frame_id]
    
#     print(f"\n--- Current Pose of {frame_name} ---")
#     print(f"Translation (x, y, z): {pose.translation}")
#     print(f"Rotation Matrix:\n{pose.rotation}")
# else:
#     print(f"Frame {frame_name} not found in URDF. Check your URDF link names!")

# # 6. How to extract individual Joint Angles?
# # Since 'q' is what YOU send to the robot, you already have the angles.
# # However, if you want to see the limits from the URDF:
# print("\n--- Joint Limits from URDF ---")
# for i in range(1, model.nq + 1):
#     print(f"Joint {i}: Min {model.lowerPositionLimit[i-1]:.2f}, Max {model.upperPositionLimit[i-1]:.2f}")

# '''
# ###
# # 2. Calculating the distance from current pose to target pose  
# ###
# '''

# # 1. Define where you WANT to go (Target Pose)
# target_pos = np.array([0.5, 0.2, 0.4]) 
# target_rot = data.oMf[frame_id].rotation # Keep current orientation for now
# target_pose = pin.SE3(target_rot, target_pos)

# # 2. Get the Current Pose from 'data'
# current_pose = data.oMf[frame_id]

# # 3. Calculate the Transformation Error
# # This is like 'Target - Current' but for 3D matrices
# error_pose = current_pose.inverse() * target_pose

# # 4. Convert to a 6D Vector [x, y, z, roll, pitch, yaw]
# # This vector represents the 'direction' to the goal
# error_6d = pin.log6(error_pose).vector

# print(f"Error Vector: {error_6d}")
# print(f"Distance to Goal: {np.linalg.norm(error_6d[:3])} meters")

# '''
# ###
# # 3. Calculate the Jacobian matrix to reach the goal for each joint 
# ###
# '''

# # 1. Calculate the Jacobian
# # This computes the matrix for a specific joint (JOINT_ID) 
# # relative to the world frame (LOCAL_WORLD_ALIGNED)
# joint_id = model.getJointId("panda_joint7")
# pin.computeJointJacobian(model, data, q, joint_id)

# # 2. Extract the matrix from 'data'
# # We use the 'LOCAL_WORLD_ALIGNED' frame because our target 
# # coordinates (Step 2) are in the Global/World frame.
# J = pin.getJointJacobian(model, data, joint_id, pin.ReferenceFrame.LOCAL_WORLD_ALIGNED)

# print(f"Jacobian Shape: {J.shape}") # Should be (6, 7)

# # 3. Calculate the change in joints (dq)
# # For simplicity, we use the Transpose of the Jacobian (J.T) 
# # It acts like a compass pointing toward the goal.
# learning_rate = 0.1
# dq = J.T @ error_6d

# # 4. Update your joint configuration
# q_new = q + dq * learning_rate

# print(f"Old q: {q}")
# print(f"New q: {q_new}")



### second complete basic IK sovler with pinnochio
import pinocchio as pin
import numpy as np

# --- SETUP ---
# Path to your URDF (using the official one we found earlier)
urdf_path = "ros2_lab_ws/src/dual_arm_control/dual_arm_control/panda.urdf" # probably this needs to be fed from mojuco node
model = pin.buildModelFromUrdf(urdf_path)
data = model.createData()
JOINT_ID = model.getJointId("panda_joint7")

# --- PARAMETERS ---
# Starting configuration (Step 1)
q = np.array([0, -0.785, 0, -2.356, 0, 1.571, 0.785])  # this needs to be fed from the camera node

# Target Pose (Where we want to go)
target_pos = np.array([0.5, 0.1, 0.4]) # fed from the camera node
target_rot = np.eye(3) # Keeping the hand level (facing down/forward)
target_pose = pin.SE3(target_rot, target_pos)

# Solver Settings
eps = 1e-4       # Error tolerance (0.1 mm)
IT_MAX = 500     # Safety break to prevent infinite loops
learning_rate = 0.1 

print("Starting IK Solver...")

# --- THE LOOP (Step 4) ---
for i in range(IT_MAX):
    # 1. Forward Kinematics & Update Frames
    pin.forwardKinematics(model, data, q)
    pin.updateFramePlacements(model, data)
    current_pose = data.oMf[JOINT_ID]

    # 2. Calculate Error (Step 2)
    error_6d = pin.log6(current_pose.inverse() * target_pose).vector

    # Check if we are close enough
    distance = np.linalg.norm(error_6d)
    if distance < eps:
        print(f"✅ Success! Target reached in {i} iterations.")
        break

    # 3. Calculate Jacobian & Update q (Step 3)
    pin.computeJointJacobian(model, data, q, JOINT_ID)
    J = pin.getJointJacobian(model, data, JOINT_ID, pin.ReferenceFrame.LOCAL_WORLD_ALIGNED)
    
    # Update joint angles
    # Using the Transpose (J.T) is the most 'basic' way to update q
    dq = J.T @ error_6d
    q = q + dq * learning_rate

    if i % 50 == 0:
        print(f"Iteration {i}: Distance to goal = {distance:.4f}")

print(f"\nFinal Joint Angles (q): {q}")