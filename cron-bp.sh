#!/bin/bash

PATH=/bin:/usr/bin:/usr/local/bin

PUBDATA=/var/www/html/wisdata/arch

set -e

cd $(dirname $0)

set $(TZ=JST-9 date '+%Y %m %d %H')
y=$1
m=$2
d=$3
h=$4
case "$h" in
17|18|19|20|21|22|23)
  h=18 ;;
11|12|13|14|15|16)
  h=12 ;;
05|06|07|08|09|10)
  h=06 ;;
*)
  set $(TZ=UTC date '+%Y %m %d')
  y=$1
  m=$2
  d=$3
  h=18
  ;;
esac
ymdhn=${y}${m}${d}${h}00

for item in cond temp
do
  ff=''
  case "$item.$h" in
  temp.06) ff='03 08' ;;  # 15JST, 1d+06JST
  cond.06) ff='01 05 07' ;;  # 00Z, 12Z, 18Z
  temp.12) ff='01 06 08' ;;  # 15JST, 1d+06JST, 1d+12JST
  cond.12) ff='03 07' ;;  # 12Z, 1d+00Z
  temp.18) ff='04 07' ;;  # 1d+06JST, 1d+15JST
  cond.18) ff='01 05 08' ;;  # 12Z, 1d+00Z, 1d+09Z
  esac
  for f in $ff
  do
    gbffx=dummy
    case $item in
    cond) gbffx=gb${f}w ;;
    temp) gbffx=gb${f}t ;;
    esac
    if test -d ${gbffx}.$ymdhn ; then
      test ! -t 2 || echo ${gbffx}.$ymdhn already present >&2
    else
      mkdir -p ${gbffx}.$ymdhn
      ruby -e 'y,m,d,h,f = ARGV.map{|s|s.to_i(10)};
        puts((Time.gm(y,m,d,h)+(f*3-9)*3600).strftime("%Y %m %d %H %M"))' \
        $y $m $d $h $f > ${gbffx}.$ymdhn/validtime.txt
      url=http://www.jma.go.jp/jp/mesh20/imgs/${item}/000/${ymdhn}-${f}.png
      if ! wget -q -O${gbffx}.${ymdhn}/${item}_000_${ymdhn}-${f}.png $url ; then
        echo cannot download $url >&2
        false
      fi
      mkdir -p ${gbffx}.$ymdhn/4/{13,14}
      ./imgproj -pr,ou,la118.7252,lz155.2842,ba47.2639,bz23.7904 \
        ${gbffx}.${ymdhn}/${item}_000_${ymdhn}-${f}.png \
        ${gbffx}.$ymdhn/4/{13,14}/{5,6}.png
      if test -d ${PUBDATA} ; then
        zip -qqr ${gbffx}.$ymdhn.zip ${gbffx}.$ymdhn
	chmod g+w ${gbffx}.$ymdhn.zip
        test -d ${PUBDATA}/files/${y}${m} || mkdir -m 0775 ${PUBDATA}/files/${y}${m}
        mv ${gbffx}.$ymdhn.zip ${PUBDATA}/files/${y}${m}
      fi
    fi

  done
done

bash -$- list.sh
