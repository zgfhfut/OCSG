#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>  // For sleep function in Linux
#include "random-gcc.h"  // Random Number Generator


#define INF 1.0e14
#define EPS 1.0e-14

#define VMAX 5.0  //the maximal velocity

#define maxpop   30 /*Max population */

#define maxagent    10 /*Max no. of agents */
#define maxgoal    10 /*Max no. of Goals */
#define maxresource    3/*Max no. of resources */

#define min(a,b) (((a)<(b))?(a):(b))

#define MAX_EXPERIMENT  1  /*Experimental times*/

#define MAX_GENERATION  500  /*Max no. of generation*/

double seed;      /*Random Seed*/

#define no_suite  10

int Resource_Agent[maxagent][maxresource] = {
    {40, 19, 35},
    {25, 12, 15},
    {33, 14, 12},
    {11, 22, 45},
    {17, 29, 16},
    {14, 23, 37},
    {31, 42, 29},
    {26, 50, 13},
    {38, 36, 28},
    {12, 49, 16},
};

int Resource_Goal[maxgoal][maxresource] = {
    {29, 54, 41},
    {23, 59, 45},
    {35, 26, 30},
    {33, 44, 30},
    {43, 37, 30},
    {51, 40, 53},
    {51, 51, 27},
    {35, 36, 52},
    {18, 49, 35},
    {52, 48, 26},
};

int Reward_Goal[maxgoal] = {
    65, 57, 78, 53, 62, 83, 61, 54, 97, 50,
};

int Interest_Set[maxagent][maxgoal] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

typedef struct {
    int position[maxgoal][maxagent];
    double velocity[maxgoal][maxagent];
    int Pbest[maxgoal][maxagent];
    int fitness;
    int FitBest;
} particle;

double C1 = 2.0;
double C2 = 2.0;

typedef struct {
    particle ind[maxpop];
    particle *ind_ptr;
} population;

int Gbest[maxgoal][maxagent] = { -1 };
int GolbalBestFit = 0;

void bininit(population *pop_ptr);
void evolve(population *pop_ptr);
void pop_repair(population *pop_ptr);
void repair_xiao(particle *ind_ptr);
void func(population *pop_ptr);
bool no_needed(int agent_res[maxresource], int goal_res[maxresource]);

FILE *bin_ptr;

