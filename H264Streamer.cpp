//Written By Antoine DUBOST-BULARD 25/10/2014
//Based on testH264VideoStreamer.cpp from Live55 RTSP library
//Get H264 Buffers from Raspivid and send them to H264 RTSP Streamer

#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include <FramedSource.hh>
#include <pthread.h>
#include "MyDeviceSource.hh"

void play(); // forward
void ThreadRTSP(); // forward

UsageEnvironment* env;
H264VideoStreamFramer* videoSource;
RTPSink* videoSink;

H264VideoStreamDiscreteFramer* videoDeviceSource;
MyDeviceSource* LiveVideoSource;

pthread_t pThreadRTSP;
int  iretThreadRTSP;

unsigned int ImageBuffer_Lenght = 0;
char ImageBuffer[1*1024*1024];
static struct timeval tm1;

static inline void start()
{
    gettimeofday(&tm1, NULL);
}

static inline void stop()
{
    struct timeval tm2;
    gettimeofday(&tm2, NULL);

    unsigned long long t = 1000 * (tm2.tv_sec - tm1.tv_sec) + (tm2.tv_usec - tm1.tv_usec) / 1000;
    printf("%llu ms\n", t);
}

void WaitStreamingDone()
{
	LiveVideoSource->WaitStreamingDone();
}

void CaptureDone()
{
	LiveVideoSource->CaptureDone();
}

char GetH264ImageType(char * Buffer)
{
	if((Buffer[0] == 0x00) && (Buffer[1] == 0x00) && (Buffer[2] == 0x00) && (Buffer[3] == 0x01) && (Buffer[4] == 0x27)) return 's';//SPS Image Start Code
	else if((Buffer[0] == 0x00) && (Buffer[1] == 0x00) && (Buffer[2] == 0x00) && (Buffer[3] == 0x01) && (Buffer[4] == 0x28)) return 'p';//PPS Image Start Code
	else if((Buffer[0] == 0x00) && (Buffer[1] == 0x00) && (Buffer[2] == 0x00) && (Buffer[3] == 0x01) && (Buffer[4] == 0x25)) return 'I';//I Image Start Code
	else if((Buffer[0] == 0x00) && (Buffer[1] == 0x00) && (Buffer[2] == 0x00) && (Buffer[3] == 0x01) && (Buffer[4] == 0x21)) return 'P';//P Image Start Code
	else return 0;
}

extern "C" void Start_Streaming()
{
	//Subscribe to Multicast Group via eth0
	system("sudo ip route add 224.0.0.0/4 dev eth0");
	iretThreadRTSP = pthread_create(&pThreadRTSP, NULL, (void *(*)(void *))&ThreadRTSP, NULL);
}

extern "C" void FeedStreamerAndRecordingManager(char * Buffer, unsigned int BufferSize)
{
	if((GetH264ImageType(Buffer) == 'I') || (GetH264ImageType(Buffer) == 'P') || (GetH264ImageType(Buffer) == 's') || (GetH264ImageType(Buffer) == 'p')) //Start Code
	{
			//Image Start
			if(ImageBuffer_Lenght) //New image comes, Send last image
			{
				//Framerate measure - Uncomment above two lines
				//stop();
				//start();
				LiveVideoSource->FeedStreamer(ImageBuffer, ImageBuffer_Lenght);
				ImageBuffer_Lenght = 0;
				CaptureDone();
				WaitStreamingDone();
			}
			
			if(sizeof(ImageBuffer) >= ImageBuffer_Lenght + BufferSize)
			{
				memcpy(ImageBuffer + ImageBuffer_Lenght, Buffer, BufferSize);
				ImageBuffer_Lenght += BufferSize;
			}
			else
			{
				fprintf(stderr, "Buffer Memory Overflow\n");
				exit(0);
			}
	}
	else 
	{
		//Image fragment
		if(sizeof(ImageBuffer) >= ImageBuffer_Lenght + BufferSize)
		{
			memcpy(ImageBuffer + ImageBuffer_Lenght, Buffer, BufferSize);
			ImageBuffer_Lenght += BufferSize;
		}
		else
		{
			fprintf(stderr, "Buffer Memory Overflow\n");
			exit(0);
		}
		
	}
}

void ThreadRTSP() {
  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  env = BasicUsageEnvironment::createNew(*scheduler);

  // Create 'groupsocks' for RTP and RTCP:
  struct in_addr destinationAddress;
  destinationAddress.s_addr = chooseRandomIPv4SSMAddress(*env);
  // Note: This is a multicast address.  If you wish instead to stream
  // using unicast, then you should use the "testOnDemandRTSPServer"
  // test program - not this test program - as a model.

  const unsigned short rtpPortNum = 18888;
  const unsigned short rtcpPortNum = rtpPortNum+1;
  const unsigned char ttl = 255;

  const Port rtpPort(rtpPortNum);
  const Port rtcpPort(rtcpPortNum);

  Groupsock rtpGroupsock(*env, destinationAddress, rtpPort, ttl);
  rtpGroupsock.multicastSendOnly(); // we're a SSM source
  Groupsock rtcpGroupsock(*env, destinationAddress, rtcpPort, ttl);
  rtcpGroupsock.multicastSendOnly(); // we're a SSM source

  // Create a 'H264 Video RTP' sink from the RTP 'groupsock':
  OutPacketBuffer::maxSize = 500000;
  videoSink = H264VideoRTPSink::createNew(*env, &rtpGroupsock, 96);

  // Create (and start) a 'RTCP instance' for this RTP sink:
  const unsigned estimatedSessionBandwidth = 15000; // in kbps; for RTCP b/w share
  const unsigned maxCNAMElen = 100;
  unsigned char CNAME[maxCNAMElen+1];
  gethostname((char*)CNAME, maxCNAMElen);
  CNAME[maxCNAMElen] = '\0'; // just in case
  RTCPInstance* rtcp
  = RTCPInstance::createNew(*env, &rtcpGroupsock,
			    estimatedSessionBandwidth, CNAME,
			    videoSink, NULL ,
			    True );
  // Note: This starts RTCP running automatically
 
  /*UserAuthenticationDatabase* authDB = NULL;
  authDB = new UserAuthenticationDatabase;
  char * Login = "ADMIN";
  char * Password = "ADMIN";
  authDB->addUserRecord(Login, Password); */

  RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554/*, authDB*/);
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }
  ServerMediaSession* sms
    = ServerMediaSession::createNew(*env, NULL, NULL,
		   "Session streamed by \"testH264VideoStreamer\"",
					   True );
  sms->addSubsession(PassiveServerMediaSubsession::createNew(*videoSink, rtcp));
  rtspServer->addServerMediaSession(sms);

  char* url = rtspServer->rtspURL(sms);
  *env << "Play this stream using the URL \"" << url << "\"\n";
  delete[] url;

  // Start the streaming:
  *env << "Beginning streaming...\n";
  play();

  env->taskScheduler().doEventLoop(); // does not return
}

void afterPlaying(void* /*clientData*/) {
  *env << "...done reading from file\n";
  videoSink->stopPlaying();
  Medium::close(videoSource);
  // Note that this also closes the input file that this source read from.

  // Start playing once again:
  play();
}

void play() {
  MyDeviceParameters params;
  LiveVideoSource = MyDeviceSource::createNew(*env,params);
  videoDeviceSource = H264VideoStreamDiscreteFramer::createNew(*env,LiveVideoSource);
  *env << "Start Streaming from RaspberrIP Camera...\n";
  videoSink->startPlaying(*videoDeviceSource, afterPlaying, videoSink);
}