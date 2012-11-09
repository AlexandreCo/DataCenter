#!/bin/bash
SCRIPT_PATH=/home/gnome/Documents/script/python/picolive
LOG_PATH=/home/gnome/Documents/log
DATE=`date "+%d/%m/%Y %H:%M:%S"`
DATE2=`date "+%Y%m%d_%H%M%S"`
HEURE=`date "+%H:%M:%S"`
if [ 1 ]; then
	read=`/usr/bin/python $SCRIPT_PATH/getReadPicolive.py`
	echo "$HEURE $read" > $LOG_PATH/$DATE2+_picolive_read.log
	/usr/bin/python $SCRIPT_PATH/setTimePicolive.py
	if echo $read | grep free; then
		/usr/bin/python $SCRIPT_PATH/setPeriodePicolive.py
		sleep 1
		/usr/bin/python $SCRIPT_PATH/razPicolive.py
		echo ok
	else
		echo ko
	fi

else
	echo "$HEURE offline" > $LOG_PATH/$DATE2+_picolive_read.log
fi

