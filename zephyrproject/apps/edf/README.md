## Overview

This project demonstrates a real-time system implemented using **Zephyr RTOS**, featuring:

- Periodic real-time tasks scheduled under EDF
- A **Deferrable Server (DS)** for handling aperiodic jobs
- Button-triggered aperiodic task release using GPIO interrupts
- System schedulability validation using **density-based acceptance tests**

---

## Team Members

- **Ganesh Patil** – 3823129
- **Farhan Balekundri** – 3823048
- **Varadraj Dundappa Yalamalli** – 3822751
- **Esther Ürek** – 3531932

---

## Code Description

### 1. Density-Based Acceptance Test

- Implemented an **iterative system density test** for periodic tasks.
- Only previously accepted tasks contribute to the density calculation.
- Task specifications are retrieved using `STRUCT_SECTION_FOREACH`, ensuring scalability and correctness.
- A task is accepted if the cumulative density is ≤ 1.0.

---

### 2. Deadline Management

- Implemented a simplified EDF deadline setter using:
  - `k_uptime_get()`
  - `z_impl_k_thread_absolute_deadline_set()`
  - `k_yield()`
- Avoids overflow issues associated with Zephyr’s CPU-cycle–based deadline handling.
- Ensures correct EDF behavior even for long task periods.

---

### 3. Button Interrupt Handling

- Button presses are handled via a **GPIO Interrupt Service Routine (ISR)**.
- ISR responsibilities are intentionally minimal:
  - Software debounce validation
  - Signaling the arrival of an aperiodic job
- Prevents blocking operations and maintains ISR safety.

---

### 4. GPIO Initialization

- Button GPIO setup is encapsulated in `init_button_gpio()`.
- Called **exactly once from `main()`**, as GPIO configuration must not be repeated.
- Includes:
  - GPIO input configuration
  - Interrupt configuration
  - Callback registration

---

### 5. Deferrable Server Implementation

- Implemented a **Deferrable Server using a semaphore**.
- Design strictly follows deferrable server theory:
  - Budget is replenished periodically
  - Budget does **not accumulate across periods**
  - At most **one aperiodic job is served per period**
- Extra button presses are **not queued**; only one pending job is handled.

**Why a semaphore?**
- Safe to signal from ISR (`k_sem_give()`)
- Avoids busy waiting
- Simple, deterministic, and easy to reason about
- Clean separation between job arrival and job execution

---

## Challenges Addressed

### 1. Button Debouncing

- Implemented **software debouncing** inside the GPIO interrupt callback.
- Used `k_uptime_get()` to ignore button presses occurring within `DEBOUNCE_MS`.
- Prevents multiple unintended aperiodic job releases due to mechanical bounce.

---

### 2. Floating Input Pin

- Enabled the **internal pull-up resistor** for the button in the Device Tree:

```dts
gpios = <&gpio0 15 (GPIO_ACTIVE_LOW | GPIO_PULL_UP)>;
```

- Eliminates floating input behavior and removes the need for an external pull-up resistor.

---

### 3. Internal Pull-Up Resistor Requires Power Cycle

- After flashing the firmware, the board must be **disconnected and repowered**.
- This ensures the internal pull-up resistor configuration is applied correctly.
- Without a power cycle, the GPIO pin may retain its previous electrical state.

---

## Validation

### 1. Task Acceptance Validation

- Verified that **only accepted periodic tasks are scheduled**.
- When total system density exceeds `1.0`, **Task 2 is correctly rejected**.
- Only Task 1, Task 3, and the Deferrable Server are created and executed.
- Confirms correctness of the density-based acceptance test.

---

### 2. Deadline and Scheduling Validation

- System density configured to **0.98** to test near-saturation conditions.
- Observations:
  - No periodic task misses its deadline
  - Aperiodic tasks execute **only when spare CPU capacity is available**
  - Confirms correct EDF scheduling and DS behavior

---

## Build and Run Instructions

Please consult the assignment sheet for detailed implementation requirements.

Build and flash the application using:

```bash
west build -p always -b esp32_devkitc/esp32/procpu --sysbuild apps/03_edf
west flash
west espressif monitor
```

---