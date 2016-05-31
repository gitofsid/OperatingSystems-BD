#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// CUDA Runtime
#include <cuda_runtime.h>
// Utility and system includes
#include <helper_cuda.h>
// helper for shared that are common to CUDA Samples
#include <helper_functions.h>
#include <helper_timer.h>

#include "colour-convert.h"

YUV_IMG run_cpu_color_test(PPM_IMG img_in);
YUV_IMG run_gpu_color_test(PPM_IMG img_in);
void timerKernelCall();

//this code verifies if gpu generated yuv image is same as cpu generated image
bool confirm_gpu_rgb2yuv(YUV_IMG gpu_img_in, YUV_IMG cpu_img_in) //Place code here that verifies your conversion
{
    int i;
    int counter = 0;
    int width = 0;
    int height = 0;
    if (gpu_img_in.w == cpu_img_in.w && gpu_img_in.h == cpu_img_in.h)
    {
        printf("Image dimensions match up.\n Dimensions are Width = %d and Height=%d  ",gpu_img_in.w, gpu_img_in.h );
        printf("\nstarting pixel by pixel comparision");
        width = gpu_img_in.w;
        height = gpu_img_in.h;

        for(i = 0; i < width * height; i ++)
        {
        //here we are matching each kind of pixel to find how many mismatch occurs
          if(abs(gpu_img_in.img_y[i] - cpu_img_in.img_y[i])>2)
          {
            printf("\nY pixel value for GPU is %d",gpu_img_in.img_y[i])  ;
            printf("\nY pixel value for CPU is %d",cpu_img_in.img_y[i])  ;
            counter++;
        }
        if(abs(gpu_img_in.img_u[i] - cpu_img_in.img_u[i])>2)
        {
          printf("\nU pixel value for GPU is %d",gpu_img_in.img_u[i])  ;
          printf("\nU pixel value for CPU is %d",cpu_img_in.img_u[i])  ;
          counter++;
      }
      if(abs(gpu_img_in.img_v[i] - cpu_img_in.img_v[i])>2)
      {
        printf("\nV pixel value for GPU is %d",gpu_img_in.img_v[i])  ;
        printf("\nV pixel value for CPU is %d",cpu_img_in.img_v[i])  ;
        counter++;
       }

    }
    //if counter is 0 that means its an exact match else not with number of mismatched pixels
    if (counter==0)
        {printf ("\nThe images are an EXACT MATCH\n");}
    else
      {printf ("\nThe images mismatch by %d pixels \n ",counter);}

    }
    else
    {
      printf("Width  and Height dont match \n");

    }

    if (counter ==0)
        return true;
    else
        return false;
}


//this code verifies if gpu generated ppm image is same as cpu generated image
bool confirm_gpu_yuv2rgb(PPM_IMG gpu_img_in, PPM_IMG cpu_img_in) //Place code here that verifies your conversion
{
int i;
int counter = 0;
int width = 0;
int height = 0;
if (gpu_img_in.w == cpu_img_in.w && gpu_img_in.h == cpu_img_in.h)
  {
    printf("Image dimensions match up.\n Dimensions are Width = %d and Height=%d  ",gpu_img_in.w, gpu_img_in.h );
    printf("\nstarting pixel by pixel comparision");
    width = gpu_img_in.w;
    height = gpu_img_in.h;

    for(i = 0; i < width * height; i ++)
      {
        //here we are matching each kind of pixel to find how many mismatch occurs
      if(abs(gpu_img_in.img_r[i] - cpu_img_in.img_r[i])>2)
        {
        printf("\nR pixel value for GPU is %d",gpu_img_in.img_r[i])  ;
        printf("\nR pixel value for CPU is %d",cpu_img_in.img_r[i])  ;
        counter++;
        }
        if(abs(gpu_img_in.img_g[i] - cpu_img_in.img_g[i])>2)
          {
          printf("\nG pixel value for GPU is %d",gpu_img_in.img_g[i])  ;
          printf("\nG pixel value for CPU is %d",cpu_img_in.img_g[i])  ;
          counter++;
          }
          if(abs(gpu_img_in.img_b[i] - cpu_img_in.img_b[i])>2)
            {
            printf("\nB pixel value for GPU is %d",gpu_img_in.img_b[i])  ;
            printf("\nB pixel value for CPU is %d",cpu_img_in.img_b[i])  ;
            counter++;
            }

      }
      //if counter is 0 that means its an exact match else not with number of mismatched pixels
        if (counter==0)
          {printf ("\nThe images are an EXACT MATCH");}
        else
        {printf ("\nThe images mismatch by %d pixels ",counter);}
      }
    else
      {
      printf("\nWidth  and Height dont match \n");

      }

    if (counter ==0)
      return true;
    else
      return false;
}



