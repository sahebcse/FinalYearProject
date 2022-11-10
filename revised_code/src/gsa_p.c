#include <util.h>
#include <heftb.h>
#include <schedule.h>
#include <float.h>
#include <time.h>
#include <limits.h>
#include <gsa.h>
#include <gsa_p.h>
#include <math.h>

#define true			1
#define false 			0
#define LIMIT 			40
#define GRAVITATION		10
#define EPS				4
#define W1				0.5
#define W2				0.3
#define W3				0.2
#define MASS_LIMIT		0.01
#define EPS_SUB 		10e-9

//double **frequency, **vrand;
static int *sequence, *best_proc, sind_t;
static gsa_schedule_p *schedule_p, best;
static char *is_done;
static int all_found, limit, ind;
static double max_global_fitness;
static double gravity;
static double ***force;

static int *sorted, sind;
static int *marked;				// 0 -> no mark, 1 -> temporary mark, 2 -> permanent mark

static int *processors_t;
static int *is_done_t;

void assign_task(int task) {
	for(int i=0; i<no_tasks; i++) {
//		int doable = 1;
		if(data[i][task] != -1) {
			// i is parent of task
			if(!is_done_t[i]) {
			//	doable = 0;
				assign_task(i);
			}
		}
	}
	is_done_t[task] = 1; 
	sequence[sind_t++] = task;
}

//calculate fitness
void calculate_fitness_p(gsa_schedule_p *schedule) {
	int is_done[no_tasks];
	int elapsed_time[no_machines];
	int end_time[no_tasks];
	for(int i=0; i<no_machines; i++)
		elapsed_time[i] = 0;
	for(int i=0; i<no_tasks; i++) {
		end_time[i] = 0;
		is_done[i] = false;
	}

	for(int i=0; i<no_tasks; i++) {
		int t = sequence[i];
		int p = schedule->proc[i];
		//printf("(%d %d)", t, p);
		int max_time = elapsed_time[p];

		int can_perform = true;
		for(int j=0; j<no_tasks; j++) {
			int pt = sequence[j];
			if(data[pt][t] != -1) {
				// j is parent of i
				if(!is_done[pt] && t) {
					printf("%d %d %d", pt, t, data[pt][t]);
					can_perform = false;
					break;
				}
				int pj = schedule->proc[j];
				if(pj == p) {
					// no need of communication
					if(max_time < end_time[pt])
						max_time = end_time[pt];
				}
				else {
					if(max_time < end_time[pt] + data[pt][t])
						max_time = end_time[pt] + data[pt][t];
				}
			}
		}
		if(!can_perform) {
			printf("Error: Task %d cannot perform\n", t);
			break;
		}

		// process i gonna start from max_time
        // int j=0;
		end_time[t] = max_time + computation_costs[t][p];
		if(elapsed_time[p] < end_time[t])
			elapsed_time[p] = end_time[t];
		is_done[t] = true;
//		printf("%d started on %d at %d, and finished at %d\n", t, p, max_time, end_time[t]);
	}
	
	schedule->energy = 0.0;
	for(int i=0; i<no_tasks; i++) {
		int t = sequence[i];
		int p = schedule->proc[i];
		schedule->energy += ((vrand[p][t]*vrand[p][t])*frequency[p][t]*computation_costs[t][p]);
                
	}
           
	schedule->mks = 0;
	int avg_end = 0;
	for(int i=0; i<no_machines; i++) {
		avg_end += elapsed_time[i];
		if(elapsed_time[i] > schedule->mks)
			schedule->mks = elapsed_time[i];
	}

	avg_end /= no_machines;
	schedule->lb = 0.0;
	for(int i=0; i<no_machines; i++)
		schedule->lb += (avg_end - elapsed_time[i])*(avg_end - elapsed_time[i]);

	schedule->lb = sqrt(schedule->lb/no_machines);
	double max_speed = 0.0;
    for(int i=0; i<no_machines;i++)
    {
    	double speedup = 0.0;
    	for(int j=0; j<no_tasks; j++)
    		speedup = speedup + computation_costs[j][i];
		if(speedup > max_speed)
			max_speed = speedup;
    }
	schedule->sp = max_speed / schedule->mks;
	schedule->fitness = W1*(1/(double)schedule->mks) + W2*(1/schedule->lb) + W3*(1/schedule->energy);
//	schedule->fitness = (1/(double)schedule->mks);
//	putchar('\n');
//	printf("MKS: %d LB:%lf SP: %lf FITNESS:%lf\n", schedule->mks, schedule->lb, schedule->sp, schedule->fitness);
}

