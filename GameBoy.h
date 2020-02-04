#pragma once
#ifndef _GAMEBOY_H
#define _GAMEBOY_H

class Emulator ;

#include "Emulator.h"
#ifdef WIN32
#include "SDL.h"
#else
#include <SDL/SDL.h>
#endif
class GameBoy
{
public:
	static				GameBoy*				CreateInstance				( const std::string& rom ) ;
	static				GameBoy*				GetSingleton				( ) ;

												~GameBoy					(void);

						void					RenderGame					( ) ;
						bool					Initialize					( ) ;
						void					SetKeyPressed				( int key ) ;
						void					SetKeyReleased				( int key ) ;
						void					StartEmulation				( ) ;
						void					HandleInput					( SDL_Event& event ) ;
private:
												GameBoy						( const std::string& rom );

						bool					CreateSDLWindow				( ) ;

						void					InitGL						( ) ;

	static				GameBoy*				m_Instance ;


						Emulator*				m_Emulator ;


};

#endif
