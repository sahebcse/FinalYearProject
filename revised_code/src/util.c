#include <util.h>

void parse_dy(FILE *fp) {
        printf("Here..");
	fscanf(fp,"%d%d",&no_tasks,&no_machines);
        printf("%d  %d ",no_tasks,no_machines);
    
computation_costs=(double**)calloc(no_tasks,sizeof(double*));
    for(int i=0; i<no_tasks; i++)
        computation_costs[i]=(double*)calloc(no_machines,sizeof(double));

    data_transfer_rate=(double**)calloc(no_machines,sizeof(double*));
    for(int i=0; i<no_machines; i++)
        data_transfer_rate[i]=(double*)calloc(no_machines,sizeof(double));

    data=(int**)calloc(no_tasks,sizeof(double*));
    for(int i=0; i<no_tasks; i++)
        data[i]=(int*)calloc(no_tasks,sizeof(double));

    tasks_upper_rank=(double *)calloc(no_tasks,sizeof(double));
    for(int i=0; i<no_tasks; i++)
        tasks_upper_rank[i]=-1;
    
    tasks_lower_rank = (double *)calloc(no_tasks, sizeof(double));
    for(int i=0; i<no_tasks; i++)
        tasks_lower_rank[i] = -1;

    sorted_tasks=(int *)calloc(no_tasks,sizeof(int));
    schedule=(struct TaskProcessor*)calloc(no_tasks,sizeof(struct TaskProcessor));
    for(int i=0; i<no_tasks; i++)
        schedule[i].processor=-1;

    for(int i=0; i<no_tasks; i++)
    {
        fscanf(fp,"%lf",&computation_costs[i][0]);
        for(int j=1; j<no_machines; j++)
        {
            computation_costs[i][j]=computation_costs[i][0];
            //printf("%lf ",computation_costs[i][j]);
        }
   }
            
    for(int i=0; i<no_machines; i++)
        for(int j=0; j<no_machines; j++) {
            data_transfer_rate[i][j] = 1;
//		data_transfer_rate[i][i] = 0; 
	}
  	for(int i=0; i<no_tasks; i++)
        for(int j=0; j<no_tasks; j++)
            fscanf(fp,"%d",&data[i][j]);
 /*           for(int i=0; i<no_tasks; i++)
              for(int j=0; j<no_tasks; j++)
            fscanf(fp,"%d",&data[i][j]); */

	//vrand = (rand()%999+1)/500.0;
	frequency = (double **)malloc(sizeof(double *)*no_machines);
	vrand = (double **)malloc(sizeof(double *)*no_machines);
	for(int i=0; i<no_machines; i++) {
		frequency[i] = (double *)malloc(sizeof(double)*no_tasks);
		vrand[i] = (double *)malloc(sizeof(double)*no_tasks);
		frequency[i][0] = (rand()%999+1)/500.0;
		vrand[i][0] = (rand()%999+1)/500.0;
		for(int j=0; j<no_tasks; j++) {
			//frequency[i][j] = (rand()%999+1)/500.0;
			//vrand[i][j] = (rand()%999+1)/500.0;
                 
			frequency[i][j] = frequency[i][0];
			vrand[i][j] = vrand[i][0];
		}
               
	}
        
}


void parse(FILE *fp) {
        //printf("Here");
	fscanf(fp,"%d%d",&no_tasks,&no_machines);
        //printf("%d%d ",no_tasks,no_machines);
    computation_costs=(double**)calloc(no_tasks,sizeof(double*));
    for(int i=0; i<no_tasks; i++)
        computation_costs[i]=(double*)calloc(no_machines,sizeof(double));

    data_transfer_rate=(double**)calloc(no_machines,sizeof(double*));
    for(int i=0; i<no_machines; i++)
        data_transfer_rate[i]=(double*)calloc(no_machines,sizeof(double));

    data=(int**)calloc(no_tasks,sizeof(double*));
    for(int i=0; i<no_tasks; i++)
        data[i]=(int*)calloc(no_tasks,sizeof(double));

    tasks_upper_rank=(double *)calloc(no_tasks,sizeof(double));
    for(int i=0; i<no_tasks; i++)
        tasks_upper_rank[i]=-1;
    
    tasks_lower_rank = (double *)calloc(no_tasks, sizeof(double));
    for(int i=0; i<no_tasks; i++)
        tasks_lower_rank[i] = -1;

    sorted_tasks=(int *)calloc(no_tasks,sizeof(int));
    schedule=(struct TaskProcessor*)calloc(no_tasks,sizeof(struct TaskProcessor));
    for(int i=0; i<no_tasks; i++)
        schedule[i].processor=-1;

    for(int i=0; i<no_tasks; i++)
        for(int j=0; j<no_machines; j++)
        {
            fscanf(fp,"%lf",&computation_costs[i][j]);
           // printf("%lf ",computation_costs[i][j]);
        }
            
    for(int i=0; i<no_machines; i++)
        for(int j=0; j<no_machines; j++)
            fscanf(fp,"%lf",&data_transfer_rate[i][j]);
            
    for(int i=0; i<no_tasks; i++)
        for(int j=0; j<no_tasks; j++)
            fscanf(fp,"%d",&data[i][j]);

	//vrand = (rand()%999+1)/500.0;
	frequency = (double **)malloc(sizeof(double *)*no_machines);
	vrand = (double **)malloc(sizeof(double *)*no_machines);
	for(int i=0; i<no_machines; i++) {
		frequency[i] = (double *)malloc(sizeof(double)*no_tasks);
		vrand[i] = (double *)malloc(sizeof(double)*no_tasks);
		for(int j=0; j<no_tasks; j++) {
			frequency[i][j] = (rand()%999+1)/500.0;
			vrand[i][j] = (rand()%999+1)/500.0;
                         printf("\n voltage: %0.3lf Frequency: %0.3lf ", vrand[i][j], frequency[i][j] );
		}
                
	}
}

//for the avarage communication cost between the processor
double avg_communicationcost(int source,int destination)
{
    int i,j;
    double avg=0.0;
    for(i=0; i<no_machines; i++)
        for(j=0; j<no_machines; j++)
        {
            if(data_transfer_rate[i][j]!=0)
                avg+=(data[source][destination]/data_transfer_rate[i][j]);
        }
    avg=avg/(no_machines*no_machines-no_machines);
    return avg;
}
