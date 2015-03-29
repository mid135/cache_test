#include <stdint.h>
#include <x86intrin.h>
#include <getopt.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>

#define STRUCT_SIZE 1
#define CYCLE_COUNT 50000

#define L1_TIME 8  //32K
#define L2_TIME 15   //256K
#define L3_TIME 100  //3072K

//CPU FREQ = 2,2 GHz



typedef struct{
  struct elem *next;
  long int payload[STRUCT_SIZE];
} elem;


int main(void)
{
    //BASIC ALLOCATION
    elem** array=malloc(sizeof(elem)*CYCLE_COUNT);
    //ALLOCATE ELEMENTS AND BIND'EM
    array[0]=(struct elem*)malloc(sizeof(elem));
    for (uint64_t i=1;i<CYCLE_COUNT;++i){
        array[i]=(struct elem*)malloc(sizeof(elem));
        array[i-1]->next=array[i];
    }

    //RANDOM ALLOCATION
    elem** random_array=malloc(sizeof(elem)*CYCLE_COUNT);
    array[0]=(struct elem*)malloc(sizeof(elem));
    elem* temp=array[0];
    for (uint64_t i=1;i<CYCLE_COUNT;++i){
        uint64_t rand_pos = rand()%CYCLE_COUNT;
        while (random_array[rand_pos]!=NULL ){
            ++rand_pos;
            if (rand_pos > CYCLE_COUNT) { rand_pos = 0; }
        }
        random_array[rand_pos]=(struct elem*)malloc(sizeof(elem));
        temp->next=array[rand_pos];
        temp=random_array[rand_pos];
    }
    temp->next=array[0];


    uint64_t x=0;

    uint64_t avg_1=0;
    uint64_t hit_1=1;
    uint64_t avg_2=0;
    uint64_t hit_2=1;
    uint64_t avg_3=0;
    uint64_t hit_3=1;

    uint64_t start;
    uint64_t stop;
    //CONSEQ ACCESS
    elem* current=array[0];
    while(current->next != NULL){
        start = _rdtsc();

        current->payload[0]++;
        current=current->next;

        stop = _rdtsc();
        if ((stop-start)<=L1_TIME) {avg_1+=(stop-start);++hit_1;}
        if ((stop-start)<=L2_TIME) {avg_2+=(stop-start);++hit_2;}
        if ((stop-start)<=L3_TIME) {avg_3+=(stop-start);++hit_3;}
    }    
    printf("conseq access avg time on L1 %" PRIu64 "\n",avg_1/(hit_1));
    printf("conseq access avg time on L2 %" PRIu64 "\n",avg_2/(hit_2));
    printf("conseq access avg time on L3 %" PRIu64 "\n",avg_3/(hit_3));

    uint64_t avg=0;
    //RANDOM_ACCESS
    current=random_array[0];
    while(current->next != NULL){
        start = _rdtsc();

        current->payload[0]++;
        current=current->next;

        stop = _rdtsc();
        avg+=(stop-start);
    }
    avg = avg/CYCLE_COUNT;
    printf("rand access avg time %" PRIu64 "\n",avg);


}

