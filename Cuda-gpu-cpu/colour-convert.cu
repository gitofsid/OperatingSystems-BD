#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "colour-convert.h"
#include <helper_timer.h>

//defining preprocessors for global usage
#define N (1000*700)
#define BLOCKX 1000
#define BLOCKY 700
#define WIDTH 1000
#define HEIGHT 700
#define NUM_THREADS 32

//if pixel values fall out of range re-adjust them
__device__ unsigned char clip_rgb_for_device(int x)
{
  if(x > 255)
    return 255;
  if(x < 0)
    return 0;
  return (unsigned char)x;
}

//initializing gpu
__global__ void intialize_gpu(void)
{

}

void timerKernelCall(){
  intialize_gpu<<<1,1>>>();
}

//kernel function for rgb to yuu conversion on gpu side. This takes value of
//r,g,b pixels and convert them into y,u,v values using given formula
__global__ void rgb2yuvKernel( unsigned char *d_r, unsigned char *d_g,
                               unsigned char *d_b, unsigned char *d_y,
                               unsigned char *d_u, unsigned char *d_v) {

  unsigned char r, g, b;

  //finding the index based on offset, block number and thread number
  int y = threadIdx.y+ blockIdx.y*blockDim.y;
  int x = threadIdx.x+ blockIdx.x*blockDim.x;
  int index = HEIGHT*y +x;

  if (y < WIDTH && x < HEIGHT)
  {
    r = d_r[index];
    g = d_g[index];
    b = d_b[index];

    //calculating the y,u,v from given r,g,b values
    d_y[index] = (unsigned char)( 0.299* r + 0.587* g +  0.114* b);
    d_v[index] = (unsigned char)( 0.499* r - 0.418* g - 0.0813* b + 128);
    d_u[index] = (unsigned char)(-0.169* r - 0.331* g +  0.499* b + 128);
  }
}

//kernel function for yuv to rgb conversion on gpu side. This takes value of
//y,u,v pixels and convert them into r,g,b values using given formula
__global__ void yuv2rgbKernel( unsigned char *d_y, unsigned char *d_u,
                               unsigned char *d_v, unsigned char *d_r,
                               unsigned char *d_g, unsigned char *d_b) {

  int  rt,gt,bt;
  int  yr,cb,cr;

  //finding the index based on offset, block number and thread number
  int y = threadIdx.y+ blockIdx.y*blockDim.y;
  int x = threadIdx.x+ blockIdx.x*blockDim.x;
  int index = HEIGHT*y +x;

  if (y < WIDTH && x < HEIGHT)
  {
   yr  = (int)d_y[index];
   cb = (int)d_u[index] - 128;
   cr = (int)d_v[index] - 128;

   rt  = (int)( yr + 1.402*cr);
   gt  = (int)( yr - 0.344*cb - 0.714*cr);
   bt  = (int)( yr + 1.772*cb);

   //calulating the r,g,b from above calculations
   d_r[index] = clip_rgb_for_device(rt);
   d_g[index] = clip_rgb_for_device(gt);
   d_b[index] = clip_rgb_for_device(bt);

   //uncomment lines below to test if image is white and comment 3 lines above
   // d_r[index] = 255;
   // d_g[index] = 255;
   // d_b[index] = 255;
 }
}

//universal cudaMalloc API for all the future calls to cudaMalloc for efficiency
void my_mallocForCuda(unsigned char **location)
{
  cudaError_t error = cudaSuccess;
  unsigned char * x = NULL;
  error = cudaMalloc((void**) &x, sizeof(unsigned char)*N);
  if(error != cudaSuccess) {
    printf("Error in allocating memory using cudaMalloc %d. Exiting\n", error );
    exit(0);
  }
  *location = x;

}

//universal cudaMemcpy API for all the future calls to cudaMalloc for efficiency
void my_memcpyForCuda(unsigned char *o_pixel, unsigned char *i_pixel, cudaMemcpyKind way)
{
  cudaError_t error = cudaSuccess;
  error = cudaMemcpy(o_pixel, i_pixel, sizeof(unsigned char)* N , way);
  if(error != cudaSuccess) {
    printf("Error in copying into memory using cudaMemcpy %d. Exiting\n", error );
    exit(0);
  }
}

