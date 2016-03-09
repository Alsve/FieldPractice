/* Nama file : ssi-proccess version 2.0 (simple-scanner image processing)
 * Author    : Alrayan
 * Tanggal dibuat : 23 Agustus 2015
 *
 * Install this dependency in order to work (ubuntu 14.04) :
 * opencv-linux     : http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/3.0.0/
 * Langkah install  : http://stackoverflow.com/questions/25726768/opencv-3-0-trouble-with-installation
 * standard library C
 *
 * library yang digunakan dalam compile : `pkg-config --libs opencv`
 *
 * Update 2.0 : Eliminasi background lebih baik
 * Weakness   : Performa lebih lambat
 *
 * NOTE: Hanya dapat digunakan pada gambar berwarna, dan berlatar selain hitam
 *       Atau dapat dilakukan untuk latar hitam : mengganti parameter eliminasi
 *       menjadi selain warna hitam
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

void preview_on_demand(IplImage *src);
void rerata_std_luas_warna(IplImage *gambar, int stddevw[], int meanw[], int *luas);

int main(int argc, char ** argv){
    IplImage *gambar = 0; //Untuk menyimpan data gambar
    uchar* data; //untuk menyimpan data gambar untuk manipulasi
    int height, width, step, channels; //Untuk menyimpan data gambar tinggi, langkah, dan layar warna;
    int i, j; //untuk interasi
    int luas;
    int b, g, r;

    //variable input perhitungan
    int mean[3], stddev[3];
    int Pxmin[4], Pxmax[4];

    //Variable perhitungan rata-rata warna dan luas
    int meanw[3], stddevw[3];

    //Variable control perhitungan
    int temp1, temp2;
    char tau;
    bool preview;
    FILE *acuan;

    //cek argumen apakah filename sudah termasuk di dalamnya(OK)
    if(argc < 2){
        printf("Pemakaian : nama_program <lintas_ke_gambar> ");
        printf("<lintas_ke_acuan_background> <PreviewAktif?> [<rentang_TAU>]\n\7");
        exit(1);
    }

    //cek acuan background (OK)
    if(argc < 3){
        printf("Pemakaian : nama_program <lintas_ke_gambar>");
        printf("<lintas_ke_acuan_background> <PreviewAktif?> [<rentang_TAU>]\n\7");
        printf("Jika tidak mempunyai data acuan warna dapat dilakukan dengan applikasi ssi-image\n");
        exit(1);
    }

    //cek ke-aktifan preview hasil (OK)
    if(argc < 4){
        printf("Preview background : Tidak aktif");
        preview = false;
    } else if(strcasecmp(argv[3], "yes") == 0 || strcasecmp(argv[3], "y") == 0 ){
        preview = true;
    } else preview = false;

    //cek argumen apakah ada nilai awal untuk rentang TAU (OK)
    if(argc < 5){
        printf("Rentang tau akan dipasang pada default (=10)");
        tau = 10;  //nilai rentang standar default TAU
    } else {
        tau = atoi(argv[4]); //Membaca nilai tau dari input argumen
    }

    //Membaca file gambar (OK)
    gambar = cvLoadImage(argv[1],CV_LOAD_IMAGE_COLOR);
    if(!gambar){
        printf("Gambar tidak dapat diolah: %s\n",argv[1]);
        exit(1);
    }

    //Membaca acuan warna latar (OK)
    acuan = fopen(argv[2],"r");
    if(acuan == NULL){
        fprintf(stderr, "Berkas acuan tidak dapat ditemukan. \n");
        exit(1);
    }

    i = 2; //Memulai baca dari R, lalu G, dan terakhir B -Alsve

    while(fscanf(acuan, "%d %d", &temp1, &temp2) != EOF){
        mean[i] = temp1;
        stddev[i] = temp2;
        i--;
    }

    fclose(acuan); //Tutup file acuan

    //Penentuan properties (rentang pixel) yang dihapus (OK)
    for(i=0;i<3;i++){
            Pxmin[i]=mean[i]-stddev[i]*tau;
            Pxmax[i]=mean[i]+stddev[i]*tau;
            if (Pxmax[i] > 255) Pxmax[i] = 255;
            if (Pxmin[i] < 0) Pxmin[i] = 0;
    }

    //debug purpose
    //for(i=0;i<3;i++)printf("%d %d\n", mean[i], stddev[i]);
    //for(i=0;i<5;i++)printf("%s ", argv[i]);
    //if (preview) printf("Preview is on!\n");
    //printf("The value of tau : %d", tau);
    //for(i=0;i<3;i++)printf("%d : %d rentang sampai dengan %d\n",i, Pxmin[i],Pxmax[i]);

    //Penentuan properties gambar
     width   = gambar->width;
     height  = gambar->height;
     step    = gambar->widthStep;
     channels= gambar->nChannels;
     data = (uchar *) gambar->imageData;

     //Eliminasi Background Per dua channels warna
     for(i=0;i<height;i++){
         for(j=0;j<width;j++){
             b = data[i*step+j*channels+0];
             g = data[i*step+j*channels+1];
             if(b > Pxmin[0] && b < Pxmax[0]){
              if(g > Pxmin[1] && g < Pxmax[1]){
                   data[i*step+j*channels+0]=0;
                   data[i*step+j*channels+1]=0;
                   data[i*step+j*channels+2]=0;
              }
             }
         }
     }

     for(i=0;i<height;i++){
         for(j=0;j<width;j++){
             b = data[i*step+j*channels+0];
             r = data[i*step+j*channels+2];
             if(b > Pxmin[0] && b < Pxmax[0]){
               if(r > Pxmin[2] && r < Pxmax[2]){
                   data[i*step+j*channels+0]=0;
                   data[i*step+j*channels+1]=0;
                   data[i*step+j*channels+2]=0;
               }
             }
         }
     }

     for(i=0;i<height;i++){
         for(j=0;j<width;j++){

             g = data[i*step+j*channels+1];
             r = data[i*step+j*channels+2];
              if(g > Pxmin[1] && g < Pxmax[1]){
               if(r > Pxmin[2] && r < Pxmax[2]){
                   data[i*step+j*channels+0]=0;
                   data[i*step+j*channels+1]=0;
                   data[i*step+j*channels+2]=0;
               }
              }
         }
     }

    //Perhitungan rerata warna dan luas
    rerata_std_luas_warna(gambar, stddevw, meanw, &luas);

    //Jika atribut preview diaktifkan maka hasil eliminasi gambar sebelum iterasi selanjutnya ditampilkan
    if (preview){
        preview_on_demand(gambar);
    }

    //Informative text
    printf("R(%d_+-_%d) ", meanw[2], stddevw[2]);
    printf("G(%d_+-_%d) ", meanw[1], stddevw[1]);
    printf("B(%d_+-_%d) ", meanw[0], stddevw[0]);
    printf("Luas(%d px)\n", luas);

    //Tulis ke berkas
    acuan = fopen("./hasil.txt","a");
    fprintf(acuan,"%d,%d,",meanw[2],stddevw[2]);
    fprintf(acuan,"%d,%d,",meanw[1],stddevw[1]);
    fprintf(acuan,"%d,%d,",meanw[0],stddevw[0]);
    fprintf(acuan,"%d,%s\n",luas,argv[1]);
    fclose(acuan);


    return 0;
}

void preview_on_demand(IplImage *src){
    //Penampilan preview grafis
    cvNamedWindow("Gambar Sesudah Diproses",CV_WINDOW_AUTOSIZE);
    cvMoveWindow("Gambar Sesudah Diproses", 100, 100);
    cvShowImage("Gambar Sesudah Diproses", src );

    cvWaitKey(0);
    cvReleaseImage(&src);
}

void rerata_std_luas_warna(IplImage *gambar, int stddevw[], int meanw[], int *luasa){
    int r,g,b; //variable sementara
    int luas = 0;
    int width, height, step, channels;
    ulong sumwR = 0;
    ulong sumwG = 0;
    ulong sumwB = 0;
    uchar *data;
    int i,j;

    //Penentuan properties gambar
    width   = gambar->width;
    height  = gambar->height;
    step    = gambar->widthStep;
    channels= gambar->nChannels;
    data = (uchar *) gambar->imageData;

    //Perhitungan sigma warna
        for(i=0;i<height;i++){
            for(j=0;j<width;j++){
                b = data[i*step+j*channels+0];
                g = data[i*step+j*channels+1];
                r = data[i*step+j*channels+2];
                if(r != 0 || g != 0 || b != 0){
                    luas+=1;
                    sumwB+=b;
                    sumwG+=g;
                    sumwR+=r;
                }
            }
        }
    //Perhitungan rerata warna
    meanw[0] = sumwB/luas; //B
    meanw[1] = sumwG/luas; //G
    meanw[2] = sumwR/luas; //R

    //clear sum
    sumwB = sumwG = sumwR = 0;

    //calculate the standard deviation of each color channels
    // calculate sum deviation value of blue pixel selected 
    for(i=0;i<height;i++){
        for(j=0;j<width;j++){
            b = data[i*step+j*channels+0];
            g = data[i*step+j*channels+1];
            r = data[i*step+j*channels+2];
            if(r != 0 || g != 0 || b != 0){
                sumwB += abs(meanw [0] - b);
                sumwG += abs(meanw [1] - g);
                sumwR += abs(meanw [2] - r);
            }
        }
    }

    //calculate deviation from sum
    stddevw[0] = sumwB/luas; //B
    stddevw[1] = sumwG/luas; //G
    stddevw[2] = sumwR/luas; //R

     //debug purpose
     //for(i=1;i<4;i++) printf("%lu ", sumw[i]);

    *luasa = luas;
}
