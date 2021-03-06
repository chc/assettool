#include <stdio.h>
#include <main.h>
#include <Generic/CGame.h>
#include <GTASA/CGTASA.h>
#include <Generic/CGeneric.h>
#include <vector>

#include "modules.h"
#include <args_handler.h>

#include <Utils.h>

//#include <vld.h>

//-inpath out.png -outpath out.txd -infmt png -outgame GTASA -outfmt txd -outparams txd_append
//-inpath out.png -outpath out2.png -infmt png -outfmt png -outparams txd_append
assetToolState g_toolState;

int main(int argc, const char* argv[]) {
	memset(&g_toolState,0,sizeof(g_toolState));
	cmdProcessorCallback *cb = NULL; 
	loadModules();
	for(int i=1;i<argc;i++) {
		if(cb == NULL && argv[i][0] == '-') {
			cb = find_processor_by_name(&argv[i][1]);
			if(cb == NULL) {
				fprintf(stderr, "Unknown switch: %s\n",&argv[i][1]);
				return -1;
			}
		} else if(cb != NULL) {
			cb->mp_func(&g_toolState,(const char *)argv[i]);
			cb = NULL;
		} else {
			fprintf(stderr, "Unknown Argument: %s\n",argv[i]);
			return -1;
		}
	}

	if(g_toolState.inFmt) {
		ImportOptions opts;
		memset(&opts,0,sizeof(opts));
		opts.path = g_toolState.inPath;
		opts.outpath = g_toolState.outPath;
		if(g_toolState.outFmt)
			opts.exporter = g_toolState.outFmt->mp_expFunc;
		opts.expArgs = g_toolState.outFmtArgs;
		opts.args = g_toolState.inFmtArgs;
		g_toolState.inFmt->mp_impFunc(&opts);
	}
	unloadModules();
	return 0;
}