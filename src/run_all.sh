#!/bin/bash
make clean
make

echo "fp_1 gshare"
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --gshare:10
echo "fp_1 tournament"
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --tournament:9:10:10
echo "fp_1 custom"
bunzip2 -kc ../traces/fp_1.bz2 | ./predictor --tournament:13:11:11

echo "fp_2 gshare"
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --gshare:10
echo "fp_2 tournament"
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --tournament:9:10:10
echo "fp_2 custom"
bunzip2 -kc ../traces/fp_2.bz2 | ./predictor --tournament:13:11:11

echo "int_1 gshare"
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --gshare:10
echo "int_1 tournament"
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --tournament:9:10:10
echo "int_1 custom"
bunzip2 -kc ../traces/int_1.bz2 | ./predictor --tournament:13:11:11

echo "int_2 gshare"
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --gshare:10
echo "int_2 tournament"
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --tournament:9:10:10
echo "int_2 custom"
bunzip2 -kc ../traces/int_2.bz2 | ./predictor --tournament:13:11:11

echo "mm_1 gshare"
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --gshare:10
echo "mm_1 tournament"
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --tournament:9:10:10
echo "mm_1 custom"
bunzip2 -kc ../traces/mm_1.bz2 | ./predictor --tournament:13:11:11

echo "mm_2 gshare"
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --gshare:10
echo "mm_2 tournament"
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --tournament:9:10:10
echo "mm_2 custom"
bunzip2 -kc ../traces/mm_2.bz2 | ./predictor --tournament:13:11:11


