#!/bin/bash

if [ $# -ne 1 ]; then
    echo Usage: $0 ./sssp_XXX
    exit 1;
fi;

for N in 48 56 
do
  echo "nthread$N"
  for i in {1..10}
  do
    ./testpara.sh $1 ../inputs/graphs/rmat16.txt ../inputs/reference-outputs/rmat16-0.txt $N 
  done
done
