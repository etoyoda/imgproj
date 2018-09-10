#!/bin/bash

PATH=/bin:/usr/bin:/usr/local/bin

PUBDATA=/var/www/html/wisdata/arch

cd $(dirname $0)

set $(TZ=JST-9 date '+%Y %m %d %H')
y=$1
m=$2
d=$3
h=$4
if (( h >= 18 )); then
  h=18
  ff='04 07'
elif (( h >= 6 )); then
  h=06
  ff='03 08'
else
  set $(TZ=UTC date '+%Y %m %d')
  y=$1
  m=$2
  d=$3
  h=18
  ff='04 07'
fi
ymdhn=${y}${m}${d}${h}00

for f in $ff
do
  for item in temp cond
  do
    gbffx=dummy
    case $item in
    cond) gbffx=gb${f}w ;;
    temp) gbffx=gb${f}t ;;
    esac
    if test -d ${gbffx}.$ymdhn ; then
      echo ${gbffx}.$ymdhn already present
    else
      mkdir -p ${gbffx}.$ymdhn
      wget -q -O${gbffx}.${ymdhn}/${item}_000_${ymdhn}-${f}.png \
        http://www.jma.go.jp/jp/mesh20/imgs/${item}/000/${ymdhn}-${f}.png
      mkdir -p ${gbffx}.$ymdhn/4/{13,14}
      ./imgproj -pr,ou,la118.7252,lz150.4015,ba47.2639,bz21.3982 \
        ${gbffx}.${ymdhn}/${item}_000_${ymdhn}-${f}.png \
        ${gbffx}.$ymdhn/4/{13,14}/{5,6,7}.png
      if test -d ${PUBDATA} ; then
        zip -qqr ${gbffx}.$ymdhn.zip ${gbffx}.$ymdhn
        test -d ${PUBDATA}/files/$ym || mkdir ${PUBDATA}/files/$ym
        mv ${gbffx}.$ymdhn.zip ${PUBDATA}/files/$ym
      fi
    fi

  done
done
