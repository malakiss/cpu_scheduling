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

 spn(processes, num_processes);
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
