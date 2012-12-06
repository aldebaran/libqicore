#!/bin/sh

echo " -----------------------------------------------------"
echo "|            LibQicore - Behavior Tester              |"
echo " -----------------------------------------------------"
echo "Starting..."

forceContinue=$1

logFile=test_result.log
errorFile=test_result.err

rm -Rf objects $logFile
fileList=`find ../../../../../../ -name 'behavior.xar' -printf "%p:"`
fileNumber="${fileList//[^:]/}"
fileNumber=${#fileNumber}

echo "$fileNumber file(s) found."
echo "Begin testing..."
echo ""

function printError
{
  echo -e "=====> [\e[0;31mFAIL\e[0m] : $1"
}

function printWarn
{
  echo -e "=====> [\e[0;33mWARN\e[0m] : $1"
}

function printPass
{
  echo -e "=====> [\e[4;32mPASS\e[0m]"
}

count=1
errorConversionCount=0
errorRunTimeCount=0
timeoutCount=0
passCount=0
IFS=:
for str in $fileList
do
  echo "[$count/$fileNumber] : $str"
  ../xar_converter.py "$str" >> $logFile 2> $errorFile
  if [ $? -ne 0 ]
  then
    errorConversionCount=$((errorConversionCount + 1))
    printError "Conversion fail"
    echo `cat $errorFile`
    echo ""
    if [ "$forceContinue" != "--continue" ]
    then
      exit
    fi
    continue
  fi
  timeout 10 python2 objects/main.py 127.0.0.1 9559 >> $logFile 2> $errorFile
  result=$?
  if [ $result -ne 0 ]
  then
    if [ $result -eq 124 ]
    then
      timeoutCount=$((timeoutCount + 1))
      printWarn "TimeOut"
      echo ""
    else
      errorRunTimeCount=$((errorRunTimeCount + 1))
      printError "RunTime Error"
      echo `cat $errorFile`
      echo ""
      if [ "$forceContinue" != "--continue" ]
      then
        exit
      fi
    fi
  else
    passCount=$((passCount + 1))
    printPass
    echo ""
  fi
  rm -Rf objects
  count=$((count + 1))
done

count=$((count - 1))
echo ""
echo "-----------------------------------------------------"
echo "Results :"
echo "-> Pass : $passCount"
echo "-> TimeOut: $timeoutCount"
echo "-> Conversion Error: $errorConversionCount"
echo "-> RunTime Error: $errorRunTimeCount"
echo "Total file(s) tested: $count"
