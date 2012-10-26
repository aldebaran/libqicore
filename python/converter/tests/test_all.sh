#!/bin/sh

IFS=:
for str in `find ../../behaviors/ -name 'behavior.xar' -printf "%p:"`
do
  echo "$str"
  ../xar_converter.py "$str"
  rm -R objects
  if [ $? -ne 0 ]
  then
    exit
  fi
done
