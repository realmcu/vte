#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>

#define MAX_LOOP 102400
#define BLOCK 1024

void run_test( char * out, char * in)
{
	memcpy(out, in, BLOCK);
}

void run_test_d( char * out, char * in)
{
	memcpy(out, in, BLOCK);
}

int main()
{
	struct timeval start, end;
	struct timezone tz;
/*static array memory copy test*/
	static char to[BLOCK] __attribute((aligned(4)));
	static char from[BLOCK] __attribute((aligned(4)));
	int i = 0;
	unsigned long last = 0;
	char *pto, *pfrom;

  memset(from,1,BLOCK);
	gettimeofday(&start, &tz);
	for (i = 0; i < MAX_LOOP; i++) {
		run_test(to,from);
	//	memcpy(to, from, BLOCK);
	}
	gettimeofday(&end, &tz);

	last = (end.tv_sec - start.tv_sec) * 1000 +
	    (end.tv_usec - start.tv_usec) / 1000;

	printf("test last %ld ms\n", last);
	if (last)
		printf("performance is %ldMB/s\n", (unsigned long)(BLOCK * MAX_LOOP) / (last * (1024 * 1024 / 1000)));

	pto = (char *)malloc(BLOCK);
	if (pto == NULL) {
		printf("malloc fail 1\n");
		return -1;
	}
	pfrom = (char *)malloc(BLOCK);
	if (pfrom == NULL) {
		printf("malloc fail 2 \n");
		return -1;
	}
  memset(from,1,BLOCK);
	gettimeofday(&start, &tz);
	for (i = 0; i < MAX_LOOP; i++) {
		memcpy(pto, pfrom, BLOCK);
	//	run_test_d(pto,pfrom);
	}
	gettimeofday(&end, &tz);

	last = (end.tv_sec - start.tv_sec) * 1000 +
	    (end.tv_usec - start.tv_usec) / 1000;

	printf("dynamic test last %ld ms\n", last);
	if (last)
		printf("dynamic performance is %ld MB/s\n",
		       (unsigned long)(BLOCK * MAX_LOOP) / (last * (1024 * 1024 / 1000)));

  if(pto)
		free(pto);
	if(pfrom)
		free(pfrom);
/*dynamic allocate mempry copy*/
	return 0;
}
