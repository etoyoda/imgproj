#!/bin/bash
set -Ceuo pipefail

: ${basetime:=$(ruby -e 'puts(Time.at(((Time.now.to_i - 3600) / 10800) * 10800).utc.strftime("%Y-%m-%dT%H:%M:%SZ"))')}
bt=$(ruby -rtime -e 'puts(Time.parse(ARGV.first).utc.strftime("%Y-%m-%dT%HZ"))' $basetime)
imgtime=$(ruby -rtime -e 'puts((Time.parse(ARGV.first) - 3600).utc.strftime("%Y%m%d%H50"))' $basetime)

if [ ! -f z-himdst.png ]; then
  url="https://www.data.jma.go.jp/gmd/env/kosa/himawari/img/DST/${imgtime}_DST.jpg"
  wget -q -O"z-himdst.jpg" $url
  convert z-himdst.jpg z-himdst.png
fi

proj='z4x3299=3753y1389=1814'
#proj='z4x12-14y5-7'
#proj=z5x25-29y10-14
#proj=z6x52-57y22-27

./imgproj -pr,ba50,bz20,la110,lz150 z-himdst.png :${proj}:himdst${bt}.png

rm -f z-himdst*
