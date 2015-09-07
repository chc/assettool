#include <vector>
#include "CGame.h"
#include "args_handler.h"
/*

//extracting all txds in GTA
assettool -ingame GTASA -inpath gta3.img -infmt imgv2 -outpath gta3_out
assettool -ingame GTASA -inpath gta3_out -outpath png_out -infmt txd -outfmt png -drop_mipmaps

//replace your imgs and rebuild to txd - in game not needed
assettool -infmt png -outfmt txd -inpath png_out -outpath newtxd.txd -outgame GTASA -gen_mipmaps

//add TXD to gta3.img - outgame not needed here
assettool -ingame GTASA -infmt imgv2 -append newtxd.txd -outpath gta3_new.img
*/
cmdProcessorCallback cmdSwitchCBs[] = {
	{"ingame",process_ingame},
	{"outgame",process_outgame},
	{"inpath",process_inpath},
	{"outpath",process_outpath},
	{"infmt",process_infmt},
	{"outfmt",process_outfmt},
	{"inparams",process_inparams},
	{"outparams",process_outparams},
};

extern std::vector<IGame*> loaded_game_interfaces;
cmdProcessorCallback *find_processor_by_name(const char *name) {
	for(int i=0;i<sizeof(cmdSwitchCBs)/sizeof(cmdProcessorCallback);i++) {
		if(!strcmp(cmdSwitchCBs[i].argument,name)) {
			return &cmdSwitchCBs[i];
		}
	}
	return NULL;
}
IGame *find_game_by_short_name(const char *name) {
	std::vector<IGame *>::iterator it = loaded_game_interfaces.begin();
	while(it != loaded_game_interfaces.end()) {
		if(!strcmp((*it)->getShortName(),name)) {
			return *it;
		}
		it++;
	}
	return NULL;
}
bool process_inpath(assetToolState *state, const char *args) {
	state->inPath = args;
	return false;
}
bool process_outpath(assetToolState *state, const char *args) {
	state->outPath = args;
	return false;
}
bool process_infmt(assetToolState *state, const char *args) {
	int count = 0;
	IGame *game = state->gameIn;
	FileFormat **fmts = game->getFormats(count);
	for(int i=0;i<count;i++) {
		if(!strcmp(fmts[i]->name,args)) {
			state->inFmt = fmts[i];
		}
	}
	return false;
}
bool process_outfmt(assetToolState *state, const char *args) {
	int count = 0;
	IGame *game = state->gameOut;
	FileFormat **fmts = game->getFormats(count);
	for(int i=0;i<count;i++) {
		if(!strcmp(fmts[i]->name,args)) {
			state->outFmt = fmts[i];
		}
	}
	return false;
}
bool process_inparams(assetToolState *state, const char *args) {
	state->inFmtArgs = args;
	return false;
}
bool process_outparams(assetToolState *state, const char *args) {
	state->outFmtArgs = args;
	return false;
}
bool process_outgame(assetToolState *state, const char *args) {
	IGame *game = find_game_by_short_name(args);
	if(game) {
		printf("Found: %s\n",game->getName());
		state->gameOut = game;
		return true;
	}
	fprintf(stderr, "Failed to find game: %s\n",args);
	return false;
}
bool process_ingame(assetToolState *state, const char *args) {
	IGame *game = find_game_by_short_name(args);
	if(game) {
		printf("Found: %s\n",game->getName());
		state->gameIn = game;
		return true;
	}
	fprintf(stderr, "Failed to find game: %s\n",args);
	return false;
}