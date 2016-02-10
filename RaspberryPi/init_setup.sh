//log into raspberry pi
ssh-keygen ##to create .ssh directory...
cd .ssh
rm id* ##remove the nonsence keys
vi authorized_keys #add public keys

##do the updates
sudo apt-get update
sudo apt-get upgrade

###setup git and the repos
sudo apt-get install git #install git
git clone git@github.com:jjvanderwal/rpi_1wire.git ##setup git repos after ssh key is installed

###setup the vnc
sudo apt-get install tightvncserver
vncserver :1 -geometry 1280x800 -depth 16 -pixelformat rgb565 ## to start the vnc server pw onewire

###setup the wifi
sudo nano /etc/network/interfaces #put the following in here
auto lo

iface lo inet loopback
iface eth0 inet dhcp

allow-hotplug wlan0
iface wlan0 inet manual
wpa-roam /etc/wpa_supplicant/wpa_supplicant.conf
iface default inet dhcp

sudo nano /etc/wpa_supplicant/wpa_supplicant.conf #put the following in here
network={
ssid="NETGEAR18"
proto=RSN
key_mgmt=WPA-PSK
pairwise=CCMP TKIP
group=CCMP TKIP
psk="WIFI-PASSWORD-GOES-HERE"
}
#reboot and wifi should be working

###now setup the one wire stuffs
sudo apt-get install owfs
lsusb #check dallas instruments installed
sudo mkdir /mnt/1wire #create the mount point
sudo nano /etc/owfs.conf #modify the config to comment out fake, enable usb server = all, ensure mount point correct, then reboot
#sudo owfs -u –mountpoint=/mnt/1wire/ #run if mount not automatic
##to auto mount owfs
sudo cp -af /home/pi/rpi_1wire/support.files/owfs.sh /etc/init.d/owfs.sh #copy over the supporting file
sudo chmod +x /etc/init.d/owfs.sh
sudo update-rc.d owfs.sh defaults



########
OUTFILE=/home/pi/DATA/DATA_`date +"%Y%m%d"`.csv
if [ ! -f $OUTFILE ]; then  echo date,time,temp1,temp2,humidity > $OUTFILE; fi
DATA=`date +"%D"`,`date +"%T"`,`cat /mnt/1wire/10.AAA4E4000800/temperature`,`cat /mnt/1wire/26.627E61000000/temperature`,`cat /mnt/1wire/26.627E61000000/humidity`
echo $DATA >> $OUTFILE

##########
OUTFILE=/home/pi/DATA/DATA_`date +"%Y%m%d"`.csv ; #define the output file
if [ ! -f $OUTFILE ]; then  echo date_time,sensorID,temperature,humidity > $OUTFILE; fi ; #initialize the file

cd /mnt/1wire/
###get just the temperature sensors
for TFILE in `find 10* -name temperature -type f`
do
TDIR=`dirname $TFILE` ; #et the directory name
echo `date +"%D %T"`,`cat ${TDIR}/id`,`cat ${TDIR}/temperature`, >> $OUTFILE
done

###get the temperature and humidity sensors
for TFILE in `find 26* -name temperature -type f`
do
TDIR=`dirname $TFILE` ; #et the directory name
echo `date +"%D %T"`,`cat ${TDIR}/id`,`cat ${TDIR}/temperature`,`cat ${TDIR}/humidity` >> $OUTFILE
done

###get the temperature and humidity sensors
for TFILE in `find 28* -name temperature -type f`
do
TDIR=`dirname $TFILE` ; #et the directory name
echo `date +"%D %T"`,`cat ${TDIR}/id`,`cat ${TDIR}/temperature`,`cat ${TDIR}/humidity` >> $OUTFILE
done



#crontab runs
#rsync -az /home/pi/DATA/ --rsh='ssh -p8822' jc165798@login.hpc.jcu.edu.au:/home/jc165798/BACKUP/home.1wire
#rsync -az /home/pi/DATA/ --rsh='ssh -p8822' jc165798@spatialecology.jcu.edu.au:/home/jc165798/1wireData

