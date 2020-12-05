#ifndef routing_algorithm_H
#define routing_algorithm_H

typedef struct node_t{
    int id;
    int backoff;
    int collision_count;
    int accum_collision_count;
    int accum_transmission_count;
}node_t;

#endif