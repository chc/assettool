#include <stdio.h>
#include <stdlib.h>
#include "txd.h"
#include <string.h>
#include <squish.h>
#include <png.h>

#include <Generic/CGame.h>
#include <Generic/CImage.h>
#include "txd.h"
#include <Utils.h>
#include <crc32.h>
#include <Generic/CTexture.h>

#include <Generic/CTextureCollection.h>

#include <libthps/misc.h>


void decompress(Img *img, const char *name)  {
		int dxt_flags = 0;
		char has_alpha = 0;
		switch(img->colourType) {
			case EColourType_DXT1:
				dxt_flags = squish::kDxt1;
				break;
			case EColourType_DXT2:
				dxt_flags = squish::kDxt1;
				has_alpha = 1;
				break;
			case EColourType_DXT3:
				dxt_flags = squish::kDxt3;
				has_alpha = 1;
				break;
			case EColourType_DXT5:
				dxt_flags = squish::kDxt5;
				has_alpha = 1;
				break;			
		}
		if(dxt_flags != 0) {
			char *cols_out = (char *)malloc(img->height*img->width*4);
			squish::DecompressImage((squish::u8*)cols_out,img->width,img->height,img->rbga_data,dxt_flags);
			img->colourType = EColourType_32BPP;
			free(img->rbga_data);
			img->rbga_data = cols_out;
		}

}
void free_img(Img* img) {
	if(img->rbga_data) {
		free(img->rbga_data);
	}
	if(img->palette) {
		free(img->palette);
	}
	if(img->num_mipmaps > 0) {
		for(int i=0;i<img->num_mipmaps;i++) {
			free(img->mipmaps[i]);
		}
		free(img->mipmaps);
	}
	free(img);
}
int gta_rw_get_txd_img_size(TXDImgHeader *txdimg, bool with_mipmaps = false) {
	int read_size = 0;
	if((txdimg->dxt_cc == ID_DXT1 || txdimg->dxt_cc == ID_DXT3 || txdimg->dxt_cc == ID_DXT5 || txdimg->image_flags & 512 ||txdimg->dxtcompression == 1 || txdimg->dxtcompression == 3) && !(txdimg->image_flags & 8192)) {
		read_size = txdimg->data_size;
	} else if(txdimg->BitsPerPixel == 8) {
		read_size = (txdimg->width * txdimg->height) + (256*sizeof(uint32_t));
	} else if(txdimg->BitsPerPixel == 16) {
		read_size = txdimg->width * txdimg->height * sizeof(uint16_t);
	} else if(txdimg->BitsPerPixel == 32) {
		read_size = txdimg->width * txdimg->height * sizeof(uint32_t);
	}
	if(with_mipmaps) {

	}
	return read_size;
}
//edit the tool to organize based on flags, scan all files and sort for anaylisis
Img* get_img_and_read(TXDImgHeader *txdimg, FILE *fd) 
{
	Img *img = (Img *)malloc(sizeof(Img));
	memset(img,0,sizeof(Img));

	img->width = txdimg->width;
	img->height = txdimg->height;

	img->num_mipmaps = txdimg->mipmaps - 1;
	int read_size = 0;
	printf("%d %d %d %d\n", txdimg->dxt_cc == ID_DXT1, txdimg->image_flags & 512, txdimg->dxtcompression == 1, !(txdimg->image_flags & 8192));
	if(((txdimg->dxt_cc == ID_DXT1 /*|| txdimg->image_flags & 512 I believe for GTAVC/3, do version check first||txdimg->dxtcompression == 1*/) && !(txdimg->image_flags & 8192))) {
		img->colourType = EColourType_DXT1;
	} else if(txdimg->dxt_cc == ID_DXT2 && !(txdimg->image_flags & 8192)) {
		img->colourType = EColourType_DXT2;
	} else if((txdimg->dxt_cc == ID_DXT3 /*|| txdimg->image_flags & 768  I believe for GTAVC/3, do version check first*/ ||txdimg->dxtcompression == 3) && !(txdimg->image_flags & 8192)) {
		img->colourType = EColourType_DXT3;
	} else if(txdimg->dxt_cc == ID_DXT5 && !(txdimg->image_flags & 8192)) {
		img->colourType = EColourType_DXT5;
	} else if(txdimg->BitsPerPixel == 8) {
		img->colourType = EColourType_8BPP_256Palette;
	} else if(txdimg->BitsPerPixel == 16) {
		img->colourType = EColourType_16BPP;
	} else if(txdimg->BitsPerPixel == 32) {
		img->colourType = EColourType_32BPP;
	}

	read_size = gta_rw_get_txd_img_size(txdimg);

	if(img->colourType == EColourType_8BPP_256Palette) {
		img->palette = (uint32_t*)malloc(256 * sizeof(uint32_t));
		fread(img->palette,sizeof(uint32_t),256,fd);
	}

	img->rbga_data = (void *)malloc(read_size);
	fread(img->rbga_data,read_size,1,fd);

	if (img->colourType == EColourType_32BPP) {
		LibTHPS::bgra_32bit_to_rgba((uint32_t*)img->rbga_data, img->width, img->height);
	}

	if(img->num_mipmaps > 0 && img->num_mipmaps != 0xffffffff) {
		img->mipmaps = (void **)malloc(img->num_mipmaps*sizeof(void *));
		img->mipmap_sizes = (uint32_t *)malloc(img->num_mipmaps*sizeof(uint32_t));
		for(int i=0;i<img->num_mipmaps;i++) {
			fread(&img->mipmap_sizes[i],sizeof(uint32_t),1,fd);
			img->mipmaps[i] = (void *)malloc(img->mipmap_sizes[i]);
			fread(img->mipmaps[i], img->mipmap_sizes[i],1,fd);
		}
	} else {
		img->num_mipmaps = 0;
	}
	return img;
}

