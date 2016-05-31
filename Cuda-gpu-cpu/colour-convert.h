//Assignment is based on a modified 5kk70 assignment, we thank the source

#ifndef HIST_EQU_COLOR_H
#define HIST_EQU_COLOR_H

typedef struct{
    int w;
    int h;
    unsigned char * img_r;
    unsigned char * img_g;
    unsigned char * img_b;
} PPM_IMG;

typedef struct{
    int w;
    int h;
    unsigned char * img_y;
    unsigned char * img_u;
    unsigned char * img_v;
} YUV_IMG;

PPM_IMG read_ppm(const char * path);
void write_ppm(PPM_IMG img, const char * path);
void write_yuv(YUV_IMG img,const char * path);
void free_ppm(PPM_IMG img);
void free_yuv(YUV_IMG img);

YUV_IMG rgb2yuv(PPM_IMG img_in);
PPM_IMG yuv2rgb(YUV_IMG img_in);


YUV_IMG rgb2yuvGPU(PPM_IMG img_in);
PPM_IMG question1(PPM_IMG img_in);
PPM_IMG yuv2rgbGPU(YUV_IMG img_in);

bool confirm_gpu_rgb2yuv(YUV_IMG gpu_img_in, YUV_IMG img_in);
bool confirm_gpu_yuv2rgb(PPM_IMG gpu_img_in, PPM_IMG img_in);


#endif
