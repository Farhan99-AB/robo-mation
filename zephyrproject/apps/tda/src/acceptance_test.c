/*
	Team Members
	************
	Ganesh Patil :- 3823129
	Farhan Balekundri :- 3823048
	Varadraj Dundappa Yalamalli :- 3822751
	Esther Ürek :- 3531932
*/

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
	int not_scheduled_tasks = 0;
	for (unsigned int i = 0; i <= task_id; i++) {
		if (i == task_id || results[i].accepted == true) {
			utilization +=
				(double)params[i]->execution_time_ms / (double)params[i]->period_ms;
		} else {
			not_scheduled_tasks++;
		}
	}

	double n =
		task_id + 1 - not_scheduled_tasks; // number of tasks involved (Doubt: if there is a
						   // failed task should we consider it in n?)
	double U_bound = n * (pow(2.0, 1.0 / n) - 1);

	accepted = utilization <= U_bound;

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

	completion_time = params[task_id]->execution_time_ms;
	int32_t task_period_ms = params[task_id]->period_ms;

	for (unsigned int i = 0; i < task_id; ++i) {
		if (i == task_id || results[i].accepted == true) {
			completion_time += ceil((double)task_period_ms / params[i]->period_ms) *
					   params[i]->execution_time_ms;
		}
	}
	accepted = (completion_time <= params[task_id]->period_ms);

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
	int32_t t_prev = 0;
	t_next = params[task_id]->execution_time_ms;

	while (t_next != t_prev && t_next <= params[task_id]->period_ms) {
		t_prev = t_next;
		t_next = params[task_id]->execution_time_ms;

		for (unsigned int i = 0; i < task_id; i++) {
			if(i == task_id ||  results[i].accepted == true){
				t_next += ceil((double)t_prev / params[i]->period_ms) *
					params[i]->execution_time_ms;
			}
		}
	}

	accepted = (t_next <= params[task_id]->period_ms);

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

	// 1. UBT → fast check (sufficient)
	utilization_bound_test(params, task_id, results);
	if (!results[task_id].accepted && results[task_id].info.util > 1) {
		printf("==> Task %d CANNOT be scheduled\n", params[task_id]->task_id);
		return;
	} else if (results[task_id].accepted) {
		printf("==> Task %d is SCHEDULABLE\n", params[task_id]->task_id);
		return;
	}

	// 2. WCS → optional check
	worst_case_simulation(params, task_id, results);
	// if accepted here, we still need to perform TDA,
	// as WCS does not guarantee schedulability

	// 3. TDA → must pass (necessary & sufficient)
	time_demand_analysis(params, task_id, results);

	if (results[task_id].accepted) {
		printf("==> Task %d is SCHEDULABLE\n", params[task_id]->task_id);
	} else {
		printf("==> Task %d CANNOT be scheduled\n", params[task_id]->task_id);
	}
}
