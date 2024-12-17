#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_PROCESSES 100

typedef struct {
    int finish_time;
    float turnaround;
    float normturn;  //turnaround / service
}Stats;

typedef struct {
    char name;       
    int arrival_time;    // Arrival time
    int service_time; 
    Stats stats;     
} Process;

typedef struct {
    int policy_id;
    int quantum=-1;
}Policy;

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
    }

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
