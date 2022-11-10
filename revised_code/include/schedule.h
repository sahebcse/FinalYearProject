#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <util.h>

struct TaskProcessor *do_schedule(int task);

void make_schedule();

void display_schedule();

// For GSA
void make_schedule_gsa(double *, double *, double *, double *);

#endif
