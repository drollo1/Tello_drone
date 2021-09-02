#ifndef TELLO_DRONE_H_
#define TELLO_DRONE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#include "sys/socket.h"
#include "sys/types.h"
#include "arpa/inet.h"
#include "netinet/in.h"

#include <boost/thread.hpp>
#include <chrono>

#include "opencv2/videoio.hpp"
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
//#include "opencv2/imgproc.hpp"

const int LOCAL_CMD_PORT = 8889;
const int LOCAL_STAT_PORT = 8890;
const int TELLO_CMD_PORT = 8889;
const int TELLO_VIDEO_PORT = 11111;
const char* const TELLO_IP = "192.168.10.1";
const char* const TELLO_UDP_STREAM = "udp://0.0.0.0:11111";

enum cmd_type{
	TELLO_TAKEOFF,
	TELLO_LAND,
	TELLO_RC_CMD
};

struct tello_cmd{
	cmd_type cmd;
	int* vals;
};

class Tello_drone
{
public:
	Tello_drone();
	~Tello_drone();
	int connectDrone();
	int printStatus();
	int streamon();
	int streamoff();
	bool getFrame(cv::OutputArray output);
	int takeoff();
	int land();
	std::queue<tello_cmd>* getcmdqueue(){return &m_cmdqueue;}

private:
	int bindSocket(int fd, int port);
	int sendCommand(char* cmd, int len);
	int video_thread();
	int stat_thread();
	int cmd_thread();
	int rc_cmd(int* vals);

private:
	bool isConnected;
	bool isStreaming;

	//io variables
	int m_cmdSockfd;
	int m_statSockfd;
	sockaddr_in m_tello_sockaddr;

	// drone commands
	std::queue<tello_cmd> m_cmdqueue;
	boost::thread* m_cmdThread;

	//stat variables
	boost::thread* m_statThread;
	boost::mutex m_statMutex;
	char m_status[256];

	// Frame capture variables
	bool frameValid;
	boost::thread* m_videoThread;
	cv::Mat* m_lastFrame;

//	cv::VideoCapture* m_videoCap;
};

#endif // TELLO_DRONE_H_
