#!/bin/sh
# /etc/init.d/owfs.sh

MOUNTPOINT='/mnt/1wire';
 case "$1" in
        start)
         echo "starting script owfs with USB support..";
         echo `owfs -u --allow_other --mountpoint=$MOUNTPOINT`
         ;;
        stop)
         echo "stopping script owfs by unloading mount-point ..";
         echo `umount $MOUNTPOINT`;
         ;;
        *)
         echo "Usage: /etc/init.d/owfs.sh {start|stop}"
         exit 1
         ;;
 esac

exit 0