int main(int argc, char* argv[]) {
    FILE *obj_ptr;
    char file1[500];
    sprintf(file1, "val_suite%d.txt", no_suite);
    obj_ptr = fopen(file1, "w+");

    FILE *tim_ptr;
    char file2[500];
    sprintf(file2, "tim_suite%d.txt", no_suite);
    tim_ptr = fopen(file2, "w+");

    char file4[500];
    sprintf(file4, "enc_suite%d.txt", no_suite);
    bin_ptr = fopen(file4, "w+");

    int int_exp = 0;
    int num_goal;

    for (int_exp = 0; int_exp < MAX_EXPERIMENT; int_exp++) {
        sleep(3);

        printf("experiment %d\n", int_exp + 1);

        int g = 0;
        int s, i, j;

        for (i = 0; i < maxgoal; i++) {
            for (j = 0; j < maxagent; j++) {
                Gbest[i][j] = -1;
            }
        }

        clock_t start, finish;
        double duration = 0;
        start = clock();

        population oldpop, *old_pop_ptr;

        seed = (1.0 / (MAX_EXPERIMENT + 1))*(int_exp + 1);
        warmup_random(seed);

        old_pop_ptr = &(oldpop);
        for (s = 0; s < maxpop; s++) {
            old_pop_ptr->ind[s].fitness = 0;

            for (i = 0; i < maxgoal; i++) {
                for (j = 0; j < maxagent; j++) {
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
        for (s = 0; s < maxpop; s++) {
            old_pop_ptr->ind[s].FitBest = old_pop_ptr->ind[s].fitness;

            for (i = 0; i < maxgoal; i++) {
                for (j = 0; j < maxagent; j++) {
                    old_pop_ptr->ind[s].Pbest[i][j] = old_pop_ptr->ind[s].position[i][j];
                }
            }
        }

        for (s = 0; s < maxpop; s++) {
            if (old_pop_ptr->ind[s].FitBest > GolbalBestFit) {
                GolbalBestFit = old_pop_ptr->ind[s].FitBest;
                for (i = 0; i < maxgoal; i++) {
                    for (j = 0; j < maxagent; j++) {
                        Gbest[i][j] = old_pop_ptr->ind[s].Pbest[i][j];
                    }
                }
            }
        }

        for (g = 0; g < MAX_GENERATION; g++) {
            old_pop_ptr = &(oldpop);
            evolve(old_pop_ptr);

            old_pop_ptr = &(oldpop);
            pop_repair(old_pop_ptr);

            old_pop_ptr = &(oldpop);
            func(old_pop_ptr);

            old_pop_ptr = &(oldpop);
            for (s = 0; s < maxpop; s++) {
                if (old_pop_ptr->ind[s].fitness > old_pop_ptr->ind[s].FitBest) {
                    old_pop_ptr->ind[s].FitBest = old_pop_ptr->ind[s].fitness;

                    for (i = 0; i < maxgoal; i++) {
                        for (j = 0; j < maxagent; j++) {
                            old_pop_ptr->ind[s].Pbest[i][j] = old_pop_ptr->ind[s].position[i][j];
                        }
                    }
                }
            }

            for (s = 0; s < maxpop; s++) {
                if (old_pop_ptr->ind[s].FitBest > GolbalBestFit) {
                    GolbalBestFit = old_pop_ptr->ind[s].FitBest;
                    for (i = 0; i < maxgoal; i++) {
                        for (j = 0; j < maxagent; j++) {
                            Gbest[i][j] = old_pop_ptr->ind[s].Pbest[i][j];
                        }
                    }
                }
            }
        }

        finish = clock();
        duration = (double)(finish - start) / CLOCKS_PER_SEC;

        fprintf(tim_ptr, "%5.4f, ", duration);
        fprintf(obj_ptr, "%d, ", GolbalBestFit);

        fprintf(bin_ptr, "%s", "The best solution:\n");

        for (i = 0; i < maxgoal; i++) {
            for (j = 0; j < maxagent; j++) {
                fprintf(bin_ptr, "%d ", Gbest[i][j]);
            }
            fprintf(bin_ptr, "%s", "\n");
        }
        fprintf(bin_ptr, "%s", "\n\n\n");
    }

    fclose(bin_ptr);

    printf("successful!\n");

    return 0;
}

void func(population *pop_ptr) {
    int i, j, s;
    int reward = 0;
    bool success;

    for (s = 0; s < maxpop; s++) {
        reward = 0;

        for (i = 0; i < maxgoal; i++) {
            success = false;
            for (j = 0; j < maxagent; j++) {
                if (pop_ptr->ind[s].position[i][j] == 1) {
                    success = true;
                    break;
                }
            }

            if (success) {
                reward += Reward_Goal[i];
            }
        }

        pop_ptr->ind[s].fitness = reward;
    }
}

void evolve(population *pop_ptr) {
    double r1 = 0, r2 = 0, r3 = 0, r4 = 0;
    int s, i, j;
    double temp1 = 0;

    for (s = 0; s < maxpop; s++) {
        for (i = 0; i < maxgoal; i++) {
            for (j = 0; j < maxagent; j++) {
                r1 = randomperc();
                r2 = randomperc();

                pop_ptr->ind[s].velocity[i][j] = pop_ptr->ind[s].velocity[i][j] + C1 * r1 * (pop_ptr->ind[s].Pbest[i][j] - pop_ptr->ind[s].position[i][j]) + C2 * r2 * (Gbest[i][j] - pop_ptr->ind[s].position[i][j]);

                if (pop_ptr->ind[s].velocity[i][j] > VMAX) {
                    r3 = randomperc();
                    pop_ptr->ind[s].velocity[i][j] = r3 * VMAX;
                }

                if (pop_ptr->ind[s].velocity[i][j] < -VMAX) {
                    r3 = randomperc();
                    pop_ptr->ind[s].velocity[i][j] = -r3 * VMAX;
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

void bininit(population *pop_ptr) {
    int i, j, s;
    double d;
    pop_ptr->ind_ptr = &(pop_ptr->ind[0]);

    for (s = 0; s < maxpop; s++) {
        for (i = 0; i < maxgoal; i++) {
            for (j = 0; j < maxagent; j++) {
                d = randomperc();

                if (d >= 0.5) {
                    pop_ptr->ind_ptr->position[i][j] = 1;
                } else {
                    pop_ptr->ind_ptr->position[i][j] = 0;
                }

                pop_ptr->ind_ptr->velocity[i][j] = 0;
            }
        }

        pop_ptr->ind_ptr->fitness = 0;
        pop_ptr->ind_ptr = &(pop_ptr->ind[s + 1]);
    }
}

void pop_repair(population *pop_ptr) {
    int s;
    pop_ptr->ind_ptr = &(pop_ptr->ind[0]);

    for (s = 0; s < maxpop; s++) {
        repair_xiao(pop_ptr->ind_ptr);
        pop_ptr->ind_ptr = &(pop_ptr->ind[s + 1]);
    }
}

void repair_xiao(particle *ind_ptr) {
    int i, j, k, r;
    int save_row[maxgoal] = { -1 };
    int row = 0;
    int demo_row = 0;
    int selected = 0;

    for (i = 0; i < maxgoal; i++)
        save_row[i] = i;

    double rnd1, rnd2, rnd3;

    int total[maxresource] = { 0 };
    bool Is_satisfied = true;

    int num_zero = 0;
    int is_zero[maxagent] = { -1 };
    int demo_zero = 0;

    int zero_ava[maxagent] = { -1 };
    int num_zero_ava = 0;

    int num_one = 0;
    int is_one[maxagent] = { -1 };
    int demo_one;

    int availabe[maxagent] = { -1 };
    int num_ava = 0;

    int agent;
    int agent_index;

    int demo_res2;

    int total_con;

    int Residual_Goal[maxgoal][maxresource] = { 0 };
    int Residual_Resource_Agent[maxagent][maxresource] = { 0 };
    int Contribution_Agent[maxagent][maxgoal][maxresource] = { 0 };

    for (j = 0; j < maxagent; j++) {
        for (k = 0; k < maxresource; k++) {
            Residual_Resource_Agent[j][k] = Resource_Agent[j][k];
        }
    }

    for (i = 0; i < maxgoal; i++) {
        for (k = 0; k < maxresource; k++) {
            Residual_Goal[i][k] = Resource_Goal[i][k];
        }
    }

    for (j = 0; j < maxagent; j++) {
        for (i = 0; i < maxgoal; i++) {
            for (k = 0; k < maxresource; k++) {
                Contribution_Agent[j][i][k] = 0;
            }
        }
    }

    row = maxgoal - 1;

    while (row >= 0) {
        num_one = 0;
        num_zero = 0;

        for (j = 0; j < maxagent; j++) {
            is_one[j] = -1;
            zero_ava[j] = -1;
        }

        Is_satisfied = true;

        for (k = 0; k < maxresource; k++)
            total[k] = 0;

        rnd1 = randomperc();
        rnd1 = row * rnd1;
        demo_row = (int)floor(rnd1);
        selected = save_row[demo_row];

        for (j = 0; j < maxagent; j++) {
            if (Interest_Set[j][selected] == 1) {
                for (k = 0; k < maxresource; k++)
                    total[k] += Residual_Resource_Agent[j][k];
            }

            if (ind_ptr->position[selected][j] == 1) {
                if (Interest_Set[j][selected] == 1 && !no_needed(Residual_Resource_Agent[j], Residual_Goal[selected])) {
                    is_one[num_one] = j;
                    num_one++;
                } else {
                    ind_ptr->position[selected][j] = 0;
                }
            } else {
                if (Interest_Set[j][selected] == 1 && !no_needed(Residual_Resource_Agent[j], Residual_Goal[selected])) {
                    is_zero[num_zero] = j;
                    num_zero++;
                }
            }
        }

        for (k = 0; k < maxresource; k++) {
            if (Resource_Goal[selected][k] > total[k]) {
                Is_satisfied = false;
                break;
            }
        }

        if (!Is_satisfied) {
            for (j = 0; j < num_one; j++) {
                if (ind_ptr->position[selected][is_one[j]] == 1) {
                    ind_ptr->position[selected][is_one[j]] = 0;
                }
            }
        } else {
            num_one--;
            num_zero--;

            for (k = 0; k < maxresource; k++) {
                if (Residual_Goal[selected][k] > 0) {
                    num_ava = 0;
                    for (j = 0; j <= num_one; j++) {
                        if (Residual_Resource_Agent[is_one[j]][k] > 0) {
                            availabe[num_ava] = is_one[j];
                            num_ava++;
                        }
                    }

                    num_ava--;

                    while ((Residual_Goal[selected][k] > 0) && (num_ava >= 0)) {
                        rnd2 = randomperc();
                        rnd2 = num_ava * rnd2;
                        demo_one = (int)floor(rnd2);

                        agent = availabe[demo_one];

                        for (j = 0; j <= num_one; j++) {
                            if (is_one[j] == agent) {
                                agent_index = j;
                                break;
                            }
                        }

                        while (agent_index < num_one) {
                            is_one[agent_index] = is_one[agent_index + 1];
                            agent_index++;
                        }
                        num_one--;

                        for (r = 0; r < maxresource; r++) {
                            demo_res2 = min(Residual_Resource_Agent[agent][r], Residual_Goal[selected][r]);

                            Contribution_Agent[agent][selected][r] = demo_res2;
                            Residual_Goal[selected][r] -= demo_res2;
                            Residual_Resource_Agent[agent][r] -= demo_res2;
                        }

                        while (demo_one < num_ava) {
                            availabe[demo_one] = availabe[demo_one + 1];
                            demo_one++;
                        }
                        num_ava--;
                    }
                }
            }

            if (num_one >= 0) {
                for (j = 0; j <= num_one; j++) {
                    ind_ptr->position[selected][is_one[j]] = 0;
                }
            }

            for (k = 0; k < maxresource; k++) {
                if (Residual_Goal[selected][k] > 0) {
                    num_zero_ava = 0;
                    for (j = 0; j <= num_zero; j++) {
                        if (Residual_Resource_Agent[is_zero[j]][k] > 0) {
                            zero_ava[num_zero_ava] = is_zero[j];
                            num_zero_ava++;
                        }
                    }

                    num_zero_ava--;

                    while ((Residual_Goal[selected][k] > 0) && (num_zero_ava >= 0)) {
                        rnd3 = randomperc();
                        rnd3 = num_zero_ava * rnd3;
                        demo_zero = (int)floor(rnd3);

                        agent = zero_ava[demo_zero];

                        ind_ptr->position[selected][agent] = 1;

                        for (j = 0; j <= num_zero; j++) {
                            if (is_zero[j] == agent) {
                                agent_index = j;
                                break;
                            }
                        }
                        while (agent_index < num_zero) {
                            is_zero[agent_index] = is_zero[agent_index + 1];
                            agent_index++;
                        }
                        num_zero--;

                        for (r = 0; r < maxresource; r++) {
                            demo_res2 = min(Residual_Resource_Agent[agent][r], Residual_Goal[selected][r]);

                            Contribution_Agent[agent][selected][r] = demo_res2;
                            Residual_Goal[selected][r] -= demo_res2;
                            Residual_Resource_Agent[agent][r] -= demo_res2;
                        }

                        while (demo_zero < num_zero_ava) {
                            zero_ava[demo_zero] = zero_ava[demo_zero + 1];
                            demo_zero++;
                        }

                        num_zero_ava--;
                    }
                }
            }
        }

        while (demo_row < row) {
            save_row[demo_row] = save_row[demo_row + 1];
            demo_row++;
        }

        row--;
    }
}

bool no_needed(int agent_res[maxresource], int goal_res[maxresource]) {
    int k = 0;

    for (k = 0; k < maxresource; k++) {
        if (min(goal_res[k], agent_res[k]) > 0) {
            return false;
        }
    }

    return true;
}