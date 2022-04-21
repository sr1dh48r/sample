#!/bin/sh
#Please take backup of  any previous log files
#before starting this this test.
LOG_FILE=rpu_stats.txt
rm -f $LOG_FILE
loop_count=0
#some system level stats
`history >> $LOG_FILE`
`free -m >> $LOG_FILE`
`cat /proc/rpu/params >> $LOG_FILE`
`iw dev >> $LOG_FILE`
`iw wlan0 link >> $LOG_FILE`
`iw reg get >> $LOG_FILE`
echo -e "This script will collect RPU statistics for a duration of 30 seconds."
while [ $loop_count -lt 10 ]
do
  echo -e "get_stats=1" > /proc/rpu/params
  echo -e "RPU Stats Collection: Iteration: $loop_count \n" >> $LOG_FILE
  `cat /proc/rpu/mac_stats >> $LOG_FILE`
  `cat /proc/rpu/phy_stats >> $LOG_FILE`
  `cat /proc/rpu/hal_stats >> $LOG_FILE`
  echo -e "RPU Stats Collection: DONE \n" >> $LOG_FILE
  loop_count=`expr $loop_count + 1`
  sleep 3
done
