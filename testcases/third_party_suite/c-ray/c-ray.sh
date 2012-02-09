#!/bin/sh

BASE=$(dirname $0)

cat ${BASE}/scene | c-ray-f > /dev/null
cat ${BASE}/scene | c-ray-mt -t 32 > /dev/null
cat ${BASE}/sphfract | c-ray-f > /dev/null
cat ${BASE}/sphfract | c-ray-mt -t 32 > /dev/null
cat ${BASE}/sphfract | c-ray-f -s 1024x768 -r 8 > /dev/null
cat ${BASE}/sphfract | c-ray-mt -t 32 -s 1024x768 -r 8 > /dev/null
cat ${BASE}/scene | c-ray-f -s 7500x3500 > /dev/null
cat ${BASE}/scene | c-ray-mt -t 32 -s 7500x3500 > /dev/null

