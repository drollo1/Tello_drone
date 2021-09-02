#include "../Display/SDL_gfx.h"
#include "Tello_drone.h"
#include <opencv2/imgcodecs.hpp>
#include <chrono>

SDL_gfx* m_display;
Tello_drone* m_drone;
bool m_quit = false;
int speed = 20;
//Tracker* m_tracker;
std::queue<tello_cmd>* drone_cmdQ;

tello_cmd build_rc(SDL_Keycode key){

	int* vals = new int[4]();
	//Change in elevation
	if(SDLK_w == key)
		 vals[2]= speed;
	else if(SDLK_s == key)
		vals[2] = -speed;
	else
		vals[2] = 0;

	//rotation
	if(SDLK_a == key)
		vals[3] = -speed;
	else if(SDLK_d == key)
		vals[3] = speed;
	else
		vals[3] = 0;

	tello_cmd cmd = {.cmd = TELLO_RC_CMD, .vals = vals};
	printf("rc command %d %d %d %d\n", cmd.vals[0], cmd.vals[1], cmd.vals[2], cmd.vals[3]);
	fflush(stdout);
	return cmd;

}

int process_key_press(SDL_KeyboardEvent& event){
	tello_cmd tmp_cmd;
	switch(event.keysym.sym){
	case SDLK_ESCAPE:
		m_quit = true;
		break;
	case SDLK_UP:
	case SDLK_DOWN:
	case SDLK_LEFT:
	case SDLK_RIGHT:
	case SDLK_w:
	case SDLK_s:
	case SDLK_a:
	case SDLK_d:
//		 tmp_cmd = build_rc(event.keysym.sym);
//		 drone_cmdQ->push(tmp_cmd);
		break;
	case SDLK_SPACE:
		tmp_cmd = {TELLO_TAKEOFF, nullptr};
		drone_cmdQ->push(tmp_cmd);
		break;
	case SDLK_LALT:
		tmp_cmd = {TELLO_LAND, nullptr};
		drone_cmdQ->push(tmp_cmd);
		break;
	}

}

int process_sdl_event(){
	SDL_Event event;
	int gotEvent = SDL_PollEvent(&event);
	int i = 0;
	while(gotEvent){
		switch(event.type){
		case SDL_KEYDOWN:
//			printf("Key down\n");
//			fflush(stdout);
			process_key_press(event.key);
			break;
		case SDL_KEYUP:
//			printf("Key up\n");
//			fflush(stdout);
//			drone_cmdQ->push(build_rc(event.key.keysym.sym));
			break;
		}
		gotEvent = SDL_PollEvent(&event);
	}

}

int update_display(cv::Mat input){
	m_display->startFrame();
	m_display->drawBackground(input);
	m_display->finishFrame();
//	m_drone->printStatus();
}

int main_loop(){
	cv::Mat frame;
	bool gotFrame = false;
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	while (!m_quit){
		while(!gotFrame)
			gotFrame = m_drone->getFrame(frame);
		// Limit  display update to 30fps
		if (33333 < std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count()){
			update_display(frame);
			start = std::chrono::steady_clock::now();
			process_sdl_event();
		}

		gotFrame = false;
	}
}

int main(int argc, char *argv[]){

	// Init SDL
	int err = SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER);
	if (err){
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		fflush(stdout);
		exit(1);
	}
	// Create frame object
	m_display = new SDL_gfx();

	// Create drone object
	m_drone = new Tello_drone();
	m_drone->connectDrone();
	m_drone->streamon();
	drone_cmdQ = m_drone->getcmdqueue();

	// Create tracker object
//	m_tracker = new tracker();

	// Start main loop
	main_loop();

	//clean up and close
	m_drone->streamoff();
	m_display->close();
	delete m_display;
	delete m_drone;
	SDL_Quit();
	return 0;
}
