#ifndef _ARGS_HANDLER_H
#define _ARGS_HANDLER_H
#include "main.h"

typedef struct {
	const char *argument;
	bool (*mp_func)(assetToolState *, const char *);
} cmdProcessorCallback;
cmdProcessorCallback *find_processor_by_name(const char *name);

bool process_outpath(assetToolState *state, const char *args);
bool process_inpath(assetToolState *state, const char *args);
bool process_infmt(assetToolState *state, const char *args);
bool process_outfmt(assetToolState *state, const char *args);
bool process_ingame(assetToolState *state, const char *args);
bool process_inparams(assetToolState *state, const char *args);
bool process_outparams(assetToolState *state, const char *args);
bool process_outgame(assetToolState *state, const char *args);
#endif //_ARGS_HANDLER_H