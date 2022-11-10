#include <util.h>
#include <heftt.h>

//for the lower rank calculation
void insertinto_t(int task,double rank)
{
    static int pos;
    int i;
    for(i=pos-1; i>=0; i--)
        if(rank<tasks_lower_rank[sorted_tasks[i]])
            sorted_tasks[i+1]=sorted_tasks[i];
        else
            break;
    sorted_tasks[i+1]=task;
    pos++;
}

//calculate the lower rank of the taks
double calculate_lower_rank(int task)
{
    int j,i;
    double avg_communication_cost,successor,avg=0.0,max=0,rank_successor;
    
    for(i=0; i<no_tasks; i++)
    {
    	if(data[i][task] != -1)
	    {
            for(j=0; j<no_machines; j++)
            {
    	        avg+=computation_costs[i][j];
    	       // printf("avg %d = %lf\n",j,avg);
            } 
            avg/=no_machines;
           // printf("avg = %lf\n",avg);
            avg_communication_cost=avg_communicationcost(i,task);
           // printf("avg communcication cost =%lf\n",avg_communication_cost);
            rank_successor = tasks_lower_rank[i];
           // printf("rank_successor = %lf\n",rank_successor);
            successor=avg_communication_cost+rank_successor + avg;
           // printf("Successor = %lf\n",successor);
            if(max<successor)
                max=successor;
            avg =0;
           // printf("%lf\n",max);
        }
     
    }

    return max;   
}
