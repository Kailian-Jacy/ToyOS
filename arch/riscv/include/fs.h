#include <defs.h>

typedef struct superblock{
	int64_t block_num;
	int64_t bit_vector;
} superblock;

void LoadFS();
void Read();