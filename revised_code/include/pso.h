#ifndef PSO_H_
#define PSO_H_

#define PROC 3 // Number of processor
#define T 8  // Number of Tasks
#define PSIZE 10   // Population size
#define BEST 3  // Few BEST out of PSIZE
#define MUTE 30
#define MAXOBJ 3   // No. of Objectives
#define ITE 400    //No. of Iterations

static float ET[T][PROC];

struct Particle
{
 float position[T];
 float Fitness[MAXOBJ+1];
 float V[T];
};

void Velocity_Position_Update(struct Particle *P, struct Particle Pbest, struct Particle Gbest);

void Update_PbestGbest(struct Particle P, struct Particle *Pbest, struct Particle *Gbest);

void Cal_Fitness(struct Particle *P);

void Create_Populations(struct Particle *Pop);

void PSO();

#endif


