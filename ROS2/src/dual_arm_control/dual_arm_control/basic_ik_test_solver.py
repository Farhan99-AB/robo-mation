import numpy as np
import time

class BasicIKSolver:
    def __init__(self, num_joints=7):
        # 1. Initialize joint angles (radians)
        self.q = np.zeros(num_joints)
        self.link_lengths = np.ones(num_joints) # Assume each link is 1 unit long
        self.learning_rate = 0.1
        self.tolerance = 1e-3

    def forward_kinematics(self, q):
        """Step 1: Calculate current End-Effector position."""
        x = np.sum(self.link_lengths * np.cos(np.cumsum(q)))
        y = np.sum(self.link_lengths * np.sin(np.cumsum(q)))
        return np.array([x, y])

    def get_jacobian(self, q):
        """Step 3: Calculate the 'Sensitivity' matrix."""
        # For a 2D arm, J is a 2x7 matrix
        # J[0,i] is how joint i affects X; J[1,i] is how it affects Y
        J = np.zeros((2, len(q)))
        cumulative_q = np.cumsum(q)
        
        for i in range(len(q)):
            # Partial derivatives of the FK equations
            J[0, i] = -np.sum(self.link_lengths[i:] * np.sin(cumulative_q[i:]))
            J[1, i] = np.sum(self.link_lengths[i:] * np.cos(cumulative_q[i:]))
        return J

    def solve(self, target_pose):
        print(f"Target: {target_pose}")
        
        for iteration in range(500):
            # Step 1: Where are we?
            current_pos = self.forward_kinematics(self.q)
            
            # Step 2: Calculate error
            error = target_pose - current_pos
            distance = np.linalg.norm(error)
            
            # Step 4: Check tolerance
            if distance < self.tolerance:
                print(f"✅ Goal reached in {iteration} iterations!")
                return self.q
            
            # Step 3: Update matrix recursively (Gradient Descent)
            J = self.get_jacobian(self.q)
            
            # The 'Magic' Step: Move joints in direction of the Transposed Jacobian
            # delta_q = J_transpose * error_vector
            dq = J.T @ error
            self.q += dq * self.learning_rate
            
            if iteration % 50 == 0:
                print(f"Iteration {iteration}: Distance to goal = {distance:.4f}")

        print("❌ Could not reach goal within iteration limit.")
        return self.q

# Run the test
solver = BasicIKSolver(num_joints=7)
target = np.array([2.0, 2.0]) # A reachable point for a 7-link arm
final_angles = solver.solve(target)
print(f"Final Joint Angles: {np.rad2deg(final_angles)}")