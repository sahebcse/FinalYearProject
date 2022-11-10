#include <util.h>
#include <cpop.h>
#include <heftb.h>
#include <heftt.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <schedule.h>

#define left(i)				(i << 1)
#define right(i)			((i << 1) + 1)
#define parent(i)			(i >> 1)

#define EPS					10e-9

int *harr;		// heap array for priority queue
int hsize;

typedef struct {
	int id;				// node id
	double rank_sum;
} info;

info *node_infos;
_Bool *is_cpm;
int cpm_count;
int *elapsed_time;			// for each processor
_Bool *is_done;

void heapify(int root) {
	int max = node_infos[harr[root]].rank_sum;
	int max_node = root;
	int l = left(root), r = right(root);

	if(l < hsize && node_infos[harr[l]].rank_sum > max) {
		max = node_infos[harr[l]].rank_sum;
		max_node = l;
	}
	if(r < hsize && node_infos[harr[r]].rank_sum > max) {
		max = node_infos[harr[r]].rank_sum;
		max_node = r;
	}

	if(max_node != root) {
		int t = harr[root];
		harr[root] = harr[max_node];
		harr[max_node] = t;
		heapify(max_node);
	}
}

void insert_heap(int node) {
	harr[hsize++] = node;
	int i = hsize-1;
	while((i > 1) && (node_infos[harr[parent(i)]].rank_sum < node_infos[harr[i]].rank_sum)) {
		int t = harr[parent(i)];
		harr[parent(i)] = harr[i];
		harr[i] = t;
		i = parent(i);
	}
}

int delete_heap() {
	int root = harr[1];
	harr[1] = harr[hsize-1];
	hsize--;
	heapify(1);
	return root;
}

void perform() {
	node_infos = (info *)malloc(no_tasks*sizeof(info));
	is_cpm = (_Bool *)malloc(no_tasks*sizeof(_Bool));
	elapsed_time = (int *)malloc(no_machines*sizeof(int));
	is_done = (_Bool *)malloc(no_tasks*sizeof(_Bool));

	for(int i=0; i<no_tasks; i++) {
		node_infos[i].id = i;
		node_infos[i].rank_sum = tasks_upper_rank[i] + tasks_lower_rank[i];
	}
	
	double cp = node_infos[0].rank_sum;
	int cpm[no_tasks];
	for(int i=0; i<no_tasks; i++) {
		if(fabs(node_infos[i].rank_sum - cp) < EPS) {
			cpm[cpm_count++] = i;
			is_cpm[i] = 1;
		}
	}

	printf("\n%lf\n", cp);
	for(int i=0; i<no_tasks; i++) {
		//printf("%d %lf %d\n", node_infos[i].id, node_infos[i].rank_sum, is_cpm[i]);
	}
	
	int min_proc = INT_MAX;
	int cpp = 0;
	for(int i=0; i<no_machines; i++) {
		int sum = 0;
		for(int j=0; j<cpm_count; j++)
			sum += computation_costs[j][i];
		if(sum < min_proc) {
			min_proc = sum;
			cpp = i;
		}
	}

	harr = (int *)malloc((no_tasks+1)*sizeof(int));
	hsize = 1;
	//printf("%d\n", cpp);
	insert_heap(0);
	while(hsize != 1) {
		int nd = delete_heap();
		if(is_cpm[nd]) {
			//printf("%d scheduled on %d", nd, cpp);
			schedule[nd].processor = cpp;
			schedule[nd].AST=elapsed_time[cpp];
			schedule[nd].AFT=computation_costs[nd][cpp] + elapsed_time[cpp];
			elapsed_time[cpp] = schedule[nd].AFT;
		}
		else {
			struct TaskProcessor *proc = do_schedule(nd);
			//printf("%d scheduled on %d", nd, proc->processor);
			schedule[nd].processor = proc->processor;
			schedule[nd].AST=proc->AST;
			schedule[nd].AFT=proc->AFT;
		//	elapsed_time[proc->processor] = schedule[nd].AFT;
		}
		is_done[nd] = 1;

		for(int j=0; j<no_tasks; j++) {
			if(data[nd][j] != -1) {
				// j is probable next entry				
				_Bool is_ready = 1;
				for(int k=0; k<no_tasks; k++) {
					if(data[k][j] != -1) {
						// k should be completed for j to be ready
						if(!is_done[k]) {
							is_ready = 0;
							break;
						}
					}
				}
				
				if(is_ready)
					insert_heap(j);
			}
		}
	}
}

