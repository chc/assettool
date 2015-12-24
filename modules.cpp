#include <main.h>
#include <GTASA/CGTASA.h>
#include <THPS/CGameTHPS.h>
#include <CHCEngine/CCHCEngine.h>
#include <Generic/CGeneric.h>
#include <XML/CCHCMax.h>
#include <Assimp/CAssImp.h>
#include <vector>
std::vector<IGame*> loaded_game_interfaces;
extern assetToolState g_toolState;
void loadModules() {
	IGame *sa = new CGTASA();
	IGame *thps = new CGameTHPS();
	IGame *generic_game = new CGeneric();
	IGame *chc_engine = new CCHCEngine();
	IGame *assimp = new CAssImp();

	IGame *chc_max = new CCHCMax();
	loaded_game_interfaces.push_back(sa);
	loaded_game_interfaces.push_back(thps);
	loaded_game_interfaces.push_back(generic_game);
	loaded_game_interfaces.push_back(chc_engine);
	loaded_game_interfaces.push_back(chc_max);
	loaded_game_interfaces.push_back(assimp);

	g_toolState.gameIn = generic_game;
	g_toolState.gameOut = generic_game;
}