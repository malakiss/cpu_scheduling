#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <queue>
#include <vector>
#include <iomanip>
#include <iostream> // For std::cout, std::endl
#include <algorithm> // For std::sort
#include <cmath>
#include <vector>
#include <unordered_map>

using namespace std;
#define MAX_PROCESSES 100

typedef struct {
    int finish_time;
    int turnaround;  //finish - arrival
    float normturn;  //turnaround / service
}Stats;

typedef struct {
    char name;       
    int arrival_time;    // Arrival time
    int service_time; 
    int start_time;
    int wait_time;
    int end_time;
    int remaining_time;
    Stats stats;  
    bool completed; 
    int timeline[20];  //array for 0: . 1: *  -1: nothingfor print trace function 
} Process;

typedef struct {
    int policy_id;
    int quantum=-1;
}Policy;

void printTrace(Process processes[], int num_processes,int policy_number) {
    const char* policy_name;
    switch (policy_number) {
        case 1: policy_name = "FCFS"; break;
        case 2: policy_name = "RR"; break;
        case 3: policy_name = "SPN"; break;
        case 4: policy_name = "SRT"; break;
        case 5: policy_name = "HRRN"; break;
        case 6: policy_name = "FB-1"; break;
        case 7: policy_name = "FB-2i"; break;
        case 8: policy_name = "AGING"; break;
        default: policy_name = "UNKNOWN"; break;
    }

    // Print the scheduling policy name
    printf("%s\t", policy_name);
    int total_time=20;
    for (int t = 0; t < total_time; t++) {
        printf("%2d ", t);
    }
    printf("\n");
    for (int t = 0; t < total_time; t++) {
        printf("----");
    }
    printf("\n");

    for (int i = 0; i < num_processes; i++) {
        printf("%c        | ", processes[i].name);
        for (int t = 0; t < total_time; t++) {
            if (processes[i].timeline[t] == 1)
                printf(" *|"); // Executing
            else if (processes[i].timeline[t] == 0)
                printf(" .|"); // In ready queue
            else
                printf("  |"); // Not ready
        }
        printf("\n");
    }
    printf("          ");
    for (int t = 0; t < total_time; t++) {
        printf("-- ");
    }
    printf("\n");
}

void printstats(Process processes[], int numProcesses,int policy_number) {
    // Print headers

   const char* policy_name;
    switch (policy_number) {
        case 1: policy_name = "FCFS"; break;
        case 2: policy_name = "RR"; break;
        case 3: policy_name = "SPN"; break;
        case 4: policy_name = "SRT"; break;
        case 5: policy_name = "HRRN"; break;
        case 6: policy_name = "FB-1"; break;
        case 7: policy_name = "FB-2i"; break;
        case 8: policy_name = "AGING"; break;
        default: policy_name = "UNKNOWN"; break;
    }

    // Print the scheduling policy name
    printf("%s\n", policy_name);

    printf("Process    |");
    for (int i = 0; i < numProcesses; i++) {
        printf("  %-6c  |", processes[i].name);
    }
    printf("\n");

    // Print arrival times
    printf("Arrival    |");
    for (int i = 0; i < numProcesses; i++) {
        printf("  %-6d  |", processes[i].arrival_time);
    }
    printf("\n");

    // Print service times
    printf("Service    |");  
    for (int i = 0; i < numProcesses; i++) {
        printf("  %-6d  |", processes[i].service_time);
    }
     printf("  Mean  |");
    printf("\n");

    // Print finish times
    printf("Finish     |");
    for (int i = 0; i < numProcesses; i++) {
        printf("  %-6d  |", processes[i].stats.finish_time);
    }
    printf("--------|");
    printf("\n");

    // Print turnaround times
    printf("Turnaround |");
    for (int i = 0; i < numProcesses; i++) {
        printf("  %-5.d   ", processes[i].stats.turnaround);printf("|");
    }
     float totalTurnaround = 0.0;
    for (int i = 0; i < numProcesses; i++) {
        totalTurnaround += processes[i].stats.turnaround;
    }
    float meanTurnaround = totalTurnaround / (float)numProcesses;
    
     printf("  %-4.2f  |", meanTurnaround);
    printf("\n");

    printf("NormTurn   |");//turnaround / service
    for (int i = 0; i < numProcesses; i++) {
        printf("  %-6.2f  |", processes[i].stats.normturn);
    }

      float totalNormturn = 0.0;
    for (int i = 0; i < numProcesses; i++) {
        totalNormturn += processes[i].stats.normturn;
    }
    float meanNormturn = totalNormturn/ (float)numProcesses;
    
     printf("  %-4.2f  |", meanNormturn);
    printf("\n");
}


