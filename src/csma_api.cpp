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

extern "C"{

typedef struct results_t
{
    float util;
    float idle;
    float collision;
}results_t;

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


results_t csma_api(int N, int L, int R, int M, int T) {

    results_t rc;
    rc.util = 0;
    rc.idle = 0;
    rc.collision = 0;
    int experiment_num = 100;

    for (int i = 0;i < experiment_num;i++){
        int channel_idle_count = 0;
        int channel_collision_count = 0;
        int channel_occpy_count = 0;
        vector<node_t> node_list;
        vector<int>R_list;
        node_list.clear();
        R_list.clear();
        R_list.push_back(R);

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
        for (int i = 0;i < M;i++){
            R_list.push_back(R * (1 << (i+1)));
        }
        
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
                for (auto it = ready_nodes.begin();it != ready_nodes.end();it++){
                    int node_id = it->id;
                    node_list[node_id].collision_count++;
                    node_list[node_id].accum_collision_count++;
                    node_list[node_id].backoff = random(R_list[node_list[node_id].collision_count % (M)]);
                }
                t+=1;
            }
        }
        rc.util += channel_occpy_count * 1.0 / T;
        rc.idle += channel_idle_count * 1.0 / T;
        rc.collision += (float)channel_collision_count;
        // printf("Channel utilization (in percentage) %f\n",channel_occpy_count * 1.0 / T);
        // printf("Channel idle fraction (in percentage) %f\n",channel_idle_count * 1.0 / T);
        // printf("Total number of collisions %d\n",channel_collision_count);
    }

    
    
    // cout << "Channel utilization (in percentage)" << channel_occpy_count * 1.0 / T << endl;
    // printf("Channel utilization (in percentage) %f\n",channel_occpy_count * 1.0 / T);
    // printf("Channel idle fraction (in percentage) %f\n",channel_idle_count * 1.0 / T);
    // printf("Total number of collisions %d\n",channel_collision_count);
    // printf("Variance in number of successful transmissions (across all nodes) %f\n",transmiss_var);
    // printf("Variance in number of collisions %f\n",collision_var);

    
    rc.util /= experiment_num;
    rc.idle /= experiment_num;
    rc.collision /= experiment_num;

    return rc;
}
}