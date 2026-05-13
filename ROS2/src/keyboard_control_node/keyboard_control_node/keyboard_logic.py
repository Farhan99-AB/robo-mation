import mujoco
import numpy as np

class TargetManager:
    def __init__(self, xml_path, q_start):
        # Load the model just to calculate the starting XYZ
        self.model = mujoco.MjModel.from_xml_path(xml_path)
        self.data = mujoco.MjData(self.model)
        
        # Set the joints to your q_start
        self.data.qpos[:] = q_start
        mujoco.mj_forward(self.model, self.data)
        
        # Get the EXACT global coordinates of the hands
        hand_l_id = mujoco.mj_name2id(self.model, mujoco.mjtObj.mjOBJ_BODY, "hand")
        hand_r_id = mujoco.mj_name2id(self.model, mujoco.mjtObj.mjOBJ_BODY, "r_hand")
        
        self.pos_l = self.data.xpos[hand_l_id].copy()
        self.pos_r = self.data.xpos[hand_r_id].copy()
        
        self.delta = 0.02

    def update_from_input(self, key):
        """Processes raw terminal input."""
        # Left Arm: Arrows (Termios sends sequences like \x1b[A for up)
        if key == '\x1b[A':   # Up Arrow
            self.pos_l[1] += self.delta
        elif key == '\x1b[B': # Down Arrow
            self.pos_l[1] -= self.delta
        elif key == '\x1b[C': # Right Arrow
            self.pos_l[0] += self.delta
        elif key == '\x1b[D': # Left Arrow
            self.pos_l[0] -= self.delta
        
        # Left Arm: Vertical
        elif key == 'u': self.pos_l[2] += self.delta
        elif key == 'j': self.pos_l[2] -= self.delta

        # Right Arm: WASD
        elif key == 'w': self.pos_r[1] += self.delta
        elif key == 's': self.pos_r[1] -= self.delta
        elif key == 'a': self.pos_r[0] -= self.delta
        elif key == 'd': self.pos_r[0] += self.delta
        
        # Right Arm: Vertical
        elif key == 'i': self.pos_r[2] += self.delta
        elif key == 'k': self.pos_r[2] -= self.delta

    def get_targets(self):
        return self.pos_l, self.pos_r