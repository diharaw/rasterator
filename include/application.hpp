#include <SDL.h>
#include <string>

#define RST_DECLARE_MAIN(APP_CLASS) \
int main(int argc, char* argv[])    \
{                                   \
    APP_CLASS app;                  \
    return app.run(argc, argv);     \
}

#define RST_SAFE_DELETE(OBJECT) if(OBJECT) { delete OBJECT; OBJECT = nullptr; }
#define RST_SAFE_DELETE_ARRAY(OBJECT) if(OBJECT) { delete[] OBJECT; OBJECT = nullptr; }
#define RST_COLOR_ARGB(RED, GREEN, BLUE, ALPHA) (static_cast<uint32_t>(ALPHA * 255.0f) << 24) | (static_cast<uint32_t>(RED * 255.0f) << 16) | (static_cast<uint32_t>(GREEN * 255.0f) << 8) | static_cast<uint32_t>(BLUE * 255.0f)
#define RST_COLOR_RGBA(RED, GREEN, BLUE, ALPHA) (static_cast<uint32_t>(RED * 255.0f) << 24) | (static_cast<uint32_t>(GREEN * 255.0f) << 16) | static_cast<uint32_t>(BLUE * 255.0f) << 8 | static_cast<uint32_t>(ALPHA * 255.0f)

class Application
{
public:
    Application();
    ~Application();
    int run(int argc, char* argv[]);
    
private:
#ifdef __EMSCRIPTEN__
	static void _main_loop(void* arg);
#endif
	void _frame();
    void _event_loop();
	bool _initialize();
	void _shutdown();
	void _update_delta_time();
	void _clear_screen(uint8_t r, uint8_t b, uint8_t g, uint8_t a);
	void _present();

protected:
	virtual bool initialize() = 0;
	virtual void frame() = 0;
	virtual void shutdown() = 0;
	void update_backbuffer(void* pixels);

private:
    bool		  m_is_running;
    SDL_Window*   m_sdl_window;
	SDL_Renderer* m_sdl_renderer;
	SDL_Texture*  m_sdl_backbuffer;
	uint32_t	  m_last_delta_time;

protected:
	float		  m_delta_time;
	uint32_t	  m_width;
	uint32_t	  m_height;
	std::string   m_title;
};
