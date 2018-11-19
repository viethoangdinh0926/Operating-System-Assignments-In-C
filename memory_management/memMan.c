#include "header.h"

int main(int argc, char **argv) {
	int polType;
	char *checkErr;

	FILE *input;
	char *manPol;

	if (argc != 4) {
		printf("Too few parameters\n");
		exit(1);
	}

	if (strcmp(argv[1], "firstfit") == 0) {
		polType = 0;
		manPol = "First Fit";
	}
	else if (strcmp(argv[1], "bestfit") == 0) {
		polType = 1;
		manPol = "Best Fit";
	}
	else if (strcmp(argv[1], "buddy") == 0) {
		polType = 2;
		manPol = "Buddy";
	}
	else{
		printf("Unrecognized memory policy\n");
		exit(1);
	}

	mem_size = strtol(argv[2], &checkErr, 10);
	if (*checkErr != '\0') {
		printf("Memory size must be positive integer\n");
		exit(1);
	}
	if (mem_size > 8192) {
		printf("Memory size must be power of 2 and less than 16,384 KB\n");
		exit(1);
	}

	mem_size = mem_size * 1024; //convert KB to Byte
	while (pow(2, exponent) < mem_size) {
		exponent++;
	}
	if (pow(2, exponent) > mem_size) {
		printf("Memory size must be power of 2\n");
		exit(1);
	}

	exponent += 1;// 21 lists for 1MB. Only use lists indexed from 5 to 20
	total_free_space = mem_size;
	largest_chunk = total_free_space;
	if ((input = fopen(argv[3], "r")) == NULL) {
		perror("file not found\n");
		exit(1);
	}
	
	printf("MANAGEMENT POLICY = %s POOL SIZE = %s KB\n\n", manPol, argv[2]);
	if (polType < 2) {
		first_best_fit(polType, input);
	}
	else {
		buddy(input);
	}
}

