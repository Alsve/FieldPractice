#!/bin/bash

#/* Nama file : scannerPixel.sh (BASH script) (Interface of scanner latar)
# * Author    : Alrayan
# * Tanggal dibuat : 23 Agustus 2015
# *
# * Install this dependency in order to work (ubuntu 14.04) :
# * 1. ss-image (Executable) [build in ubuntu 14.04, GCC 4.8.2, opencv]
# * 2. ssi-process (Executable) [build in ubuntu 14.04, GCC 4.8.2, opencv]
# * 3. Kalibrasi warna latar -> ss-image
# * 4. Eliminasi background, hitung luas, rerata warna, dan standar deviasi pergambar -> ssi-process
# * 
# * Cara menggunakan : ./scannerPixel.h
# *
# * NOTE: Hanya dapat digunakan pada gambar berwarna, dan berlatar selain hitam
# *       Atau dapat dilakukan untuk latar hitam : mengganti parameter eliminasi
# *       menjadi selain warna hitam, hal ini diganti pada source dan built parameter sendiri 
# */


#Konstanta path ke applikasi
BERKAS_ACUAN_RGB=$PWD/RGB.txt
CONFIGURE_APP=$PWD/ss-image
PROCESS_APP=$PWD/ssi-process-v01

#Fungsi konfigurasi warna latar tereliminasi (OK)
function configure_gambar() 
{
    printf "<PATH TO> Gambar latar : "
    read GambarAcuan
    if [ ! -f "$GambarAcuan" ]; then
       printf "\nERROR : Gambar tidak ditemukan. Keluar...\n"
       exit 1
    fi
    $CONFIGURE_APP $GambarAcuan
}

#Fungsi proses gambar
function process_gambar() 
{
    printf "\ntampilkan preview? (ya/tidak) "
    read yatidakdong
    printf "\nthreshold penghapusan latar : (0--takhingga) "
    read thresholddong

    if [ "$yatidakdong" == "ya" ];then
        preview_dong="yes"
    else
        preview_dong="no"
    fi

    #cek input variabel threshold adalah angka    
    re='^[0-9]+$'
    if ! [[ $thresholddong =~ $re ]] ; then
      printf "ERROR : Input bukan angka. Setting default threshold";
      unset thresholddong
      thresholddong=10
      exit 1
    fi
    
    #set default threeshold 10
    if (( $thresholddong < 0 )); then
        printf "set default threshold ke 10" 
        thresholddong=10
    fi
    
    #teks informasi jumlah gambar yang diproses
    jumlah_gambar=$(echo "`ls -l "$folderdimaksud" | grep ".jpg" |wc -l`" | bc);
    printf "\nMelakukan proses terhadap $jumlah_gambar gambar\n"

    #process dilakukan
    cd $folderdimaksud #Pembukaan folder gambar
    for namaberkas in *; do #Iterasi gambar
        cekbenerga=`echo $namaberkas | grep -qi ".jpg"`
        if [ $cekbenerga ]; then
            printf "$namaberkas"
            continue
        fi
        printf "$namaberkas : "
        $PROCESS_APP $namaberkas $BERKAS_ACUAN_RGB $preview_dong $thresholddong #eksekusi
    done
}

#Check applikasi apakah ada DAN dapat dijalankan
if [ ! -x "$CONFIGURE_APP" -a ! -x "$PROCESS_APP" ] ; then
    printf "ERROR : ss-image dan/atau ssi-process tidak ada\n"
    printf "ERROR : ss-image dan/atau ssi-process tidak dapat dijalankan\n"
    printf "Keluar...\n"
    exit 1;
fi

#Melakukan cek apakah ada tidaknya file acuan RGB.txt
if [ ! -f "$BERKAS_ACUAN_RGB" ]; then
    printf "File RGB.txt (acuan latar) tidak ada..\n" 
    printf "lakukan pengaturan warna eliminasi latar? (ya\tidak) "
    if read -t 30 respon; then
        if [ "$respon" == "ya" ]; then
            configure_gambar
        else
            printf "\nERROR : RGB.txt tidak ditemukan. Keluar... \n" 
            exit 1
        fi
    fi
fi

#Input nama folder dari gambar
printf "Masukan directory : "
read folderdimaksud
if [ ! -d "$folderdimaksud" ];then
    printf "ERROR : folder tidak ada. Keluar ... \n"
    exit 1
fi

#Penghapusan latar dan proses dimulai
clear
printf "\nMasuk kedalam --->>> process eliminasi latar\n"
process_gambar