//universal cudaFree API for all the future calls to cudaMalloc for efficiency
void my_freeForCuda(unsigned char *location)
{
  cudaError_t error = cudaSuccess;
  error = cudaFree(location);
  if(error != cudaSuccess) {
    printf("Error in freeing memory using cudaFree %d. Exiting\n", error );
    exit(0);
  }
}

PPM_IMG question1(PPM_IMG h_in)
{

  StopWatchInterface *timer=NULL;

  //output image storage and memory initialization
  PPM_IMG h_out;

  h_out.w = h_in.w;
  h_out.h = h_in.h;
  h_out.img_r = (unsigned char *)malloc( N * sizeof(unsigned char));
  h_out.img_g = (unsigned char *)malloc( N * sizeof(unsigned char));
  h_out.img_b = (unsigned char *)malloc( N * sizeof(unsigned char));
  if (h_out.img_r == NULL || h_out.img_g == NULL ||  h_out.img_b == NULL) {
    printf("Error in allocating memory for output image in rgb2yuvGPU. Exiting");
    exit(0);
  }

  // declare GPU memory pointers for both kinds of images
  unsigned char * d_r, * d_g, * d_b;
  unsigned char * o_r, * o_g, * o_b;

  //threads sync
  cudaDeviceSynchronize();

  // allocate GPU memory for different pixels structure
  my_mallocForCuda(&d_r); my_mallocForCuda(&d_g); my_mallocForCuda(&d_b);
  my_mallocForCuda(&o_r); my_mallocForCuda(&o_g); my_mallocForCuda(&o_b);

	// transfer the array to the GPU

  sdkCreateTimer(&timer);
  sdkStartTimer(&timer);

  //copying input image to GPU
  my_memcpyForCuda(d_r, h_in.img_r, cudaMemcpyHostToDevice);
  my_memcpyForCuda(d_g, h_in.img_g, cudaMemcpyHostToDevice);
  my_memcpyForCuda(d_b, h_in.img_b, cudaMemcpyHostToDevice);

  sdkStopTimer(&timer);

  printf("Data copy time from HOST(CPU) TO DEVICE(HOST): %f (ms)\n", sdkGetTimerValue(&timer));
  sdkDeleteTimer(&timer);



  sdkCreateTimer(&timer);
  sdkStartTimer(&timer);

  //now copy the output gpu yuv image to host cpu for further processing
  my_memcpyForCuda(h_out.img_r, o_r, cudaMemcpyDeviceToHost);
  my_memcpyForCuda(h_out.img_g, o_g, cudaMemcpyDeviceToHost);
  my_memcpyForCuda(h_out.img_b, o_b, cudaMemcpyDeviceToHost);

  printf("Data copy time from DEVICE(GPU) TO HOST (CPU): %f (ms)\n", sdkGetTimerValue(&timer));
  sdkDeleteTimer(&timer);

  //free all the allocated memory
  my_freeForCuda(d_r); my_freeForCuda(d_g); my_freeForCuda(d_b);
  my_freeForCuda(o_r); my_freeForCuda(o_g); my_freeForCuda(o_b);

  cudaDeviceSynchronize();
  return h_out;
}





