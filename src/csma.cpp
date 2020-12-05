#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <set>
#include <fstream>
#include <iostream>
#include "csma.h"
#include <time.h>


#define random(x) (rand()%x)

using namespace std;

int N, L, R, M, T;
vector<node_t> node_list;
int channel_idle_count;
int channel_occpy_count;
int channel_collision_count;
vector<int> R_list;

void read_input(FILE* f){
    fscanf(f,"N %d\n", &N);
    fscanf(f,"L %d\n", &L);
    int temp;
    fscanf(f,"R %d",&temp);
    R_list.push_back(temp);
    while(fscanf(f," %d",&temp)==1){
        R_list.push_back(temp);
    }
    R = R_list[0];
    fscanf(f,"M %d\n", &M);
    fscanf(f,"T %d", &T);
    fclose(f);
}

void init_simulization(){
    // init node list
    srand((unsigned)clock());
    for (int i = 0;i < N;i++){
        node_t n;
        n.id = i;
        n.collision_count = 0;
        n.backoff = random(R);
        n.accum_transmission_count = 0;
        n.accum_collision_count = 0;
        node_list.push_back(n);
    }
    channel_idle_count = 0;
    channel_collision_count = 0;
    channel_occpy_count = 0;
}

float get_var(float *x, int N){
    float mean = 0.0;
    float var = 0.0;
    for (int i = 0;i < N;i++){
        mean += x[i];
    }
    mean = mean / N;
    
    for (int i = 0;i < N;i++){
        var += (x[i] - mean) * (x[i] - mean);
    }
    var = var / N;

    return var;
}

void write_output(FILE* f){
    float transmiss_count_lst[N];
    for (int i = 0;i < N;i++){
        transmiss_count_lst[i] = (float) node_list[i].accum_transmission_count;
    }
    float transmiss_var = get_var(transmiss_count_lst, N);

    float collision_count_lst[N];
    for (int i = 0;i < N;i++){
        collision_count_lst[i] = (float) node_list[i].accum_collision_count;
        // printf("%d\n",node_list[i].accum_collision_count);
    }
    float collision_var = get_var(collision_count_lst, N);

    fprintf(f, "Channel utilization (in percentage) %f\n",channel_occpy_count * 1.0 / T);
    fprintf(f, "Channel idle fraction (in percentage) %f\n",channel_idle_count * 1.0 / T);
    fprintf(f, "Total number of collisions %d\n",channel_collision_count);
    fprintf(f, "Variance in number of successful transmissions (across all nodes) %f\n",transmiss_var);
    fprintf(f, "Variance in number of collisions %f\n",collision_var);

}


int main(int argc, char** argv) {
    if (argc != 2){
        printf("Usage ./csma input.txt\n");
        return -1;
    }

    FILE* input = fopen(argv[1], "r");
    read_input(input);
    init_simulization();
    

    int t = 0;
    //main event loop
    while(t < T){
        //check back off value
        vector<node_t> ready_nodes;
        for (auto it = node_list.begin();it != node_list.end();it++){
            if (it->backoff == 0) ready_nodes.push_back(*it);
        }

        //No packets ready
        if (ready_nodes.size() == 0){
            for (auto it = node_list.begin();it != node_list.end();it++){
                it->backoff --;
            }
            channel_idle_count++;
            t++;
        }

        //
        if (ready_nodes.size() == 1){
            int node_id = ready_nodes[0].id;
            node_list[node_id].backoff = random(R);
            node_list[node_id].collision_count = 0;
            int send_time = L;
            if ((T - t) < send_time) send_time = T-t;
            t += send_time;
            channel_occpy_count+=send_time;
            if (send_time == L) node_list[node_id].accum_transmission_count++;
        }

        if (ready_nodes.size() > 1){
            channel_collision_count++;
            // channel_idle_count++;
            // channel_occpy_count++;
            for (auto it = ready_nodes.begin();it != ready_nodes.end();it++){
                int node_id = it->id;
                node_list[node_id].collision_count++;
                node_list[node_id].accum_collision_count++;
                node_list[node_id].backoff = random(R_list[node_list[node_id].collision_count % (M)]);
            }
            t+=1;
        }
    }

    FILE* output_f = fopen("output.txt","w");
    write_output(output_f);
    fclose(output_f);


    return 0;
}