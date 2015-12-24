#ifndef _CTHPS_H
#define _CTHPS_H
#include <Generic/CGame.h>


class CGameTHPS : public IGame {
public:
	CGameTHPS();
};


bool thps_xbx_import_img(ImportOptions* opts);
bool thps_xbx_export_img(ExportOptions *expOpts);
bool thps_xbx_export_textable(ExportOptions *expOpts);
bool thps_xbx_import_textable(ImportOptions* opts);
#endif //_CTHPS_H