
// BPSO-OCSG.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "stdio.h"
#include "stdlib.h"
#include "math.h"


#include "time.h"

#include "string.h"
/********************************************************************/
#include "random.h"       /*Random Number Generator*/
/********************************************************************/

# define INF 1.0e14
# define EPS 1.0e-14

#define VMAX 5.0  //the maximal velocity

#define maxpop   30 /*Max population */

#define maxagent    10 /*Max no. of agents */
#define maxgoal    10 /*Max no. of Goals */
#define maxresource    3/*Max no. of resources */

#define min(a,b) (((a)<(b))?(a):(b))
/********************************************************************/
#define MAX_EXPERIMENT  1  /*Experimental times*/

#define MAX_GENERATION  500  /*Max no. of generation*/
/********************************************************************/
/********************************************************************/
double seed;      /*Random Seed*/

#define no_suite  10




//int num_one_operations;
//int num_zero_operations;

int Resource_Agent[maxagent][maxresource] = {
	40, 19, 35,
	25, 12, 15,
	33, 14, 12,
	11, 22, 45,
	17, 29, 16,
	14, 23, 37,
	31, 42, 29,
	26, 50, 13,
	38, 36, 28,
	12, 49, 16,
};


int Resource_Goal[maxgoal][maxresource] = {
	29, 54, 41,
	23, 59, 45,
	35, 26, 30,
	33, 44, 30,
	43, 37, 30,
	51, 40, 53,
	51, 51, 27,
	35, 36, 52,
	18, 49, 35,
	52, 48, 26,
};



int Reward_Goal[maxgoal] = {
	65, 57, 78, 53, 62, 83, 61, 54, 97, 50, };


int Interest_Set[maxagent][maxgoal] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};


typedef struct       /*particle properties*/
{
	int position[maxgoal][maxagent]; /*particle's position*/
	double velocity[maxgoal][maxagent];/*particle's velocity*/
	int Pbest[maxgoal][maxagent]; /*particle's current best position*/
	int fitness;/*Fitness values */
	int FitBest;/*particle's current best Fitness*/
}particle;        /*Structure defining particle*/

//the best fitness in the current generation

double C1 = 2.0;//social parameters
double C2 = 2.0;

typedef struct
{
	particle ind[maxpop]; /*Different Individuals*/
	particle *ind_ptr;

}population;             /*Population Structure*/

int Gbest[maxgoal][maxagent] = { -1 };
int GolbalBestFit = 0;
/********************************************************************/

void bininit(population *pop_ptr);/*initializes the population*/

void evolve(population *pop_ptr);/*population evolution*/

void pop_repair(population *pop_ptr);

//void repair_zhang(particle *ind_ptr);/*zhang et al.'s algorithm for individual repairs*/
//void check_row(particle *ind_ptr);
//void check_column(particle *ind_ptr);
void repair_xiao(particle *ind_ptr);

//void our_repair(particle *ind_ptr);/*our heuristic for individual repairs*/

void func(population *pop_ptr);/*evaluate the value of the function*/

bool no_needed(int agent_res[maxresource], int goal_res[maxresource]);


FILE *bin_ptr;

/********************************************************************/
void sleep(clock_t wait)
{
	clock_t goal;
	goal = wait + clock();
	while (goal > clock());
}

