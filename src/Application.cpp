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
    printf("Initializing..\n");
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

	render_text(std::to_string(m_delta_time) + "ms", 8, 8);
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
    Uint32 flags = SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_HAPTIC | SDL_INIT_JOYSTICK;
    
    if (SDL_Init(flags) != 0)
        return false;
    
	m_sdl_window = SDL_CreateWindow("Software Rasterizer | Dihara Wijetunga (c) 2018",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
							   m_width,
							   m_height,
							   SDL_WINDOW_OPENGL);
    if (!m_sdl_window)
        return false;
    
	m_sdl_renderer = SDL_CreateRenderer(m_sdl_window, -1, SDL_RENDERER_ACCELERATED);
	m_sdl_backbuffer = SDL_CreateTexture(m_sdl_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, m_width, m_height);

	TTF_Init();
	m_ttf_font = TTF_OpenFont("fonts/DroidSans.ttf", 20);

	if (!initialize())
		return false;
	
	m_is_running = true;
    return true;
}

void Application::_shutdown()
{
	shutdown();

	TTF_CloseFont(m_ttf_font);
	SDL_DestroyTexture(m_sdl_backbuffer);
	SDL_DestroyRenderer(m_sdl_renderer);
    SDL_DestroyWindow(m_sdl_window);

	TTF_Quit();
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

void Application::render_text(const std::string& text, int x, int y)
{
	SDL_Color color = { 128, 128, 128 };
	SDL_Surface * surface = TTF_RenderText_Blended(m_ttf_font, text.c_str(), color);
	SDL_Texture * texture = SDL_CreateTextureFromSurface(m_sdl_renderer, surface);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { x, y, texW, texH };
	SDL_RenderCopy(m_sdl_renderer, texture, NULL, &dstrect);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}

void Application::update_backbuffer(void* pixels)
{
	SDL_UpdateTexture(m_sdl_backbuffer, NULL, pixels, m_width * sizeof(uint32_t));
	SDL_RenderCopy(m_sdl_renderer, m_sdl_backbuffer, NULL, NULL);
}