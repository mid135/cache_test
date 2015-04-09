#include <stdint.h>
#include <x86intrin.h>
#include <getopt.h>
#include <stdio.h>
#include <time.h>
#include <inttypes.h>


#define BIG_CYCLE_COUNT 20        //data acces cycles count
#define SMALL_CYCLE_COUNT 1000000 //data access operations

//CPU FREQ = 2,2 GHz

struct elem{
  struct elem *next;
  uint64_t payload;
};

void build_list_conseq(void* head, uint32_t size, uint16_t elem_size) {
    struct elem* current = head;
    for (uint64_t i = 1; i < size; ++i) {
        current->next = head + i * elem_size;
        current = current->next;
    }
    current->next = head;
}

void buid_list_random(void* head, uint64_t size, uint64_t elem_size){
    struct elem* current = head;
    srand((unsigned int) time(NULL));
    for (uint64_t i = 1; i < size; ++i) {
        uint64_t next = rand() % size;
        void* temp = head + next * elem_size;
        while (temp == current || ((struct elem*) temp)->next) {
            next = ++next%size;
            temp = head + next * elem_size;
        }
        current->next = temp;
        current = current->next;
    }
    current->next = head;
}

uint64_t measure(struct elem* head, uint32_t size) {
    struct elem* current = head;
    uint64_t avg = 0;

    for (int i = 0; i < BIG_CYCLE_COUNT; ++i) {

        uint64_t start = __rdtsc();

        for (int j = 0; j < SMALL_CYCLE_COUNT; ++j) {
            current->payload += 1;
            current = current->next;
        }

        uint64_t stop = __rdtsc();
        avg += (stop - start) / SMALL_CYCLE_COUNT;
    }

    avg /= BIG_CYCLE_COUNT;

    return avg;
}

int main(int argc, char** argv){
    uint64_t list_size = 1;
    uint64_t node_size = 1;
    uint8_t is_random = 0;
    int c = 1;
    while ((c = getopt(argc, argv, "rn:l:")) != -1)
        switch (c) {
            case 'l': {
                (list_size) = (uint64_t) atoi(optarg);
                break;}
            case 'n':{
                (node_size) = (uint64_t) atoi(optarg);
                break;}
            case 'r':{
                (is_random) = 1;
                break;}
            default:
                exit(1);
        }

    node_size *= sizeof(struct elem);

        void* list = calloc(list_size, node_size);
        if (list) {
            if (is_random == 1)
                buid_list_random(list, list_size, node_size);
            else
                build_list_conseq(list, list_size, node_size);

           //    if (is_random) printf("random access:\n");
            //else printf("conseq access:\n");

            //printf("node size = %d list size = %d total = %lu Kb\n", node_size, list_size, ((uint64_t) node_size * list_size) >> 10);
            printf("%lu\n", ((uint64_t) node_size * list_size) >> 10);
            printf("%" PRIu64 " ", measure(list, list_size));
            free(list);
        }
        return 0;
}