// agent representation
void perform_gsa_main_p() {
	double min_fit = DBL_MAX, max_fit = 0.0;
	for(int i=0; i<LIMIT; i++) {
		for(int j=0; j<no_tasks; j++) {
			schedule_p[i].agent[j] = (rand()%999 + 1)/1000.0;
			schedule_p[i].proc[j] = schedule_p[i].agent[j]*no_machines;
		}

		for(int j=0; j<no_tasks; j++)
			 //printf("%0.4lf(%d) ", schedule_p[i].agent[j], schedule_p[i].proc[j]);
		//putchar('\n');
		calculate_fitness_p(&schedule_p[i]);
		if(schedule_p[i].fitness < min_fit)
			min_fit = schedule_p[i].fitness;
		if(schedule_p[i].fitness > max_fit)
			max_fit = schedule_p[i].fitness;
		if(schedule_p[i].fitness > max_global_fitness)
			max_global_fitness = schedule_p[i].fitness;
	}

	for(int i=0; i<LIMIT; i++) {
		if(max_fit != min_fit)
			schedule_p[i].mass = (schedule_p[i].fitness - min_fit)/(max_fit-min_fit);
		else
			schedule_p[i].mass = 0.0;

//		if(schedule_p[i].mass < MASS_LIMIT)
//			schedule_p[i].mass = 0.02; // updation of mass

		if(fabs(schedule_p[i].mass - 1.00) < EPS_SUB) {
			printf("Best Schedule found:\n");
			//printf("Best Schedule found:\n");
				for(int k=0; k<no_tasks; k++)
					printf("%d ", schedule_p[i].proc[k]);
				putchar('\n');	
			//printf("MKS: %d LB: %lf SP: %lf Energy: %lf Fitness: %lf Mass: %lf\n", schedule_p[i].mks, schedule_p[i].lb, 
						//	schedule_p[i].sp, schedule_p[i].energy, schedule_p[i].fitness, schedule_p[i].mass);
		}
	//printf("MKS: %d LB:%lf SP: %lf FITNESS:%lf MASS:%lf\n", schedule_p[i].mks, schedule_p[i].lb, schedule_p[i].sp, schedule_p[i].fitness, schedule_p[i].mass);
	}
}


// agent updation
void update_gsa_p() {
	double force[LIMIT][LIMIT][no_tasks];
	for(int i=0; i<LIMIT; i++) {
		for(int j=0; j<LIMIT; j++) {
			if(i == j)
				continue;
			double dist = 0.0;
			for(int k=0; k<no_tasks; k++)
				dist += ((schedule_p[i].agent[k] - schedule_p[j].agent[k])*(schedule_p[i].agent[k] - schedule_p[j].agent[k]));
			dist = sqrt(dist);
			//printf("distance between %d and %d is %lf\n", i, j, dist);
			for(int k=0; k<no_tasks; k++) {
				if(dist)
					force[i][j][k] = (gravity*schedule_p[i].mass*schedule_p[j].mass*(schedule_p[j].agent[k] - schedule_p[i].agent[k]))/(dist);
				else
					force[i][j][k] = 0.0;
				//if(k == 4)
				// printf("Force between %dth agent and %dth agent at %dth position : %lf\n", i, j, k, force[i][j][k]);
			}
		}
	}

	double tforce[LIMIT][no_tasks];
	for(int i=0; i<LIMIT; i++) {
	//	int pos[no_tasks];
		for(int j=0; j<no_tasks; j++) {
			tforce[i][j] = 0.0;
			for(int k=0; k<LIMIT; k++) {
				if(k == i)
					continue;
				double r = (1 + rand()%999)/1000.0;
				tforce[i][j] += (r*force[i][k][j]);
			}
			
			if(schedule_p[i].mass < MASS_LIMIT) {
				for(int k=0; k<no_tasks; k++) {
					schedule_p[i].proc[k] = best_proc[k];
					schedule_p[i].agent[k] = best_proc[k]/(double)no_machines;
				}
			}
			else {
			//	printf("Force on %d agent at %d location: %lf\n", i, j, tforce[i][j]);
				if(schedule_p[i].mass < MASS_LIMIT)
					printf("Correct it\n");
				double accel = tforce[i][j]/schedule_p[i].mass;
			//	printf("(%lf %lf)", tforce[i][j], schedule_p[i].mass);
				double r = (1 + rand()%999)/1000.0;
				schedule_p[i].velocity[j] = (r*schedule_p[i].velocity[j]) + accel;
				schedule_p[i].agent[j] += schedule_p[i].velocity[j];
				if(schedule_p[i].agent[j] >= 1.0)
					schedule_p[i].agent[j] = 0.99;
				else if(schedule_p[i].agent[j] < 0.0)
					schedule_p[i].agent[j] = 0.0;
				schedule_p[i].proc[j] = schedule_p[i].agent[j]*no_machines;
			}
		}
	}
}

