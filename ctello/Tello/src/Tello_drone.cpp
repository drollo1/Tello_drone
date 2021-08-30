#include "Tello_drone.h"

Tello_drone::Tello_drone(){
	isConnected = false;
	m_cmdSockfd = socket(AF_INET, SOCK_DGRAM, 0);
	m_statSockfd = socket(AF_INET, SOCK_DGRAM, 0);
	frameValid = false;
}

Tello_drone::~Tello_drone(){
	close(m_cmdSockfd);
	close(m_statSockfd);
	isConnected = false;
}

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
		return 1;
	}
	else{
		printf("Already Connected to tello drone\n");
		fflush(stdout);
		return 0;
	}
}

int Tello_drone::sendCommand(char* cmd, int len){
	sendto(m_cmdSockfd, (char*) cmd, len, 0, (sockaddr*)&m_tello_sockaddr, sizeof(m_tello_sockaddr));
	char buffer[256];
	socklen_t sock_len;
	int n = recvfrom(m_cmdSockfd, (char*)buffer, 256, 0, (sockaddr*)&m_tello_sockaddr, &sock_len);
	buffer[n] = '\0';
	printf("%s: %s\n", cmd, buffer);
	fflush(stdout);
}

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

bool Tello_drone::getFrame(cv::OutputArray output){
	if (frameValid){
		output.assign(*m_lastFrame);
		return true;
	}
	else
		return false;
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

int Tello_drone::video_thread(){
	cv::VideoCapture cap(TELLO_UDP_STREAM, cv::CAP_FFMPEG);
	cv::Mat* tmp;
	bool isValid;
	int i = 0;
	isStreaming = true;
//	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	while(isStreaming){
		tmp = new cv::Mat;
		isValid = cap.read(*tmp);
//		printf("Time elapsed(%d): %d\n", i, (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start)));
//		fflush(stdout);
		i++;
//		start = std::chrono::steady_clock::now();
		if (isValid){
			frameValid = false;
			m_lastFrame = tmp;
		}
		frameValid = isValid;
	}
}
