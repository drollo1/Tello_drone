#include "SDL_gfx.h"

SDL_gfx::SDL_gfx()
{

	m_window = SDL_CreateWindow( "Tello Video Feed", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
	if (NULL == m_window)
	{
		printf("Window could not be created. SDL_Error: %s\n", SDL_GetError());
		fflush(stdout);
		exit(1);
	}
	else
		m_surface = SDL_GetWindowSurface(m_window);
	startFrame();
	drawWhiteBackground();
	finishFrame();
}

SDL_gfx::~SDL_gfx()
{
}

int SDL_gfx::close(){
	SDL_FreeSurface(m_surface);
	SDL_DestroyWindow(m_window);
}

int SDL_gfx::startFrame()
{
	return 1;
}

int SDL_gfx::drawBackground(cv::Mat _img){
	cv::Mat tmp;
	cv::cvtColor(_img, tmp, cv::COLOR_RGB2RGBA);
	SDL_LockSurface(m_surface);
	cv::Mat screen(m_surface->h, m_surface->w, CV_8UC4, m_surface->pixels, m_surface->pitch);
	tmp.copyTo(screen);
	SDL_UnlockSurface(m_surface);
}

int SDL_gfx::drawWhiteBackground()
{
	SDL_FillRect(m_surface, NULL, SDL_MapRGB(m_surface->format, 0xFF, 0xFF, 0xFF));
	return 1;
}

int SDL_gfx::finishFrame()
{
//	SDL_Delay(33);
	SDL_UpdateWindowSurface(m_window);
	return 1;
}