void visit(int node) {
	if(marked[node] == 2)
		return;

	if(marked[node] == 1) {
		printf("NOT a DAG\n");
		return;
	}
	marked[node] = 1;
	for(int i=0; i<no_tasks; i++) {
		if(data[node][i] != -1)
			visit(i);
	}
	marked[node] = 2;
	sorted[sind--] = node;
}

void do_tsort() {
	marked = (int *)malloc(no_tasks*sizeof(int));
	for(int i=0; i<no_tasks; i++)
		marked[i] = 0;
	sind = no_tasks-1;
	
	while(true) {
		int next = -1;
		for(int i=0; i<no_tasks; i++) {
			if(!marked[i]) {
				next = i;
				break;
			}
		}

		if(next == -1)
			break;
	//	printf("%d ", next);
	//	printf("%d %d %d\n", no_machines, no_tasks, next);
		visit(next);
	}
}

void create_sequence() {
	double avg_cost[no_tasks];
	
	for(int i=0; i<no_tasks; i++) {
		int time = 0;
		for(int j=0; j<no_machines; j++)
			time += computation_costs[i][j];
		avg_cost[i] = time/(double)no_machines;
	}
	
	do_tsort();
	for(int i=0; i<no_tasks; i++)
		printf(":%d ", sorted[i]);
    }

