#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#ifdef __ARM_NEON__
#include <arm_neon.h>
#endif

#define LOOPS 100
 
typedef float FLOAT_TYPE;

#ifdef vfp
inline FLOAT_TYPE af_filter_fir(register unsigned int n, const FLOAT_TYPE* w,
                                const FLOAT_TYPE* x)
 {
	int i;
    register FLOAT_TYPE y; // Output
    y = 0.0;
    for(i =1 ; i <= n; i++)
	{
      y+=w[n - i]*x[n - i];
    }
    return y;
 }

#elif defined neon_asm

inline FLOAT_TYPE af_filter_fir(register unsigned int n, const FLOAT_TYPE* w,
                                const FLOAT_TYPE* x)
{
    register FLOAT_TYPE y; // Output
    y = 0.0;
}
#elif defined neon_intrinsics
inline FLOAT_TYPE af_filter_fir(register unsigned int n, const FLOAT_TYPE* w,
                                const FLOAT_TYPE* x)
 {
    register FLOAT_TYPE y; // Output
	register int c = n;
	FLOAT_TYPE * mw = (FLOAT_TYPE *)w;
	FLOAT_TYPE * mx = (FLOAT_TYPE *)x;
    float32x4_t f0 = { 0.0f, 0.0f, 0.0f, 0.0f };
	FLOAT_TYPE iy[4];
	for (c = n;c;c -= 8, mw += 8, mx += 8)
	{
		float32x4_t f1 = vmulq_f32(vld1q_f32(mw),vld1q_f32(mx));
		float32x4_t f2 = vmulq_f32(vld1q_f32(&mw[4]),vld1q_f32(&mx[4]));
		f0  = vaddq_f32(f1, f0);
		f0  = vaddq_f32(f0, f2);
    }
    vst1q_f32((FLOAT_TYPE *)iy, f0);
	y = iy[0] + iy[1] + iy[2] + iy[3];
    return y;
 }
#endif


int main(int argc, char ** argv)
{
  int count = 0, i = 0;
  long iloop = LOOPS;
  int size = 32;
  FLOAT_TYPE y = 1.0;
  struct timeval tv_start, tv_current;
  if (argc > 1 && argv[1])
    iloop = atol(argv[1]);
  if (argc > 2 && argv[2])
    size = atoi(argv[2]);

  if (iloop < 1)
	  iloop = 1;
  
  printf("size is %d, and loop %ld \n", size, iloop);

  FLOAT_TYPE * w = (FLOAT_TYPE *)malloc(sizeof(FLOAT_TYPE)*size);
  FLOAT_TYPE * x = (FLOAT_TYPE *)malloc(sizeof(FLOAT_TYPE)*size);

  for(i=0; i < size ; i++)
  {
     w[i] = 1.0 * random()/RAND_MAX;
     x[i] = 1.0 * random()/RAND_MAX;
   }

  
  gettimeofday(&tv_start, 0);
  while (count++ < iloop)
  {
    y = af_filter_fir(size,w, x);
  }
  gettimeofday(&tv_current, 0);
  printf("result is %f\n", y);
  long tt = (tv_current.tv_sec - tv_start.tv_sec)* 1000000L + \
	(tv_current.tv_usec - tv_start.tv_usec);
  printf("total time is %ld us\n",tt);
  printf("average time is %lf us\n", 1.0 * tt / iloop);
  return 0;
}
