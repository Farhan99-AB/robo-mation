/*
	Team Members
	************
	Ganesh Patil :- 3823129
	Farhan Balekundri :- 3823048
	Varadraj Dundappa Yalamalli :- 3822751
	Esther Ürek :- 3531932
*/

#include <assert.h>
#include <stdio.h>

#include "display.h"
#include "tasks.h"

#define BUTTON_NODE DT_NODELABEL(button)
#define DEBOUNCE_MS 400
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(BUTTON_NODE, gpios, {0});
static struct gpio_callback button_cb_data;

/* Signals arrival of an aperiodic job */
K_SEM_DEFINE(aperiodic_job_sem, 0, 1);

/* Signals start of a new DS period (budget replenishment) */
K_SEM_DEFINE(ds_period_sem, 0, 1);

/**
 * @brief Let LED blink for (roughly) delay_ms.
 *
 * @param params    Task parameters
 */
void useless_load_periodic_tasks(struct task_params *params)
{
	assert(params->execution_time_ms % BLINK_PERIOD_MS == 0 && BLINK_PERIOD_MS % 2 == 0);

	// Lock scheduler to avoid time sharing if the deadline of two or multiple tasks is equal
	k_sched_lock();
	for (int32_t d = 0; d < params->execution_time_ms; d += BLINK_PERIOD_MS) {
		printf(" Task %d with deadline %d\n", params->task_id,
		       params->thread->base.prio_deadline);
		gpio_pin_set_dt(params->led, 1);
		k_busy_wait(BLINK_PERIOD_MS * 1000 / 2);
		gpio_pin_set_dt(params->led, 0);
		k_busy_wait(BLINK_PERIOD_MS * 1000 / 2);
	}
	k_sched_unlock();
}

/**
 * @brief Show animation on SSD1306 for one second
 */
void useless_load_aperiodic_tasks()
{
	k_sched_lock();
	for (int32_t d = 0; d < MSEC_PER_SEC; d += BLINK_PERIOD_MS) {
		printf(" Aperiodic task\n");
		ssd1306_print_aperiodic_task();
		k_busy_wait(BLINK_PERIOD_MS * 1000);
	}
	k_sched_unlock();
}

/**
 * @brief Simplified version of z_impl_k_thread_deadline_set
 *
 * Unfortunately, z_impl_k_thread_deadline_set relies on deadlines
 * relative to the CPU cycle counter, while casting the result in
 * int32_t. For our dummy tasks, with periods and latencies in the
 * range of multiple seconds, this will result in frequent overflows
 * that breaks EDF as a consequence.
 *
 * @param thread    thread handle
 * @param deadline  thread deadline in milliseconds
 */
void thread_deadline_set(struct k_thread *thread, int deadline)
{
	/* TODO (Part a): Write a simplified version of z_impl_k_thread_deadline_set
	 * that specified the deadline in milliseconds rather than
	 * in CPU cycle counts.
	 * Hint: You can use k_yield() after updating the deadline
	 * to dequeue the thread from the ready queue. */
	int32_t newdl = k_uptime_get() + deadline;
	printf("setting deadline for thread: %d\n", newdl);
	z_impl_k_thread_absolute_deadline_set((k_tid_t)thread, newdl);
	k_yield();
}

/**
 * @brief System density test for periodic tasks
 *
 * @params params task specification
 */
bool acceptance_test(struct task_params *params)
{
	static bool acceptance_test_results[CONFIG_NUM_TASKS];

	/* TODO (Part a): The system density test should account for the deferrable server (DS).
	 * That is, we assume the DS is always accepted and the acceptance is solely performed for
	 * periodic tasks. Moreover the acceptance test is called iteratively (see main.c), meaning
	 * that Task i must only account for the previously accepted tasks.
	 *
	 * Your solution should
	 *  - use STRUCT_SECTION_FOREACH for retrieving the task specification of all other tasks
	 *    (see main.c for reference)
	 *  - store the acceptance test result in *acceptance_test_results*
	 * */
	printf("acceptance test\n");
	double density = 0;
	STRUCT_SECTION_FOREACH(task_params, current_params) {
		if (current_params->type == PERIODIC_TASK) {
			if (current_params->task_id == params->task_id) {
				break;
			}
			if (acceptance_test_results[current_params->task_id]) {
				density +=
					(double)current_params->execution_time_ms /
					(double)current_params->period_ms;				// since deadline = period
			}
		}
	}
	density += (double)params->execution_time_ms / (double)params->period_ms;
	bool accepted = density <= 1.0;
	acceptance_test_results[params->task_id] = accepted;
	printf(" Task %d: density = %.2f -> %s\n", params->task_id, density,
	       accepted ? "ACCEPTED" : "REJECTED");
	return accepted;
}

