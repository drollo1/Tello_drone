#include "Tello_drone.h"

Tello_drone::Tello_drone(){
	isConnected = false;
	m_cmdSockfd = socket(AF_INET, SOCK_DGRAM, 0);
	m_statSockfd = socket(AF_INET, SOCK_DGRAM, 0);
	frameValid = false;
}

Tello_drone::~Tello_drone(){
	isConnected = false;
	m_readThread->join();
	close(m_cmdSockfd);
	close(m_statSockfd);
	if(isStreaming){
		isStreaming = false;
		m_videoThread->join();
	}
}

/********************************************************************************************************************
*	Sets up the command, status, and imagery messaging between class and drone.
********************************************************************************************************************/
int Tello_drone::connectDrone(){
	if(!isConnected){
		//bind the port to send command on and get address for listening
		bindSocket(m_cmdSockfd, LOCAL_CMD_PORT);
		m_tello_sockaddr.sin_family = AF_INET;
		m_tello_sockaddr.sin_port = htons(TELLO_CMD_PORT);
		m_tello_sockaddr.sin_addr.s_addr = inet_addr(TELLO_IP);

		//bind to the stat output for tello drone
		bindSocket(m_statSockfd, LOCAL_STAT_PORT);

		printf("Sending command...\n");
		fflush(stdout);
		sendCommand("command", strlen("command"));
		isConnected = true;
		// Spawn read thread
		m_readThread = new boost::thread(boost::bind(&Tello_drone::read_thread, this));
		return 1;
	}
	else{
		printf("Already Connected to tello drone\n");
		fflush(stdout);
		return 0;
	}
}

/********************************************************************************************************************
*	Sends command to the drone using the command udp socket and waits for response.  The command and the response
*	are printed out.  This is blocking and has to be rethought for fast
*	input:
*		char* cmd: 	The commmand to be sent to the drone
*		int len:	The length of the command to be sent
********************************************************************************************************************/
int Tello_drone::sendCommand(char* cmd, int len){
	sendto(m_cmdSockfd, (char*) cmd, len, 0, (sockaddr*)&m_tello_sockaddr, sizeof(m_tello_sockaddr));
}

/********************************************************************************************************************
*	Prints out the status of the drone.  The drone sends it's status in the format:
*	“pitch:%d;roll:%d;yaw:%d;vgx:%d;vgy%d;vgz:%d;templ:%d;temph:%d;tof:%d;h:%d;bat:%d;baro:%.2f; time:%d;agx:%.2f;agy:%.2f;agz:%.2f;\r\n”
********************************************************************************************************************/
int Tello_drone::printStatus(){
	char buffer[256];
	sockaddr_in tmp;
	socklen_t sock_len;
	int n = recvfrom(m_statSockfd, (char*)buffer, 256, 0, (sockaddr*)&tmp, &sock_len);
	buffer[n] = '\0';
	printf("Status: %s\n", buffer);
	fflush(stdout);
}

int Tello_drone::streamon(){
	sendCommand("streamon", strlen("streamon"));
	m_videoThread = new boost::thread(boost::bind(&Tello_drone::video_thread, this));
}

int Tello_drone::streamoff(){
	isStreaming = false;
	m_videoThread->join();
	sendCommand("streamoff", strlen("streamoff"));
}
/********************************************************************************************************************
*	If there is a valid frame received from the drone save to output and return true other wise retrun false
*	Input:
*		cv::OutputArray output: A frame is passed in and assigned with a good frame if one exists.
********************************************************************************************************************/
//TODO: add in boost mutex locking to make sure not overwriting data as it's beeing read.
bool Tello_drone::getFrame(cv::OutputArray output){
	if (frameValid){
		output.assign(*m_lastFrame);
		return true;
	}
	else
		return false;
}

int Tello_drone::takeoff(){
	sendCommand("takeoff", strlen("takeoff"));
}
int Tello_drone::land(){
	sendCommand("land", strlen("land"));
}

/*
 * Private methods
 */


int Tello_drone::bindSocket(int fd, int port){
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(0 > bind(fd, (sockaddr*) &addr, sizeof(addr))){
		printf("Couldn't bind to port %d\n", port);
		fflush(stdout);
		return -1;
	}
	return 1;
}
/********************************************************************************************************************
*	This thread is spawned when the streamon method is called.  A capture device just updates frames as fast as the
*	drone sends them.
********************************************************************************************************************/
//TODO: add in boost mutex locking to make sure not overwriting data as it's beeing read.
//TODO: Should think about using some kind of event system to tell tracker when the frame is ready.
int Tello_drone::video_thread(){
	cv::VideoCapture cap(TELLO_UDP_STREAM, cv::CAP_FFMPEG);
	cv::Mat* tmp;
	bool isValid;
//	int i = 0;
	isStreaming = true;
//	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	while(isStreaming){
		tmp = new cv::Mat;
		isValid = cap.read(*tmp);
//		printf("Time elapsed(%d): %d\n", i, (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start)));
//		fflush(stdout);
//		i++;
//		start = std::chrono::steady_clock::now();
		// only update if frame is valid
		if (isValid){
			frameValid = false;
			m_lastFrame = tmp;
		}
		frameValid = isValid;
	}
}

int Tello_drone::read_thread(){
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(m_cmdSockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	while(isConnected){
		char buffer[256];
		socklen_t sock_len;
		int n = recvfrom(m_cmdSockfd, (char*)buffer, 256, 0, (sockaddr*)&m_tello_sockaddr, &sock_len);
		if (n > 0){
			buffer[n] = '\0';
			printf("%s\n", buffer);
			fflush(stdout);
		}
	}
}
