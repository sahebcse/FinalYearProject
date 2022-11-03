import sys
import pandas as pd

import networkx as nx
import numpy as np
import matplotlib.pyplot as plt


graph=[]
vertices=[]
vertices_no=0

def add_vertex(v):
    global graph
    global vertices_no
    global vertices
    if v in vertices:
        print("Vertex",v,"already exists")
    else:
        vertices_no=vertices_no+1
        vertices.append(v)
        if vertices_no >1:
            for vertex in graph:
                vertex.append(0)
        temp=[]
        for i in range(vertices_no):
            temp.append(0)
        graph.append(temp)
    
def add_edge(v1,v2,e):
    global graph
    global vertices_no
    global vertices
    
    if v1 not in vertices:
        print("Vertex ",v1,"doesn't exist.")
    elif v2 not in vertices:
        print("Vertex ",v2,"doesn't exist.")
    else:
        index1=vertices.index(v1)
        index2=vertices.index(v2)
        graph[index1][index2]=e


def print_graph():
    global graph
    global vertices_no
    for i in range(vertices_no):
        for j in range(vertices_no):
            if graph[i][j]!=0:
                print(vertices[i],"-> ",vertices[j]," edge weight: ",graph[i][j])
                

def generate_graph(tasks,edges):
    global graph
    for task in range(tasks):
        add_vertex('T'+str(task+1))
    for edge in edges:
        add_edge(edge[0],edge[1],edge[2])
    return graph

def show_wgraph(G):
    plt.figure() 
    pos = nx.spring_layout(G)
    weight_labels = nx.get_edge_attributes(G,'weight')
    nx.draw(G,pos,font_color = 'white', node_shape = 's', with_labels = True,)
    output = nx.draw_networkx_edge_labels(G,pos,edge_labels=weight_labels)
    

def visualize_graph():
    G = nx.DiGraph()
    for task in range(tasks):
        G.add_node('T'+str(task+1))
    for edge in edges:
        G.add_edge(edge[0],edge[1],weight=edge[2])
    show_wgraph(G)
                

# Expected processing time = no of instructions * CPI(avg.) /Clockrate
def calculate_processing_times(processors,tasks,processor_clock_speed,instruction_per_task,CPI):
    expected_processing_times={}
    for processor in range(processors):
        task_processing_time=[]
        for task in range(tasks):
            processing_time=(instruction_per_task *CPI)/(processor_clock_speed[processor]*pow(10,9))
            processing_time=round(processing_time, 3)
            task_processing_time.append(processing_time)
        expected_processing_times.update({'P'+str(processor+1):task_processing_time})
    return expected_processing_times

def max_predecessor(task,final_order):
    predecessors=latency.index[latency['T'+str(task)] > 0].tolist()
    
    if not predecessors:
        return 0

    maxTime=-sys.maxsize-1
    for pred_task,pred_value in final_order.items():
        if pred_task in predecessors and maxTime <pred_value[1]:
                maxTime=pred_value[1]
    return maxTime
    
def adj_matrix_to_list_(graph):
    adj_List={}
    for i in range(len(graph)):
        temp_list=[]
        for j in range(len(graph)):
            if graph[i][j]!=0:
                temp_list.append('T'+str(graph[i][j]))
        adj_List.update({'T'+str(i+1):temp_list})
            
    return adj_List
                
def topological_sort(start, visited, sort,adj_list):
    current = start
    visited.append(current)
    neighbors = adj_list[current]
    for neighbor in neighbors:
        if neighbor not in visited:
            sort = topological_sort(neighbor, visited, sort,adj_list)
    sort.append(current)
    if len(visited) != len(vertices):
        for vertice in vertices:
            if vertice not in visited:
                sort = topological_sort(vertice, visited, sort,adj_list)
    return sort

def generate_task_sequence():
    pass

def schedule_tasks(processors,tasks,sequence_order,latency,EPT_Matrix):
    final_order={}
    p_clock=[0]*processors
    p_idle_time=[0]*processors
    p_tasks=[0]*processors

    for i in range(tasks):
        task=sequence_order[i][1:2]
        minCost=sys.maxsize
        processor_used=0
    
        for processor in range(processors): 
            predecessor_completion_time=max_predecessor(task,final_order)
            present_processor_time=p_clock[processor]
            max_time= max(predecessor_completion_time,present_processor_time)
        
            burst_time=EPT_Matrix['P'+str(processor+1)]['T'+str(task)]
        
            total_time=max_time+burst_time
#             print("Using Processor "+str(processor+1)+" for task "+str(task)+":")
#             print("pred_time : "+str(predecessor_completion_time))
#             print("present time: "+str(present_processor_time))
#             print("max_time: "+str(max_time))
#             print("burst_time: "+str(burst_time))
#             print("total time: "+str(total_time))
#             print("\n")

            if(total_time<minCost):
                minCost=total_time
                processor_used=processor
        
        minCost=round(minCost,3)
#         print("PROCESSOR "+str(processor_used+1)+" USED WITH "+str(minCost)+" TIME\n")
        
        for processor in range(processors):
            if processor == processor_used:
                p_tasks[processor]+=1
                continue
            p_idle_time[processor]+=minCost
            
        p_clock[processor_used]+=minCost
        final_order.update({'T'+str(task): ['P'+str(processor_used+1),minCost]})
        print("processor "+str(processor_used+1)+" takes minimum time of "+str(minCost)+" time units for task "+str(task)+
          " and ends at "+str(p_clock[processor_used])+" time unit ")
    print("\n")
    print("Individual time taken by processors are "+str(p_clock)+"\n")
    print("Total time taken for all tasks : "+str(max(p_clock))+" time units\n")
    print("Idle time of each processors are : "+str(p_idle_time)+"\n")
    print("Total no. of tasks executed by each processor are : "+str(p_tasks)+"\n")
    print(str(final_order)+"\n")



    
    

if __name__=="__main__":
	tasks=8
	processors=3
	processor_clock_speed= [3.5,2,2.5] #In GHz
	instruction_count= 300 * pow(10,6) 
	CPI=4
	edges=[['T1','T2',3],
		   ['T1','T3',2],
		   ['T2','T4',2],
		   ['T3','T5',2],
		   ['T4','T6',5],
		   ['T5','T6',4],
		   ['T8','T3',2],
		   ['T7','T6',2]]


	#-------------------------------------------------

	adj_matrix=generate_graph(tasks,edges)

	#Currently latency data is used to check for predecessors, but delay is not currently added
	latency_data={}
	for task in range(tasks):
		latency_data.update({'T'+str(task+1):adj_matrix[task]})
	latency = pd.DataFrame(latency_data, index=vertices).transpose()

	instruction_per_task= instruction_count/tasks
	expected_processing_times=calculate_processing_times(processors,tasks,processor_clock_speed,instruction_per_task,CPI)
	EPT_Matrix = pd.DataFrame(expected_processing_times, index=vertices)

	# sequence_order=generate_sequence()
	sequence_order=['T1', 'T8', 'T7', 'T2', 'T3', 'T4', 'T5', 'T6']
	schedule_tasks(processors,tasks,sequence_order,latency,EPT_Matrix)

