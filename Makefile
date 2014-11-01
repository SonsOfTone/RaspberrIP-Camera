#Written By Antoine DUBOST-BULARD 25/10/2014

INCLUDES = -I../Live555/UsageEnvironment/include -I../Live555/groupsock/include -I../Live555/liveMedia/include -I../Live555/BasicUsageEnvironment/include -I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I$(SDKSTAGE)/opt/vc/include/interface/vmcs_host/linux -I./

COMPILE_OPTS =	$(INCLUDES) -Wall -I. -O2
C_FLAGS =		$(COMPILE_OPTS)
CPLUSPLUS_FLAGS =	$(COMPILE_OPTS)

LDFLAGS+= -rdynamic -L$(SDKSTAGE)/opt/vc/lib/ -L/home/pi/share/RaspIPCam_0.7/RaspIPCam -lvcos -lmmal -lmmal_core -lmmal_util -lpthread -lbcm_host -lmmal_components

all: RaspiVid.c
	gcc $(CFLAGS) $(INCLUDES) -c -o RaspiCamControl.o RaspiCamControl.c 
	gcc $(CFLAGS) $(INCLUDES) -c -o RaspiCLI.o RaspiCLI.c 
	gcc $(CFLAGS) $(INCLUDES) -c -o RaspiPreview.o RaspiPreview.c
	gcc $(CFLAGS) $(INCLUDES) -c -o RaspiVid.o RaspiVid.c
	g++ $(CPLUSPLUS_FLAGS) $(INCLUDES) -g -c H264Streamer.cpp -o H264Streamer.o
	g++ $(CPLUSPLUS_FLAGS) $(INCLUDES) -g -c MyDeviceSource.cpp -o MyDeviceSource.o
	
	g++ RaspiCamControl.o RaspiCLI.o RaspiPreview.o RaspiVid.o H264Streamer.o MyDeviceSource.o $(LDFLAGS) -rdynamic ../Live555/liveMedia/libliveMedia.a ../Live555/groupsock/libgroupsock.a ../Live555/BasicUsageEnvironment/libBasicUsageEnvironment.a ../Live555/UsageEnvironment/libUsageEnvironment.a -o RaspberrIPCam

clean:
	-rm -rf *.o RaspberrIPCam