#!/bin/bash

docker network create demo-net

./runImage.sh "client1" "assystem-demo:latest"
echo "created client1"
./runImage.sh "client2" "assystem-demo:latest"
echo "created client2"
./runImage.sh "client3" "assystem-demo:latest"
echo "created client3"
./runImage.sh "client4" "assystem-demo:latest"
echo "created client4"

./runImage.sh "generator1" "assystem-demo:latest"
echo "created generator1"
./runImage.sh "generator2" "assystem-demo:latest"
echo "created generator2"

./runImage.sh "broker" "assystem-demo:latest"
echo "created broker"
