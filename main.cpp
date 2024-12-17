#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <queue>
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

void round_robin(Process p[],int process_time, int num_processes, int quantum) {
    queue<Process*> process_queue;  // Queue to hold processes
    int current_time = 0;           // Current time in the schedule
    int completed_count = 0; 
    std::vector<char> timeline(process_time, ' ');       // Track completed processes

    // Enqueue processes in order of arrival
    for (int i = 0; i < num_processes; ++i) {
        p[i].start_time = -1;  // -1 indicates the process hasn't started yet
        process_queue.push(&p[i]);
    }

    while (completed_count < num_processes) {
        if (process_queue.empty()) {
            // If no processes are ready, move time forward
            current_time++;
            for (int i = 0; i < num_processes; ++i) {
                if (!p[i].completed && p[i].arrival_time <= current_time) {
                    process_queue.push(&p[i]);
                }
            }
            
            continue;
        }

        // Get the next process from the queue
        Process* current_process = process_queue.front();
        process_queue.pop();

        // If the process hasn't started yet, set its start time
        if (current_process->start_time == -1) {
            current_process->start_time = current_time;
        }

        // Execute the process for the time quantum or remaining service time
        int time_to_run = min(quantum, current_process->service_time);
        current_process->service_time -= time_to_run;
        current_time = (current_time +time_to_run);
        // Mark the timeline with the process execution (for each time slice)
        for (int t = current_time - time_to_run; t < current_time; ++t) {
            timeline[t] = current_process->name;  // Use the first character of the process name
        }


        // If the process is completed, calculate stats
        if (current_process->service_time == 0) {

            current_process->completed = true;
            completed_count++;
            current_process->end_time = current_time -1;
            cout << "Process: " << current_process->name
                 << ", Start Time: " << current_process->start_time
                 << ", End Time: " << current_process->end_time << endl;


            // Update stats
            current_process->stats.finish_time = current_process->end_time;
            current_process->stats.turnaround = current_process->stats.finish_time - current_process->arrival_time;
            current_process->stats.normturn = current_process->stats.turnaround / (float)(current_process->end_time - current_process->start_time);
        } else {
            // If the process is not finished, put it back in the queue
            process_queue.push(current_process);
        }

        // Add newly arrived processes to the queue
        for (int i = 0; i < num_processes; ++i) {
            if (!p[i].completed && p[i].arrival_time <= current_time) {
                bool already_in_queue = false;

                // Check if the process is already in the queue
                queue<Process*> temp_queue = process_queue;
                while (!temp_queue.empty()) {
                    if (temp_queue.front() == &p[i]) {
                        already_in_queue = true;
                        break;
                    }
                    temp_queue.pop();
                }

                // Enqueue if not already in queue
                if (!already_in_queue) {
                    process_queue.push(&p[i]);
                }
            }
        }
    }
    for(int t=0;t<=process_time;t++){
        printf("%c",timeline[t]);
    }
    printf("\n");
    //print_trace(p, num_processes, process_time, "RR-" + to_string(quantum),timeline);
}

    // Print trace output (timeline)
   
    




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
        processes[i].stats.turnaround = 0.0;
        processes[i].stats.normturn = 0.0;
        processes[i].completed=false;
    }
    //if(strcmp(mode,"trace")==0){
       //print_trace(processes,num_processes,21,"RR-4");
    //}


 //spn(processes, num_processes);
 round_robin(processes,timeline,num_processes,policy.quantum);

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
