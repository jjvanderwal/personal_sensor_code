OUTFILE=/home/pi/DATA.csv
if [ ! -f $OUTFILE ]; then
	echo date,time,family,id,temp,humidity,VDD,VAD > $OUTFILE
fi

MNT=/mnt/1wire/
cd $MNT
while : ; do
	for TDIR in `find . -maxdepth 2 -nowarn -name address`; do
		TDIR=`dirname $TDIR`
		TEMP=${TDIR}/temperature
		if [ -e $TEMP ]; then
			FAMILY=`cat ${TDIR}/family`
			ID=`cat ${TDIR}/id`
			TEMP=`cat ${TDIR}/temperature`
			if [ -e ${TDIR}/humidity ]; then
				HUM=`cat ${TDIR}/humidity`
				VAD=`cat ${TDIR}/VAD`
				VDD=`cat ${TDIR}/VDD`
			else
				HUM=NA
				VAD=NA
				VDD=NA
			fi
			DATA=`date +"%D"`,`date +"%T"`,$FAMILY,$ID,$TEMP,$HUM,$VDD,$VAD
			echo $DATA >> $OUTFILE
		fi
	done
	sleep 10
done 
