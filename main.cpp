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
const int TIMELINE_SIZE = 1000; 

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

   
    for (int i = 0; i < num_processes; ++i) {
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
    // Print trace output (timeline)
void srt(Process processes[], int num_processes, int timeline) {
    int time = 0;
    int completed_processes = 0;
    std::vector<char> timeline_(timeline, ' ');
    int t=0;
    

    while (completed_processes < num_processes && time < timeline) {
        int shortest = -1;
        int min_remaining_time = INT_MAX;

        for (int i = 0; i < num_processes; i++) {
            if (!processes[i].completed && processes[i].arrival_time <= time && processes[i].remaining_time < min_remaining_time) {
                shortest = i;
                min_remaining_time = processes[i].remaining_time;
            }
        }

        if (shortest == -1) {
            time++;
            continue;
        }

        // Process the shortest remaining time process
        if (processes[shortest].remaining_time == processes[shortest].service_time) {
            processes[shortest].start_time = time;
        }
         //printf("%c",  processes[shortest].name);
         timeline_[t] = processes[shortest].name;
         t++;


        processes[shortest].remaining_time--;
        time++;

        // Check if the process is completed
        if (processes[shortest].remaining_time == 0) {
            processes[shortest].completed = true;
            processes[shortest].end_time = time-1;
            //calculate_stats(&processes[shortest], time);
            completed_processes++;
        }
    }
     for(int k=0;k<=timeline;k++){
        printf("%c",timeline_[k]);
    }
    printf("\n");
    
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
void systemCheck(Process p[], int num_processes, int currentTime, std::vector<std::queue<int>>& queues) {
    for (int i = 0; i < num_processes; i++) {
        if (!p[i].completed && p[i].arrival_time == currentTime&& p[i].remaining_time==p[i].service_time) {
            queues[0].push(i);
          }
    }
}
int executeProcess(Process* p, int quantum, int currentTime) {
    int actualExecution = std::min(quantum, p->remaining_time);
    for (int i = 0; i < actualExecution; i++) {
        p->timeline[currentTime + i] = 1; // Mark as executing
    }
    p->remaining_time -= actualExecution; 
     //--//TODO

    if (p->remaining_time <= 0) {
        p->completed = true;
        p->end_time = currentTime + actualExecution-1;
        p->stats.finish_time=p->end_time+1;
    }
   // currentTime=currentTime;
    return currentTime+actualExecution;
}
int checkCompleteness(std::queue<int>& processQueue, Process p[], std::queue<int>& tempQueue) {
    while (!processQueue.empty()) {
        int front_index = processQueue.front();
        processQueue.pop();

        if (!p[front_index].completed) {
            return front_index; // Return the index of the valid process
        } else {
            // Skip completed processes and store them temporarily
            tempQueue.push(front_index);
        }
    }

    // Restore skipped processes back into the queue
    while (!tempQueue.empty()) {
    int idx = tempQueue.front();
    tempQueue.pop();
    if (!p[idx].completed) {
        processQueue.push(idx); // Restore only incomplete processes
    }
}

    return -1; // No valid process found
}
void fb_2i(Process p[], int num_processes) {
    const int max_queues = num_processes;
    std::vector<std::queue<int>> queues(max_queues);
    int currentTime = 0;
    int currentTimeAfterEx=currentTime;

    // Initialize timeline and push initial processes
    for (int i = 0; i < num_processes; i++) {
         if (p[i].arrival_time == 0) {
            queues[0].push(i);
        }
    }

    while (true) {
        bool allQueuesEmpty = true;
         // Check arrivals at the current time
        systemCheck(p, num_processes, currentTime, queues);

        for (int j = 0; j < max_queues; j++) {

        
            if (!queues[j].empty()) {
                // Check arrivals at the current time
                if(currentTimeAfterEx!=currentTime)
        systemCheck(p, num_processes, currentTimeAfterEx, queues);

                allQueuesEmpty = false;
                 // Check for newly arrived processes in previous queues
                for(int i=0;i<j;i++){
                   if(!queues[i].empty() ){
                      j=i;
                    }
                 }
              
                std::queue<int> tempQueue; // Temporary queue for skipped processes
                int process_index = checkCompleteness(queues[j], p, tempQueue);

                // If no valid process is found, continue to the next queue
                if (process_index == -1) {
                    continue;
                }
                Process* currentProcess = &p[process_index];
                int quantum = std::pow(2, j);

                currentTimeAfterEx= executeProcess(currentProcess, quantum, currentTimeAfterEx);
                if (currentProcess->completed) {
                } else if (!currentProcess->completed) {
                    if (j + 1 < max_queues) {
                        queues[j + 1].push(process_index);
                    } else {
                        queues[j].push(process_index);
                    }
                }
                currentTime++;
                 break; // Exit after handling one process to simulate timeline iteration
            }
        }

        if (allQueuesEmpty) {
            bool anyProcessIncomplete = false;
            for (int i = 0; i < num_processes; i++) {
                if (!p[i].completed) {
                    anyProcessIncomplete = true;
                    break;
                }
            }
            if (!anyProcessIncomplete) {
                break;
            }
        }
    }

    // Fill in wait times and mark idle times
    for (int i = 0; i < num_processes; i++) {
        int arrivalTime = p[i].arrival_time;
        for (int k = arrivalTime; k < p[i].end_time; k++) {
            if (p[i].timeline[k] != 1) {
                p[i].timeline[k] = 0;
            }
        }
        p[i].wait_time = p[i].end_time - p[i].arrival_time - p[i].service_time;
        p[i].stats.turnaround = p[i].stats.finish_time- p[i].arrival_time;
        p[i].stats.normturn = static_cast<float>(p[i].stats.turnaround) / p[i].service_time;
    }
}

void fb_1(Process p[], int num_processes) {
    const int max_queues = num_processes;
    std::vector<std::queue<int>> queues(max_queues);
    int currentTime = 0;
    int currentTimeAfterEx=currentTime;

    // Initialize timeline and push initial processes
    for (int i = 0; i < num_processes; i++) {
         if (p[i].arrival_time == 0) {
            queues[0].push(i);
        }
    }

    while (true) {
        bool allQueuesEmpty = true;
         // Check arrivals at the current time
        systemCheck(p, num_processes, currentTime, queues);

        for (int j = 0; j < max_queues; j++) {

        
            if (!queues[j].empty()) {
                // Check arrivals at the current time
                if(currentTimeAfterEx!=currentTime)
        systemCheck(p, num_processes, currentTimeAfterEx, queues);

                allQueuesEmpty = false;
                 // Check for newly arrived processes in previous queues
                for(int i=0;i<j;i++){
                   if(!queues[i].empty() ){
                      j=i;
                    }
                 }
              
                std::queue<int> tempQueue; // Temporary queue for skipped processes
                int process_index = checkCompleteness(queues[j], p, tempQueue);

                // If no valid process is found, continue to the next queue
                if (process_index == -1) {
                    continue;
                }
                Process* currentProcess = &p[process_index];
                int quantum = 1;

                currentTimeAfterEx= executeProcess(currentProcess, quantum, currentTimeAfterEx);
                if (currentProcess->completed) {
                  } else if (!currentProcess->completed) {
                    if (j + 1 < max_queues) {
                        queues[j + 1].push(process_index);
                    } else {
                        queues[j].push(process_index);
                    }
                }
         
                currentTime++;
                 break; // Exit after handling one process to simulate timeline iteration
            }
        }

        if (allQueuesEmpty) {
            bool anyProcessIncomplete = false;
            for (int i = 0; i < num_processes; i++) {
                if (!p[i].completed) {
                    anyProcessIncomplete = true;
                    break;
                }
            }
            if (!anyProcessIncomplete) {
                break;
            }
        }
    }

    // Fill in wait times and mark idle times
    for (int i = 0; i < num_processes; i++) {
        int arrivalTime = p[i].arrival_time;
        for (int k = arrivalTime; k < p[i].end_time; k++) {
            if (p[i].timeline[k] != 1) {
                p[i].timeline[k] = 0;
            }
        }
        p[i].wait_time = p[i].end_time - p[i].arrival_time - p[i].service_time;
        p[i].stats.turnaround = p[i].stats.finish_time- p[i].arrival_time;
        p[i].stats.normturn = static_cast<float>(p[i].stats.turnaround) / p[i].service_time;
    }
}

void spn(Process p[], int num_processes) {
    int current_time = 0;
    int completed_count = 0;


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

void executePolicy(int policy_id, int quantum, Process processes[], int num_processes) {
    switch (policy_id) {
        case 1:
            printf("FCFS:\n");
            fcfs(processes, num_processes);
            break;
        case 2:
            printf("RR-%d:\n", quantum);
           // rr(processes, num_processes, quantum, timeline);
            break;
        case 3:
            printf("SPN:\n");
            spn(processes, num_processes);
            break;
        case 4:
            printf("SRT:\n");
            //srt(processes, num_processes);
            break;
        case 5:
            printf("HRRN:\n");
            hrrn(processes, num_processes);
            break;
        case 6:
            printf("FB-1:\n");
            fb_1(processes, num_processes);
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
        processes[i].completed = false;
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
        executePolicy(policy_id, quantum, cloned_processes, num_processes);
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