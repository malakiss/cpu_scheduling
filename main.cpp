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
using namespace std;
#define MAX_PROCESSES 100

typedef struct {
    int finish_time;
    float turnaround;  //finish - arrival
    float normturn;  //turnaround / service
}Stats;

typedef struct {
    char name;       
    int arrival_time;    // Arrival time
    int service_time; 
    int start_time;
    int end_time;
    int remaining_time;
    Stats stats;  
    bool completed;   
} Process;

typedef struct {
    int policy_id;
    int quantum=-1;
}Policy;
void fcfs(Process p[], int num_processes) {
    queue<Process*> process_queue;  //queue to hold processes
    int current_time = 0;
    for (int i = 0; i < num_processes; ++i) {
        process_queue.push(&p[i]);
    }

    while (!process_queue.empty()) {
        Process* current_process = process_queue.front();
        process_queue.pop();
        //CPU is idle until process arrives
        if (current_time <= current_process->arrival_time) {
            current_time = current_process->arrival_time;
        }

        
        current_process->start_time = current_time;
        current_process->end_time = current_time + current_process->service_time-1;

        //update stats pf process
        current_process->stats.finish_time = current_process->end_time;
        current_process->stats.turnaround = current_process->stats.finish_time - current_process->arrival_time;
        current_process->stats.normturn = current_process->stats.turnaround / (float)current_process->service_time;

        //move current time forward (as output)
        current_time = current_process->end_time+1;
    }

}
void print_trace(Process p[], int num_processes, int timeline_length, const string& method, const vector<char>& timeline) {
    // Print header for the timeline
    cout << method << "  ";
    for (int t = 0; t < 21; ++t) {
        cout << t % 10 << " ";  // Time units
    }
    cout << endl;
    cout << "------------------------------------------------" << endl;

    // Print the timeline for each process
    for (int i = 0; i < num_processes; ++i) {
        cout << p[i].name << "    |";
        for (int t = 0; t < timeline_length; ++t) {
            // Mark the process as executing at the respective time slice
            if (timeline[t] == p[i].name) {
                cout << "*|";  // Execution time
            } else if (p[i].arrival_time <= t && t< p[i].start_time  ) {
                cout << ".|";  // Waiting time, process is in the queue but not executed yet
            } else {
                cout << " |";  // Idle time
            }
        }
        cout << endl;
    }
    cout << "------------------------------------------------" << endl;
}



// Updated Round Robin function using STL queue
void round_robin(Process processes[], int num_processes, int quantum, int timeline) {
    int time = 0;
    int completed_processes = 0;
    std::queue<int> ready_queue;

    // Initialize process attributes
    for (int i = 0; i < num_processes; i++) {
        processes[i].remaining_time = processes[i].service_time;
        processes[i].completed = false;
    }

    

    while (completed_processes < num_processes && time < timeline) {
        // Add newly arrived processes to the queue
        for (int i = 0; i < num_processes; i++) {
            if (processes[i].arrival_time == time && processes[i].remaining_time > 0 && !processes[i].completed) {
                ready_queue.push(i);
            }
        }

        if (ready_queue.empty()) {
            //printf("%d\t-\n", time);
            time++;
            continue;
        }

        int current = ready_queue.front();
        ready_queue.pop();

        if (processes[current].remaining_time == processes[current].service_time) {
            processes[current].start_time = time;
        }

        int execution_time = (processes[current].remaining_time < quantum) ? processes[current].remaining_time : quantum;

        for (int t = 0; t < execution_time; t++) {
            printf("%c", processes[current].name);
        }

        processes[current].remaining_time -= execution_time;

        if (processes[current].remaining_time == 0) {
            processes[current].completed = true;
            processes[current].end_time = time;
            //calculate_stats(&processes[current], time);
            completed_processes++;
        } else {
            ready_queue.push(current); // Re-enqueue the process if not completed
        }
    }

    printf("\nRound Robin Scheduling Results:\n");
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

    while (completed_count < num_processes) {
        // Create a vector to store ready processes
        vector<Process*> ready_queue;

        // Collect processes that have arrived but are not yet completed
        for (int i = 0; i < num_processes; i++) {
            if (!p[i].completed && p[i].arrival_time <= current_time) {
                ready_queue.push_back(&p[i]);
            }
        }

        // Sort the ready queue based on service time (shortest first)
        if (!ready_queue.empty()) {
            sort(ready_queue.begin(), ready_queue.end(),
                 [](Process* a, Process* b) { return a->service_time < b->service_time; });

            // Select the process with the shortest service time
            Process* current_process = ready_queue.front();
            current_process->start_time = current_time;
            current_time += current_process->service_time;
            current_process->end_time = current_time-1;
            current_process->completed = true;

            completed_count++;
            cout << "Process: " << current_process->name
                 << ", Start Time: " << current_process->start_time
                 << ", End Time: " << current_process->end_time << endl;

             //update stats pf process
        current_process->stats.finish_time = current_process->end_time;
        current_process->stats.turnaround = current_process->stats.finish_time - current_process->arrival_time;
        current_process->stats.normturn = current_process->stats.turnaround / (float)current_process->service_time;

      
                 
        } else {
            // If no process is ready, move time forward
            current_time++;
        }
    }
   // print_trace(p, num_processes, 21, "SPN");
}


int main() {

  /*  char inputt[50];  // Buffer for each line of input
    while (fgets(inputt, sizeof(inputt), stdin) != NULL) {
        printf("Input read: '%s'\n", inputt);  // Check what is read from stdin
    }
    return 0;*/
    
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
    }
    //if(strcmp(mode,"trace")==0){
       //print_trace(processes,num_processes,21,"RR-4");
    //}


 //spn(processes, num_processes);
 //round_robin(processes,num_processes,policy.quantum,timeline);
 feedback_scheduling( processes, num_processes, timeline,mode);
 
//srt(processes, num_processes, timeline);

// TEST FCFS
  /*     cout << "FCFS Scheduling:" << endl;
    fcfs(processes, num_processes);
    cout << "Name\tArrival\tService\tStart\tEnd\tTurnaround\tNormTurn\n";
    for (int i = 0; i < num_processes; ++i) {
        cout << processes[i].name << "\t"
             << processes[i].arrival_time << "\t"
             << processes[i].service_time << "\t"
             << processes[i].start_time << "\t"
             << processes[i].end_time << "\t"
             << fixed << setprecision(2) << processes[i].stats.turnaround << "\t\t"
             << processes[i].stats.normturn << "\n";
    }

*/

  /*  printf("Processes Entered:\n");
    printf("Name\tArrival\tService\n");
    for (int i = 0; i < num_processes; i++) {
        printf("%c\t%d\t%d\n", processes[i].name, 
               processes[i].arrival_time, 
               processes[i].service_time);
    }
*/

    return 0;
}
