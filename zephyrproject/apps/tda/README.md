Overview
********

Team Members
************
Ganesh Patil :- 3823129
Farhan Balekundri :- 3823048
Varadraj Dundappa Yalamalli :- 3822751
Esther Ürek :- 3531932

Code Description
****************
1. Utilization Bound Test (UBT) – A sufficient test that checks whether total CPU utilization stays below the Liu & Layland bound. Fast but conservative.

2. Worst-Case Simulation (WCS) – An approximation test that is neither necessary nor sufficient; it estimates worst-case workload but cannot guarantee correctness.

3. Time-Demand Analysis (TDA) – A necessary and sufficient test that iteratively checks if worst-case interference fits within the task’s deadline.

4. Overall Flow
   1. Run UBT. If it passes, the task set is schedulable stop.  
   2. If UBT fails, run WCS (approximation only; never return based on this).  
   3. Finally, run TDA. Its result gives the final schedulability decision.

***Calculated Values***
UBT:-
Task 1: 0.5
Task 2: 0.779
Task 3: 0.91

WCS:-
Task 1: 2
Task 2: 6
Task 3: 9

TDA:-
Task 1: 2
Task 2: 4
Task 3: 7


***Observed Values***
UBT:-
Task 1: 0.500
Task 2: 0.786
Task 3: 0.911

WCS:-
Task 1: 0000
Task 2: 0000
Task 3: 9000

TDA:-
Task 1: 0000
Task 2: 0000
Task 3: 7000

*Note: Since UBT passed WCS and TDA values were not calculated for Tasks 1 and 2.*

Please consult the assignment sheet for the implementation requirements.

You can build and flash the application via
```bash
$ west build -p always -b esp32_devkitc/esp32/procpu --sysbuild apps/02_tda
$ west flash
$ west espressif monitor
```