void first_best_fit(int type, FILE *input) {
	int i, tempSize;
	char * line = NULL;
	char * subStr;
	size_t len = 0;
	int req_or_free;
	struct free_list *tempStruct;
	list_head = (struct free_list *)malloc(sizeof(struct free_list));
	list_head->next = NULL;
	list_head->previous = NULL;
	list_head->block_size = total_free_space;
	list_head->block_adr = 0;
	list_head->adjacent_adr = -1;
	top = list_head;
	printf("%12s%10s%10s%12s%12s%15s\n","SERIAL-NUM","REQUEST","SIZE","ALLOC-ADDR","TOTAL-FREE","LARGEST-PART");
	for (i = 1; i < NUMBER_ENTRIES; ++i) {
		getline(&line, &len, input);
		subStr = strtok(line, " ");
		subStr = strtok(NULL, " ");
		while (subStr != NULL) {
			if (strcmp(subStr, "alloc") == 0) {
				req_array[i].is_req = TRUE;
				req_or_free = 0;
			}
			else if (strcmp(subStr, "free") == 0) {
				req_array[i].is_req = FALSE;
				req_or_free = 1;
			}
			else {//process alloc or free
				tempSize = atoi(subStr);
				if (req_or_free == 0) {
					if (type == 0) {//FIRST FIT
						while (top != NULL) {
							if (top->block_size >= tempSize) {
								break;
							}
							top = top->next;
						}
					}
					else {//BEST FIT
						tempStruct = NULL;
						while (top != NULL) {
							if (top->block_size >= tempSize) {
								if (tempStruct == NULL) {
									tempStruct = top;
								}
								else if(top->block_size < tempStruct->block_size){
									tempStruct = top;
								}
							}
							top = top->next;
						}
						top = tempStruct;
					}
					if (top == NULL) {//cannot find a free chunk big enough
						req_array[i].is_allocated = FALSE;
						req_array[i].size = tempSize;
						req_array[i].base_adr = -1;
						req_array[i].next_boundary_adr = -1;
						req_array[i].memory_left = total_free_space;
						req_array[i].elements_on_free_list = free_list_length;
						total_allocation_fail++;
					}
					else {//find FIRST node in free list which has big enough chunk
						req_array[i].is_allocated = TRUE;
						total_free_space = total_free_space - tempSize; 
						req_array[i].memory_left = total_free_space;
						req_array[i].size = tempSize;
						req_array[i].base_adr = top->block_adr;
						req_array[i].next_boundary_adr = req_array[i].base_adr + req_array[i].size;
						if (req_array[i].next_boundary_adr == mem_size) {
							req_array[i].next_boundary_adr = -1;
						}
						if (top->block_size > tempSize) {//block size is greater than requested size
							req_array[i].elements_on_free_list = free_list_length;
							top->block_size = top->block_size - tempSize;
							top->block_adr = req_array[i].next_boundary_adr;
							if (top != list_head) {
								top->previous->adjacent_adr = top->block_adr;
							}
						}
						else {//block size equals to requested size
							free_list_length = free_list_length - 1;
							req_array[i].elements_on_free_list = free_list_length;
							if (top == list_head) {//the HEAD of the free list
								if (top->next != NULL) {
									list_head = list_head->next;
									free(top);
								}
								else {
									free(list_head);
								}
							}
							else {
								if (top->next != NULL) {
									top->previous->adjacent_adr = top->next->block_adr;
									top->previous->next = top->next;
									top->next->previous = top->previous;
								}
								top->previous->adjacent_adr = -1;
								top->previous->next = NULL;
								free(top);
							}
						}
						total_allocation_sucess++;
					}
				}//end of process alloc
				else {//process free
					if (req_array[tempSize].is_allocated == TRUE) {//if the corresponding ALLOC is done
						req_array[i].is_allocated = TRUE;
						req_array[i].memory_left = total_free_space + req_array[tempSize].size;
						total_free_space += req_array[tempSize].size;
						if (list_head == NULL) {
							list_head = (struct free_list *)malloc(sizeof(struct free_list));
							list_head->next = NULL;
							list_head->previous = NULL;
							list_head->block_size = req_array[i].size;
							list_head->block_adr = req_array[i].base_adr;
							list_head->adjacent_adr = -1;
						}
						else if (req_array[tempSize].next_boundary_adr < list_head->block_adr) {
							tempStruct = (struct free_list *)malloc(sizeof(struct free_list));
							tempStruct->next = list_head;
							tempStruct->previous = NULL;
							tempStruct->block_size = req_array[tempSize].size;
							tempStruct->block_adr = req_array[tempSize].base_adr;
							tempStruct->adjacent_adr = list_head->block_adr;
							list_head->previous = tempStruct;
							list_head = tempStruct;
							free_list_length += 1;
						}
						else if (req_array[tempSize].next_boundary_adr == list_head->block_adr) {
							list_head->block_size += req_array[tempSize].size;
							list_head->block_adr = req_array[tempSize].base_adr;
						}
						else {
							top = list_head;
							while (top != NULL) {
								if (top->block_adr + top->block_size == req_array[tempSize].base_adr &&
											top->adjacent_adr == req_array[tempSize].next_boundary_adr) {
									top->block_size += req_array[tempSize].size;
									if (top->next != NULL) {
										tempStruct = top->next;
										top->block_size += tempStruct->block_size;
										top->adjacent_adr = tempStruct->adjacent_adr;
										top->next = tempStruct->next;
										if (tempStruct->next != NULL) {
											tempStruct->next->previous = top;
										}
										free(tempStruct);
										free_list_length -= 1;
									}
									break;
								}
								else if (top->block_adr + top->block_size == req_array[tempSize].base_adr) {
									top->block_size += req_array[tempSize].size;
									break;
								}
								else if (top->adjacent_adr == req_array[tempSize].next_boundary_adr) {
									if (top->adjacent_adr == -1) {
										tempStruct = (struct free_list *)malloc(sizeof(struct free_list));
										tempStruct->next = NULL;
										tempStruct->previous = top;
										tempStruct->block_size = req_array[tempSize].size;
										tempStruct->block_adr = req_array[tempSize].base_adr;
										tempStruct->adjacent_adr = -1;
										top->next = tempStruct;
										top->adjacent_adr = tempStruct->block_adr;
										free_list_length += 1;
									}
									else {
										top->next->block_adr -= req_array[tempSize].size;
										top->next->block_size += req_array[tempSize].size;
										top->adjacent_adr = top->next->block_adr;
									}
									break;
								}
								else if (top->block_adr + top->block_size < req_array[tempSize].base_adr &&
									(top->adjacent_adr > req_array[tempSize].next_boundary_adr || top->next == NULL)) {
									tempStruct = (struct free_list *)malloc(sizeof(struct free_list));
									tempStruct->next = top->next;
									tempStruct->previous = top;
									tempStruct->block_size = req_array[tempSize].size;
									tempStruct->block_adr = req_array[tempSize].base_adr;
									tempStruct->adjacent_adr = top->adjacent_adr;
									if (top->next != NULL) {
										top->next->previous = tempStruct;
									}
									top->adjacent_adr = tempStruct->block_adr;
									top->next = tempStruct;
									free_list_length += 1;
									break;
								}
								top = top->next;
							}
						}
						req_array[i].elements_on_free_list = free_list_length;
						total_free++;
					}//end processing FREE the corresponding ALLOC
					else {// if the corresponding ALLOC is denied
						req_array[i].is_allocated = FALSE;
						req_array[i].memory_left = total_free_space;
						req_array[i].elements_on_free_list = free_list_length;
					}
				}//end of process free
				//update largest chunk of the memory;
				top = list_head;
				req_array[i].largest_chunk = 0;
				while (top != NULL) {
					if (top->block_size > req_array[i].largest_chunk) {
						req_array[i].largest_chunk = top->block_size;
					}
					top = top->next;
				}
				top = list_head;
			}//end of process alloc or free
			
			subStr = strtok(NULL, " ");
		}
		printf("%12d%10s%10d", i, req_array[i].is_req?"alloc":"free",
			req_array[i].is_req ? req_array[i].size: req_array[tempSize].size);
		if (req_array[i].is_req) {
			printf("%12d", req_array[i].base_adr);
		}
		else {
			printf("%12s", "");
		}
		printf("%12d%15d\n", req_array[i].memory_left, req_array[i].largest_chunk);
	}

	top = list_head;
	while (list_head != NULL) {
		list_head = list_head->next;
		free(top);
		top = list_head;
	}
	printf("TOTAL ALLOCATION: %d\nTOTAL ALLOCATION FAIL: %d\nTOTAL FREE: %d\n", total_allocation_sucess, total_allocation_fail, total_free);
}

