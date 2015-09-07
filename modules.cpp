#include "main.h"
#include "CGTASA.h"
#include "CGameTHPS.h"
#include "CGeneric.h"
#include <vector>
std::vector<IGame*> loaded_game_interfaces;
extern assetToolState g_toolState;
void loadModules() {
	IGame *sa = new CGTASA();
	IGame *thps = new CGameTHPS();
	IGame *generic_game = new CGeneric();
	loaded_game_interfaces.push_back(sa);
	loaded_game_interfaces.push_back(thps);
	loaded_game_interfaces.push_back(generic_game);

	g_toolState.gameIn = generic_game;
	g_toolState.gameOut = generic_game;
}