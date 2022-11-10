#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//declare number of the tasks and the number of the machiens
static int no_tasks,no_machines;

//decalre dynamic multidimension array using pointer
static double **computation_costs,**data_transfer_rate, *tasks_upper_rank, *tasks_lower_rank;
static int *sorted_tasks, **data;

//declare a structre for each task to store the processor number, earlist start time and earliset finishing tinme
struct TaskProcessor
{
    int processor;
    double AST;
    double AFT;
};

static struct TaskProcessor *schedule;
static double **frequency, **vrand;
void parse_dy(FILE *);
void parse(FILE *);

double avg_communicationcost(int source, int destination);

#endif
