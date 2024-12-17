#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int id;             
    int arrival_time;   
    int burst_time;     
    int remaining_time; // For algorithms that preempt
    int waiting_time;   // Waiting time
    int turnaround_time;
    int completed;      // Flag for completion
} Process;
