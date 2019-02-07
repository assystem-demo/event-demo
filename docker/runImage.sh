#!/bin/bash

if [ "$#" -lt 2 ]; then
  echo "wrong number of arguments!"
  echo "usage:"
  echo "./runImage.sh dockerImage:tag client|generator|broker [eventid]"
  exit 1
fi

if [ "$2" = "client" ]; then
  if [ "$#" -ne 3 ]; then
    echo "wrong number of arguments"
    echo "if client is selected, provide an event id"
    exit 1
  fi
fi

if [ "$2" = "client" ]; then
  docker run  "$1" event_client "$3"
elif [ "$2" = "generator" ]; then
  echo "not supported yet" 
elif [ "$2" = "broker" ]; then
  docker run -p 8082:8082 -p 8081:8081 "$1" event_broker
else
  echo "no valid arguments found"
  echo "usage:"
  echo "./runImage.sh dockerImage:tag client|generator|broker"
fi

