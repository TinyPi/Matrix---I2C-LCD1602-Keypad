#! /bin/bash

function check(){
	count=`ps -ef |grep $1 |grep -v "grep" |wc -l`
	#echo $count
	if [ 0 == $count ];then
		$1 &
	fi
}

while [ 1 ]
do
	check ckt_analysis
	sleep 5
done