void fcfs(Process p[], int num_processes) {
    queue<Process*> process_queue; // Queue to hold processes
    int current_time = 0;

    // Initialize timelines for all processes
    for (int i = 0; i < num_processes; ++i) {
        for (int t = 0; t < 20; t++) { // Assuming a maximum time of 100
            p[i].timeline[t] = -1; // Initially, set all timeline values to idle (-1)
        }
        process_queue.push(&p[i]);
    }

    while (!process_queue.empty()) {
        Process* current_process = process_queue.front();
        process_queue.pop();

        // CPU is idle until the process arrives
        if (current_time < current_process->arrival_time) {
            for (int t = current_time; t < current_process->arrival_time; ++t) {
                for (int i = 0; i < num_processes; ++i) {
                    if (!p[i].completed) {
                        p[i].timeline[t] = -1; // Idle state until the next process arrives
                    }
                }
            }
            current_time = current_process->arrival_time;
        }

        // Update timeline: Set "ready" (0) for other processes that arrive during execution
        for (int t = current_time; t < current_time + current_process->service_time; ++t) {
            for (int i = 0; i < num_processes; ++i) {
                if (!p[i].completed && &p[i] != current_process && p[i].arrival_time <= t) {
                    if (p[i].timeline[t] == -1) {
                        p[i].timeline[t] = 0; // Ready state
                    }
                }
            }
            // Set the current process's timeline to "executing" (1)
            current_process->timeline[t] = 1;
        }

        // Update stats of the current process
        current_process->start_time = current_time;
        current_process->end_time = current_time + current_process->service_time;
        current_process->stats.finish_time = current_process->end_time;
        current_process->stats.turnaround = current_process->stats.finish_time - current_process->arrival_time;
        current_process->stats.normturn = 
            (float)current_process->stats.turnaround / (float)current_process->service_time;

        // Mark process as completed
        current_process->completed = true;

        // Move current time forward
        current_time = current_process->end_time;
    }
  
}
// Updated Round Robin function using STL queue
void round_robin(Process processes[], int num_processes, int quantum, int timeline) {
    int remaining_processes = num_processes;
    float current_time = 0, min_arrival_time = INT_MAX, burst_remaining[num_processes], arrival_time[num_processes];
    int total_wait_time, total_turnaround_time;
    int schedule[num_processes][20];
    char execution_sequence[1000];
    int start_times[num_processes][20];
    int exec_index = 0;
    bool process_found = false;
    int process_index = -1;
    int i, j;
      for (int i = 0; i < num_processes; i++) {
        printf("%c",processes[i].name);
      }
    // Initialize stats
    for (int i = 0; i < num_processes; i++) {
        processes[i].remaining_time = processes[i].service_time;  // Set remaining time to burst time
        processes[i].completed = false;
        
        arrival_time[i] = processes[i].arrival_time;
        burst_remaining[i] = processes[i].remaining_time;
        for (j = 0; j < 20; j++) {
            start_times[i][j]=-1;
            schedule[i][j] = -1;
            processes[i].timeline[j]=-1;
        }
    }

    while (remaining_processes != 0) {
        min_arrival_time = INT_MAX;
        process_found = false;

        for (i = 0; i < num_processes; i++) {
            float adjusted_time = current_time + 0.1;
            if (arrival_time[i] <= adjusted_time && min_arrival_time > arrival_time[i] && burst_remaining[i] > 0) {
                process_index = i;
                min_arrival_time = arrival_time[i];
                process_found = true;
            }
        }
         
        for (int i = 0; i < num_processes; i++) {
        if (processes[i].completed) {
            processes[i].timeline[(int)current_time] = -1; // Process is completed
        } else if (i == process_index ) {
            processes[i].timeline[(int)current_time] = 1;  // Process is executing
        } else if (arrival_time[i] <= current_time && burst_remaining[i] > 0) {
            processes[i].timeline[(int)current_time] = 0;  // Process is waiting
        }else if (processes[i].timeline[(int)current_time - 1] == -1 && processes[i].arrival_time <= (int)current_time- 1){
            processes[i].timeline[(int)current_time-1] = 0;
        }
        else {
            processes[i].timeline[(int)current_time] = -1; // Process hasn't arrived yet
        }

    }
        for (int i = 0; i < num_processes; i++) {
            if (!processes[i].completed && processes[i].arrival_time <= (int)current_time) {
                for (int t = processes[i].arrival_time; t < (int)current_time; t++) {
                    if (processes[i].timeline[t] == -1) {
                        processes[i].timeline[t] = 0; // Ready state
                    }
                }
            }
        }
        
        if (!process_found) {
            for (int i = 0; i < num_processes; i++) {
                if (!processes[i].completed && arrival_time[i] <= current_time ) {
                    processes[i].timeline[(int)current_time] = 0; // Waiting
                }
            }
            current_time++;
            continue;
        }


        // Record execution sequence
        execution_sequence[exec_index++] = processes[process_index].name;

        // calculating start time for the process
        j = 0;
        
        while (schedule[process_index][j] != -1) {
            j++;
        }

        

        if (schedule[process_index][j] == -1) {
            schedule[process_index][j] = current_time;
            start_times[process_index][j]=current_time;
            
        }
         

        // process execution based on time quantum
        if (burst_remaining[process_index] <= quantum) {
            for (int t = (int)current_time; t < (int)current_time+burst_remaining[process_index]; t++) {
                processes[process_index].timeline[t] = 1; // Finished
            }
            current_time += burst_remaining[process_index];
            burst_remaining[process_index] = 0;
            
        
        } else {
            for (int t = (int)current_time; t < (int)current_time+quantum; t++) {
                processes[process_index].timeline[t] = 1; // Finished
            }
            current_time += quantum;
            burst_remaining[process_index] -= quantum;
        }

        if (burst_remaining[process_index] > 0) {
            if(arrival_time[process_index]<=(int)current_time){
                processes[process_index].timeline[(int)current_time] = 0;
                
            }
            arrival_time[process_index] = current_time + 0.1;
        }

        if (burst_remaining[process_index] == 0) {
            remaining_processes--;
            processes[process_index].completed=true;
            processes[process_index].end_time = current_time - 1; // Set the actual end time when the process is fully completed
            processes[process_index].wait_time = processes[process_index].end_time - processes[process_index].arrival_time - processes[process_index].service_time;
            total_wait_time += processes[process_index].wait_time;
            processes[process_index].stats.turnaround = processes[process_index].service_time + processes[process_index].wait_time;
            total_turnaround_time += processes[process_index].stats.turnaround;
            for (int t = (int)current_time; t < 20; t++) {
                processes[process_index].timeline[t] = -1; // Finished
            }
        } else {
            // If burst_remaining is not 0, process isn't done yet
            arrival_time[process_index] = current_time + 0.1; // Re-queue process for further execution
        }
    }

    // Output execution sequence
    printf("\nExecution sequence: ");
    for (int i = 0; i < exec_index; i++) {
        printf("%C ", execution_sequence[i]);
    }
    for (int i = 0; i < num_processes; ++i) {
        processes[i].start_time=start_times[i][0];
    }


    
    // Print results
    printf("\nName\tArrival\tService\tStart\tEnd\tTurnaround\tNormTurn\n");
    for (int i = 0; i < num_processes; ++i) {
        printf("%c\t%d\t%d\t%d\t%d\t%d\t%.2f\n",
               processes[i].name,
               processes[i].arrival_time,
               processes[i].service_time,
               processes[i].start_time, // First start time
               processes[i].end_time,
               processes[i].stats.turnaround,
               (float)processes[i].stats.turnaround/ processes[i].service_time);
    }
   
}


    // Print trace output (timeline)