//this funtion returns a yuv image from and input rgb ppm image
YUV_IMG rgb2yuvGPU(PPM_IMG h_in)
{

  //StopWatchInterface *timer=NULL;

  //deciding on threads and blocks for parallel processing
  const dim3 threads(NUM_THREADS,NUM_THREADS,1);
  const dim3 blocks(BLOCKX, BLOCKY, 1);

  //output image storage and memory initialization
  YUV_IMG h_out;

  h_out.w = h_in.w;
  h_out.h = h_in.h;
  h_out.img_y = (unsigned char *)malloc( N * sizeof(unsigned char));
  h_out.img_u = (unsigned char *)malloc( N * sizeof(unsigned char));
  h_out.img_v = (unsigned char *)malloc( N * sizeof(unsigned char));
  if (h_out.img_y == NULL || h_out.img_u == NULL ||  h_out.img_v == NULL) {
    printf("Error in allocating memory for output image in rgb2yuvGPU. Exiting");
    exit(0);
  }

  // declare GPU memory pointers for both kinds of images
  unsigned char * d_r, * d_g, * d_b;
  unsigned char * d_y, * d_u, * d_v;

  //threads sync
  cudaDeviceSynchronize();

  // allocate GPU memory for different pixels structure
  my_mallocForCuda(&d_r); my_mallocForCuda(&d_g); my_mallocForCuda(&d_b);
  my_mallocForCuda(&d_y); my_mallocForCuda(&d_u); my_mallocForCuda(&d_v);

	// transfer the array to the GPU

  //sdkCreateTimer(&timer);
  //sdkStartTimer(&timer);

  //copying input image to GPU
  my_memcpyForCuda(d_r, h_in.img_r, cudaMemcpyHostToDevice);
  my_memcpyForCuda(d_g, h_in.img_g, cudaMemcpyHostToDevice);
  my_memcpyForCuda(d_b, h_in.img_b, cudaMemcpyHostToDevice);

  //sdkStopTimer(&timer);

  //printf("Data copy time from HOST(CPU) TO DEVICE(HOST): %f (ms)\n", sdkGetTimerValue(&timer));
  //sdkDeleteTimer(&timer);

  // launch the kernel with given size of block and threads
  rgb2yuvKernel<<<blocks,threads>>>(d_r, d_g, d_b, d_y, d_u, d_v);

  // copy back the result array to the CPU


  //sdkCreateTimer(&timer);
  //sdkStartTimer(&timer);

  //now copy the output gpu yuv image to host cpu for further processing
  my_memcpyForCuda(h_out.img_y, d_y, cudaMemcpyDeviceToHost);
  my_memcpyForCuda(h_out.img_u, d_u, cudaMemcpyDeviceToHost);
  my_memcpyForCuda(h_out.img_v, d_v, cudaMemcpyDeviceToHost);

  //printf("Data copy time from DEVICE(GPU) TO HOST (CPU): %f (ms)\n", sdkGetTimerValue(&timer));
  //sdkDeleteTimer(&timer);

  //free all the allocated memory
  my_freeForCuda(d_r); my_freeForCuda(d_g); my_freeForCuda(d_b);
  my_freeForCuda(d_y); my_freeForCuda(d_u); my_freeForCuda(d_v);

  cudaDeviceSynchronize();

  //return yuv image
  return h_out;
}

//this funtion returns a rgb image from and input yuv image
PPM_IMG yuv2rgbGPU(YUV_IMG h_in)
{

  //StopWatchInterface *timer=NULL;

  //deciding on threads and blocks for parallel processing
  const dim3 threads(NUM_THREADS,NUM_THREADS,1);
  const dim3 blocks(BLOCKX, BLOCKY, 1);

  //output image and its memory initialization
  PPM_IMG h_out;

  h_out.w = h_in.w;
  h_out.h = h_in.h;
  h_out.img_r = (unsigned char *)malloc(sizeof(unsigned char)*N);
  h_out.img_g = (unsigned char *)malloc(sizeof(unsigned char)*N);
  h_out.img_b = (unsigned char *)malloc(sizeof(unsigned char)*N);
  if (h_out.img_r == NULL || h_out.img_g == NULL ||  h_out.img_b == NULL) {
    printf("Error in allocating memory for output image in yuv2rgbGPU. Exiting");
    exit(0);
  }

  // declare GPU memory pointers for both kinds of images
  unsigned char * d_y, * d_u, * d_v;
  unsigned char * d_r, * d_g, * d_b;

  //threads sync
  cudaDeviceSynchronize();

  //allocate GPU memory for different pixels structure
  my_mallocForCuda(&d_r); my_mallocForCuda(&d_g); my_mallocForCuda(&d_b);
  my_mallocForCuda(&d_y); my_mallocForCuda(&d_u); my_mallocForCuda(&d_v);

  //sdkCreateTimer(&timer);
  //sdkStartTimer(&timer);

  //copying yuv image from host to cuda gpu
  my_memcpyForCuda(d_y, h_in.img_y, cudaMemcpyHostToDevice);
  my_memcpyForCuda(d_u, h_in.img_u, cudaMemcpyHostToDevice);
  my_memcpyForCuda(d_v, h_in.img_v, cudaMemcpyHostToDevice);

  //sdkStopTimer(&timer);
  //printf("Data copy time from HOST(CPU) TO DEVICE(HOST): %f (ms)\n", sdkGetTimerValue(&timer));
  //sdkDeleteTimer(&timer);

  //calling kernel function with blocks and threads
  yuv2rgbKernel<<<blocks,threads>>>(d_y, d_u, d_v, d_r, d_g, d_b);

  //sdkCreateTimer(&timer);
  //sdkStartTimer(&timer);

  //now copy the output gpu rgb image to host cpu for further processing
  my_memcpyForCuda(h_out.img_r, d_r, cudaMemcpyDeviceToHost);
  my_memcpyForCuda(h_out.img_g, d_g, cudaMemcpyDeviceToHost);
  my_memcpyForCuda(h_out.img_b, d_b, cudaMemcpyDeviceToHost);

  //printf("Data copy time from DEVICE(GPU) TO HOST (CPU): %f (ms)\n", sdkGetTimerValue(&timer));
  //sdkDeleteTimer(&timer);

  //freeing all allocated memory
  my_freeForCuda(d_r); my_freeForCuda(d_g); my_freeForCuda(d_b);
  my_freeForCuda(d_y); my_freeForCuda(d_u); my_freeForCuda(d_v);

  //returning final image
  return h_out;
}

