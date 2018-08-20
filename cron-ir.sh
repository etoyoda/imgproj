#!/bin/bash

PATH=/bin:/usr/bin:/usr/local/bin

cd $(dirname $0)

set $(TZ=JST-9 date '+%Y%m%d%H %M')
ymdhn=${1}00
n=$2
if [[ $n < 18 ]]; then
  ymdhn=$(TZ=PRV-8 date +%Y%m%d%H50)
fi
if test -d gmsir.$ymdhn ; then
  echo gmsir.$ymdhn already present
  exit 1
fi

mkdir -p gmsir.$ymdhn

wget -Ogmsir.${ymdhn}/gmsir.png \
  http://www.jma.go.jp/jp/gms/imgs/6/infrared/1/${ymdhn}-00.png

mkdir -p gmsir.$ymdhn/4/{10,11,12,13,14,15,0,1,2}
./imgproj -pp,lc140.7,sw494.9,sh491.9,cw511.5,ch512.0 gmsir.${ymdhn}/gmsir.png \
  gmsir.$ymdhn/4/{10,11,12,13,14,15,0,1,2}/{4,5,6,7,8,9}.png
zip gmsir.$ymdhn.zip gmsir.$ymdhn
mv gmsir.$ymdhn.zip ../../wisdata/arch/files

ln -s -f gmsir.$ymdhn gmsir
