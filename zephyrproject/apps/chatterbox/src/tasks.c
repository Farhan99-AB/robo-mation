#include "tasks.h"
#include <assert.h>
#include <stdio.h>

/**
 * @brief TODO: Let LED blink for (roughly) delay_ms.
 *
 * @param led	    LED device.
 * @param delay_ms  Delay in milliseconds. Must be a multiple of
 *		    BLINK_PERIOD_MS.
 */
void useless_load(int task_id, const struct gpio_dt_spec *led, int32_t delay_ms)
{
	assert(delay_ms % BLINK_PERIOD_MS == 0 && BLINK_PERIOD_MS % 2 == 0);

	// Let LED blink for (roughly) delay_ms
	int ret;
	for (int32_t d = 0; d < delay_ms; d += BLINK_PERIOD_MS) {
		printf(" Task %d\n", task_id);

		/** Modify this method so that *led* is blinking while this loop is
		 * executed. Afterwards, the LED should be in the OFF state. */
		ret = gpio_pin_toggle_dt(led);
		if (ret < 0) {
			return 0;
		}
		k_busy_wait(BLINK_PERIOD_MS * 1000);
	}
	ret = gpio_pin_set_dt(led, 0); //setting it to off
	if (ret < 0) {
			return 0;
	}
}

/**
 * @brief TODO: Implementation of the chatterbox task
 */
void chatterbox_task(struct task_params *params)
{
	/** Implement the chatterbox task for generic *task_params*.
	 * This consists of:
	 *  - Initializing the GPIO pin for params->led
	 *  - Periodically executing the task by calling *useless_load*
	 *  - Suspending the task until the next cycle
	 */

	if (!gpio_is_ready_dt(params->led)) {
		return 0;
	}

	int ret = gpio_pin_configure_dt(params->led, GPIO_OUTPUT_ACTIVE);

	// Start the timer with release and period
    k_timer_start(params->timer,
                  K_NO_WAIT,
                  K_MSEC(params->period_ms));

	if (ret < 0) {
		return 0;
	}

	while(1)
	{
		// Wait for period expiration
        k_timer_status_sync(params->timer);

		useless_load(params->task_id, params->led, params->execution_time_ms);
	}
}

/** TODO: After correctly setting the GPIO assignments for
 * Task 2 and 3 in boards/esp_wrover_kit_procpu.overlay,
 * uncomment the following lines.
 */
INITIALIZE_TASK(1);
INITIALIZE_TASK(2);
INITIALIZE_TASK(3);
