#include <util.h>
#include <heftb.h>
#include <schedule.h>
#include <float.h>
#include <time.h>
#include <gsa.h>
#include <string.h>

#define true			1
#define false 			0
#define LIMIT 			40
#define GRAVITATION		5
#define EPS				4	
#define W1				0.5
#define W2				0.2
#define W3				0.2
#define W4				0.1
#define MASS_LIMIT		0.01
#define ALPHA			0.5
#define EPS_SUB			10e-9

static int *processors_t, *is_done_t;
static int *sequence, sind_t;
static gsa_schedule_p *schedule_p;
static char *is_done;
static int all_found, limit, ind;
static double max_global_fitness;
static gsa_schedule_p *best_schedule;
static double gravity;
static double ***force;

void assign_task_gsa(int task) {
	for(int i=0; i<no_tasks; i++) {
//		int doable = 1;
		if(data[i][task] != -1) {
			// i is parent of task
			if(!is_done_t[i]) {
			//	doable = 0;
				assign_task_gsa(i);
			}
		}
	}
	is_done_t[task] = 1; 
	sequence[sind_t++] = task;
}

//calculate fitness
void calculate_fitness(gsa_schedule_p *schedule) {
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
		//printf("(%d %d) ", t, p);
		int max_time = elapsed_time[p];

		int can_perform = true;
		for(int j=0; j<no_tasks; j++) {
			int pt = sequence[j];
			if(data[pt][t] != -1) {
				// j is parent of i
				if(!is_done[pt]) {
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

	schedule->fitness = 1/(1 + W1*schedule->mks);
        //schedule->fitness = (1/ALPHA*schedule->mks);
}

// agent representation
void perform_gsa_main() {
	double min_fit = DBL_MAX, max_fit = 0.0;
	max_global_fitness = 0.0;
	best_schedule = (gsa_schedule_p *)malloc(sizeof(gsa_schedule_p));
	for(int i=0; i<LIMIT; i++) {
		for(int j=0; j<no_tasks; j++) {
			schedule_p[i].agent[j] = (rand()%999 + 1)/1000.0;
			schedule_p[i].proc[j] = schedule_p[i].agent[j]*no_machines;
		}
                 int j=0;
		for(j=0; j<no_tasks; j++)
			 //printf("%0.4lf(%d) ", schedule_p[i].agent[j], schedule_p[i].proc[j]);
               // printf("(%d) ", schedule_p[i].proc[j]);
		//putchar('\n');
		calculate_fitness(&schedule_p[i]);
		
		if(schedule_p[i].fitness < min_fit)
			min_fit = schedule_p[i].fitness;
		if(schedule_p[i].fitness > max_fit)     
			max_fit = schedule_p[i].fitness;
		if(schedule_p[i].fitness > max_global_fitness)
			max_global_fitness = schedule_p[i].fitness;
/*			//best_schedule = schedule_p[i];
			memcpy(best_schedule, &schedule_p[i], sizeof(schedule_p[i]));
			printf("Test: %lf %lf %lf %lf\n", best_schedule->fitness, best_schedule->mass, schedule_p[i].fitness, schedule_p[i].mass);
		}	*/
	}

	for(int i=0; i<LIMIT; i++) {
		if(max_fit != min_fit)
			schedule_p[i].mass = (schedule_p[i].fitness - min_fit)/(max_fit-min_fit);
		else
			schedule_p[i].mass = 0.0;

		if(fabs(schedule_p[i].mass-1.00) < EPS_SUB) {
			memcpy(best_schedule, &schedule_p[i], sizeof(schedule_p[i]));
			printf("Best Schedule found: %lf %lf\n", best_schedule->fitness, best_schedule->mass);
		}
	}
/*
	for(int i=0; i<LIMIT; i++) {	
		if(schedule_p[i].mass < MASS_LIMIT) {
		//	schedule_p[i].mass = 0.02; // updation of mass
		//	schedule_p[i] = best_schedule;
			memcpy(&schedule_p[i], best_schedule, sizeof(best_schedule));
			int rand_pos = ((rand()%999 + 1)/1000.0)*no_tasks;
			int rand_proc = ((rand()%999 + 1)/1000.0)*no_machines;
			printf("Test: %d %d\n", rand_pos, rand_proc);
			schedule_p[i].proc[rand_pos] = rand_proc;
			printf("Replacing inferior one by best one: Darwin's Theory");
		//	calculate_fitness (&schedule_p[i]);
		//	printf("New Mass: %lf\n", schedule_p[i].mass);
		} */
	//printf("MKS: %d LB:%lf SP: %lf FITNESS:%lf MASS:%lf\n", schedule_p[i].mks, schedule_p[i].lb, schedule_p[i].sp, schedule_p[i].fitness, schedule_p[i].mass);
//	}
}


// agent updation
void update_gsa() {
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

		//	printf("Force on %d agent at %d location: %lf\n", i, j, tforce[i][j]);
			if(schedule_p[i].mass < MASS_LIMIT) {
				memcpy(&schedule_p[i], best_schedule, sizeof(best_schedule));
				int rand_pos = ((rand()%999 + 1)/1000.0)*no_tasks;
				int rand_proc = ((rand()%999 + 1)/1000.0)*no_machines;
			//	printf("Test: %d %d\n", rand_pos, rand_proc);
				schedule_p[i].proc[rand_pos] = rand_proc;
			//	printf("Replacing inferior one by best one: Darwin's Theory");
			}
			else {
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

void perform_gsa() {
	printf("====================By Existing Hybrid GSA Algo====================\n");
//	srand(time(NULL));
	is_done = (char *)malloc(no_tasks*sizeof(char));
	sequence = (int *)malloc(no_tasks*sizeof(int));

/*	for(int i=no_tasks; i>=0; i--)
    {
        if(tasks_upper_rank[i]==-1)
        {
            tasks_upper_rank[i]=calculate_upper_rank(i);
            insertinto_b(i,tasks_upper_rank[i]);
        }
    } */
	sind_t = 0;
	is_done_t = (int *)malloc(no_tasks*sizeof(int));
	processors_t= (int *)malloc(no_tasks*sizeof(int));
	for(int i=0; i<no_tasks; i++) {
		if(!is_done_t[i])		
			assign_task_gsa(i);
	}
//	for(int i=0; i<no_tasks; i++)
//		sequence[i] = i; 
	printf("%d\n", sind_t);
	for(int i=0; i<no_tasks; i++)
		printf("%d ", sequence[i]);
	
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
	
	perform_gsa_main();
	gravity = GRAVITATION;

	
	for(int i=0; i<LIMIT; i++) {
		printf(" =========================== ITERATION %d=======================================\n", i+1);
		update_gsa();
		for(int j=0; j<LIMIT; j++) {
			schedule_p[j].mks = 0;
			schedule_p[j].lb = schedule_p[j].sp = schedule_p[j].fitness = schedule_p[j].mass = 0.0;
	/*		for(int k=0; k<no_tasks; k++)
				printf("%lf(%d) ", schedule_p[j].agent[k], schedule_p[j].proc[k]);
			putchar('\n'); */
		}
		double min_fit = DBL_MAX, max_fit = 0.0;
		best_schedule = (gsa_schedule_p *)malloc(sizeof(gsa_schedule_p));
		for(int j=0; j<LIMIT; j++) {
			calculate_fitness(&schedule_p[j]);
	
			if(schedule_p[j].fitness < min_fit)
				min_fit = schedule_p[j].fitness;
			if(schedule_p[j].fitness > max_fit)
				max_fit = schedule_p[j].fitness;
			if(schedule_p[j].fitness > max_global_fitness) {
				max_global_fitness = schedule_p[j].fitness;
				memcpy(best_schedule, &schedule_p[j], sizeof(schedule_p[j]));
			}
		}

		double best_mass = 0.0;
		for(int j=0; j<LIMIT; j++) {
	//		schedule_p[j].mass = (schedule_p[j].fitness - min_fit)/(max_fit-min_fit);
	/*		if(max_fit != min_fit)
				schedule_p[j].mass = (schedule_p[j].fitness - min_fit)/(max_fit-min_fit);
			else
				schedule_p[j].mass = 0.02;
			if(schedule_p[j].mass < MASS_LIMIT)
				schedule_p[j].mass = 0.02; // updation of mass */
			if(max_fit != min_fit)
				schedule_p[j].mass = (schedule_p[j].fitness - min_fit)/(max_fit-min_fit);
			else
				schedule_p[j].mass = 0.0;

			if(fabs(schedule_p[j].mass-1.00) < EPS_SUB) {
				memcpy(best_schedule, &schedule_p[j], sizeof(schedule_p[j]));
				printf("Best Schedule found:\n");
				for(int i=0; i<no_tasks; i++)
					printf("%d ", best_schedule->proc[i]);
				putchar('\n');					

				printf("MKS: %d LB: %lf Energy: %lf Fitness: %lf Mass: %lf\n", best_schedule->mks, best_schedule->lb, 
							 best_schedule->energy, best_schedule->fitness, best_schedule->mass);
			}
		}
/*			
		for(int j=0; j<LIMIT; j++) {
			if(schedule_p[j].mass < MASS_LIMIT) {
			//	schedule_p[i].mass = 0.02; // updation of mass
			//	schedule_p[j] = best_schedule;
				memcpy(&schedule_p[j], best_schedule, sizeof(best_schedule));
				int rand_pos = ((rand()%999 + 1)/1000.0)*no_tasks;
				int rand_proc = ((rand()%999 + 1)/1000.0)*no_machines;
				printf("Test: %d %d\n", rand_pos, rand_proc);
				schedule_p[j].proc[rand_pos] = rand_proc;
				printf("Replacing inferior one by best one: Darwin's Theory");
				calculate_fitness (&schedule_p[j]);
			} 
			printf("MKS: %d LB:%lf SP: %lf FITNESS:%lf MASS:%lf ENERGY:%lf\n", schedule_p[j].mks, schedule_p[j].lb, schedule_p[j].sp, schedule_p[j].fitness, schedule_p[j].mass, schedule_p[j].energy); */
	//	}
	}
}
