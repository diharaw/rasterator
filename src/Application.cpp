#include <application.hpp>
#include <stdio.h>
#include <iostream>

Application::Application() : m_is_running(false),
                             m_sdl_window(nullptr),
							 m_delta_time(0), 
							 m_last_delta_time(0),
							 m_width(1280),
							 m_height(720)
{
    
}

Application::~Application()
{
    
}

int Application::run(int argc, char* argv[])
{
    if(!_initialize())
        return 1;

    while (m_is_running)
    {
        _frame();
    }
    
    _shutdown();
    
    return 0;
}

void Application::_frame()
{
	_clear_screen(0, 0, 0, 255);
    _event_loop();

	frame();

	_update_delta_time();
	_present();
}

void Application::_update_delta_time()
{
	uint32_t ticks = SDL_GetTicks();
	m_delta_time = ticks - m_last_delta_time;
	m_last_delta_time = ticks;

	std::string msg = m_title + std::to_string(m_delta_time) + "ms";
	SDL_SetWindowTitle(m_sdl_window, msg.c_str());
}

void Application::_clear_screen(uint8_t r, uint8_t b, uint8_t g, uint8_t a)
{
	SDL_SetRenderDrawColor(m_sdl_renderer, r, b, g, a);
	SDL_RenderClear(m_sdl_renderer);
}

void Application::_present()
{
	SDL_RenderPresent(m_sdl_renderer);
}

bool Application::_initialize()
{
	m_title = "Software Rasterizer | Dihara Wijetunga (c) 2018 | ";

    Uint32 flags = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_HAPTIC | SDL_INIT_JOYSTICK;
    
    if (SDL_Init(flags) != 0)
        return false;
    
	m_sdl_window = SDL_CreateWindow(m_title.c_str(),
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
							   m_width,
							   m_height,
							   SDL_WINDOW_OPENGL);
    if (!m_sdl_window)
        return false;
    
	m_sdl_renderer = SDL_CreateRenderer(m_sdl_window, -1, SDL_RENDERER_ACCELERATED);
	m_sdl_backbuffer = SDL_CreateTexture(m_sdl_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, m_width, m_height);

	if (!initialize())
		return false;
	
	m_is_running = true;
    return true;
}

void Application::_shutdown()
{
	shutdown();

	SDL_DestroyTexture(m_sdl_backbuffer);
	SDL_DestroyRenderer(m_sdl_renderer);
    SDL_DestroyWindow(m_sdl_window);

	SDL_Quit();
}

void Application::_event_loop()
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
				m_is_running = false;
                break;
                
            default:
                break;
        }
    }
}

void Application::update_backbuffer(void* pixels)
{
	SDL_UpdateTexture(m_sdl_backbuffer, NULL, pixels, m_width * sizeof(uint32_t));
	SDL_RenderCopy(m_sdl_renderer, m_sdl_backbuffer, NULL, NULL);
}
