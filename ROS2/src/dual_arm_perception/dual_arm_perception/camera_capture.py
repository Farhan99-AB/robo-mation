import numpy as np
from collections import deque


class HandProcessor:
    def __init__(self):
        # ─── 1. EXPONENTIAL SMOOTHING ─────────────────────────────────────────
        # Separate alphas: position moves slower, rotation can be snappier
        self.alpha_pos   = 0.50   # Raised for responsiveness — lower toward 0.25 if too jittery
        self.alpha_theta = 0.55   # Raised for responsiveness

        # ─── 2. VELOCITY CLAMPING ────────────────────────────────────────────
        self.max_velocity = 0.50  # Raised — less clamping = faster tracking

        # ─── 3. DEAD ZONES ───────────────────────────────────────────────────
        self.dead_zone_pos   = 0.004   # Metres — suppresses micro-tremor
        self.dead_zone_theta = 0.015   # Radians (~0.85°) — suppresses roll flicker

        # ─── 4. TEMPORAL MEDIAN FILTER (anti-flicker) ────────────────────────
        # Keeps a small history and median-filters before EMA.
        # Odd window → clean median. 3 = one frame lag, 5 = two frame lag.
        self._hist_len = 3        # Keep at 3 — going higher adds lag
        self._pos_hist   = {"Left": deque(maxlen=self._hist_len),
                            "Right": deque(maxlen=self._hist_len)}
        self._theta_hist = {"Left": deque(maxlen=self._hist_len),
                            "Right": deque(maxlen=self._hist_len)}

        # ─── 5. ROBOT WORKSPACE LIMITS ───────────────────────────────────────
        self.y_fixed = -0.30          # Depth is fixed for both arms

        self.x_limit_left  = [-0.95,  0.11]
        self.z_limit_left  = [ 0.11,  0.85]

        self.x_limit_right = [-0.09,  1.05]  # 1.05 = left-most on screen
        self.z_limit_right = [ 0.11,  0.85]

        # ─── 6. STATE ─────────────────────────────────────────────────────────
        self.prev_pos   = {"Left": None, "Right": None}
        self.prev_theta = {"Left": 0.0,  "Right": 0.0}

    # ─────────────────────────────────────────────────────────────────────────
    # SYNC
    # ─────────────────────────────────────────────────────────────────────────
    def sync_to_mujoco_coords(self, label: str, x, y, z):
        """Warm-start the filter at the robot's current home position."""
        pos = np.array([float(x), float(y), float(z)])
        self.prev_pos[label] = pos
        for _ in range(self._hist_len):
            self._pos_hist[label].append(pos.copy())

    # ─────────────────────────────────────────────────────────────────────────
    # GEOMETRY HELPERS
    # ─────────────────────────────────────────────────────────────────────────
    def calculate_midpoint(self, lm):
        """Thumb tip (4) ↔ index tip (8) midpoint — maps to robot target."""
        return (lm[4].x + lm[8].x) / 2, (lm[4].y + lm[8].y) / 2

    def calculate_wrist_roll(self, lm) -> float:
        """
        Compute the pronation / supination angle of the wrist (the
        'screwdriver' rotation about the forearm axis).

        Strategy
        --------
        1.  Build a hand-plane coordinate frame from three stable landmarks:
              • wrist      (0)
              • index MCP  (5)
              • pinky MCP  (17)
        2.  The knuckle vector  v_knuckle = lm17 – lm5  lies across the palm.
        3.  The forearm  vector  v_fore    = lm9  – lm0  points up the hand.
        4.  Their cross product gives the palm normal.
        5.  Project the palm normal onto the image plane (ignore z from
            MediaPipe's normalised 3-D coords) and take atan2 → roll angle.

        When the palm faces the camera:  roll ≈ 0
        Rotate hand clockwise (screwing in):  roll increases positively
        Rotate counter-clockwise:             roll goes negative

        Returns
        -------
        float  Roll angle in radians,  range (−π, π).
        """
        # Use MediaPipe's pseudo-3D (z is relative depth, same scale as x/y)
        def pt(idx):
            return np.array([lm[idx].x, lm[idx].y, lm[idx].z])

        wrist     = pt(0)
        index_mcp = pt(5)
        pinky_mcp = pt(17)
        mid_mcp   = pt(9)

        # Two in-plane vectors
        v_knuckle = pinky_mcp - index_mcp   # across the palm
        v_fore    = mid_mcp   - wrist        # up the hand

        # Palm normal via cross product
        normal = np.cross(v_knuckle, v_fore)

        # The component of the normal that lies in the XY (image) plane
        # represents how much the palm is "twisted" relative to the camera.
        nx, ny = normal[0], normal[1]

        roll = np.arctan2(ny, nx)
        return roll

    def calculate_theta(self, lm) -> float:
        """
        Legacy planar orientation (yaw in image plane).
        Uses wrist → middle-finger-MCP vector.
        Kept for HUD display; use calculate_wrist_roll for twist.
        """
        return np.arctan2(lm[9].y - lm[0].y, lm[9].x - lm[0].x)

    # ─────────────────────────────────────────────────────────────────────────
    # MAPPING  (camera → robot workspace)
    # ─────────────────────────────────────────────────────────────────────────
    def map_to_robot(self, mx: float, my: float, label: str):
        """
        Maps normalised camera midpoint (mx, my) to robot (x, y, z).

        Pipeline
        --------
        raw position
            → temporal median filter  (removes single-frame glitches)
            → velocity clamp          (rejects landmark teleports)
            → dead-zone gate          (suppresses sub-threshold tremor)
            → exponential smoothing   (low-pass for remaining noise)
        """
        # 1. Workspace mapping
        if label == "Left":
            norm_x   = np.clip(mx / 0.5, 0.0, 1.0)
            limits_x = self.x_limit_left
            limits_z = self.z_limit_left
        else:
            norm_x   = np.clip((mx - 0.5) / 0.5, 0.0, 1.0)
            limits_x = self.x_limit_right
            limits_z = self.z_limit_right

        target_x = limits_x[0] + norm_x * (limits_x[1] - limits_x[0])
        target_y = self.y_fixed
        # my=0 → top of screen → z_limit[1] (high);  my=1 → bottom → z_limit[0]
        target_z = limits_z[1] - (my * (limits_z[1] - limits_z[0]))

        raw = np.array([target_x, target_y, target_z])

        # ── Warm-start guard ──────────────────────────────────────────────
        if self.prev_pos[label] is None:
            self.prev_pos[label] = raw.copy()
            for _ in range(self._hist_len):
                self._pos_hist[label].append(raw.copy())
            return float(raw[0]), float(raw[1]), float(raw[2])

        # ── Temporal median filter ────────────────────────────────────────
        self._pos_hist[label].append(raw.copy())
        stack   = np.stack(self._pos_hist[label])   # shape (hist_len, 3)
        med_pos = np.median(stack, axis=0)

        # ── Velocity clamp ────────────────────────────────────────────────
        delta = med_pos - self.prev_pos[label]
        dist  = np.linalg.norm(delta)
        if dist > self.max_velocity:
            med_pos = self.prev_pos[label] + (delta / dist) * self.max_velocity

        # ── Dead-zone gate ────────────────────────────────────────────────
        if np.linalg.norm(med_pos - self.prev_pos[label]) < self.dead_zone_pos:
            s = self.prev_pos[label]
            return float(s[0]), float(s[1]), float(s[2])

        # ── Exponential smoothing ─────────────────────────────────────────
        smoothed = self.alpha_pos * med_pos + (1 - self.alpha_pos) * self.prev_pos[label]
        self.prev_pos[label] = smoothed

        return float(smoothed[0]), float(smoothed[1]), float(smoothed[2])

    def map_theta(self, raw_theta: float, label: str) -> float:
        """
        Smooth the wrist-roll angle through median → dead-zone → EMA.
        Handles wrap-around at ±π correctly.
        """
        self._theta_hist[label].append(raw_theta)
        med = np.median(list(self._theta_hist[label]))

        prev = self.prev_theta[label]

        # Wrap-aware delta
        delta = (med - prev + np.pi) % (2 * np.pi) - np.pi
        if abs(delta) < self.dead_zone_theta:
            return prev

        smoothed = prev + self.alpha_theta * delta
        # Normalise to (−π, π)
        smoothed = (smoothed + np.pi) % (2 * np.pi) - np.pi
        self.prev_theta[label] = smoothed
        return float(smoothed)
