#!/bin/bash

if [ "$#" -ne 4 ]; then
  echo "wrong number of arguments!"
  echo "usage:"
  echo "./buildImage clientPath generatorPath brokerPath imageName"
  exit 1
fi

clientPath=$1
generatorPath=$2
brokerPath=$3
name=$4

cp "$clientPath" .
cp "$generatorPath" .
cp "$brokerPath" .
client=$(basename "$clientPath")
broker=$(basename "$brokerPath")
generator=$(basename "$generatorPath")

docker build --build-arg CLIENT_PATH="$client" --build-arg GENERATOR_PATH="$generator" --build-arg BROKER_PATH="$broker" -t "$4" .
rm "$client"
rm "$broker"
rm "$generator"

