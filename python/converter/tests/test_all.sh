#!/bin/sh

logFile=test_result.log

rm -Rf objects $logFile
IFS=:
for str in `find ../../../../../ -name 'behavior.xar' -printf "%p:"`
do
  echo "$str"
  ../xar_converter.py "$str" >> $logFile
  if [ $? -ne 0 ]
  then
    echo "Error while converting $str"
    exit
  fi
  cd objects
  timeout 10 python2 main.py >> $logFile
  result=$?
  if [ $result -ne 0 ]
  then
    if [ $result -eq 124 ]
    then
      echo "Warning: behavior $str has timed out"
    else
      echo "Error while executing $str"
      exit
    fi
  fi
  cd ..
  rm -Rf objects
done