int _tmain(int argc, _TCHAR* argv[])
{

	FILE *obj_ptr;
	char file1[500];
	sprintf_s(file1, "val_suite%d.txt", no_suite);
	fopen_s(&obj_ptr, file1, "w+");


	FILE *tim_ptr;
	char file2[500];
	sprintf_s(file2, "tim_suite%d.txt", no_suite);
	fopen_s(&tim_ptr, file2, "w+");

	char file4[500];
	sprintf_s(file4, "enc_suite%d.txt", no_suite);
	fopen_s(&bin_ptr, file4, "w+");


	int int_exp = 0;
	int num_goal;

	for (int_exp = 0; int_exp < MAX_EXPERIMENT; int_exp++)
	{
		sleep((clock_t)3 * CLOCKS_PER_SEC);

		printf_s("experiment %d\n", int_exp + 1);

		int g = 0;
		int s, i, j;

		for (i = 0; i < maxgoal; i++)
		{
			for (j = 0; j < maxagent; j++)
			{
				Gbest[i][j] = -1;
			}
		}


		clock_t start, finish;
		double duration = 0;
		start = clock();


		population oldpop, *old_pop_ptr;//Defining the population Structures

		seed = (1.0 / (MAX_EXPERIMENT + 1))*(int_exp + 1);
		warmup_random(seed);

		old_pop_ptr = &(oldpop);
		for (s = 0; s < maxpop; s++)
		{
			old_pop_ptr->ind[s].fitness = 0;

			for (i = 0; i < maxgoal; i++)
			{
				for (j = 0; j < maxagent; j++)
				{
					old_pop_ptr->ind[s].position[i][j] = -1;
					old_pop_ptr->ind[s].velocity[i][j] = -1.0;
				}
			}
		}

		old_pop_ptr = &(oldpop);
		bininit(old_pop_ptr);

		old_pop_ptr = &(oldpop);
		pop_repair(old_pop_ptr);
		old_pop_ptr = &(oldpop);
		func(old_pop_ptr);

		GolbalBestFit = -65535;

		old_pop_ptr = &(oldpop);
		for (s = 0; s < maxpop; s++)
		{
			old_pop_ptr->ind[s].FitBest = old_pop_ptr->ind[s].fitness;

			for (i = 0; i < maxgoal; i++)
			{
				for (j = 0; j < maxagent; j++)
				{
					old_pop_ptr->ind[s].Pbest[i][j] = old_pop_ptr->ind[s].position[i][j];
				}
			}
		}

		for (s = 0; s < maxpop; s++)
		{
			if (old_pop_ptr->ind[s].FitBest > GolbalBestFit)
			{
				GolbalBestFit = old_pop_ptr->ind[s].FitBest;
				for (i = 0; i < maxgoal; i++)
				{
					for (j = 0; j < maxagent; j++)
					{
						Gbest[i][j] = old_pop_ptr->ind[s].Pbest[i][j];
					}
				}
			}
		}

		for (g = 0; g < MAX_GENERATION; g++)
		{
			old_pop_ptr = &(oldpop);
			evolve(old_pop_ptr); //population evolution	

			old_pop_ptr = &(oldpop);
			pop_repair(old_pop_ptr);


			old_pop_ptr = &(oldpop);
			func(old_pop_ptr);//----------FUNCTION EVALUATION-----------

			old_pop_ptr = &(oldpop);
			for (s = 0; s < maxpop; s++)
			{
				if (old_pop_ptr->ind[s].fitness>old_pop_ptr->ind[s].FitBest)
				{
					old_pop_ptr->ind[s].FitBest = old_pop_ptr->ind[s].fitness;

					for (i = 0; i < maxgoal; i++)
					{
						for (j = 0; j < maxagent; j++)
						{
							old_pop_ptr->ind[s].Pbest[i][j] = old_pop_ptr->ind[s].position[i][j];
						}
					}
				}
			}

			for (s = 0; s < maxpop; s++)
			{
				if (old_pop_ptr->ind[s].FitBest > GolbalBestFit)
				{
					GolbalBestFit = old_pop_ptr->ind[s].FitBest;
					for (i = 0; i < maxgoal; i++)
					{
						for (j = 0; j < maxagent; j++)
						{
							Gbest[i][j] = old_pop_ptr->ind[s].Pbest[i][j];
						}
					}
				}
			}

		}

		finish = clock();
		duration = (double)(finish - start) / CLOCKS_PER_SEC;
		/*report*/

		//////////////////////////////////////////////////////////////////////////	

		fprintf_s(tim_ptr, "%5.4f, ", duration);
		//////////////////////////////////////////////////////////////////////////	
		
		fprintf_s(obj_ptr, "%d, ", GolbalBestFit);


		fprintf_s(bin_ptr, "%s", "The best solution:\n");

		for (i = 0; i < maxgoal; i++)
		{
			for (j = 0; j < maxagent; j++)
			{
				fprintf_s(bin_ptr, "%d ", Gbest[i][j]);
			}
			fprintf_s(bin_ptr, "%s", "\n");
		}
		fprintf_s(bin_ptr, "%s", "\n\n\n");

	}


	fclose(bin_ptr);

	printf_s("successful!\n");
	//getchar();

	return 0;
}


