typedef struct{
	unsigned int RTSP_Port;
	char RTSP_URL[128];
}s_RaspberrIPCam_Network_Config;

typedef struct{
	s_RaspberrIPCam_Network_Config Network;
}s_RaspberrIPCam_Config;

void CaptureDone();
void SetImageBufferSize(unsigned int Size);
void WaitStreamingDone();
void SetImageBufferAddr(char * Addr);
void Start_Streaming(char * URL, unsigned int Port);
void FeedStreamerAndRecordingManager(char * Buffer, unsigned int BufferSize);
