#!/bin/bash

PATH=/bin:/usr/bin:/usr/local/bin

cd $(dirname $0)

export TZ=JST-9

: > z.tmp
for dir in g*.20*
do
  if $(ruby -rtime -e 'exit(Time.parse(ARGV.first) > Time.now - 86400 ? 0 : 1)' $dir)
  then
    dhn=
    if test -f $dir/validtime.txt ; then
      set $(cat $dir/validtime.txt)
      dhn=${1}-${2}-${3}T${4}${5}
    else
      dhn=$(ruby -rtime -e \
       'puts Time.parse(ARGV.first).utc.strftime("%Y-%m-%dT%H%M")' ${dir})
    fi
    nm=X
    url='https://www.jma.go.jp/jp/gms/large.html?area=6'
    case $dir in
    gmsir*)  nm=Himawari-IR ;;
    gmswv*)  nm=Himawari-WV ;;
    gmsvs*)  nm=Himawari-VS ;;
    gskwx*)  nm=SuikeiWx ;;
    gskte*)  nm=SuikeiT ;;
    gb??t*)  nm=T-fcst$(echo $dir | cut -b3-4) ;;
    gb??w*)  nm=Wx-fcst$(echo $dir | cut -b3-4) ;;
    esac
    echo "${dhn};\"${dir}\": {\"vn\":\"${nm}/${dhn}\", \"url\":\"${url}\"}," >> z.tmp
  else
    rm -rf $dir
  fi
done
echo '{' > z2.tmp
sort z.tmp | sed 's/^[^;]*;//' >> z2.tmp
echo '"stop": "dummy" }' >> z2.tmp
rm -f z.tmp

mv -f z2.tmp tiles.json