void func(population *pop_ptr)
{
	int i, j, s;

	int reward = 0;

	int demo = 0;

	bool success;

	for (s = 0; s < maxpop; s++)
	{
		reward = 0;

		for (i = 0; i < maxgoal; i++)
		{

			success = false;
			for (j = 0; j < maxagent; j++)
			{
				if (pop_ptr->ind[s].position[i][j] == 1)// the agent a_j is in coalition C_i
				{

					success = true;
					break;

				}
			}

			if (success)
			{
				reward += Reward_Goal[i];
			}

		}

		pop_ptr->ind[s].fitness = reward;
	}
}


void evolve(population *pop_ptr)
{
	double r1 = 0, r2 = 0, r3 = 0, r4 = 0;

	int s, i, j;

	double temp1 = 0;

	for (s = 0; s < maxpop; s++)
	{
		for (i = 0; i < maxgoal; i++)
		{
			for (j = 0; j < maxagent; j++)
			{
				r1 = randomperc();
				r2 = randomperc();

				pop_ptr->ind[s].velocity[i][j] = pop_ptr->ind[s].velocity[i][j] + C1*r1*(pop_ptr->ind[s].Pbest[i][j] - pop_ptr->ind[s].position[i][j]) + C2*r2*(Gbest[i][j] - pop_ptr->ind[s].position[i][j]);

				if (pop_ptr->ind[s].velocity[i][j] >VMAX)
				{
					r3 = randomperc();
					pop_ptr->ind[s].velocity[i][j] = r3*VMAX;
				}

				if (pop_ptr->ind[s].velocity[i][j] <-VMAX)
				{
					r3 = randomperc();
					pop_ptr->ind[s].velocity[i][j] = -r3*VMAX;
				}


				temp1 = 1 / (1 + exp(-pop_ptr->ind[s].velocity[i][j]));

				r4 = randomperc();

				if (r4 < temp1)
					pop_ptr->ind[s].position[i][j] = 1;
				else
					pop_ptr->ind[s].position[i][j] = 0;
			}
		}
	}

}

void bininit(population *pop_ptr)
{
	int i, j, s;
	double d;
	pop_ptr->ind_ptr = &(pop_ptr->ind[0]);

	for (s = 0; s < maxpop; s++)
	{
		for (i = 0; i < maxgoal; i++)
		{
			for (j = 0; j < maxagent; j++)
			{
				/*Generate a Random No. if it is less than 0.5 it generates a 0 in the string otherwise 1*/
				d = randomperc();

				if (d >= 0.5)
				{
					pop_ptr->ind_ptr->position[i][j] = 1;
				}
				else
				{
					pop_ptr->ind_ptr->position[i][j] = 0;
				}

				pop_ptr->ind_ptr->velocity[i][j] = 0;
			}
		}

		pop_ptr->ind_ptr->fitness = 0;

		pop_ptr->ind_ptr = &(pop_ptr->ind[s + 1]);
	}

}

void pop_repair(population *pop_ptr)
{
	int s;

	pop_ptr->ind_ptr = &(pop_ptr->ind[0]);

	for (s = 0; s < maxpop; s++)
	{

		repair_xiao(pop_ptr->ind_ptr);
		pop_ptr->ind_ptr = &(pop_ptr->ind[s + 1]);
	}
}

