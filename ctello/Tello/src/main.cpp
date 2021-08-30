#include "../Display/SDL_gfx.h"
#include "Tello_drone.h"
#include <opencv2/imgcodecs.hpp>
#include <chrono>

SDL_gfx* m_display;
Tello_drone* m_drone;
bool m_quit = false;
int speed = 20;
//Tracker* m_tracker;

int process_key_press(SDL_KeyboardEvent& event){
	if(SDL_KEYDOWN == event.type){
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
			//TODO: Build rc control message to be sent
			break;
		case SDLK_SPACE:
			m_drone->takeoff();
			break;
		case SDLK_LALT:
			m_drone->land();
			break;
		}
	}
}

int process_sdl_event(){
	SDL_Event event;
	int gotEvent = SDL_PollEvent(&event);
	while(gotEvent){
		switch(event.type){
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			process_key_press(event.key);
			break;
		}
		gotEvent = SDL_PollEvent(&event);
	}

}

int update_display(cv::Mat input){
	m_display->startFrame();
	m_display->drawBackground(input);
	m_display->finishFrame();
}

int main_loop(){
	cv::Mat frame;
	bool gotFrame = false;
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	while (!m_quit){
		process_sdl_event();
		while(!gotFrame)
			gotFrame = m_drone->getFrame(frame);
		// Limit  display update to 30fps
		if (gotFrame && (33333 < std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count())){
			update_display(frame);
			start = std::chrono::steady_clock::now();
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
	m_drone->sendCommand("battery?", strlen("battery?")); // Check battery  //TODO: In future just add this to the screen while flying.
	m_drone->streamon();

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
