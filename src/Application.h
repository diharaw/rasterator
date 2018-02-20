#include <SDL.h>
#include <SDL_ttf.h>

#include <string>

#define TE_DECLARE_MAIN(APP_CLASS) \
int main(int argc, char* argv[])   \
{                                  \
    APP_CLASS app;                 \
    return app.Run(argc, argv);    \
}

#define TE_SAFE_DELETE(OBJECT) if(OBJECT) { delete OBJECT; OBJECT = nullptr; }
#define TE_SAFE_DELETE_ARRAY(OBJECT) if(OBJECT) { delete[] OBJECT; OBJECT = nullptr; }
#define COLOR_ARGB(RED, GREEN, BLUE, ALPHA) (static_cast<uint32_t>(ALPHA * 255.0f) << 24) | (static_cast<uint32_t>(RED * 255.0f) << 16) | (static_cast<uint32_t>(GREEN * 255.0f) << 8) | static_cast<uint32_t>(BLUE * 255.0f)
#define COLOR_RGBA(RED, GREEN, BLUE, ALPHA) (static_cast<uint32_t>(RED * 255.0f) << 24) | (static_cast<uint32_t>(GREEN * 255.0f) << 16) | static_cast<uint32_t>(BLUE * 255.0f) << 8 | static_cast<uint32_t>(ALPHA * 255.0f)

class Application
{
public:
    Application();
    ~Application();
    int Run(int argc, char* argv[]);
    
private:
	void _Frame();
    void _EventLoop();
	bool _Init();
	void _Shutdown();
	void _UpdateDeltaTime();
	void _ClearScreen(uint8_t r, uint8_t b, uint8_t g, uint8_t a);
	void _Present();

protected:
	virtual bool Init() = 0;
	virtual void Frame() = 0;
	virtual void Shutdown() = 0;
	void UpdateBackBuffer(void* pixels);
	void RenderText(const std::string& text, int x, int y);
    
private:
    bool		  m_IsRunning;
    SDL_Window*   m_SDLWindow;
	SDL_Renderer* m_SDLRenderer;
	SDL_Texture*  m_SDLBackBuffer;
	TTF_Font*	  m_TTFFont;
	uint32_t	  m_LastDeltaTime;

protected:
	float		  m_DeltaTime;
	uint32_t	  m_Width;
	uint32_t	  m_Height;
};
