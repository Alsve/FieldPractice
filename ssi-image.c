/* Nama file      : ss-image (configure simple-scanner image processing)
 * Author         : Alrayan
 * Tanggal dibuat : 23 Agustus 2015
 *
 * Install this dependency in order to work (ubuntu 14.04) :
 * opencv-linux     : http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/3.0.0/
 * Langkah install  : http://stackoverflow.com/questions/25726768/opencv-3-0-trouble-with-installation
 * standard library C
 *
 * library yang digunakan dalam compile : `pkg-config --libs opencv`
 *
 * NOTE: Hanya dapat digunakan pada gambar berwarna, dan berlatar selain hitam
 *       Atau dapat dilakukan untuk latar hitam : mengganti parameter eliminasi
 *       menjadi selain warna hitam
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>


int main(int argc, char *argv[])
{
  IplImage* img = 0;
  int height,width,step,channels;
  uchar *data;
  int i,j;

  ulong temp, mean[3];
  ulong sum[4], stddev[3];
  int Px, PxminR, PxmaxR, PxminG, PxmaxG, PxminB, PxmaxB;
  int tau;

  FILE *keacuan;

  if(argc<2){
    printf("Usage: main <image-file-name> <std-dev-multiplier> \n\7");
    exit(0);
  }

  printf("Please insert tau => ");
  scanf("%d",&tau);

  // load an image
  img=cvLoadImage(argv[1],CV_LOAD_IMAGE_COLOR);
  if(!img){
    printf("Could not load image file: %s\n",argv[1]);
    exit(0);
  }

  // get the image data
  height    = img->height;
  width     = img->width;
  step      = img->widthStep;
  channels  = img->nChannels;
  data      = (uchar *)img->imageData;
  printf("Processing a %dx%d image with %d channels\n",height,width,channels);

  // create a window
  cvNamedWindow("mainWin", CV_WINDOW_AUTOSIZE);
  cvMoveWindow("mainWin", 100, 100);

  // calculate sum value of blue pixel
  for(i=0;i<height;i++){
      for(j=0;j<width;j++){
         temp = data[i*step+j*channels+0];
         sum[1] += temp;
      }
  }

  //calculate sum value of green pixel
  for(i=0;i<height;i++){
      for(j=0;j<width;j++){
          temp = data[i*step+j*channels+1];
          sum[2] += temp;
      }
  }

  //calculate sum value of red pixel
  for(i=0;i<height;i++){
      for(j=0;j<width;j++){
          temp = data[i*step+j*channels+2];
          sum[3] += temp;
      }
  }

  //calculate the mean of BGR of pixel image
  mean[0] = sum[1]/(img->width * img->height); //B
  mean[1] = sum[2]/(img->width * img->height); //G
  mean[2] = sum[3]/(img->width * img->height); //R

  //clear sum
  sum[1] = sum[2] = sum[3] = 0;

  //calculate the standard deviation of each color channels
  // calculate sum deviation value of blue pixel
  for(i=0;i<height;i++){
      for(j=0;j<width;j++){
         temp = abs(mean [0] - data[i*step+j*channels+0]);
         sum[1] += temp;
      }
  }

  //calculate sum deviation value of green pixel
  for(i=0;i<height;i++){
      for(j=0;j<width;j++){
          temp = abs(mean [1] - data[i*step+j*channels+1]);
          sum[2] += temp;
      }
  }

  //calculate sum deviation value of red pixel
  for(i=0;i<height;i++){
      for(j=0;j<width;j++){
          temp = abs(mean [2] - data[i*step+j*channels+2]);
          sum[3] += temp;
      }
  }

  //calculate deviation from sum
  stddev[0] = sum[1]/(img->width * img->height); //B
  stddev[1] = sum[2]/(img->width * img->height); //G
  stddev[2] = sum[3]/(img->width * img->height); //R


  //calculate the deviation of BGR of pixel image
  printf("The mean of BLUE value of image is %lu +- %lu \n",mean[0],stddev[0]);
  printf("the mean of GREEN value of image is %lu +- %lu \n",mean[1],stddev[1]);
  printf("The mean of RED value of image is %lu +- %lu \n",mean[2],stddev[2]);

  // show the image
  cvShowImage("mainWin", img );

  PxminB = (mean[0] - stddev[0]*tau)<0? 0: (mean[0] - stddev[0]*tau);
  PxminG = (mean[1] - stddev[1]*tau)<0? 0: (mean[1] - stddev[1]*tau);
  PxminR = (mean[2] - stddev[2]*tau)<0? 0: (mean[2] - stddev[2]*tau);

  PxmaxB = (mean[0] + stddev[0]*tau)>255? 255: (mean[0] + stddev[0]*tau);
  PxmaxG = (mean[1] + stddev[1]*tau)>255? 255: (mean[1] + stddev[1]*tau);
  PxmaxR = (mean[2] + stddev[2]*tau)>255? 255: (mean[2] + stddev[2]*tau);

  keacuan = fopen("./RGB.txt","w");
  fprintf(keacuan,"%lu %lu\n",mean[2],stddev[2]); //write mean<space>standarddeviation of R
  fprintf(keacuan,"%lu %lu\n",mean[1],stddev[1]); //write mean<space>standarddeviation of G
  fprintf(keacuan,"%lu %lu\n",mean[0],stddev[0]); //write mean<space>standarddeviation of B
  fclose(keacuan);

  //checking background
  for(i=0;i<height;i++){
      for(j=0;j<width;j++){
            Px = data[i*step + j*channels+0];
            if(Px < PxmaxB && Px > PxminB){
                data[i*step + j*channels+0]=0;
                data[i*step + j*channels+1]=0;
                data[i*step + j*channels+2]=0;
            }
      }
  }
  for(i=0;i<height;i++){
      for(j=0;j<width;j++){
          Px = data[i*step + j*channels+1];
          if(Px < PxmaxG && Px > PxminG){
              data[i*step + j*channels+0]=0;
              data[i*step + j*channels+1]=0;
              data[i*step + j*channels+2]=0;
          }
      }
  }
  for(i=0;i<height;i++){
      for(j=0;j<width;j++){
          Px = data[i*step + j*channels+2];
          if(Px < PxmaxR && Px > PxminR){
              data[i*step + j*channels+0]=0;
              data[i*step + j*channels+1]=0;
              data[i*step + j*channels+2]=0;
          }
      }
  }

  // show the image
  cvShowImage("Eliminated data from calibration", img );


  // wait for a key
  cvWaitKey(0);

  // release the image
  cvReleaseImage(&img );
  return 0;
}