void srt(Process processes[], int num_processes, int timeline) {
    int time = 0;
    int completed = 0;

    while (completed < num_processes && time < timeline) {
        int shortest_idx = -1;
        int shortest_time = INT_MAX;

        // Find the process with the shortest remaining time that has arrived
        for (int i = 0; i < num_processes; i++) {
            if (!processes[i].completed &&
                processes[i].arrival_time <= time &&
                processes[i].remaining_time < shortest_time &&
                processes[i].remaining_time > 0) {
                shortest_idx = i;
                shortest_time = processes[i].remaining_time;
            }
        }

        if (shortest_idx == -1) {
            // No process is ready, advance time
            printf("NOUURR");
            time++;
            continue;
        }

        Process &current = processes[shortest_idx];

        // Update start_time only the first time the process is selected
        if (current.start_time == -1) {
            current.start_time = time; // Record the time the process starts
        }

        // Update timeline
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time <= time && !processes[i].completed) {
                processes[i].timeline[time] = (i == shortest_idx) ? 1 : 0;
            }
        }

        // Execute the process for 1 unit of time
        current.remaining_time--;
        time++;

        // Check if the process is completed
        if (current.remaining_time == 0) {
            current.completed = true;
            current.end_time = time-1; // Record the end time
            current.stats.finish_time = time;
            current.stats.turnaround = current.stats.finish_time - current.arrival_time;
            current.stats.normturn = (float)current.stats.turnaround / current.service_time;
            completed++; // Increment the completed process count
        }
    }
    printf("\nSRT Scheduling Results:\n");
    printf("Name\tArrival\tService\tStart\tEnd\n");
    for (int i = 0; i < num_processes; i++) {
        printf("%c\t%d\t%d\t%d\t%d\n",
               processes[i].name,
               processes[i].arrival_time,
               processes[i].service_time,
               processes[i].start_time,
               processes[i].end_time);
    }
}
 

