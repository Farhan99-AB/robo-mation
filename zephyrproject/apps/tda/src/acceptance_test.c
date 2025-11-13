#include <math.h>
#include <stdio.h>

#include "acceptance_test.h"

void utilization_bound_test(struct task_params **params, unsigned int task_id,
			    AcceptanceTestResult results[])
{
	double utilization = 0;
	bool accepted = false;

	/*--------------------------------------------------------------------
	 * TODO 1: Implement the Utilization Bound Test from the lecture
	 ---------------------------------------------------------------------*/

	results[task_id].accepted = accepted;
	results[task_id].info.util = utilization;

	printf("Utiliization Test (Task %d): %f (%s)\n", task_id + 1, utilization,
	       accepted ? "accepted" : "rejected");
}

void worst_case_simulation(struct task_params **params, unsigned int task_id,
			   AcceptanceTestResult results[])
{
	int32_t completion_time = 0;
	bool accepted = false;

	/*--------------------------------------------------------------------
	 * TODO 2: Implement the Worst Case Simulation from the lecture
	 ---------------------------------------------------------------------*/
	// ___________________________ END _____________________________________
	results[task_id].accepted = accepted;
	results[task_id].info.wcs_result = completion_time;

	printf("Worst Case Simulation (Task %d): %d ms (%s)\n", task_id + 1, completion_time,
	       accepted ? "accepted" : "rejected");
}

void time_demand_analysis(struct task_params **params, unsigned int task_id,
			  AcceptanceTestResult results[])
{
	int32_t t_next = 0;
	bool accepted = false;

	/*--------------------------------------------------------------------
	 * TODO 3: Implement the Time Demand Analysis from the lecture
	 ---------------------------------------------------------------------*/
	results[task_id].accepted = accepted;
	results[task_id].info.tda_result = t_next;

	printf("Time Demand Analysis (Task %d): %d ms (%s)\n", task_id + 1, t_next,
	       accepted ? "accepted" : "rejected");
}

/* Determine if params[task_id] can be scheduled.
 * - params: array of all task parameters (e.g., needed to perform TDA)
 * - task_id: index such that params[task_id] is the task under consideration
 * - results: output parameter yielding the acceptance test result */
void acceptance_test(struct task_params **params, unsigned int task_id,
		     AcceptanceTestResult results[])
{
	/*--------------------------------------------------------------------
	 * TODO 4: Call the above acceptance tests in a suitable order.
	 *  In particular, recall which of these tests are necessary,
	 *  sufficient, or both.
	 *  Ensure that the final value of result->accepted is true if and
	 *  only if the task encoded by params[task_id] can be scheduled.
	 ---------------------------------------------------------------------*/
}