bool gta_rw_import_txd(ImportOptions* opts) {
	FILE *fd = fopen(opts->path, "rb");
	if(!fd) return false;
	int len = file_len(fd);
	printf("Reading: %s\n",opts->path);
	TXDFileHeader head;
	TXDRecordInfo record;
	TXDImgHeader img;

	CTextureCollection *tex_col = new CTextureCollection();

	int tex_count;

	fread(&head,1,sizeof(head),fd);
	fread(&record,1,sizeof(record),fd);

	tex_count = record.texturecount;
	if(tex_count > 0) {
		fread(&img,1,sizeof(img),fd);
		if(img.TXDVersion == 0x00325350) {
			printf("Skipping PS2 TXD\n");
			goto end;
		}
		Img* aimg = get_img_and_read(&img,fd);
		//decompress(aimg, img.name);
		CImage *imp_img = new CImage();
		CTexture *tex = new CTexture();
		imp_img->setDimensions(aimg->width,aimg->height);
		imp_img->setNumMipMaps(aimg->num_mipmaps);
		imp_img->setColourData(aimg->colourType,(void *)aimg->rbga_data, img.data_size, 1);
		for (int m = 0; m < aimg->num_mipmaps; m++) {
			imp_img->setColourData(aimg->colourType, (void *)aimg->mipmaps[m], aimg->mipmap_sizes[m], 1, m + 1);
		}
		imp_img->setPalette(aimg->palette); //will be NULL if not foudn but its all good!
		tex->setChecksum(crc32(0, img.name, strlen(img.name)));
		tex->setPath(img.name);
		tex->setImage(imp_img);
		tex_col->AddTexture(tex);
		free_img(aimg);
		for(int i=1;i<tex_count;i++) {
			imp_img = new CImage();
			tex = new CTexture();
			fread(&record,sizeof(record),1,fd);
			fread(&img,sizeof(img),1,fd);
			aimg = get_img_and_read(&img,fd);
			//decompress(aimg, img.name);
			imp_img->setNumMipMaps(aimg->num_mipmaps);
			imp_img->setDimensions(aimg->width,aimg->height);
			imp_img->setColourData(aimg->colourType,(void *)aimg->rbga_data, img.data_size, 1);
			for (int m = 0; m < aimg->num_mipmaps; m++) {
				imp_img->setColourData(aimg->colourType, (void *)aimg->mipmaps[m], aimg->mipmap_sizes[m], 1, m+1);
			}
			imp_img->setPalette(aimg->palette); //will be NULL if not foudn but its all good!
			tex->setImage(imp_img);
			tex->setPath(img.name);
			tex->setChecksum(crc32(0, img.name, strlen(img.name)));
			tex_col->AddTexture(tex);
			free_img(aimg);
		}
	}
	end:
	fclose(fd);

	ExportOptions expOpts;
	memset(&expOpts, 0, sizeof(expOpts));
	expOpts.path = opts->outpath;
	expOpts.srcPath = opts->path;
	expOpts.dataClass = (void *)tex_col;
	expOpts.extra = opts->extra;
	expOpts.args = opts->expArgs;

	opts->exporter(&expOpts);
	Core::Vector<CTexture *> vec = tex_col->getTextures();
	Core::Iterator<Core::Vector<CTexture *>, CTexture *> it = vec.begin();
	while(it != vec.end()) {
		if(*it) {
			CImage *img = (*it)->getImage();
			if(img)
				delete img;
			delete *it;
		}
		it++;
	}
	delete tex_col;
	return true;
}

void make_empty_txd(FILE* fd) {
	TXDFileHeader head;
	TXDRecordInfo record;
	memset(&head,0,sizeof(head));
	memset(&record,0,sizeof(record));

	head.type = 22;
	head.size = 0;
	head.gameid = 402915327;
	head.split = 1;

	record.RwTxdExt = 4;
	record.RWVersion = ERWVersion_3_18;
	record.texturecount = 0;
	record.dummy = 2;
	record.texturenative = 21;
	record.sizeofTextureNative = 8308;
	record.RWVersionA = ERWVersion_3_18;
}

