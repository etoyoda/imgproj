#!/bin/bash

PATH=/bin:/usr/bin:/usr/local/bin

cd $(dirname $0)

export TZ=JST-9

echo '{' > z.tmp
for dir in g*.20*
do
  if $(ruby -rtime -e 'exit(Time.parse(ARGV.first) > Time.now - 86400 ? 0 : 1)' $dir)
  then
    dhn=$(ruby -rtime -e 'puts Time.parse(ARGV.first).utc.strftime("%Y-%m-%dT%H%M")' ${dir})
    nm=X
    url='https://www.jma.go.jp/jp/gms/large.html?area=6'
    case $dir in
    gmsir*)  nm=Himawari-IR ;;
    gmswv*)  nm=Himawari-WV ;;
    gmsvs*)  nm=Himawari-VS ;;
    esac
    echo "\"${dir}\": {\"vn\":\"${nm}/${dhn}\", \"url\":\"${url}\"}," >> z.tmp
  else
    rm -rf $dir
  fi
done
echo '"stop": "dummy" }' >> z.tmp

mv -f z.tmp tiles.json
