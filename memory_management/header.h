#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define NUMBER_ENTRIES  (1001)
#define FALSE           (0)
#define TRUE            (1)
#define DONE            (2)

struct request {
	int is_req;
	int is_allocated;
	int size;
	int base_adr;
	int next_boundary_adr;
	int memory_left;
	int largest_chunk;
	int elements_on_free_list;
}req_array[NUMBER_ENTRIES];

struct free_list {
	struct free_list *next;
	struct free_list *previous;
	int block_size;
	int block_adr;
	int adjacent_adr;
}*list_head, *top;

int total_free_space, mem_size, total_free = 0;
int free_list_length = 1,  total_allocation_fail = 0, total_allocation_sucess = 0;

void first_best_fit(int type, FILE *input);//0: first first; other: best first


// a block list element on one of the block sized
// list of addresses
struct lel {
	struct lel *parent;
	struct lel *left;
	struct lel *right;
	int buddy_adr;
	int adr;
	int bit;
	int occupied;
} *root;


struct request_bd {
	int is_req;
	int is_allocated;
	int size;
	int act_size;
	int base_adr;
	int next_boundary_adr;
	int memory_left;
	int largest_chunk;
	int elements_on_free_list;
	struct lel *this_req;
}req_array_bd[NUMBER_ENTRIES];

int exponent = 10;
int largest_chunk;

int traverse_allocate(struct lel*, const int, struct request_bd *);
void traverse_free(struct lel*, const struct request_bd *, struct request_bd *);
int find_largest_chunk(const struct lel*);
struct lel *find_min(struct lel*, int);
void free_bd(struct lel*);

void buddy(FILE *input);