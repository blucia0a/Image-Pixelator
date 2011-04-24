#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

#define NUM_XSTEPS 20
#define NUM_YSTEPS 20

int main(int argc, char *argv[])
{
  IplImage* img = 0; 
  IplImage* gimg = 0; 
  int height,width,step,channels;
  uchar *data;
  int i,j,k;

  if(argc<2){
    printf("Usage: main <image-file-name>\n\7");
    exit(0);
  }

  // load an image  
  img=cvLoadImage(argv[1]);
  if(!img){
    printf("Could not load image file: %s\n",argv[1]);
    exit(0);
  }
  gimg = cvCreateImage( cvGetSize(img), 8, 1);

  cvCvtColor(img,gimg,CV_RGB2GRAY);

  // get the image data
  height    = gimg->height;
  width     = gimg->width;
  step      = gimg->widthStep;
  channels  = gimg->nChannels;
  data      = (uchar *)gimg->imageData;
  printf("Processing a %dx%d image with %d channels and step of %d\n",height,width,channels,step); 
  
  unsigned perX = width / NUM_XSTEPS + 1;
  unsigned perY = height / NUM_YSTEPS + 1;

  // invert the image
  unsigned long long avgIntensity[NUM_XSTEPS * NUM_YSTEPS];
  memset(avgIntensity,0,NUM_XSTEPS * NUM_YSTEPS * sizeof(avgIntensity[0]));

  for(i=0;i<height;i++){ 

    for(j=0;j<width;j++){ 

      for(k=0;k<channels;k++){

        uchar d = data[ i*step  +  j*channels  +  k];

        unsigned ycoord = (i / perY);
        unsigned xcoord = (j / perX);
        assert(xcoord < NUM_XSTEPS);
        assert(ycoord < NUM_YSTEPS);

        avgIntensity[ xcoord * NUM_XSTEPS + ycoord ] += d;
      }
    }
  }
  float favgIntensity[NUM_XSTEPS * NUM_YSTEPS];
  float segPixels = (float)(height * width) / (float)(NUM_XSTEPS * NUM_YSTEPS);
  fprintf(stderr,"height = %u. width = %u. xs = %u. ys = %u. seg = %f\n",height,width,NUM_XSTEPS, NUM_YSTEPS,segPixels);
  for(int i = 0; i < NUM_XSTEPS * NUM_YSTEPS; i++ ){
    printf("%d: %llu divided by %f = %f\n",i,avgIntensity[i],segPixels,((float)avgIntensity[i])/segPixels);
    favgIntensity[i] = (float)avgIntensity[i]/segPixels;
  } 

  for(i=0;i<height;i++){ 
    for(j=0;j<width;j++){ 
      for(k=0;k<channels;k++){

        unsigned ycoord = (i / perY);
        unsigned xcoord = (j / perX);
        assert(xcoord < NUM_XSTEPS);
        assert(ycoord < NUM_YSTEPS);

        data[ i*step  +  j*channels  +  k] = favgIntensity[ xcoord * NUM_XSTEPS + ycoord ];
      }
    }
  }
 

  cvSaveImage("out.jpg",gimg);

  // release the image
  cvReleaseImage(&img );
  return 0;
}
