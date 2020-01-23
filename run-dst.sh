#!/bin/bash
set -Ceuo pipefail

imgtime=$(ruby -e 'puts (Time.now - 4900).utc.strftime("%Y%m%d%H50")')

if [ ! -f z-himdst.png ]; then
  url="https://www.data.jma.go.jp/gmd/env/kosa/himawari/img/DST/${imgtime}_DST.jpg"
  wget -q -O"z-himdst.jpg" $url
  convert z-himdst.jpg z-himdst.png
fi

proj=z4x12-14y5-7
#proj=z5x25-29y10-14
#proj=z6x52-57y22-27

./imgproj -pr,ba50,bz20,la110,lz150 z-himdst.png :${proj}:himdst${imgtime}-${proj}.png

rm -f z-himdst*
