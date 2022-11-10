#include <schedule.h>
#include <util.h>
#include <stdio.h>
#include <float.h>

// For GSA purposes
double glb_mks, glb_ut, glb_lb, glb_sp;

static void insertslots(double **machineFreeTime,int current_pos, double start,double end)
{
    int i;
    
    if(start < 0)
        start=0;
    for(i=current_pos-1; i>=0; i--)
    {
        if(start < machineFreeTime[i][0])
        {
            machineFreeTime[i+1][0]=machineFreeTime[i][0];
            machineFreeTime[i+1][1]=machineFreeTime[i][1];
        }
        else
            break;
    }
    machineFreeTime[i+1][0]=start;
    machineFreeTime[i+1][1]=end;
}

static void findfreeslots(int processor,double **machineFreeTime,int *noslots)
{
    int i,j;
    *noslots=0;
    double highest_AFT=-99999.0,min=99999.0;
    for(i=0; i<no_tasks; i++)
    {
        min=99999.0;
        if(schedule[i].processor==processor)
        {
            if(schedule[i].AFT>highest_AFT)
                highest_AFT=schedule[i].AFT;
            for(j=0; j<no_tasks; j++)
            {
                if((i==j) || (schedule[j].processor!=processor))
                    continue;
                if((schedule[j].AST>=schedule[i].AFT) && (schedule[j].AST<min))
                {
                    min=schedule[j].AST;
                }
            }
            if(min<99998.0)
            {
                insertslots(machineFreeTime,*noslots,schedule[i].AFT,min);
                (*noslots)++;
            }
        }
    }
    insertslots(machineFreeTime,*noslots,highest_AFT,99999.0);
    (*noslots)++;
}

static int isEntryTask(int task)
{
    int i;
    for(i=0; i<no_tasks; i++)
    {
        if(data[i][task]!=-1)
            return 0;
    }
    return 1;
}

static double find_EST(int task,int processor)
{
    int i;
    double ST,EST=-99999.0,comm_cost;
    for(i=0; i<no_tasks; i++)
    {
        if(data[i][task]!=-1)
        {
		//	printf("before %d %d\n", schedule[i].processor, processor);
        // 	printf("%lf ", data_transfer_rate[schedule[i].processor][processor]);
		//	printf("after\n");
            if(data_transfer_rate[schedule[i].processor][processor]==0)
                comm_cost=0;
        
            else
                comm_cost=data[i][task]/data_transfer_rate[schedule[i].processor][processor];
            ST=schedule[i].AFT + comm_cost;
            if(EST<ST)
                EST=ST;
        }
    }
    return EST;
}

static void calculate_EST_EFT(int task,int processor,struct TaskProcessor *EST_EFT)
{
    double **machineFreeTime,EST;
    int i;
    machineFreeTime=(double**)calloc(1000,sizeof(double*));
    for(i=0; i<1000; i++)
        machineFreeTime[i]=(double*)calloc(2,sizeof(double));
    int noslots=0;
    findfreeslots(processor,machineFreeTime,&noslots);
//	printf("here\n");
 //   printf("\n\n");
    EST=find_EST(task,processor);
  // printf("here\n");
    EST_EFT->AST=EST;
    EST_EFT->processor=processor;
    for(i=0; i<noslots; i++)
    {
        if(EST<machineFreeTime[i][0])
        {
            if((machineFreeTime[i][0] + computation_costs[task][processor]) <= machineFreeTime[i][1])
            {
                EST_EFT->AST=machineFreeTime[i][0];
                EST_EFT->AFT=machineFreeTime[i][0] + computation_costs[task][processor];
                return;
            }
        }
        if(EST>=machineFreeTime[i][0])
        {
            if((EST + computation_costs[task][processor]) <= machineFreeTime[i][1])
            {
                EST_EFT->AFT=EST_EFT->AST + computation_costs[task][processor];
                return;
            }
        }
    }
}

struct TaskProcessor *do_schedule(int task) {
	double minCost=DBL_MAX, min_EFT=DBL_MAX;
	struct TaskProcessor *min_proc;
	struct TaskProcessor *EST_EFT;
    EST_EFT=(struct TaskProcessor *)calloc(1,sizeof(struct TaskProcessor));
    for(int j=0; j<no_machines; j++)
    {
    	calculate_EST_EFT(task,j,EST_EFT);
        if(min_EFT>(EST_EFT->AFT))
        {
			min_proc = EST_EFT;
			min_EFT=EST_EFT->AFT;
        }
    }
    return min_proc;
}

void make_schedule_gsa(double *mks, double *lb, double *ut, double *sp) {
	make_schedule();
	display_schedule();
	*mks = glb_mks;
	*lb = glb_lb;
	*ut = glb_ut;
	*sp = glb_sp;
}