long gta_rw_txd_file_exists(FILE *fd, const char *name, TXDRecordInfo *record, TXDImgHeader *txdimg) {
	TXDImgHeader img;
	TXDRecordInfo rec;
	int pos = ftell(fd), retoffset = -1;
	for(int i=0;i<record->texturecount;i++) {
		fread(&rec,sizeof(rec),1,fd);
		fread(&img,sizeof(img),1,fd);
		if(!strcmp(name,img.name)) {
			retoffset = ftell(fd);
			fseek(fd,pos,SEEK_SET);
			memcpy(txdimg,&img,sizeof(img));
			return retoffset-sizeof(img)-sizeof(rec);
		}
		fseek(fd,gta_rw_get_txd_img_size(&img),SEEK_CUR);
	}
	fseek(fd,pos,SEEK_SET);
	return -1;
}

bool gta_rw_export_txd(ExportOptions *expOpts) {
	printf("I must make/append TXD at: %s\n",expOpts->path);
	FILE *fd = NULL;
	
	TXDFileHeader head;
	TXDRecordInfo record;
	TXDImgHeader img;

	TXDRecordInfo findRec;
	TXDImgHeader findImg;

	memset(&img,0,sizeof(img));
	memset(&head,0,sizeof(head));
	memset(&record,0,sizeof(record));

	strcpy(img.name,expOpts->srcPath);
	
	fd = fopen(expOpts->path,"rb");
	bool create = !fd;
	int offset = -1;
	if(create) {
		fd = fopen(expOpts->path,"wb");
		head.type = 22;
		head.size = 0;
		head.gameid = ERWVersion_3_18;
		head.split = 1;

		record.RwTxdExt = 4;
		record.RWVersion = ERWVersion_3_18;
		record.texturecount = 1;
		record.dummy = 2;
		record.texturenative = 21;
		record.sizeofTextureNative = 8308;
		record.RWVersionA = ERWVersion_3_18;
		fwrite(&head,sizeof(head),1,fd);
		fwrite(&record,sizeof(record),1,fd);
	} else {
		
		fclose(fd);
		fd = fopen(expOpts->path,"rb+");
		fread(&head,sizeof(head),1,fd);
		fread(&record,sizeof(record),1,fd);	
		fseek(fd,-((int)sizeof(record)),SEEK_CUR);

		offset = gta_rw_txd_file_exists(fd, img.name, &record, &findImg);
		int imgdata_sz =  gta_rw_get_txd_img_size(&findImg);
		if(offset != -1) {
			//remove file from record
			fclose(fd);
			fd = slice_file(expOpts->path,expOpts->path,offset,imgdata_sz+sizeof(TXDImgHeader)+sizeof(TXDRecordInfo));
			fseek(fd,sizeof(TXDFileHeader),SEEK_SET);
			fwrite(&record,sizeof(record),1,fd);
			fseek(fd,0,SEEK_END);
		} else {
			//incement record count because adding
			record.texturecount++;
			fseek(fd,-((int)sizeof(record)),SEEK_CUR);
			fwrite(&record,sizeof(record),1,fd);
			fseek(fd,0,SEEK_END);
		}
	}



	img.TxdStruct = 1;
	img.sizeofTXDStruct = 8284;
	img.RWVersionB = ERWVersion_3_18;
	img.TXDVersion = 9;
	img.FilterFlags = 4353;
	img.image_flags = 512;//means dxt
	img.dxt_cc = ID_DXT1;
	img.dxtcompression = 8;

	CImage *tex = (CImage *)expOpts->dataClass;
	uint32_t w,h;
	tex->getDimensions(w,h);
	img.width = w;
	img.height = h;
	img.mipmaps = 1;
	img.BitsPerPixel = 16;

	int dxt_flags = squish::kDxt1;
	uint32_t col_len = (squish::GetStorageRequirements(w,h,dxt_flags));
	char *rbga_data = (char *)malloc(col_len);
	memset(rbga_data,0,col_len);
	squish::CompressImage((squish::u8*)tex->getRGBA(),w,h,(void *)rbga_data,dxt_flags);
	img.data_size = col_len;

	if(record.texturecount != 1) {
		record.RwTxdExt = 4;
		record.RWVersion = ERWVersion_3_18;
		record.texturecount = -1;
		record.dummy = 6147;
		record.texturenative = 21;
		record.sizeofTextureNative = 8308;
		record.RWVersionA = ERWVersion_3_18;
		record.texturecount = -1;
		fwrite(&record,sizeof(record),1,fd);
	}
	fwrite(&img,sizeof(img),1,fd);

	int wlen = fwrite(rbga_data,img.data_size,1,fd);
	free(rbga_data);
	fclose(fd);
	return false;
}