/**
 * @brief Dummy task implementation that toggles LED
 */
void periodic_task_implementation(void *p0, void *p1, void *p2)
{
	struct task_params *params = (struct task_params *)p0;

	if (!gpio_is_ready_dt(params->led)) {
		return;
	}
	int ret = gpio_pin_configure_dt(params->led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return;
	}

	k_timer_start(params->timer, K_NO_WAIT, K_MSEC(params->period_ms));
	for (int32_t i = 1;; i++) {
		k_timer_status_sync(params->timer);
		printf("setting deadline for thread:%d",params->task_id);
		thread_deadline_set(params->thread, i * params->period_ms);
		useless_load_periodic_tasks(params);
	}
}

/* TODO (Part b): Configure the button in the DTS overlay and initialize the GPIO driver here.
 * The goal is to release an aperiodic job whenever the button is pressed and to use the deferrable
 * server (as introduced in the lecture) to schedule them. To this end, you have to complete
 * the implementation of
 *  - button_press_callback
 *  - init_button_gpio
 *  - deferrable_server_implementation
 * */

void button_press_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	/* Utilize a suitable synchronization primitive to let the DS know that
	 * the button was pressed. Note that
	 *  - Interrupt service routines (ISRs) are very different from normal threads and many
	 *    kernel APIs behave differently when called from an ISR or from a thread. The
	 *    implementation of this ISR should therefore be as lightweight as possible. For more
	 *    information, see https://docs.zephyrproject.org/latest/kernel/services/interrupts.html
	 *  - Deferrable servers are supposed to yield their execution to the scheduler if there
	 *    are no asynchronous jobs available. Thus, your synchronization primitive must not
	 *    result in busy waiting that may start periodic tasks.
	 * */
	printf("button press callback\n"); 
	int val = gpio_pin_get_dt(&button);
	static int64_t last_press_time = 0;
    int64_t now = k_uptime_get();

    if (now - last_press_time < DEBOUNCE_MS) {
        return; // Ignore bounce
    }
    last_press_time = now;
	printf("button value: %d\n", val);
	ARG_UNUSED(dev);
    ARG_UNUSED(cb);
    ARG_UNUSED(pins);

    k_sem_give(&aperiodic_job_sem);
}

int init_button_gpio()
{
	/* TODO: Initialize the GPIO pin to trigger a callback to *button_press_callback* whenever
	 *the button is pressed. For reference, you may want to have a look at:
	 *https://docs.zephyrproject.org/latest/samples/basic/button/README.html */
	printf("init button gpio from main\n"); 
	int ret;

    if (!device_is_ready(button.port)) {
        return -ENODEV;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret < 0) {
        return ret;
    }

    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret < 0) {
        return ret;
    }

    gpio_init_callback(&button_cb_data, button_press_callback, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);
	printf("Set up button at %s pin %d\n", button.port->name, button.pin);

    return 0;
}

void deferrable_server_implementation(void *p0, void *p1, void *p2)
{
	struct task_params *params = (struct task_params *)p0;

	/* TODO: Implement a deferrable server that replenishes periodically, but
	 * that does not accumulate its budget over multiple cycles. For simplicity,
	 * you may assume the following:
	 *  - Every aperiodic job is identical and wants to run *useless_load_aperiodic_tasks()*
	 *  - The budget of the DS is equal to the budget of one aperiodic job per period
	 * As a starting point, you can likely reuse parts of *periodic_task_implementation*.
	 * */

    /* Start DS period timer */
    k_timer_start(params->timer, K_NO_WAIT, K_MSEC(params->period_ms));

    for (int32_t i = 1;; i++) {

        /* Wait for next DS period (budget replenishment) */
        k_timer_status_sync(params->timer);

        /* Set EDF deadline */
        thread_deadline_set(params->thread, i * params->period_ms);

        /* Check if an aperiodic job is pending */
        if (k_sem_take(&aperiodic_job_sem, K_NO_WAIT) == 0) {

            /* Consume entire budget for ONE job */
            useless_load_aperiodic_tasks();

        } else {
            /* No job → do not consume budget */
            k_yield();
        }
    }
}

/**
 * @brief Prints current time in seconds
 */
void print_time()
{
	static int time_sec = 0;
	printf("T = %ds:\n", time_sec);
	time_sec++;
}
K_TIMER_DEFINE(time_keeper, print_time, NULL);

/**
 * @brief Initializes periodic timer interrupt to print time in seconds
 */
void start_time_keeper(void)
{
	k_timer_start(&time_keeper, K_NO_WAIT, K_MSEC(1000));
}
