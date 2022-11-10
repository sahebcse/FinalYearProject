#include <pso.h>
#include <util.h>
#include <time.h>
#include <math.h>


float w1=1.0,w2=0.0,w3=0.0;

float Wc=0.7968,C1=1.4962,C2=1.4962;
//float Wc=2.0000,C1=1.5,C2=1.5;
float Vmax=0.5, Vmin=-0.5, Xmax=1.0, Xmin=0.0;

void Velocity_Position_Update(struct Particle *P, struct Particle Pbest, struct Particle Gbest)
{
     int i;
     float R1=0.0,R2=0.0;
     unsigned int iseed = (unsigned int)time(NULL);
     srand (iseed); 
         ///******Velocity Calculation and Position Update********///  	 
         for(i=0;i<T;i++)
		 {
		   R1=(float)(1+(rand()%999))/1000.0; // always generates a number. 0<R1<1.
		   R2=(float)(1+(rand()%999))/1000.0; // always generates a number. 0<R2<1.
		   
		   P->V[i]=Wc*P->V[i]+C1*R1*(Pbest.position[i]-P->position[i])+C2*R2*(Gbest.position[i]-P->position[i]);//Velocity update
		           if(P->V[i]>=Vmax)
                      P->V[i]=Vmax;
                   if(P->V[i]<=Vmin)
                      P->V[i]=Vmin;
		   
		   P->position[i]=P->position[i]+P->V[i];//Position Update.
                   if(P->position[i]>=Xmax)
                      P->position[i]=0.99;
                   if(P->position[i]<=Xmin)
                      P->position[i]=0.000001;

		 }
                 Cal_Fitness(P);
}

void Update_PbestGbest(struct Particle P, struct Particle *Pbest, struct Particle *Gbest)
{
    int i=0;
    if(P.Fitness[0]<Pbest->Fitness[0])//Minimization function
    {
       for(i=0;i<T;i++)
         Pbest->position[i]=P.position[i];
      Cal_Fitness(Pbest);
    }
    if(Gbest->Fitness[0]>Pbest->Fitness[0])//Minimization function
    {
       for(i=0;i<T;i++)
         Gbest->position[i]=Pbest->position[i];
      Cal_Fitness(Gbest);
    }
}


void Cal_Fitness(struct Particle *P)
{
   // float F=0.0;
    float x=0.0,max=0.0,UT=0.0,LB=0.0,avg=0.0;
    int pro_no=-1,i=0;
    float FT[PROC];
    
    for(i=0;i<PROC;i++)
      FT[i]=0.0;
      
    for(i=0;i<T;i++)
    {
    	 x=0.0;
         pro_no=-1;
         while(P->position[i]>x)
         {
            x=x+1.0/(float)PROC;
            pro_no++;
         }
         FT[pro_no]=FT[pro_no]+(float)computation_costs[i][pro_no];
	}
    
    for(i=0;i<PROC;i++)
      if(FT[i]>max)
	    max=FT[i];
      
    //F=3*P->position[0]*P->position[0]-5*P->position[1]*P->position[1]+4*P->position[2]*P->position[2]*P->position[2]-2*P->position[3]*P->position[3]+3*P->position[4]*P->position[4]*P->position[4]*P->position[4];
    P->Fitness[1]=max;//makespan
    // Utilization
    for(i=0;i<PROC;i++)
		avg=avg+FT[i];
		
    avg=avg/(float)PROC;
	UT=avg/max;
	P->Fitness[2]=UT;//Utilization
	
	for(i=0;i<PROC;i++)// load balance
		LB=LB+(avg-FT[i])*(avg-FT[i]);
	LB=LB/(float)PROC;
	LB=sqrt(LB);
	
	P->Fitness[3]=LB;//load balance
	
	P->Fitness[0]=w1*max+w2*(1.0-UT)+w3*LB;
	//	P->Fitness[0]=0.0*max+1.0*LB;
	//P->Fitness[0]=0.1*max+0.6*LB;
}

/*
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
*/

void Create_Populations(struct Particle *Pop)
{
   int i,j;
   unsigned int iseed = (unsigned int)time(NULL);
   srand (iseed); 

   for(i=0;i<PSIZE;i++)
   {
      for(j=0;j<T;j++)
      {
         Pop[i].position[j]=(float)(1+(rand()%99))/100.0;// always generates a number. 0.0<r<1.0.
         Pop[i].V[j]=0.0;
      }
      Cal_Fitness(&Pop[i]);
   }
} 
