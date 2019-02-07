#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "wrong number of arguments!"
  echo "usage:"
  echo "./buildImage path/to/exec/to/copy"
  exit 1
fi

cp $1 .
filename=$(basename $1)
echo $filename
docker build --build-arg EXECUTABLE_PATH=$filename -t assystem-demo .
rm $filename
