#!/bin/bash
export LD_LIBRARY_PATH=/opt/vc/lib:$LD_LIBRARY_PATH
/home/pi/RaspberrIP-Camera/RaspberrIPCam -o - >/dev/null