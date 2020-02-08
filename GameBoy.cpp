#include "Config.h"
#include "Emulator.h"
#include "GameBoy.h"

#ifdef WIN32
#define _NO_CRT_STDIO_INLINE 
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <SDL.h>
#include <SDL_opengl.h>
#else
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
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
extern int windowWidth ; //Important, try best not to change
extern int windowHeight ; //Important, try best not to change
GLuint tex;
int windowWidth = 160; //160
int windowHeight = 144;//144

//Upscale 4x... utter failure
//GLsizei windowWidth = 640;
//GLsizei windowHeight = 576;

///////////////////////////////////////////////////////////////////////////////////////

static int total = 0 ;
static int timer = 0 ;
static int current = 0 ;
static int counter = 0 ;
static bool first = true ;
static SDL_Window* window;

static void CheckFPS( )
{
	if (first)
	{
		first = false ;
		timer = SDL_GetTicks() ;
	}

	counter++ ;
	current = SDL_GetTicks() ;
	if ((timer + 1000) < current)
	{
		timer = current ;
		total = counter ;
		counter = 0 ;

		//OutputDebugStr(outputMessage);
	}
}

///////////////////////////////////////////////////////////////////////////////////////

static void DoRender( )
{
	GameBoy* gb = GameBoy::GetSingleton() ;
	gb->RenderGame( ) ;
}


///////////////////////////////////////////////////////////////////////////////////////

GameBoy* GameBoy::m_Instance = 0 ;

GameBoy* GameBoy::CreateInstance( const std::string& rom )
{
	if (0 == m_Instance)
	{
		m_Instance = new GameBoy( rom ) ;
		m_Instance->Initialize( ) ;
	}
	return m_Instance ;
}

//////////////////////////////////////////////////////////////////////////////////////////

GameBoy* GameBoy::GetSingleton( )
{
	return m_Instance ;
}

//////////////////////////////////////////////////////////////////////////////////////////

GameBoy::GameBoy( const std::string& rom ) :
	m_Emulator(NULL)
{
	m_Emulator = new Emulator( ) ;
	m_Emulator->LoadRom(rom) ;
	m_Emulator->InitGame( DoRender ) ;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool GameBoy::Initialize( )
{
	return CreateSDLWindow( ) ;
}

//////////////////////////////////////////////////////////////////////////////////////////
void GameBoy::StartEmulation( )
{
	bool quit = false ;
	SDL_Event event;

	double fps = 60; //59.73 ;
	double interval = 700; //100; //1000 ;
	interval /= fps ;

	unsigned int time2 = SDL_GetTicks( ) ;

	while (!quit)
	{
		while( SDL_PollEvent( &event ) )
		{
			HandleInput( event ) ;

			if( event.type == SDL_QUIT )
			{
				quit = true;
			}
		}

		unsigned int current = SDL_GetTicks( ) ;

		if( (time2 + interval) < current )
		{
			CheckFPS( ) ;
			m_Emulator->Update( ) ;
			time2 = current ;
		}

	}
	SDL_Quit( ) ;
}

//////////////////////////////////////////////////////////////////////////////////////////

GameBoy::~GameBoy(void)
{
	delete m_Emulator ;
}

//////////////////////////////////////////////////////////////////////////////////////////

//void GameBoy::RenderGame()
//{
//	//EmulationLoop() ;
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glMatrixMode(GL_NEAREST);
//	glLoadIdentity();
//	glRasterPos2i(-1, 1);
//	glBindTexture(GL_TEXTURE_2D, tex);
//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, m_Emulator->m_ScreenData);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glBindTexture(GL_TEXTURE_2D, 0);
//	SDL_GL_SwapWindow(window);
//}

void GameBoy::RenderGame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glRasterPos2i(-1, 1);
	glPixelZoom(1, -1);
	glViewport(0, 0, windowWidth, windowHeight);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, m_Emulator->m_ScreenData);
	SDL_GL_SwapWindow(window);
}

/*SDL_GL_SetSwapInterval(fps);
	SDL_GL_SwapWindow(window);*/

//////////////////////////////////////////////////////////////////////////////////////////

void GameBoy::SetKeyPressed(int key)
{
	m_Emulator->KeyPressed(key) ;
}

//////////////////////////////////////////////////////////////////////////////////////////

void GameBoy::SetKeyReleased(int key)
{
	m_Emulator->KeyReleased(key) ;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool GameBoy::CreateSDLWindow()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		return false;
	}
	if ((window = SDL_CreateWindow("PangoBoy", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN)) == NULL)
	{
		return false;
	}

	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED);

	InitGL();

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////

void GameBoy::InitGL( )
{
	glViewport(0, 0, windowWidth, windowHeight);
	glMatrixMode(GL_MODELVIEW | GL_NEAREST);
	glLoadIdentity();
	glOrtho(0, windowWidth, windowHeight, 0, -1.0, 1.0);
	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_FLAT);
	glGenTextures(1, &tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DITHER);
	glDisable(GL_BLEND);
	/*glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DITHER);
	glEnable(GL_BLEND);*/
}

//////////////////////////////////////////////////////////////////////////////////////////

void GameBoy::HandleInput(SDL_Event& event)
{
	if( event.type == SDL_KEYDOWN )
	{
		int key = -5 ;
		switch( event.key.keysym.sym )
		{
			case SDLK_a : key = 4 ; break ;
			case SDLK_s : key = 5 ; break ;
			case SDLK_RETURN : key = 7 ; break ;
			case SDLK_SPACE : key = 6; break ;
			case SDLK_RIGHT : key = 0 ; break ;
			case SDLK_LEFT : key = 1 ; break ;
			case SDLK_UP : key = 2 ; break ;
			case SDLK_DOWN : key = 3 ; break ;
			case SDLK_1: windowHeight = 144; windowWidth = 160; break;
			case SDLK_2: windowHeight = 288; windowWidth = 320; break;
			case SDLK_3: windowHeight = 432; windowWidth = 480; break;
			case SDLK_4: windowHeight = 576; windowWidth = 640; break;
		}
		if (key != -5)
		{
			SetKeyPressed(key) ;
		}
	}
	//If a key was released
	else if( event.type == SDL_KEYUP )
	{
		int key = -5 ;
		switch( event.key.keysym.sym )
		{
			case SDLK_a : key = 4 ; break ;
			case SDLK_s : key = 5 ; break ;
			case SDLK_RETURN : key = 7 ; break ;
			case SDLK_SPACE : key = 6; break ;
			case SDLK_RIGHT : key = 0 ; break ;
			case SDLK_LEFT : key = 1 ; break ;
			case SDLK_UP : key = 2 ; break ;
			case SDLK_DOWN : key = 3 ; break ;
			case SDLK_1: windowHeight = 144; windowWidth = 160; break; //glScalef(windowWidth, windowHeight, 0);
			case SDLK_2: windowHeight = 288; windowWidth = 320; break;
			case SDLK_3: windowHeight = 432; windowWidth = 480; break;
			case SDLK_4: windowHeight = 576; windowWidth = 640; break;
		}
		if (key != -5)
		{
			SetKeyReleased(key) ;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
