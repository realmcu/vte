/*
 * Copyright 2008 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Defines */
#define SIZE_1K			1024
#define SIZE_4K			(4*SIZE_1K)
#define SIZE_1M			(1024*1024)
#define START_SIZE		(2*SIZE_1K)
#define END_SIZE		SIZE_1M
#define ALIGN			SIZE_4K
#define START_LOOPS		200000

/* Function Prototypes */
void help(char *cmd);

/* Main */
int main(int argc, char *argv[])
{
	int size = START_SIZE;
	int end_size = END_SIZE;
	int salign = ALIGN;
	int dalign = ALIGN;
	int loops = START_LOOPS;
	int src, dst, asrc, adst;

	/* Parse command line */
	if (argc > 1)
	{
		loops = atoi(argv[1]) * 1000;
		if (loops <= 0)
			help(argv[0]);
	}
	if (argc > 2)
	{
		size = atoi(argv[2]) * SIZE_1K;
		if (size <= 0)
			help(argv[0]);
	}
	if (argc > 3)
	{
		end_size = atoi(argv[3]) * SIZE_1K;
		if (end_size < size)
			help(argv[0]);
	}
	if (argc > 4)
	{
		salign = atoi(argv[4]);
		if ((salign < 0) || (salign > SIZE_4K))
			help(argv[0]);
	}
	if (argc > 5)
	{
		dalign = atoi(argv[5]);
		if ((dalign < 0) || (dalign > SIZE_4K))
			help(argv[0]);
	}

	/* Allocate buffers */
	if ((src = (int) malloc(end_size + salign + SIZE_4K)) == 0)
	{
		printf("%s: insufficient memory\n", argv[0]);
		return -1;
	}
	memset((void*)src, 0xaa, end_size + salign + SIZE_4K);
	if ((dst = (int) malloc(end_size + dalign + SIZE_4K)) == 0)
	{
		free((void*)src);
		printf("%s: insuficient memory\n", argv[0]);
		return -1;
	}
	memset((void*)dst, 0x55, end_size + dalign + SIZE_4K);

	/* Align buffers */
	if (src % SIZE_4K == 0)
		asrc = src + salign;
	else
		asrc = src + SIZE_4K - (src % SIZE_4K) + salign;
	if (dst % SIZE_4K == 0)
		adst = dst + dalign;
	else
		adst = dst + SIZE_4K - (dst % SIZE_4K) + dalign;

	/* Print Banner */
	printf("\nMEMCPY Benchmark\n\n");
	printf("Src Buffer 0x%08x\n", asrc);
	printf("Dst Buffer 0x%08x\n\n", adst);
	printf("%10s %10s\n", "Cached", "Bandwidth");
	printf("%10s %10s\n", "(KBytes)", "(MB/sec)");

	/* Loop over copy sizes */
	while (size <= end_size)
	{
		clock_t start_time;
		float elapsed_time;
		int loop;

		printf("%10d", size / SIZE_1K);

		/* Do data copies */
		start_time = clock();
		for (loop = 0; loop < loops; loop++)
			memcpy((void*)adst, (void*)asrc, size);
		elapsed_time = ((float) clock() - start_time) / CLOCKS_PER_SEC;

		printf(" %10.0f\n", ((float)size*loops*2)/elapsed_time/SIZE_1M);

		/* Adjust for next test */
		size *= 2;
		loops /= 2;
	}

	/* Free buffers */
	free((void*)src);
	free((void*)dst);
}

/* Help display */
void help(char *cmd)
{
	printf("\nMEMCPY Benchmark\n\n");
	printf("usage: %s [loops] [start] [end] [salign] [dalign]\n\n", cmd);
	printf("options:  loops  - initial loop count (thousands)\n");
	printf("          start  - starting buffer size (Kbytes)\n");
	printf("          end    - ending buffer size (Kbytes)\n");
	printf("          salign - source alignment (bytes)\n");
	printf("          dalign - destination alignment (bytes)\n");
	printf("\n");
	exit(0);
}