void fb_2i(Process processes[], int num_processes) {
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq; // Pair of priority level and process index
    unordered_map<int, int> remainingServiceTime; // Map from process index to remaining service time
    int j = 0;

    // Initialize timeline to -1 for all processes
    for (int i = 0; i < num_processes; i++) {
        for (int t = 0; t < 20; t++) {
            processes[i].timeline[t] = -1;
        }
    }

    int current_time = 0;

    // Add the first process if it arrives at time 0
    while (j < num_processes && processes[j].arrival_time <= current_time) {
        pq.push(make_pair(0, j));
        remainingServiceTime[j] = processes[j].service_time;
        processes[j].timeline[current_time] = 0; // In ready queue
        j++;
    }

    // Main scheduling loop
    while (!pq.empty() || j < num_processes) {
        if (!pq.empty()) {
            int priorityLevel = pq.top().first;
            int processIndex = pq.top().second;
            pq.pop();

            int currentQuantum = pow(2, priorityLevel);
            int temp = current_time;

            // Execute the current process within its quantum
            while (currentQuantum > 0 && remainingServiceTime[processIndex] > 0) {
                // Mark the current process as executing
                processes[processIndex].timeline[temp] = 1;
                remainingServiceTime[processIndex]--;
                currentQuantum--;
                temp++;

                // Add newly arrived processes during execution
                while (j < num_processes && processes[j].arrival_time <= temp) {
                    pq.push(make_pair(0, j));
                    remainingServiceTime[j] = processes[j].service_time;
                    processes[j].timeline[temp] = 0; // Mark as ready
                    j++;
                }

                // Mark other ready processes in the timeline
                for (int i = 0; i < num_processes; i++) {
                    if (!processes[i].completed && processes[i].timeline[temp - 1] == -1 && processes[i].arrival_time <= temp - 1) {
                        processes[i].timeline[temp - 1] = 0; // In ready queue
                    }
                }
            }

            // Check if the process is completed
            if (remainingServiceTime[processIndex] == 0) {
                processes[processIndex].end_time = temp; // Set finish time
                processes[processIndex].completed = true;
                 // Update process statistics
                processes[processIndex].stats.finish_time = processes[processIndex].end_time;
                processes[processIndex].stats.turnaround = processes[processIndex].stats.finish_time - processes[processIndex].arrival_time;
                processes[processIndex].stats.normturn = (float)processes[processIndex].stats.turnaround / (float)processes[processIndex].service_time;
           
            } else {
                // Increment priority level and reinsert into the queue
                pq.push(make_pair(priorityLevel + 1, processIndex));
            }

            current_time = temp; // Update current time
        }

        // Add any newly arrived processes
        while (j < num_processes && processes[j].arrival_time <= current_time) {
            pq.push(make_pair(0, j));
            remainingServiceTime[j] = processes[j].service_time;
            processes[j].timeline[current_time] = 0; // In ready queue
            j++;
        }

        // If no processes are ready, move the time forward
        if (pq.empty() && j < num_processes) {
            current_time = processes[j].arrival_time;
        }
    }

    
}

