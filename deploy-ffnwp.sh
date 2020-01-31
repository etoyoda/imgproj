#!/bin/bash
set -Ceuxo pipefail
test -d /nwp/bin
sudo -u nwp cp imgproj run-dst.sh /nwp/bin
