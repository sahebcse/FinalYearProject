#include <pso.h>
#include <stdio.h>
#include <time.h>
#include <util.h>

#define D 10

void PSO()
{

	struct Particle Pop[PSIZE];
	struct Particle Pbest[PSIZE];
	struct Particle Gbest;

    int t=0,i=0,j;
    Create_Populations(Pop);

    for(i=0;i<PSIZE;i++)
    {
      for(j=0;j<T;j++)
        Pbest[i].position[j]=Pop[i].position[j];
      Cal_Fitness(&Pbest[i]);
    } 
    

      for(j=0;j<T;j++)
        Gbest.position[j]=Pop[0].position[j];
      Cal_Fitness(&Gbest);
      

   for(i=0;i<PSIZE;i++)
   {
      printf("\n");
     for(j=0;j<T;j++)
      {
         printf("%0.3f ",Pop[i].position[j]); // always generates a number. 0.0<r<1.0.
     }
      printf(" :    %0.3f ",Pop[i].Fitness[0]);
   }
printf("\nPbset\n");
for(i=0;i<PSIZE;i++)
   {
      printf("\n");
      for(j=0;j<D;j++)
      {
         printf("%0.3f ",Pbest[i].position[j]); // always generates a number. 0.0<r<1.0.
      }
      printf(" :    %0.3f ",Pbest[i].Fitness[0]);
   }

    while(t<ITE)
    {
       for(i=0;i<PSIZE;i++)
       {
           Velocity_Position_Update(&Pop[i], Pbest[i], Gbest);
           Update_PbestGbest(Pop[i],&Pbest[i], &Gbest);
       }
       t++;
       //printf("\nIteration %2d: %0.3f ",t,Gbest.Fitness[0]);
       //printf("%0.3f, ",Gbest.Fitness[0]);
    } 
    
   ///Final Result///
   float x=0.0;
   int pro_no=-1;
   float FT[PROC];
    
    for(i=0;i<PROC;i++)
      FT[i]=0.0;
      
   for(i=0;i<T;i++)
    {
    	 x=0.0;
         pro_no=-1;
         while(Gbest.position[i]>x)
         {
            x=x+1.0/(float)PROC;
            pro_no++;
         }
         FT[pro_no]=FT[pro_no]+(float)computation_costs[i][pro_no];
       printf("Task-no=%d on processor%d, %0.3f} \n",i,pro_no,computation_costs[i][pro_no]);
	}
	
	printf("\n\n### FINAL SCHEDULING for PSO. ####\n==========================");
	for(i=0;i<PROC;i++)
	  printf("\nProcessor %d: %0.3f ",i,FT[i]);
    printf("\n                      Makespan: %0.3f, UT=%0.3f, LB=%0.3f\n\n",Gbest.Fitness[1],Gbest.Fitness[2],Gbest.Fitness[3]);
}
