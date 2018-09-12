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

for item in wthr temp
do
  gskxx=dummy
  case $item in
  wthr) gskxx=gskwx ;;
  temp) gskxx=gskte ;;
  esac
  if test -d ${gskxx}.$ymdhn ; then
    echo ${gskxx}.$ymdhn already present
  else
    mkdir -p ${gskxx}.$ymdhn
    wget -q -O${gskxx}.${ymdhn}/${item}_000_${ymdhn}.png \
      http://www.data.jma.go.jp/obd/bunpu/img/${item}/000/${item}_000_${ymdhn}.png
    mkdir -p ${gskxx}.$ymdhn/4/{13,14}
    ./imgproj -pr,la117.9977,lz149.9933,ba47.9815,bz19.8322 \
      ${gskxx}.${ymdhn}/${item}_000_${ymdhn}.png \
      ${gskxx}.$ymdhn/4/{13/6,14/5,14/6}.png
    if test -d ${PUBDATA} ; then
      zip -qqr ${gskxx}.$ymdhn.zip ${gskxx}.$ymdhn
      test -d ${PUBDATA}/files/$ym || mkdir ${PUBDATA}/files/$ym
      mv ${gskxx}.$ymdhn.zip ${PUBDATA}/files/$ym
    fi
  fi

done

bash -$- list.sh