void make_schedule()
{
    int i,j,k,t=0,processor,task;
    double minCost=99999.99, min_EFT=99999.99;
    struct TaskProcessor *EST_EFT;
    EST_EFT=(struct TaskProcessor *)calloc(1,sizeof(struct TaskProcessor));

    for(i=0; i<no_tasks; i++)
    {
        min_EFT=9999.99;
        task=sorted_tasks[i];
//	printf("%d ", task);
    
        if(isEntryTask(task))
        {
            for(j=0; j<no_machines; j++)
            {
                if(minCost>computation_costs[task][j])
                {
                    minCost=computation_costs[task][j];
                    processor=j;
                }
            }
            schedule[task].processor=processor;
            schedule[task].AST=0;
            schedule[task].AFT=minCost;
        }
        else
        {
			printf("here\n");
            for(j=0; j<no_machines; j++)
            {
                calculate_EST_EFT(task,j,EST_EFT);
              //  printf("%lf %lf %d",EST_EFT->AST,EST_EFT->AFT,EST_EFT->processor);
                if(min_EFT>(EST_EFT->AFT))
                {
                    schedule[task]=*EST_EFT;
                    min_EFT=EST_EFT->AFT;
                }
            }
        }

    //   printf("Task scheduled %d\n",task);
    //   printf("%d %lf %lf\n",schedule[task].processor,schedule[task].AST,schedule[task].AFT);
   //     printf("------\n");
	//	printf("%d ", schedule[task].processor);
    }

	float makespan = 0.0;
	for(int i=0; i<no_tasks; i++) {
		if(makespan < schedule[i].AFT)
			makespan = schedule[i].AFT;
	}
	glb_mks = makespan;
//	printf("\nMakespan : %f\n", makespan);
}


void display_schedule()
{
    int i,j,k;
    double t1;
    double arr[no_tasks];
    double arr1[no_machines];
    double avg =0;
    double ut =0;
    double load_balance = 0.0;
    double number;
    double total1 =0.0;
    double speedup;
    
    float makespan = -9999.99;
    float maximum = -9999.99;
    printf("SCHEDULE\n");
    for(i=0; i<no_tasks; i++)
    {
       //printf("TASK :%d PROCESSOR :%d AST :%lf AFT :%lf\n",i+1,schedule[i].processor+1,schedule[i].AST,schedule[i].AFT);
        total1 = total1 + schedule[i].AFT;
        if(makespan <= schedule[i].AFT)
        {
        	makespan = schedule[i].AFT;
		}
    }
 //   printf("\n");
   //printf("***********MAKESPAN************\n");
    //printf("Makespan of the task MKS : %lf\n",makespan);
  //  printf("\n");
   // printf("***********Load Balance*********\n");
    k=0;
    for(i=1; i<=no_machines; i++)
    {
    	for(j=0; j<no_tasks; j++)
    	{
    		if(schedule[j].processor+1 == i)
    		{
    			arr[k] = schedule[j].AFT;
			}
		}
	//	printf("%lf ",arr[k]);
		k++;
	}
	for(i=0; i<k; i++)
	{
		avg = avg + arr[i];
	}
	avg = avg/no_machines;

       /*for(i=0;i<PROC;i++)// load balance
		LB=LB+(avg-FT[i])*(avg-FT[i]);
	LB=LB/(float)PROC;
	LB=sqrt(LB);
	
	P->Fitness[3]=LB;//load balance */
	//printf("value of u : %lf\n",avg);
	
	for(i=0; i<no_machines; i++)
	{
		number = avg - arr[i];
		number = number*number;
		load_balance = load_balance + number;
	}
	load_balance = load_balance / no_machines;
	load_balance = sqrt(load_balance);
	glb_lb = load_balance;
	//printf("load balance : %lf\n",load_balance);
	
/*	for(i=0; i<k; i++)
	{
		ut = ut + arr[i];
	}
	ut = ut *100;
	ut = ut/(makespan*no_machines);
	glb_ut = ut;
	printf("processor utilization :%lf\n",ut); */
	/* schedule->energy = 0.0;
	for(int i=0; i<no_tasks; i++) {
		int t = sequence[i];
		int p = schedule->proc[i];
		schedule->energy += ((vrand*vrand)*frequency[p][t]*computation_costs[t][p]);
         */
	double max_speed = 0.0;
    	for(i=0; i<no_machines;i++)
    	{
    		speedup = 0.0;
    		for(j=0; j<no_tasks; j++)
    			speedup = speedup + computation_costs[j][i];
		if(speedup > max_speed)
			max_speed = speedup;
     	}
	double max_speedup = max_speed / makespan;
	glb_sp = max_speedup;
	printf("Maksespan : %lf Load balance :%lf Energy: %lf\n",makespan,  load_balance, max_speedup);
}
