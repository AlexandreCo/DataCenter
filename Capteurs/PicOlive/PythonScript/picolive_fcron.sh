#!/bin/bash
SCRIPT_PATH=/home/gnome/Documents/script/python/picolive
LOG_PATH=/home/gnome/Documents/log
DATE=`date "+%d/%m/%Y %H:%M:%S"`
HEURE=`date "+%H:%M:%S"`
if [ 1 ]; then
	sample=`/usr/bin/python $SCRIPT_PATH/getPicolive.py`
	free=`/usr/bin/python $SCRIPT_PATH/getFreeBlockPicolive.py`
	time=`/usr/bin/python $SCRIPT_PATH/getTimePicolive.py`
	echo "$time $sample" > $LOG_PATH/picolive_current
	echo "$free" 	      >> $LOG_PATH/picolive_current
	echo "$DATE $sample" >> $LOG_PATH/picolive.log
	
else
	echo "$HEURE offline" > $LOG_PATH/picolive_current
	echo "$DATE offline" >> $LOG_PATH/picolive.log
fi