//Convert RGB to YUV444, all components in [0, 255]
YUV_IMG rgb2yuv(PPM_IMG img_in)
{
    YUV_IMG img_out;
    int i;//, j;
    unsigned char r, g, b;
    unsigned char y, cb, cr;

    img_out.w = img_in.w;
    img_out.h = img_in.h;
    img_out.img_y = (unsigned char *)malloc(sizeof(unsigned char)*img_out.w*img_out.h);
    img_out.img_u = (unsigned char *)malloc(sizeof(unsigned char)*img_out.w*img_out.h);
    img_out.img_v = (unsigned char *)malloc(sizeof(unsigned char)*img_out.w*img_out.h);

    for(i = 0; i < img_out.w*img_out.h; i ++){
        r = img_in.img_r[i];
        g = img_in.img_g[i];
        b = img_in.img_b[i];

        y  = (unsigned char)( 0.299*r + 0.587*g +  0.114*b);
        cb = (unsigned char)(-0.169*r - 0.331*g +  0.499*b + 128);
        cr = (unsigned char)( 0.499*r - 0.418*g - 0.0813*b + 128);

        img_out.img_y[i] = y;
        img_out.img_u[i] = cb;
        img_out.img_v[i] = cr;
    }

    return img_out;
}

unsigned char clip_rgb(int x)
{
    if(x > 255)
        return 255;
    if(x < 0)
        return 0;

    return (unsigned char)x;
}

//Convert YUV to RGB, all components in [0, 255]
PPM_IMG yuv2rgb(YUV_IMG img_in)
{
    PPM_IMG img_out;
    int i;
    int  rt,gt,bt;
    int y, cb, cr;


    img_out.w = img_in.w;
    img_out.h = img_in.h;
    img_out.img_r = (unsigned char *)malloc(sizeof(unsigned char)*img_out.w*img_out.h);
    img_out.img_g = (unsigned char *)malloc(sizeof(unsigned char)*img_out.w*img_out.h);
    img_out.img_b = (unsigned char *)malloc(sizeof(unsigned char)*img_out.w*img_out.h);

    for(i = 0; i < img_out.w*img_out.h; i ++){
        y  = (int)img_in.img_y[i];
        cb = (int)img_in.img_u[i] - 128;
        cr = (int)img_in.img_v[i] - 128;

        rt  = (int)( y + 1.402*cr);
        gt  = (int)( y - 0.344*cb - 0.714*cr);
        bt  = (int)( y + 1.772*cb);

        img_out.img_r[i] = clip_rgb(rt);
        img_out.img_g[i] = clip_rgb(gt);
        img_out.img_b[i] = clip_rgb(bt);
    }

    return img_out;
}
