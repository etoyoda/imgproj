#!/bin/bash

PATH=/bin:/usr/bin:/usr/local/bin

PUBDATA=/var/www/html/wisdata/arch

cd $(dirname $0)

set $(TZ=JST-9 date '+%Y%m %d%H %M %H')
ym=${1}
ymdhn=${1}${2}00
n=$3
h=$4
if [[ $n < 18 ]]; then
  set $(TZ=PRV-8 date '+%Y%m%d%H00 %H')
  ymdhn=$1
  h=$2
fi

if test -d gmsir.$ymdhn ; then
  echo gmsir.$ymdhn already present
else
  mkdir -p gmsir.$ymdhn
  wget -q -Ogmsir.${ymdhn}/gmsir.png \
    http://www.jma.go.jp/jp/gms/imgs/6/infrared/1/${ymdhn}-00.png
  mkdir -p gmsir.$ymdhn/4/{10,11,12,13,14,15,0,1,2}
  ./imgproj -pp,lc140.7,sw494.7,sh491.9,cw511.5,ch511.6,oc gmsir.${ymdhn}/gmsir.png \
    gmsir.$ymdhn/4/{11,12,13,14,15,0,1,2}/{4,5,6,7,8,9}.png \
    gmsir.$ymdhn/4/10/{7,8}.png \
    gmsir.$ymdhn/4/{14,15}/10.png
  zip -qqr gmsir.$ymdhn.zip gmsir.$ymdhn
  test -d ${PUBDATA}/files/$ym || mkdir ${PUBDATA}/files/$ym
  mv gmsir.$ymdhn.zip ${PUBDATA}/files/$ym
fi

if test X$h != X09 -a X$h != X21 ; then
  : nothing
elif test -d gmswv.$ymdhn ; then
  echo gmswv.$ymdhn already present
else
  mkdir -p gmswv.$ymdhn
  wget -q -Ogmswv.${ymdhn}/gmswv.png \
    http://www.jma.go.jp/jp/gms/imgs/6/watervapor/1/${ymdhn}-00.png
  mkdir -p gmswv.$ymdhn/4/{11,12,13,14,15,0,1,2}
  ./imgproj -pp,lc140.7,sw494.7,sh491.9,cw511.5,ch511.6,ob gmswv.${ymdhn}/gmswv.png \
    gmswv.$ymdhn/4/{11,12,13,14,15,0,1,2}/{4,5,6,7,8,9}.png \
    gmswv.$ymdhn/4/{14,15}/10.png
  zip -qqr gmswv.$ymdhn.zip gmswv.$ymdhn
  test -d ${PUBDATA}/files/$ym || mkdir ${PUBDATA}/files/$ym
  mv gmswv.$ymdhn.zip ${PUBDATA}/files/$ym
fi

if test X$h != X09 -a X$h != X15 ; then
  : nothing
elif test -d gmsvs.$ymdhn ; then
  echo gmsvs.$ymdhn already present
else
  mkdir -p gmsvs.$ymdhn
  wget -q -Ogmsvs.${ymdhn}/gmsvs.png \
    http://www.jma.go.jp/jp/gms/imgs_c/6/visible/1/${ymdhn}-00.png
  mkdir -p gmsvs.$ymdhn/4/{11,12,13,14,15,0,1}
  ./imgproj -pp,lc140.7,sw494.7,sh491.9,cw511.5,ch511.6 gmsvs.${ymdhn}/gmsvs.png \
    gmsvs.$ymdhn/4/{11,12,13,14,15,0,1}/{4,5,6,7,8,9}.png \
    gmsvs.$ymdhn/4/{14,15}/10.png
  zip -qqr gmsvs.$ymdhn.zip gmsvs.$ymdhn
  test -d ${PUBDATA}/files/$ym || mkdir ${PUBDATA}/files/$ym
  mv gmsvs.$ymdhn.zip ${PUBDATA}/files/$ym
fi

bash -$- list.sh

rm -f gmsir
ln -s -f gmsir.$ymdhn gmsir
