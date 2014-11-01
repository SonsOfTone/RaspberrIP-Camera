#!/bin/bash
export LD_LIBRARY_PATH=/opt/vc/lib:$LD_LIBRARY_PATH
/home/pi/RaspberrIPCam -o - -t 0 -b 5000000 -w 1280 -h 720 -ss 30000 -g 15 -fps 30