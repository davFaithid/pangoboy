#include "Config.h"
#include "Emulator.h"
#include "GameBoy.h"

#ifdef WIN32
#define _NO_CRT_STDIO_INLINE
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif
#include <fstream>
using namespace std;

//Original
//static const int windowWidth = 160;
//static const int windowHeight = 144 ;

//SDL_Event e;
//while (SDL_PollEvent(&e)) {
//	if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
//		GLsizei windowWidth = 160;
//		GLsizei windowHeight = 144;
//		break;
//	else {
//		GLsizei windowWidth = 160;
//		GLsizei windowHeight = 144;
//		break;
//	}
//	}
//}
SDL_Renderer* renderer;
SDL_Texture* texture;
extern int windowWidth; //Important, try best not to change
extern int windowHeight; //Important, try best not to change
int windowWidth = 160; //160
int windowHeight = 144;//144

//Upscale 4x... utter failure
//GLsizei windowWidth = 640;
//GLsizei windowHeight = 576;

///////////////////////////////////////////////////////////////////////////////////////

static int total = 0;
static int timer = 0;
static int current = 0;
static int counter = 0;
static bool first = true;
static SDL_Window* window;

static void CheckFPS()
{
	if (first)
	{
		first = false;
		timer = SDL_GetTicks();
	}

	counter++;
	current = SDL_GetTicks();
	if ((timer + 1000) < current)
	{
		timer = current;
		total = counter;
		counter = 0;

		//OutputDebugStr(outputMessage);
	}
}

///////////////////////////////////////////////////////////////////////////////////////

static void DoRender()
{
	GameBoy* gb = GameBoy::GetSingleton();
	gb->RenderGame();
}

///////////////////////////////////////////////////////////////////////////////////////

GameBoy* GameBoy::m_Instance = 0;

GameBoy* GameBoy::CreateInstance(const std::string& rom)
{
	if (0 == m_Instance)
	{
		m_Instance = new GameBoy(rom);
		m_Instance->Initialize();
	}
	return m_Instance;
}

//////////////////////////////////////////////////////////////////////////////////////////

GameBoy* GameBoy::GetSingleton()
{
	return m_Instance;
}

//////////////////////////////////////////////////////////////////////////////////////////

GameBoy::GameBoy(const std::string& rom) :
	m_Emulator(NULL)
{
	m_Emulator = new Emulator();
	m_Emulator->LoadRom(rom);
	m_Emulator->InitGame(DoRender);
}

//////////////////////////////////////////////////////////////////////////////////////////

bool GameBoy::Initialize()
{
	return CreateSDLWindow();
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameBoy::StartEmulation()
{
	bool quit = false;
	SDL_Event event;

	double fps = 60; //59.73 ;
	double interval = 700; //100; //1000 ;
	interval /= fps;

	unsigned int time2 = SDL_GetTicks();

	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			HandleInput(event);

			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		unsigned int current = SDL_GetTicks();

		if ((time2 + interval) < current)
		{
			CheckFPS();
			m_Emulator->Update();
			time2 = current;
		}
	}
	SDL_Quit();
}

//////////////////////////////////////////////////////////////////////////////////////////

GameBoy::~GameBoy(void)
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	delete m_Emulator;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool GameBoy::CreateSDLWindow()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		return false;
	}
	if ((window = SDL_CreateWindow("PangoBoy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_RESIZABLE)) == NULL)
	{
		return false;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, windowWidth, windowHeight);
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////

void GameBoy::RenderGame() {
	SDL_UpdateTexture(texture, NULL, m_Emulator->m_ScreenData, windowWidth * sizeof(uint8_t) * 3);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

/*SDL_GL_SetSwapInterval(fps);
	SDL_GL_SwapWindow(window);*/

	//////////////////////////////////////////////////////////////////////////////////////////

void GameBoy::SetKeyPressed(int key)
{
	m_Emulator->KeyPressed(key);
}

//////////////////////////////////////////////////////////////////////////////////////////

void GameBoy::SetKeyReleased(int key)
{
	m_Emulator->KeyReleased(key);
}

void GameBoy::doSaveState()
{
	std::ofstream outfile("save.bin", std::ofstream::binary);
	outfile.write((char*)&reg, sizeof(Registers));
	/*outfile.write((char*)&bgMap, sizeof(bgMap));*/
	/*outfile.write((char*)&tiles, sizeof(tiles));*/
	outfile.write((char*)&sprites, sizeof(sprites));
}

void GameBoy::HandleInput(SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN)
	{
		int key = -5;
		switch (event.key.keysym.sym)
		{
		case SDLK_a: key = 4; break;
		case SDLK_s: key = 5; break;
		case SDLK_RETURN: key = 7; break;
		case SDLK_SPACE: key = 6; break;
		case SDLK_RIGHT: key = 0; break;
		case SDLK_LEFT: key = 1; break;
		case SDLK_UP: key = 2; break;
		case SDLK_DOWN: key = 3; break;
		case SDLK_r: doRewind(); break;
		case SDLK_F1: doSaveState(); break;
		case SDLK_F2: doLoadState(); break;
		case SDLK_1: SDL_SetWindowSize(window, 160, 144); break;
		case SDLK_2: SDL_SetWindowSize(window, 320, 288); break;
		case SDLK_3: SDL_SetWindowSize(window, 480, 432); break;
		case SDLK_4: SDL_SetWindowSize(window, 640, 576); break;
		}
		if (key != -5)
		{
			SetKeyPressed(key);
		}
	}
	//If a key was released
	else if (event.type == SDL_KEYUP)
	{
		int key = -5;
		switch (event.key.keysym.sym)
		{
		case SDLK_a: key = 4; break;
		case SDLK_s: key = 5; break;
		case SDLK_RETURN: key = 7; break;
		case SDLK_SPACE: key = 6; break;
		case SDLK_RIGHT: key = 0; break;
		case SDLK_LEFT: key = 1; break;
		case SDLK_UP: key = 2; break;
		case SDLK_DOWN: key = 3; break;
		case SDLK_1: SDL_SetWindowSize(window, 160, 144); break;
		case SDLK_2: SDL_SetWindowSize(window, 320, 288); break;
		case SDLK_3: SDL_SetWindowSize(window, 480, 432); break;
		case SDLK_4: SDL_SetWindowSize(window, 640, 576); break;
		}
		if (key != -5)
		{
			SetKeyReleased(key);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////