int main(){
    PPM_IMG img_ibuf_c;
    PPM_IMG img_ibuf_g;

    //New variable to read end result from disk
    PPM_IMG read_rgb_ibuf_c;
    PPM_IMG read_rgb_ibuf_g;

    //New variables  for comparision purposes
    YUV_IMG g_img_obuf_yuv;
    YUV_IMG c_img_obuf_yuv;

    // intialize_gpu
    timerKernelCall();

    //question1

    printf("Running colour space converter .\n");
    img_ibuf_c = read_ppm("in.ppm");
    img_ibuf_g = read_ppm("in.ppm");

    //time the image copy from CPU to GPU AND VICE VERSA
    question1(img_ibuf_g);

    c_img_obuf_yuv=run_cpu_color_test(img_ibuf_c);
    g_img_obuf_yuv=run_gpu_color_test(img_ibuf_g);

    //calling comparision functions
    printf("\n\n\n Comparing YUV images now\n ");
    confirm_gpu_rgb2yuv( g_img_obuf_yuv,  c_img_obuf_yuv);

    //Reading RGB result from disk
    read_rgb_ibuf_c = read_ppm("h_out_rgb.ppm");
    read_rgb_ibuf_g = read_ppm("d_out_rgb.ppm");

    //calling comparision functions
    printf("\n\n\n Comparing RGB images now \n");
    confirm_gpu_yuv2rgb( read_rgb_ibuf_g,  read_rgb_ibuf_c);

    //free up memory
    free_ppm(img_ibuf_c);
    free_ppm(img_ibuf_g);
    free_ppm(read_rgb_ibuf_c);
    free_ppm(read_rgb_ibuf_g);
    free_yuv(g_img_obuf_yuv);
    free_yuv(c_img_obuf_yuv);

    return 0;
}


YUV_IMG run_gpu_color_test(PPM_IMG img_in)
{
    StopWatchInterface *timer=NULL;
    PPM_IMG img_obuf_rgb;
    YUV_IMG img_obuf_yuv;

    printf("\n\n\nStarting GPU processing...\n");


    sdkCreateTimer(&timer);
    sdkStartTimer(&timer);

    img_obuf_yuv = rgb2yuvGPU(img_in); //Start RGB 2 YUV
    sdkStopTimer(&timer);

    printf("RGB to YUV conversion time(GPU): %f (ms)\n", sdkGetTimerValue(&timer));
    sdkDeleteTimer(&timer);

    sdkCreateTimer(&timer);
    sdkStartTimer(&timer);

    img_obuf_rgb = yuv2rgbGPU(img_obuf_yuv); //Start YUV 2 RGB

    sdkStopTimer(&timer);

    printf("YUV to RGB conversion time(GPU): %f (ms)\n", sdkGetTimerValue(&timer));
    sdkDeleteTimer(&timer);

    //generating both images from device gpu
    write_ppm(img_obuf_rgb, "d_out_rgb.ppm");
    write_yuv(img_obuf_yuv, "d_out_yuv.yuv");

   free_ppm(img_obuf_rgb); //Uncomment these when the images exist
   return img_obuf_yuv;
   //free_yuv(img_obuf_yuv);

}


