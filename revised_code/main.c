//#include <util.h>
#include <heftb.h>
//#include <heftt.h>
#include <schedule.h>
//#include <gsa.h>
#include <gsa_p.h>
//#include <pso.h>
#include <time.h>

#define EPS						10e-9

int main() {
	srand(time(NULL));
	int i;
	FILE *fp;
//fp=fopen("Data Sets/ff-17-3.txt","r+");
fp=fopen("Dy-data/sample.txt","r+");
//fp=fopen("Data Sets/input1.txt","r+");
// fp=fopen("Data sets/Cybershake200.txt","r");
 //fp=fopen("Data Sets/Cybershake400.txt","r");
 //fp=fopen("Data Sets/Cybershake100.txt","r");

//fp=fopen("Data Sets/Epigenomic200.txt","r");
// fp=fopen("Data Sets/Epigenomic400.txt","r");
//fp=fopen("Data Sets/Epigenomic100.txt","r");

//fp=fopen("Data Sets/Inspiral202.txt","r");
// fp=fopen("Data Sets/Inspiral402.txt","r");
 //  fp=fopen("Data Sets/Inspiral101.txt","r");

//fp=fopen("Data Sets/Montage202.txt","r");
//fp=fopen("Data Sets/Montage398.txt","r");
// fp=fopen("Data Sets/Montage100.txt","r");

// fp=fopen("Data Sets/Sipht200.txt","r");
// fp=fopen("Data Sets/Sipht400.txt","r");
// fp=fopen("Data Sets/Sipht100.txt","r");


      //parse_dy(fp);
	parse(fp);

	for(int i=0; i<no_tasks; i++) {
		for(int j=0; j<no_tasks; j++) {
			if(data[i][j] == 0)
				data[i][j] = -1;
		}
	}
	//printf("%d\n", data[25][0]);
/*	for(int i=0; i<no_tasks; i++) {
		for(int j=0; j<no_tasks; j++)
			printf("%3d ", data[i][j]);
		putchar('\n');
	} */
	//perform_gsa_p();
	//perform_gsa();
//perform_heftb();
/*
	for(i=no_tasks; i>=0; i--)
    {
        if(tasks_upper_rank[i]==-1)
        {
            tasks_upper_rank[i]=calculate_upper_rank(i);
            insertinto_b(i,tasks_upper_rank[i]);
        }
    }

	printf("**********UPPER RANKS OF TASKS- HEFT-B************ :\n");
    for(i=0; i<no_tasks; i++)
        printf("TASK NO. %d : %.2lf\n",i,tasks_upper_rank[i]);
    for(i=0; i<no_tasks; i++)
        printf("TASK NO. : %d\n",sorted_tasks[i]); 

    make_schedule();
    display_schedule();
    
    for(i=0; i<no_tasks; i++)
       schedule[i].processor=-1;
    
    for(i=0; i<=no_tasks; i++)
    {
        if(tasks_lower_rank[i]==-1)
        {
            tasks_lower_rank[i]=calculate_lower_rank(i);
            insertinto_t(i,tasks_lower_rank[i]);
        }
    }
    printf("\n\n**********LOWER RANKS OF TASKS-HEFT-T************ :\n");
    for(i=0; i<no_tasks; i++)
        printf("TASK NO. %d : %.2lf\n",i,tasks_lower_rank[i]);
    for(i=0; i<no_tasks; i++)
        printf("TASK NO. : %d\n",sorted_tasks[i]); 
        
    make_schedule();
    display_schedule();
*/
	
	perform_gsa_p();
}


