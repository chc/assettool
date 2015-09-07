#ifndef _GTA_TXD_H
#define _GTA_TXD_H
#include <stdint.h>
#include "CGame.h"
#include "CImage.h"
/*
// 134283263 = GTA3,
                    // 67239935 = gta3 frontend
                    // 268697599 = GTAVC
                    // 201523199 = gtavc frontend
                    // 335609855 = xbox vc
*/
#define GTA_FILE_MAXLEN 32
typedef struct {
	uint32_t type; //0
	uint32_t size; //4
	uint32_t gameid; //8
	uint32_t split; //12
} TXDFileHeader;
typedef struct {
	uint32_t RwTxdExt;
	uint32_t RWVersion;
	uint16_t texturecount;
	uint16_t dummy;
	uint32_t texturenative;
	uint32_t sizeofTextureNative;
	uint32_t RWVersionA;
} TXDRecordInfo;
typedef struct {

	uint32_t TxdStruct;
	uint32_t sizeofTXDStruct;
	uint32_t RWVersionB;
	uint32_t TXDVersion;
	uint32_t FilterFlags;
	char name[GTA_FILE_MAXLEN];
	char alphaname[GTA_FILE_MAXLEN];
	uint32_t image_flags;
	uint32_t dxt_cc;
	uint16_t width;
	uint16_t height;
	char BitsPerPixel;
	char mipmaps;
	char set_to_4;
	char dxtcompression;
	uint32_t data_size;
} TXDImgHeader;


typedef struct {
	uint32_t width;
	uint32_t height;
	EColourType colourType;
	void *rbga_data;
	uint32_t *palette;
	uint32_t num_mipmaps;
	void **mipmaps;
} Img;

#define ID_DXT1   0x31545844
#define ID_DXT2   0x32545844
#define ID_DXT3   0x33545844
#define ID_DXT5   0x35545844

bool gta_rw_import_txd(ImportOptions* opts);
bool gta_rw_export_txd(ExportOptions *expOpts);
#endif //_GTA_TXD_H