void buddy(FILE *input) {
	int tempSize, req_or_free, bit, i;
	char *subStr;
	struct lel *tempStruct;
	char * line = NULL;
	size_t len = 0;

	root = (struct lel*)malloc(sizeof(struct lel));
	root->adr = 0;
	root->bit = exponent - 1;
	root->buddy_adr = -1;
	root->left = NULL;
	root->right = NULL;
	root->parent = NULL;
	root->occupied = 0;
	printf("%12s%10s%10s%12s%12s%15s\n", "SERIAL-NUM", "REQUEST", "SIZE", "ALLOC-ADDR", "TOTAL-FREE", "LARGEST-PART");
	for (i = 1; i < NUMBER_ENTRIES; ++i) {
		getline(&line, &len, input);
		subStr = strtok(line, " ");
		subStr = strtok(NULL, " ");
		while (subStr != NULL) {
			if (strcmp(subStr, "alloc") == 0) {
				req_array_bd[i].is_req = TRUE;
				req_or_free = 0;
			}
			else if (strcmp(subStr, "free") == 0) {
				req_array_bd[i].is_req = FALSE;
				req_or_free = 1;
			}
			else {//process alloc or free
				tempSize = atoi(subStr);
				if (req_or_free == 0) {//allocating
					req_array_bd[i].size = tempSize;
					bit = ceil(log2(tempSize));
					req_array_bd[i].act_size = pow(2, bit);
					traverse_allocate(root, bit, &req_array_bd[i]);
				}//end allocating
				else {//freeing
					traverse_free(root, &req_array_bd[tempSize], &req_array_bd[i]);
				}//end freeing
			}
			subStr = strtok(NULL, " ");
		}
		printf("%12d%10s%10d", i, req_array_bd[i].is_req ? "alloc" : "free",
			req_array_bd[i].is_req ? req_array_bd[i].act_size : req_array_bd[tempSize].act_size);
		if (req_array_bd[i].is_req) {
			printf("%12d", req_array_bd[i].base_adr);
		}
		else {
			printf("%12s", "");
		}
		printf("%12d%15d\n", req_array_bd[i].memory_left, req_array_bd[i].largest_chunk);
	}
	free_bd(root);// free the tree
	printf("TOTAL ALLOCATION: %d\nTOTAL ALLOCATION FAIL: %d\nTOTAL FREE: %d\n", total_allocation_sucess, total_allocation_fail, total_free);
}