//beta3t nour
void feedback_scheduling(Process processes[], int num_processes, int timeline, const char *mode) {
    vector<queue<Process>> mlfq(10); // Multi-level Feedback Queue with 10 levels
    vector<Process> completed_processes; // To store completed processes
    int current_time = 0;

    // Push all processes into the highest priority queue at their arrival time
    for (int i = 0; i < num_processes; ++i) {
        processes[i].start_time = -1; // Initialize start time
        processes[i].end_time = -1;  // Initialize end time
    }

    while (current_time < timeline || !completed_processes.empty()) {
        // Push newly arrived processes to the highest priority queue
          bool all_queues_empty = true;
    for (const auto &q : mlfq) {
        if (!q.empty()) {
            all_queues_empty = false;
            break;
        }
    }
    if (all_queues_empty && completed_processes.size() == num_processes) {
        break; // Exit if all processes are completed
    }
        for (int i = 0; i < num_processes; ++i) {
            if (processes[i].arrival_time == current_time && !processes[i].completed) {
                mlfq[0].push(processes[i]);
            }
        }

        bool processed = false;

        // Iterate over queues and process tasks
        for (size_t i = 0; i < mlfq.size(); ++i) {
            if (!mlfq[i].empty()) {
                Process current = mlfq[i].front();
                mlfq[i].pop();

                if (current.start_time == -1) {
                    current.start_time = current_time;
                }

                // Process for one quantum (q=1)
                current.remaining_time -= 1;
                current_time++;
                processed = true;

                // Trace mode output
                if (strcmp(mode, "trace") == 0) {
                    cout << "Time " << current_time << ": Process " << current.name
                         << " executed (queue " << i + 1 << ")\n";
                }

                // Check if process is completed
                if (current.remaining_time == 0) {
                    current.completed = true;
                    current.end_time = current_time;
                    current.stats.turnaround = current.end_time - current.arrival_time;
                    current.stats.normturn = current.stats.turnaround / (float)current.service_time;
                    completed_processes.push_back(current);
                } else {
                    // Push to next lower priority queue
                    if (i + 1 < mlfq.size()) {
                        mlfq[i + 1].push(current);
                    } else {
                        mlfq[i].push(current); // Stay in the same queue if no lower queue exists
                    }
                }

                break; // Process only one task per time step
            }
        }

        if (!processed) {
            current_time++; // Increment time if no task is processed
        }
    }

    // Stats mode output
    if (strcmp(mode, "stats") == 0) {
        cout << "Name\tArrival\tService\tStart\tEnd\tTurnaround\tNormTurn\n";
        for (const auto &process : completed_processes) {
            cout << process.name << "\t" << process.arrival_time << "\t" << process.service_time << "\t"
                 << process.start_time << "\t" << process.end_time << "\t"
                 << fixed << setprecision(2) << process.stats.turnaround << "\t\t"
                 << process.stats.normturn << "\n";
        }
    }
}




