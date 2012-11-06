#!/bin/sh

rm -Rf objects
IFS=:
for str in `find ../../../../../behaviors/ -name 'behavior.xar' -printf "%p:"`
do
  echo "$str"
  ../xar_converter.py "$str"
  if [ $? -ne 0 ]
  then
    exit
  fi
  rm -Rf objects
done