int traverse_allocate(struct lel *top, const int bit, struct request_bd *mem) {

	struct lel *tempStruct = find_min(top, bit);

	if (tempStruct == NULL) {
		mem->is_allocated = FALSE;
		mem->base_adr = -1;
		mem->next_boundary_adr = -1;
		mem->memory_left = total_free_space;
		mem->largest_chunk = largest_chunk;
		mem->elements_on_free_list = free_list_length;
		mem->this_req = NULL;
		++total_allocation_fail;
		return 1;//fail
	}
	
	while (tempStruct->bit > bit) {
		tempStruct->occupied = 1;

		tempStruct->left = (struct lel*)malloc(sizeof(struct lel));
		tempStruct->left->adr = tempStruct->adr;
		tempStruct->left->bit = tempStruct->bit - 1;
		tempStruct->left->left = NULL;
		tempStruct->left->right = NULL;
		tempStruct->left->parent = tempStruct;
		tempStruct->left->occupied = 1;

		tempStruct->right = (struct lel*)malloc(sizeof(struct lel));
		tempStruct->right->adr = tempStruct->adr + pow(2, tempStruct->bit - 1);
		tempStruct->right->bit = tempStruct->bit - 1;
		tempStruct->right->left = NULL;
		tempStruct->right->right = NULL;
		tempStruct->right->parent = tempStruct;
		tempStruct->right->occupied = 0;

		tempStruct->left->buddy_adr = tempStruct->right->adr;
		tempStruct->right->buddy_adr = tempStruct->left->adr;
		free_list_length++;
		tempStruct = tempStruct->left;
	}

	free_list_length--;
	tempStruct->occupied = 1;
	mem->is_allocated = TRUE;
	mem->base_adr = tempStruct->adr;
	mem->next_boundary_adr = tempStruct->adr + mem->act_size;
	mem->memory_left = total_free_space - mem->act_size;
	largest_chunk = find_largest_chunk(root);
	mem->largest_chunk = largest_chunk;
	mem->elements_on_free_list = free_list_length;
	mem->this_req = tempStruct;
	++total_allocation_sucess;
	total_free_space = mem->memory_left;
	return 0;//success
}

int find_largest_chunk(const struct lel* top) {
	int left, right;
	if (top->occupied == 0) {
		return pow(2, top->bit);
	}
	if (top->left == NULL) {
		return 0;
	}
	left = find_largest_chunk(top->left);
	right = find_largest_chunk(top->right);
	return left > right ? left : right;
}

void traverse_free(struct lel* top, const struct request_bd *mem, struct request_bd *free_cmd) {
	struct lel *parent;
	free_cmd->is_allocated = FALSE;
	free_cmd->base_adr = -1;
	free_cmd->this_req = NULL;
	free_cmd->next_boundary_adr = -1;
	free_cmd->size = 0;
	if (mem->is_req == TRUE && mem->is_allocated == TRUE) {
		total_free_space += mem->act_size;
		if (mem->this_req != root) {	
			mem->this_req->occupied = 0;
			free_list_length++;
			parent = mem->this_req->parent;
			while (parent != NULL) {
				if (parent->left->occupied != 0 || parent->right->occupied != 0) {
					break;
				}
				parent->occupied = 0; 
				parent->left = NULL; 
				parent->right = NULL;
				parent = parent->parent;
				free_list_length--;
			}
			largest_chunk = find_largest_chunk(root);
		}
		else {
			root->occupied = 0;
			largest_chunk = mem_size;
			total_free_space = mem_size;
			free_list_length++;
		}
		++total_free;
		free_cmd->size = mem->size;
	}
	free_cmd->memory_left = total_free_space;
	free_cmd->largest_chunk = largest_chunk;
	free_cmd->elements_on_free_list = free_list_length;
}

struct lel *find_min(struct lel* top, int bit) {
	struct lel *a = NULL, *b = NULL;
	if (top != NULL) {
		if (top->bit >= bit) {
			if (top->occupied == 0) {
				a = top;
			}
			else if (top->bit > bit) {
				a = find_min(top->left, bit);
				b = find_min(top->right, bit);
			}
			if (a == NULL && b == NULL) {
				return NULL;
			}
			else if (a == NULL) {
				return b;
			}
			else if (b == NULL) {
				return a;
			}
			else {
				return a->bit <= b->bit ? a : b;
			}
		}

		return NULL;
	}
	
	return NULL;
}

void free_bd(struct lel* top) {
	if (top != NULL) {
		free_bd(top->left);
		free_bd(top->right);
		free(top);
	}
}