void spn(Process p[], int num_processes) {
    int current_time = 0;
    int completed_count = 0;

    // Initialize all timelines to -1 (nothing happening)
    for (int i = 0; i < num_processes; i++) {
        for (int t = 0; t < 20; t++) { // Assuming max time as 100
            p[i].timeline[t] = -1;
        }
    }

    while (completed_count < num_processes) {
        // Create a vector to store ready processes
        vector<Process*> ready_queue;

        // Collect processes that have arrived but are not yet completed
        for (int i = 0; i < num_processes; i++) {
            if (!p[i].completed && p[i].arrival_time <= current_time) {
                ready_queue.push_back(&p[i]);
            }
        }
        for (int i = 0; i < num_processes; i++) {
            if (!p[i].completed && p[i].arrival_time <= current_time) {
                for (int t = p[i].arrival_time; t < current_time; t++) {
                    if (p[i].timeline[t] == -1) {
                        p[i].timeline[t] = 0; // Ready state
                    }
                }
            }
        }

        // Sort the ready queue based on service time (shortest first)
        if (!ready_queue.empty()) {
            sort(ready_queue.begin(), ready_queue.end(),
                 [](Process* a, Process* b) { return a->service_time < b->service_time; });

            // Select the process with the shortest service time
            Process* current_process = ready_queue.front();

            // Mark other ready processes in the timeline as "ready" (0)
            for (int i = 0; i < num_processes; i++) {
                if (!p[i].completed && p[i].arrival_time <= current_time && &p[i] != current_process) {
                    p[i].timeline[current_time] = 0;
                }
            }

            // Execute the current process
            current_process->start_time = current_time;
            for (int t = current_time; t < current_time + current_process->service_time; t++) {
                current_process->timeline[t] = 1; // Executing
            }
            current_time += current_process->service_time;
            current_process->end_time = current_time - 1;
            current_process->completed = true;
            completed_count++;

            // Update process stats
            current_process->stats.finish_time = current_process->end_time+1;
            current_process->stats.turnaround = current_process->stats.finish_time - current_process->arrival_time;
            current_process->stats.normturn = current_process->stats.turnaround / (float)current_process->service_time;

        } else {
            // If no process is ready, move time forward
            for (int i = 0; i < num_processes; i++) {
                if (!p[i].completed && p[i].arrival_time > current_time) {
                    p[i].timeline[current_time] = -1; // Idle time
                }
            }
            current_time++;
        }
    }

    // Print the trace after scheduling is done
    //printTrace(p, num_processes, current_time);
}
void hrrn(Process processes[], int numProcesses) {
    int current_time = 0;
    int completed = 0;

    // Initialize timelines for all processes
    for (int i = 0; i < numProcesses; ++i) {
        for (int t = 0; t < 20; t++) { // Assuming a maximum time of 100
            processes[i].timeline[t] = -1; // Initially, set all timeline values to idle (-1)
        }
    }

    while (completed < numProcesses) {
        int selected_index = -1;
        float max_response_ratio = -1.0;

        // Calculate the response ratio for each process that has arrived
        for (int i = 0; i < numProcesses; i++) {
            if (processes[i].arrival_time <= current_time && !processes[i].completed) {
                int waiting_time = current_time - processes[i].arrival_time;
                float response_ratio = (float)(waiting_time + processes[i].service_time) / processes[i].service_time;

                // Select the process with the highest response ratio
                if (response_ratio > max_response_ratio) {
                    max_response_ratio = response_ratio;
                    selected_index = i;
                }
            }
        }

        // If no process is available to run, increment the current time
        if (selected_index == -1) {
            for (int i = 0; i < numProcesses; ++i) {
                if (!processes[i].completed && processes[i].arrival_time <= current_time) {
                    processes[i].timeline[current_time] = 0; // Mark as ready
                }
            }
            current_time++;
        } else {
            // Start executing the selected process
            Process* current_process = &processes[selected_index];
            current_process->start_time = current_time;

            for (int t = current_time; t < current_time + current_process->service_time; ++t) {
                // Set executing process to 1
                current_process->timeline[t] = 1;

                // Set other ready processes to 0
                for (int i = 0; i < numProcesses; ++i) {
                    if (!processes[i].completed && &processes[i] != current_process && processes[i].arrival_time <= t) {
                        if (processes[i].timeline[t] == -1) {
                            processes[i].timeline[t] = 0; // Ready state
                        }
                    }
                }
            }

            current_process->end_time = current_time + current_process->service_time;
            current_process->completed = true;

            // Update stats for the current process
            current_process->stats.finish_time = current_process->end_time;
            current_process->stats.turnaround = current_process->stats.finish_time - current_process->arrival_time;
            current_process->stats.normturn =
                (float)current_process->stats.turnaround / (float)current_process->service_time;

            current_time = current_process->end_time;
            completed++;
        }
    }

    // Optionally call a function to print the final trace and stats
 // printTrace(processes, numProcesses, current_time);
}

void executePolicy(int policy_id, int quantum, Process processes[], int num_processes,int timeline) {
    switch (policy_id) {
        case 1:
            printf("FCFS:\n");
            fcfs(processes, num_processes);
            break;
        case 2:
            printf("RR-%d:\n", quantum);
            round_robin(processes, num_processes, quantum, timeline);
            break;
        case 3:
            printf("SPN:\n");
            spn(processes, num_processes);
            break;
        case 4:
            printf("SRT:\n");
            srt(processes, num_processes,timeline);
            break;
        case 5:
            printf("HRRN:\n");
            hrrn(processes, num_processes);
            break;
        case 6:
            printf("FB-1:\n");
           // feedback_scheduling(processes, num_processes);
            break;
        case 7:
            printf("FB-2i:\n");
            fb_2i(processes, num_processes);
            break;
        default:
            printf("Invalid policy ID: %d\n", policy_id);
            break;
    }
}

