#!/bin/bash

PATH=/bin:/usr/bin:/usr/local/bin

PUBDATA=/var/www/html/wisdata/arch

cd $(dirname $0)

set $(TZ=JST-9 date '+%Y%m %d%H %M %H')
ym=${1}
ymdhn=${1}${2}00
n=$3
h=$4
if [[ $n < 20 ]]; then
  set $(TZ=PRV-8 date '+%Y%m%d%H00 %H')
  ymdhn=$1
  h=$2
fi

if test -d gskwx.$ymdhn ; then
  echo gskwx.$ymdhn already present
else
  mkdir -p gskwx.$ymdhn
  wget -q -Ogskwx.${ymdhn}/gskwx.png \
    http://www.data.jma.go.jp/obd/bunpu/img/wthr/000/wthr_000_${ymdhn}.png
  mkdir -p gskwx.$ymdhn/4/{12,13,14}
  ./imgproj -pl, gskwx.${ymdhn}/gskwx.png \
    gskwx.$ymdhn/4/{12,13,14}/{5,6,7}.png
  zip -qqr gskwx.$ymdhn.zip gskwx.$ymdhn
  test -d ${PUBDATA}/files/$ym || mkdir ${PUBDATA}/files/$ym
  mv gskwx.$ymdhn.zip ${PUBDATA}/files/$ym
fi

bash -$- list.sh

