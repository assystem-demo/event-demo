#!/bin/bash

docker network create demo-net

./runImage.sh "client1" "assystem-demo:test"
./runImage.sh "client2" "assystem-demo:test"
./runImage.sh "client3" "assystem-demo:test"
./runImage.sh "client4" "assystem-demo:test"

./runImage.sh "generator1" "assystem-demo:test"
./runImage.sh "generator2" "assystem-demo:test"

./runImage.sh "broker" "assystem-demo:test"