YUV_IMG run_cpu_color_test(PPM_IMG img_in)
{
    StopWatchInterface *timer=NULL;
    PPM_IMG img_obuf_rgb;
    YUV_IMG img_obuf_yuv;


    printf("\n\n\nStarting CPU processing...\n");

    sdkCreateTimer(&timer);
    sdkStartTimer(&timer);

    img_obuf_yuv = rgb2yuv(img_in); //Start RGB 2 YUV

    sdkStopTimer(&timer);
    printf("RGB to YUV conversion time: %f (ms)\n", sdkGetTimerValue(&timer));
    sdkDeleteTimer(&timer);



    sdkCreateTimer(&timer);
    sdkStartTimer(&timer);

    img_obuf_rgb = yuv2rgb(img_obuf_yuv); //Start YUV 2 RGB

    sdkStopTimer(&timer);
    printf("YUV to RGB conversion time: %f (ms)\n", sdkGetTimerValue(&timer));
    sdkDeleteTimer(&timer);

    //generating both imahes from host cpu
    write_yuv(img_obuf_yuv, "h_out_yuv.yuv");
    write_ppm(img_obuf_rgb, "h_out_rgb.ppm");

    free_ppm(img_obuf_rgb);
    return img_obuf_yuv;
    //free_yuv(img_obuf_yuv);

}




PPM_IMG read_ppm(const char * path){
    FILE * in_file;
    char sbuf[256];

    char *ibuf;
    PPM_IMG result;
    int v_max, i;
    in_file = fopen(path, "r");
    if (in_file == NULL){
        printf("Input file not found!\n");
        exit(1);
    }
    /*Skip the magic number*/
    fscanf(in_file, "%s", sbuf);


    //result = malloc(sizeof(PPM_IMG));
    fscanf(in_file, "%d",&result.w);
    fscanf(in_file, "%d",&result.h);
    fscanf(in_file, "%d\n",&v_max);
    printf("Image size: %d x %d\n", result.w, result.h);


    result.img_r = (unsigned char *)malloc(result.w * result.h * sizeof(unsigned char));
    result.img_g = (unsigned char *)malloc(result.w * result.h * sizeof(unsigned char));
    result.img_b = (unsigned char *)malloc(result.w * result.h * sizeof(unsigned char));
    ibuf         = (char *)malloc(3 * result.w * result.h * sizeof(char));


    fread(ibuf,sizeof(unsigned char), 3 * result.w*result.h, in_file);

    for(i = 0; i < result.w*result.h; i ++){
        result.img_r[i] = ibuf[3*i + 0];
        result.img_g[i] = ibuf[3*i + 1];
        result.img_b[i] = ibuf[3*i + 2];
    }

    fclose(in_file);
    free(ibuf);

    return result;
}

void write_yuv(YUV_IMG img, const char * path){//Output in YUV444 Planar
    FILE * out_file;
    int i;


    out_file = fopen(path, "wb");
    fwrite(img.img_y,sizeof(unsigned char), img.w*img.h, out_file);
    fwrite(img.img_u,sizeof(unsigned char), img.w*img.h, out_file);
    fwrite(img.img_v,sizeof(unsigned char), img.w*img.h, out_file);
    fclose(out_file);
}


void write_yuv2(YUV_IMG img, const char * path){ //Output in YUV444 Packed
    FILE * out_file;
    int i;

    char * obuf = (char *)malloc(3 * img.w * img.h * sizeof(char));

    for(i = 0; i < img.w*img.h; i ++){
        obuf[3*i + 0] = img.img_y[i];
        obuf[3*i + 1] = img.img_u[i];
        obuf[3*i + 2] = img.img_v[i];
    }

    out_file = fopen(path, "wb");
    fwrite(obuf,sizeof(unsigned char), 3*img.w*img.h, out_file);
    fclose(out_file);
    free(obuf);
}


void write_ppm(PPM_IMG img, const char * path){
    FILE * out_file;
    int i;

    char * obuf = (char *)malloc(3 * img.w * img.h * sizeof(char));

    for(i = 0; i < img.w*img.h; i ++){
        obuf[3*i + 0] = img.img_r[i];
        obuf[3*i + 1] = img.img_g[i];
        obuf[3*i + 2] = img.img_b[i];
    }
    out_file = fopen(path, "wb");
    fprintf(out_file, "P6\n");
    fprintf(out_file, "%d %d\n255\n",img.w, img.h);
    fwrite(obuf,sizeof(unsigned char), 3*img.w*img.h, out_file);
    fclose(out_file);
    free(obuf);
}

void free_yuv(YUV_IMG img)
{
    free(img.img_y);
    free(img.img_u);
    free(img.img_v);
}

void free_ppm(PPM_IMG img)
{
    free(img.img_r);
    free(img.img_g);
    free(img.img_b);
}