int main() {
    char mode[10]; // "trace" or "stats"
    char input[50]; // Line 2: list of policies
    int timeline, num_processes;
    Process processes[MAX_PROCESSES];

    // Input: Mode
    scanf("%s", mode);

    // Input: Policies
    scanf("%s", input);

    // Input: Timeline
    scanf("%d", &timeline);

    // Input: Number of processes
    scanf("%d", &num_processes);

    // Input: Process data
    for (int i = 0; i < num_processes; i++) {
        scanf(" %c,%d,%d", &processes[i].name, &processes[i].arrival_time, &processes[i].service_time);
        processes[i].completed = 0;
        processes[i].end_time = 0;
        processes[i].start_time = -1;
        processes[i].wait_time=0;
        processes[i].remaining_time=processes[i].service_time;

        for (int j = 0; j < timeline; j++) {
            processes[i].timeline[j] = -1;
        }
    }

    // Parse multiple policies
    char* policy_token = strtok(input, ",");
    while (policy_token != NULL) {
        int policy_id, quantum = -1;
        if (strchr(policy_token, '-') != NULL) {
            sscanf(policy_token, "%d-%d", &policy_id, &quantum);
        } else {
            policy_id = atoi(policy_token);
        }

        // Clone processes for the current algorithm
        Process cloned_processes[MAX_PROCESSES];
        memcpy(cloned_processes, processes, sizeof(Process) * num_processes);

        // Execute the policy
        executePolicy(policy_id, quantum, cloned_processes, num_processes,timeline);
        if (strcmp(mode,"trace")==0){
        printTrace(cloned_processes,num_processes,policy_id);
        }else printstats(cloned_processes,num_processes,policy_id);
        // Move to the next policy
        policy_token = strtok(NULL, ",");
    }

    return 0;
}

/*int main() {

   char mode[10]; //trace or stats
    Policy policy;
    Process processes[MAX_PROCESSES];
   
    char input[50]; //line 2
    int num_processes=0;
    int timeline;
    scanf("%s", mode);
    scanf("%s", input);

     if (strchr(input, '-') != NULL) { // Case: input contains a dash (2-4)
        sscanf(input, "%d-%d", &policy.policy_id, &policy.quantum);
    } else { // Case: input without a dash 
        policy.policy_id = atoi(input);
        policy.quantum = -1; // No quantum provided
    }
     
     
    scanf("%d",&timeline);
 
    scanf("%d",&num_processes);
     

     printf("PolicyNumber: %d\n", policy.policy_id);
    if (policy.quantum != -1) {
        printf("Quantum: %d\n", policy.quantum);
    } else {
        printf("Quantum: Not applicable\n");
    }
getchar(); 

    for (int i = 0; i < num_processes; i++) {
        char input[50]; // Buffer for each line of input
         
        if (fgets(input, sizeof(input), stdin) == NULL) {
            fprintf(stderr, "Error: Unable to read process input.\n");
              return 1;
        }
 

        if (sscanf(input, "%c,%d,%d", 
                   &processes[i].name, 
                   &processes[i].arrival_time, 
                   &processes[i].service_time) != 3) {
            fprintf(stderr, "Error: Invalid process input format.\n");
            return 1;
        }

        // Initialize stats to zero
        processes[i].remaining_time = processes[i].service_time;
        processes[i].stats.turnaround = 0.0;
        processes[i].stats.normturn = 0.0;
        processes[i].completed=false;
        for (int i = 0; i < 20; i++) {
                processes[i].timeline[i]=-1;
}
    }

 //spn(processes, num_processes);
 //fcfs(processes, num_processes);
  // hrrn(processes,num_processes);
 fb_2i(processes,num_processes);
printstats(processes, num_processes,policy.policy_id);
printTrace(processes,num_processes,policy.policy_id);


 //round_robin(processes,num_processes,policy.quantum,timeline);
// feedback_scheduling( processes, num_processes, timeline,mode);
 
//srt(processes, num_processes, timeline);

    return 0;
}
*/