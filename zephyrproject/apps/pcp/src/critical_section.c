/*
	Team Members
	************
	Ganesh Patil :- 3823129
	Farhan Balekundri :- 3823048
	Varadraj Dundappa Yalamalli :- 3822751
	Esther Ürek :- 3531932
*/

#include "critical_section.h"
#include "tasks.h"
#include <assert.h>

static int 	system_ceiling = INT_MAX;

/*
 * TODO (Part b): Initialize resource priority ceilings and system ceiling
 */
void initialize_priority_ceilings()
{
	/* Step 1: initialize all resource ceilings to lowest priority */
	STRUCT_SECTION_FOREACH(resource_params, r) {
		r->priority_ceiling = INT_MAX;
	}

	/* Step 2: compute ceilings from critical sections */
	STRUCT_SECTION_FOREACH(critical_section_params, cs) {
		int task_prio = cs->task->priority;
		struct resource_params *r = cs->resource;

		if (task_prio < r->priority_ceiling) {
			r->priority_ceiling = task_prio;
		}
	}

	/* Step 3: initialize system ceiling (no resource locked) */
	system_ceiling = INT_MAX;

	/* Optional debug output */
	printf("Initialized resource priority ceilings:\n");
	STRUCT_SECTION_FOREACH(resource_params, r) {
		printf(" R%d ceiling = %d\n", r->id, r->priority_ceiling);
	}
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
    struct k_thread *current = k_current_get();
    
    // 1. LOCK SCHEDULER: Critical for IPCP.
    // This prevents a context switch between priority elevation and mutex acquisition.
    k_sched_lock();

    // 2. IMMEDIATE PROMOTION (The "I" in IPCP)
    // We rise to the ceiling BEFORE taking the lock.
    if (k_thread_priority_get(current) > r->priority_ceiling) {
        k_thread_priority_set(current, r->priority_ceiling);
    }

    // 3. UPDATE SYSTEM CEILING
    if (r->priority_ceiling < system_ceiling) {
        system_ceiling = r->priority_ceiling;
    }

    k_sched_unlock();

    // 4. ACQUIRE MUTEX
    // Since we are already at the ceiling, no higher-priority task 
    // can preempt us to create a deadlock cycle.
    return k_mutex_lock(r->mutex, K_FOREVER);
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
    struct k_thread *current = k_current_get();

    /* 1. Release the physical Zephyr mutex */
    int err = k_mutex_unlock(r->mutex);
    if (err != 0) {
        return err;
    }

    /* 2. Initialize variables for recalculation */
    int new_system_ceiling = INT_MAX;
    int new_prio = params->priority; // Start with the task's base priority

    /* 3. Single pass to recompute both system_ceiling and thread priority */
    STRUCT_SECTION_FOREACH(resource_params, res) {
        
        /* Check if the resource is locked by ANYONE (for system ceiling) */
        if (res->mutex->lock_count > 0) {
            if (res->priority_ceiling < new_system_ceiling) {
                new_system_ceiling = res->priority_ceiling;
            }

            /* Check if THIS thread still holds this resource (for nesting) */
            if (res->mutex->owner == current) {
                if (res->priority_ceiling < new_prio) {
                    new_prio = res->priority_ceiling;
                }
            }
        }
    }

    /* 4. Update the global system state and the thread's priority */
    system_ceiling = new_system_ceiling;
    k_thread_priority_set(current, new_prio);

    return 0;
}

/*
 * TODO (Part c): Compute the maximum blocking time of the given task under the assumption that
 * all lower-priority tasks are scheduled.
 */
int blocking_time(const struct task_params *params)
{
	int block_time = 0;

	STRUCT_SECTION_FOREACH(critical_section_params, cs) {

		/* only lower-priority tasks */
		if (cs->task->priority > params->priority) {

			struct resource_params *r = cs->resource;

			/* resource can block this task */
			if (r->priority_ceiling <= params->priority) {

				int cs_len = cs->release_time - cs->access_time;

				if (cs_len > block_time) {
					block_time = cs_len;
				}
			}
		}
	}

	return block_time;
}
