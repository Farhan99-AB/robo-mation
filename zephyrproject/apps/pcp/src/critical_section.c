#include "critical_section.h"
#include "tasks.h"
#include <assert.h>

/*
 * TODO (Part b): Initialize resource priority ceilings and system ceiling
 */
void initialize_priority_ceilings()
{
}

/*
 * TODO (Part b): Blocks the current thread until it is safe to lock the resource
 *
 * The semantic of this method depends on PIP (default), IPCP, or PCP:
 *  - PIP: Utilizes the normal mutex provided by Zephyr (see
 *    https://docs.zephyrproject.org/latest/kernel/services/synchronization/mutexes.html)
 *  - IPCP: Locks the resource if and only if the thread's priority exceeds the system ceiling.
 *    If so, the thread's priority is immediately elevated to the resource ceiling.
 *  - PCP: Locks the resource if and only if the thread's priority exceeds the system ceiling.
 *    If it is unable to do so (i.e., due to priority ceiling blocking), elevate the priority
 *    of the thread that holds the resource associated with the system ceiling.
 */
int mutex_lock(struct resource_params *r)
{
	// k_sched_lock();

	int err = k_mutex_lock(r->mutex, K_FOREVER);
	if (err != 0) {
		return err;
	}

	// k_sched_unlock();
	return 0;
}

/*
 * TODO (Part b): Unlock the resource
 *
 * The semantic of this method depends on PIP (default), IPCP, or PCP:
 *  - PIP: Utilizes the normal mutex provided by Zephyr (see
 *    https://docs.zephyrproject.org/latest/kernel/services/synchronization/mutexes.html)
 *  - IPCP & PCP: Unlocks the mutex and resets the thread's priority accordingly.
 */
int mutex_unlock(struct resource_params *r, const struct task_params *params)
{
	// k_sched_lock();

	int err = k_mutex_unlock(r->mutex);
	if (err != 0) {
		return err;
	}

	// k_sched_unlock();
	return 0;
}

/*
 * TODO (Part c): Compute the maximum blocking time of the given task under the assumption that
 * all lower-priority tasks are scheduled.
 */
int blocking_time(const struct task_params *params)
{
	return 0;
}