void repair_xiao(particle *ind_ptr)
{
	int i, j, k, r;

	int save_row[maxgoal] = { -1 };//record the selected row
	int row = 0;
	int demo_row = 0;
	int selected = 0;//the current row

	for (i = 0; i < maxgoal; i++)
		save_row[i] = i;//save the row index

	double rnd1, rnd2, rnd3;

	int total[maxresource] = { 0 };
	bool Is_satisfied = true;



	int num_zero = 0;
	int is_zero[maxagent] = { -1 };//the index for bits ``0'' in a row
	int demo_zero = 0;

	int zero_ava[maxagent] = { -1 };
	int num_zero_ava = 0;

	int num_one = 0;
	int is_one[maxagent] = { -1 };//the index for bits ``1'' in a row
	int demo_one;

	int availabe[maxagent] = { -1 };
	int num_ava = 0;
	//int total_need;// The sum of all resources required by the goal now



	int agent;//the a_j which is selected.
	int agent_index;

	//int demo_res1;
	int demo_res2;


	int total_con;




	int Residual_Goal[maxgoal][maxresource] = { 0 };

	int Residual_Resource_Agent[maxagent][maxresource] = { 0 };

	int Contribution_Agent[maxagent][maxgoal][maxresource] = { 0 };

	for (j = 0; j < maxagent; j++)
	{
		for (k = 0; k < maxresource; k++)
		{
			Residual_Resource_Agent[j][k] = Resource_Agent[j][k];
		}
	}

	for (i = 0; i < maxgoal; i++)
	{
		for (k = 0; k < maxresource; k++)
		{
			Residual_Goal[i][k] = Resource_Goal[i][k];
		}
	}

	for (j = 0; j < maxagent; j++)
	{
		for (i = 0; i < maxgoal; i++)
		{
			for (k = 0; k < maxresource; k++)
			{
				Contribution_Agent[j][i][k] = 0;
			}
		}
	}
	row = maxgoal - 1;
	//row = maxgoal;

	while (row >= 0)
	{
		num_one = 0;
		num_zero = 0;

		for (j = 0; j < maxagent; j++)
		{
			is_one[j] = -1;
			zero_ava[j] = -1;
		}

		Is_satisfied = true;

		for (k = 0; k < maxresource; k++)
			total[k] = 0;

		rnd1 = randomperc();
		rnd1 = row * rnd1;
		demo_row = (int)floor(rnd1);
		selected = save_row[demo_row];//select randomly a row
		/****************************************************************************************/
		//determine whether the goal can be satisfied?
		for (j = 0; j < maxagent; j++)
		{
			if (Interest_Set[j][selected] == 1)
			{
				for (k = 0; k < maxresource; k++)
					total[k] += Residual_Resource_Agent[j][k];
			}

			if (ind_ptr->position[selected][j] == 1)
			{
				if (Interest_Set[j][selected] == 1 && !no_needed(Residual_Resource_Agent[j], Residual_Goal[selected]))
				{
					is_one[num_one] = j;
					num_one++;
				}
				else
				{
					ind_ptr->position[selected][j] = 0;
					//num_zero_operations++;
				}
			}
			else//the bit is "0"
			{
				if (Interest_Set[j][selected] == 1 && !no_needed(Residual_Resource_Agent[j], Residual_Goal[selected]))
				{
					is_zero[num_zero] = j;
					num_zero++;
				}
			}
		}

		for (k = 0; k < maxresource; k++)
		{
			if (Resource_Goal[selected][k] > total[k])//the total resources of all the valid agents cannot satisfy the needs of the selected goal
			{
				Is_satisfied = false;
				break;
			}
		}

		if (!Is_satisfied)//cannot satisfy
		{
			for (j = 0; j < num_one; j++)
			{
				if (ind_ptr->position[selected][is_one[j]] == 1)
				{
					ind_ptr->position[selected][is_one[j]] = 0;//let all bits ``1'' exit from the coalition
					//num_zero_operations++;
				}
			}
		}
		else//can satisfy
		{


			num_one--;
			num_zero--;


			//printf("%s", "interest:");
			//for (j = 0; j < maxagent; j++)
			//{
			//	printf("%d, ", Interest_Set[j][selected]);
			//}
			//printf("\n");
			//printf("%s", "bin_code:");
			//for (j = 0; j < maxagent; j++)
			//{
			//	printf("%d, ", ind_ptr->position[selected][j]);
			//}
			//printf("\n");
			//printf("\n");



			for (k = 0; k < maxresource; k++)
			{
				if (Residual_Goal[selected][k] > 0)
				{
					num_ava = 0;
					for (j = 0; j <= num_one; j++)
					{
						if (Residual_Resource_Agent[is_one[j]][k]>0)
						{
							availabe[num_ava] = is_one[j];
							num_ava++;
						}
					}

					num_ava--;

					while ((Residual_Goal[selected][k] > 0) && (num_ava >= 0))//
					{
						rnd2 = randomperc();
						rnd2 = num_ava * rnd2;
						demo_one = (int)floor(rnd2);

						agent = availabe[demo_one];


						for (j = 0; j <= num_one; j++)
						{
							if (is_one[j] == agent)
							{
								agent_index = j;
								break;
							}
						}

						while (agent_index < num_one)
						{
							is_one[agent_index] = is_one[agent_index + 1];//delete the checked column
							agent_index++;
						}
						num_one--;


						for (r = 0; r < maxresource; r++)
						{

							demo_res2 = min(Residual_Resource_Agent[agent][r], Residual_Goal[selected][r]);

							Contribution_Agent[agent][selected][r] = demo_res2;
							Residual_Goal[selected][r] -= demo_res2;
							Residual_Resource_Agent[agent][r] -= demo_res2;
						}


						while (demo_one < num_ava)
						{
							availabe[demo_one] = availabe[demo_one + 1];//delete the checked column
							demo_one++;
						}
						num_ava--;
					}
				}
			}

			if (num_one >= 0)//some bits ``1'' are useless
			{
				for (j = 0; j <= num_one; j++)
				{
					ind_ptr->position[selected][is_one[j]] = 0;//covert
					//num_zero_operations++;
				}
			}


			for (k = 0; k < maxresource; k++)
			{
				if (Residual_Goal[selected][k] > 0)
				{

					num_zero_ava = 0;
					for (j = 0; j <= num_zero; j++)
					{
						if (Residual_Resource_Agent[is_zero[j]][k] > 0)
						{
							zero_ava[num_zero_ava] = is_zero[j];
							num_zero_ava++;
						}
					}

					num_zero_ava--;


					while ((Residual_Goal[selected][k] > 0) && (num_zero_ava >= 0))//
					{
						rnd3 = randomperc();
						rnd3 = num_zero_ava * rnd3;
						demo_zero = (int)floor(rnd3);

						agent = zero_ava[demo_zero];

						ind_ptr->position[selected][agent] = 1;//convert


						for (j = 0; j <= num_zero; j++)
						{
							if (is_zero[j] == agent)
							{
								agent_index = j;
								break;
							}
						}
						while (agent_index < num_zero)
						{
							is_zero[agent_index] = is_zero[agent_index + 1];//delete the checked column
							agent_index++;
						}
						num_zero--;

						for (r = 0; r < maxresource; r++)
						{
							demo_res2 = min(Residual_Resource_Agent[agent][r], Residual_Goal[selected][r]);

							Contribution_Agent[agent][selected][r] = demo_res2;
							Residual_Goal[selected][r] -= demo_res2;
							Residual_Resource_Agent[agent][r] -= demo_res2;
						}

						while (demo_zero < num_zero_ava)
						{
							zero_ava[demo_zero] = zero_ava[demo_zero + 1];//delete the checked column
							demo_zero++;
						}

						num_zero_ava--;

					}
				}

			}

		}


		//for (j = 0; j < maxagent; j++)
		//{
		//	total_con = 0;
		//	for (r = 0; r < maxresource; r++)
		//	{
		//		total_con += Contribution_Agent[j][selected][r];

		//	}
		//	if (total_con>0)
		//		ind_ptr->position[selected][j] = 1;
		//	else
		//		ind_ptr->position[selected][j] = 0;
		//}








		/****************************************************************************************/
		while (demo_row <row)
		{
			save_row[demo_row] = save_row[demo_row + 1];//delete the checked row
			demo_row++;
		}

		row--;
	}



	/*	for (i = 0; i < maxgoal; i++)
	{
	for (j = 0; j < maxagent; j++)
	{
	fprintf_s(bin_ptr, "%d ", ind_ptr->position[i][j]);
	}
	fprintf_s(bin_ptr, "%s", "\n");
	}
	fprintf_s(bin_ptr, "%s", "\n\n\n");*/
}


bool no_needed(int agent_res[maxresource], int goal_res[maxresource])
{
	int k = 0;

	for (k = 0; k < maxresource; k++)
	{
		if (min(goal_res[k], agent_res[k]) > 0)// has required resource
		{
			return false;
		}
	}

	return true;
}
