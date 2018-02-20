#include "Application.h"
#include <stdio.h>
#include <iostream>

Application::Application() : m_IsRunning(false),
                             m_SDLWindow(nullptr),
							 m_DeltaTime(0), 
							 m_LastDeltaTime(0),
							 m_Width(800),
							 m_Height(600)
{
    printf("Initializing..\n");
}

Application::~Application()
{
    
}

int Application::Run(int argc, char* argv[])
{
    if(!_Init())
        return 1;

    while (m_IsRunning)
    {
        _Frame();
    }
    
    _Shutdown();
    
    return 0;
}

void Application::_Frame()
{
	_ClearScreen(0, 0, 0, 255);
    _EventLoop();

	Frame();

	_UpdateDeltaTime();
	_Present();
}

void Application::_UpdateDeltaTime()
{
	uint32_t ticks = SDL_GetTicks();
	m_DeltaTime = ticks - m_LastDeltaTime;
	m_LastDeltaTime = ticks;

	RenderText(std::to_string(m_DeltaTime) + "ms", 8, 8);
}

void Application::_ClearScreen(uint8_t r, uint8_t b, uint8_t g, uint8_t a)
{
	SDL_SetRenderDrawColor(m_SDLRenderer, r, b, g, a);
	SDL_RenderClear(m_SDLRenderer);
}

void Application::_Present()
{
	SDL_RenderPresent(m_SDLRenderer);
}

bool Application::_Init()
{
    Uint32 flags = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_HAPTIC | SDL_INIT_JOYSTICK;
    
    if (SDL_Init(flags) != 0)
        return false;
    
    m_SDLWindow = SDL_CreateWindow("Software Rasterizer | Dihara Wijetunga (c) 2018",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
							   m_Width,
                               m_Height,
							   SDL_WINDOW_OPENGL);
    if (!m_SDLWindow)
        return false;
    
	m_SDLRenderer = SDL_CreateRenderer(m_SDLWindow, -1, SDL_RENDERER_ACCELERATED);
	m_SDLBackBuffer = SDL_CreateTexture(m_SDLRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, m_Width, m_Height);

	TTF_Init();
	m_TTFFont = TTF_OpenFont("fonts/DroidSans.ttf", 20);

	if (!Init())
		return false;
	
	m_IsRunning = true;
    return true;
}

void Application::_Shutdown()
{
	Shutdown();

	TTF_CloseFont(m_TTFFont);
	SDL_DestroyTexture(m_SDLBackBuffer);
	SDL_DestroyRenderer(m_SDLRenderer);
    SDL_DestroyWindow(m_SDLWindow);

	TTF_Quit();
	SDL_Quit();
}

void Application::_EventLoop()
{
    SDL_Event event;
    
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_MOUSEWHEEL:
                break;
                
            case SDL_MOUSEMOTION:
            {
                SDL_bool relative = SDL_GetRelativeMouseMode();
                break;
            }
                
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
            {

                break;
            }
                
            case SDL_KEYUP:
            case SDL_KEYDOWN:
            {
                if(event.key.repeat == 0)
                {

                }
                break;
            }
                
            case SDL_QUIT:
                m_IsRunning = false;
                break;
                
            default:
                break;
        }
    }
}

void Application::RenderText(const std::string& text, int x, int y)
{
	SDL_Color color = { 128, 128, 128 };
	SDL_Surface * surface = TTF_RenderText_Blended(m_TTFFont, text.c_str(), color);
	SDL_Texture * texture = SDL_CreateTextureFromSurface(m_SDLRenderer, surface);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { x, y, texW, texH };
	SDL_RenderCopy(m_SDLRenderer, texture, NULL, &dstrect);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}

void Application::UpdateBackBuffer(void* pixels)
{
	SDL_UpdateTexture(m_SDLBackBuffer, NULL, pixels, m_Width * sizeof(uint32_t));
	SDL_RenderCopy(m_SDLRenderer, m_SDLBackBuffer, NULL, NULL);
}