/* need to be updated  */
void perform_gsa_p() {
	best.fitness = 0.0;
	printf("====================Proposed Improved GSA Algo====================\n");
//	srand(time(NULL));
	is_done = (char *)malloc(no_tasks*sizeof(char));
	sequence = (int *)malloc(no_tasks*sizeof(int));
	best_proc = (int *)malloc(no_tasks*sizeof(int));
/*	vrand = (double **)malloc(sizeof(double *)*no_machines);
	frequency = (double **)malloc(sizeof(double *)*no_machines);
	for(int i=0; i<no_machines; i++) {
		frequency[i] = (double *)malloc(sizeof(double)*no_tasks);
		vrand[i] = (double *)malloc(sizeof(double)*no_tasks);
		for(int j=0; j<no_tasks; j++) {
			frequency[i][j] = (rand()%999+1)/500.0;
			vrand[i][j] = (rand()%999+1)/500.0;
            printf("vrand=%lf frequency=%lf", vrand[i][j], frequency[i][j]);
		}
	} 
	for(int i=0; i<no_tasks; i++)
		schedule[i].processor = 0;
	for(int i=no_tasks-1; i>=0; i--)
    {
        if(tasks_upper_rank[i]==-1)
        {
            tasks_upper_rank[i]=calculate_upper_rank(i);
            insertinto_b(i,tasks_upper_rank[i]);
        }
    }
	
	int elapsed[no_machines];
	for(int i=0; i<no_machines; i++)
		elapsed[i] = 0;
	for(int i=0; i<no_tasks; i++) {
		sequence[i] = sorted_tasks[i];
		int min_time = INT_MAX, min_proc = -1;
		for(int j=0; j<no_machines; j++) {
			if(elapsed[j] + computation_costs[sequence[i]][j] < min_time) {
				min_time = elapsed[j] + computation_costs[sequence[i]][j];
				min_proc = j;
			}
		}
		best_proc[i] = min_proc;
		elapsed[min_proc] = min_time;
		printf("(%d %d)", sequence[i], best_proc[i]);
	} 
//	make_schedule();
//	for(int i=0; i<no_tasks; i++)
//		best_proc[i] = schedule[sorted_tasks[i]].processor;
//	sorted = (int *)malloc(no_tasks*sizeof(int));
	printf("Generated Sequence:\n");
	create_sequence();						// activate for manual DAG
//	putchar('\n');
	
	for(int i=0; i<no_tasks; i++) {
		sequence[i] = sorted[i];			// activate for manual DAG
		//sequence[i] = i;
		printf("%d ", sequence[i]);
	} 
*/	
	is_done_t = (int *)malloc(no_tasks*sizeof(int));
	processors_t= (int *)malloc(no_tasks*sizeof(int));
	for(int i=0; i<no_tasks; i++) {
		if(!is_done_t[i])		
			assign_task(i);
	}

/*	printf("Generated Sequence:\n");
	for(int i=0; i<no_tasks; i++) {
		printf("%d ", sequence[i]);
	}
*/

	int elapsed[no_machines];
	for(int i=0; i<no_machines; i++)
		elapsed[i] = 0;
	for(int i=0; i<no_tasks; i++) {
//		sequence[i] = sorted_tasks[i];
		int min_time = INT_MAX, min_proc = -1;
		for(int j=0; j<no_machines; j++) {
			if(elapsed[j] + computation_costs[sequence[i]][j] < min_time) {
				min_time = elapsed[j] + computation_costs[sequence[i]][j];
				min_proc = j;
			}
		}
		best_proc[i] = min_proc;
		elapsed[min_proc] = min_time;
		printf("(%d %d)", sequence[i], best_proc[i]);
	} 
	putchar('\n');
	schedule_p = (gsa_schedule_p *)malloc(LIMIT*sizeof(gsa_schedule_p));
	for(int i=0; i<LIMIT; i++) {
		schedule_p[i].agent = (double *)malloc(no_tasks*sizeof(double));
		schedule_p[i].proc = (int *)malloc(no_tasks*sizeof(int));
		schedule_p[i].velocity = (double *)malloc(no_tasks*sizeof(double));
	}

	force = (double ***)malloc(ind*sizeof(int **));
	for(int i=0; i<ind; i++) {
		force[i] = (double **)malloc(ind*sizeof(int *));
		for(int j=0; j<ind; j++)
			force[i][j] = (double *)malloc(no_tasks*sizeof(int));
	}
	
	perform_gsa_main_p();
	gravity = GRAVITATION;

	
	for(int i=0; i<LIMIT; i++) {
		printf(" =========================== ITERATION %d=======================================\n", i+1);
		update_gsa_p();
		for(int j=0; j<LIMIT; j++) {
			schedule_p[j].mks = 0;
			schedule_p[j].lb = schedule_p[j].sp = schedule_p[j].fitness = schedule_p[j].mass = 0.0;
	/*		for(int k=0; k<no_tasks; k++)
				printf("%lf(%d) ", schedule_p[j].agent[k], schedule_p[j].proc[k]);
			putchar('\n'); */
		}
		double min_fit = DBL_MAX, max_fit = 0.0;
		for(int j=0; j<LIMIT; j++) {
			calculate_fitness_p(&schedule_p[j]);
	
			if(schedule_p[j].fitness < min_fit)
				min_fit = schedule_p[j].fitness;
			if(schedule_p[j].fitness > max_fit)
				max_fit = schedule_p[j].fitness;
			if(schedule_p[j].fitness > max_global_fitness)
				max_global_fitness = schedule_p[j].fitness;
		}

		for(int j=0; j<LIMIT; j++) {
	//		schedule_p[j].mass = (schedule_p[j].fitness - min_fit)/(max_fit-min_fit);
			if(max_fit != min_fit)
				schedule_p[j].mass = (schedule_p[j].fitness - min_fit)/(max_fit-min_fit);
			else
				schedule_p[j].mass = 0.0;

	//		if(schedule_p[j].mass < MASS_LIMIT)
	//			schedule_p[j].mass = 0.02; // updation of mass
			
			if(fabs(schedule_p[j].mass - 1.00) < EPS_SUB) {
				printf("Best Schedule found:\n");
				for(int k=0; k<no_tasks; k++)
					printf("%d ", schedule_p[j].proc[k]);
				putchar('\n');
				printf("MKS: %d LB: %lf Energy: %lf Fitness: %lf Mass: %lf\n", schedule_p[j].mks, schedule_p[j].lb, 
							schedule_p[j].energy, schedule_p[j].fitness, schedule_p[j].mass);
			}
		//	printf("MKS: %d LB:%lf SP: %lf FITNESS:%lf MASS:%lf ENERGY:%lf\n", schedule_p[j].mks, schedule_p[j].lb, schedule_p[j].sp, schedule_p[j].fitness, schedule_p[j].mass, schedule_p[j].energy);
			if(best.fitness < schedule_p[j].fitness)
				best = schedule_p[j];
		}
	} 
	// return best;
}
