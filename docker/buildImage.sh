#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "wrong number of arguments!"
  echo "usage:"
  echo "./buildImage path/to/exec/to/copy"
  exit 1
fi

docker build --build-arg EXECUTABLE_PATH=$1 -t assystem-demo .
