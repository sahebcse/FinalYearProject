#ifndef GSA_H
#define GSA_H

typedef struct {
	double *agent;
	int *proc;
	int mks;
	double lb, sp;
	double energy;
	double fitness;
	double mass;
	double *velocity;
} gsa_schedule_p;
void perform_gsa();

#endif 

