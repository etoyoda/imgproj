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
    case $dir in
    gmsir*)
      nm=Himawari-IR
      url='https://www.jma.go.jp/jp/gms/large.html?area=6&element=0'
      ;;
    gmswv*)
      nm=Himawari-WV
      url='https://www.jma.go.jp/jp/gms/large.html?area=6&element=2'
      ;;
    gmsvs*)
      nm=Himawari-VS
      url='https://www.jma.go.jp/jp/gms/largec.html?area=6&element=1'
      ;;
    gskwx*)
      nm=Suikei-Wx
      url='http://www.data.jma.go.jp/obj/bunpu/index.html?000?wthr'
      ;;
    gskte*)
      nm=Suikei-T
      url='http://www.data.jma.go.jp/obj/bunpu/index.html?000?temp'
      ;;
    gb??t*)
      nm=T-fcst$(echo $dir | cut -b3-4)
      url='http://www.jma.go.jp/jp/mesh20/000.html?elementCode=1'
      ;;
    gb??w*)
      nm=Wx-fcst$(echo $dir | cut -b3-4)
      url='http://www.jma.go.jp/jp/mesh20/000.html?elementCode=0'
      ;;
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
