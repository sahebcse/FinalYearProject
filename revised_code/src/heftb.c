#include <util.h>
#include <heftb.h>
#include <limits.h>

#define true				1
#define false				0
#define max(a,b)			((a>b)?a:b)


typedef struct {
	int task;
	double urank;
} node;

node *upper_ranks;
int *is_end;
/*
//for the upper rank calculation
void insertinto_b(int task,double rank)
{
    static int pos;
    int i;
    for(i=pos-1; i>=0; i--)
        if(rank>tasks_upper_rank[sorted_tasks[i]])
            sorted_tasks[i+1]=sorted_tasks[i];
        else
            break;
    sorted_tasks[i+1]=task;
    pos++;
}

// for calculate the upper rank of the tasks
double calculate_upper_rank(int task)
{
    int j;
    double avg_communication_cost,successor,avg=0.0,max=0,rank_successor;

    for(j=0; j<no_machines; j++)
        avg+=computation_costs[task][j];
    avg/=no_machines;

    for(j=0; j<no_tasks; j++)
    {
        if(data[task][j]!=-1)
        {
            avg_communication_cost=avg_communicationcost(task,j);
            rank_successor = tasks_upper_rank[j];
            successor=avg_communication_cost+rank_successor;
            if(max<successor)
                max=successor;
        }
    }
    return(avg+max);
}
*/

//calculate fitness
void calculate_makespan_heftb() {
	int elapsed_time[no_machines];
	int end_time[no_tasks];
	int assigned_proc[no_tasks];
	for(int i=0; i<no_tasks; i++) {
		end_time[i] = 0;
		assigned_proc[i] = -1;
	}
	for(int i=0; i<no_machines; i++)
		elapsed_time[i] = 0;
	
	int can_be_done = true;
//	int min_time = INT_MAX;
	for(int i=0; i<no_tasks; i++) {
		// task to assign
		int t = upper_ranks[i].task;
		int min_time = INT_MAX, min_proc = -1;

		for(int k=0; k<no_machines; k++) {				
			// Let us assign t to processor k
			int max_time = 0;
			for(int j=0; j<no_tasks; j++) {
				int time = computation_costs[t][k];
				if(data[j][t] != -1) {
				//	printf("%d %d %d %d %d %d\n", t, k, j, data[j][t], end_time[j], elapsed_time[k]);
					// j is parent of t
					if(assigned_proc[j] == -1) {
						can_be_done = false;
						break;
					}
					// check when j has finished and on which processor. Find maximum among them
					if(assigned_proc[j] == k)
						time += max(elapsed_time[k], end_time[j]);
					else
						time += max(elapsed_time[k], end_time[j] + data[j][t]);
				}	
					
				if(max_time < time)
					max_time = time;
				//printf("%d %d %d\n", t, k, max_time);
				// so, t will finish on k at time max_time
			}
			if(!can_be_done)
				break;

			if(max_time < min_time) {
				min_time = max_time;
				min_proc = k;
			}
		}
				
		assigned_proc[t] = min_proc;
		end_time[t] = min_time;
		elapsed_time[min_proc] = max(elapsed_time[min_proc], end_time[t]);
		// assign t on min_proc
		printf("%d assigned on %d with end time %d\n", t, assigned_proc[t], end_time[t]);

		if(!can_be_done) {
			printf("Error: cannot perform %d\n", t);
			break;
		}
	}

	int mks = 0;
	int avg_end = 0;
	for(int i=0; i<no_machines; i++) {
		avg_end += elapsed_time[i];
		if(elapsed_time[i] > mks)
			mks = elapsed_time[i];
	}

	avg_end /= no_machines;
	
	double lb = 0.0;
	for(int i=0; i<no_machines; i++)
		lb += (avg_end - elapsed_time[i])*(avg_end - elapsed_time[i]);

	lb = sqrt(lb/no_machines);

	double energy = 0.0;
	for(int i=0; i<no_tasks; i++) {
		int t = upper_ranks[i].task;
		int p = assigned_proc[t];
		energy += ((vrand[p][t]*vrand[p][t])*frequency[p][t]*computation_costs[t][p]);
	}

	printf("Makespan: %d, LB: %lf, Energy: %lf\n", mks, lb, energy);
}

int cmpfunc (const void * a, const void * b) {
   return ( ((node*)b)->urank - ((node*)a)->urank );
}

double calculate_upper_rank(int task) {
	double avg_communication_cost, avg=0.0;

    for(int j=0; j<no_machines; j++)
        avg+=computation_costs[task][j];
    avg/=no_machines;

	if(is_end[task])
		return avg;

	double max_val = 0.0;
	for(int i=0; i<no_tasks; i++) {
		if(data[task][i] != -1) {
			//printf("%d %d %d\n", task, i, data[task][i]);
			max_val = max(max_val, data[task][i] + calculate_upper_rank(i)); 
		}
	}
	return (avg + max_val);
} 

void perform_heftb() {
	int indeg[no_tasks], outdeg[no_tasks];
	is_end = (int *)malloc(no_tasks*sizeof(int));
	for(int i=0; i<no_tasks; i++) {
		for(int j=0; j<no_tasks; j++) {
			if(data[i][j] != -1) {
				outdeg[i]++;
				indeg[j]++;
			}
		}
	}

	int start = -1, end = -1;
	for(int i=0; i<no_tasks; i++) {
		if(outdeg[i] == 0)
			is_end[i] = 1;
		if(indeg[i] == 0)
			start = i;
	}
	upper_ranks = (node *)malloc(no_tasks*sizeof(node));
	for(int i=0; i<no_tasks; i++) {
		upper_ranks[i].task = i;
		upper_ranks[i].urank = calculate_upper_rank(i);
	}

	qsort(upper_ranks, no_tasks, sizeof(node), cmpfunc);
	for(int i=0; i<no_tasks; i++)
		printf("Task: %d Rank: %lf\n", upper_ranks[i].task, upper_ranks[i].urank);
	calculate_makespan_heftb();
}
