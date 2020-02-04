#include "Config.h"
#include "GameBoy.h"
#include "GameSettings.h"


int main(int argc, char *argv[])
{
	printf(std::to_string(argc).c_str());
	if(argc < 2) {
		printf("Pass rom as argument.");
		return 1;
	}

	LogMessage* log = LogMessage::CreateInstance() ;
	GameSettings* settings = GameSettings::CreateInstance();

	GameBoy* gb = GameBoy::CreateInstance( argv[1] ) ;

	gb->StartEmulation( ) ;

	delete gb ;
	delete settings ;
	delete log ;

	return 0;
}
