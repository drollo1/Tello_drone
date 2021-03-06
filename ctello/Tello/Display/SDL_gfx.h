#ifndef SDL_GFX_H_
#define SDL_GFX_H_

#import <SDL2/SDL.h>
#include <opencv2/opencv.hpp>
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 720;

class SDL_gfx
{
	SDL_Window* m_window;
	SDL_Surface* m_surface;

public:
	SDL_gfx();
	~SDL_gfx();

	int startFrame();
	int drawBackground(cv::Mat _img);
	int drawWhiteBackground();
	int finishFrame();
	int close();
};
#endif // SDL_GFX
