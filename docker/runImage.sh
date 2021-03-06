#!/bin/bash

if [ "$#" -ne 2 ]; then
  echo "wrong number of arguments!"
  echo "usage:"
  echo "./runImage.sh name dockerImage:tag"
  exit 1
fi

name=$1
image=$2

echo "cleaning existing docker container"
docker stop "$name" > /dev/null
docker rm "$name" > /dev/null

docker create -it --name "$name" --net demo-net "$image" /bin/bash

docker start "$name" > /dev/null
