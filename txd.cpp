#include <stdio.h>
#include <stdlib.h>
#include "txd.h"
#include <string.h>
#include <squish.h>
#include <png.h>

#include "CGame.h"
#include "CImage.h"
#include "txd.h"

int file_len(FILE *fd) {
	int pos = ftell(fd);
	fseek(fd,0,SEEK_END);
	int len = ftell(fd);
	fseek(fd,pos,SEEK_SET);
	return len;
}

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
//edit the tool to organize based on flags, scan all files and sort for anaylisis
Img* get_img_and_read(TXDImgHeader *txdimg, FILE *fd) 
{
	Img *img = (Img *)malloc(sizeof(Img));
	memset(img,0,sizeof(Img));

	img->width = txdimg->width;
	img->height = txdimg->height;

	img->num_mipmaps = txdimg->mipmaps - 1;

	int read_size = 0;

	if((txdimg->dxt_cc == ID_DXT1 || txdimg->image_flags & 512 ||txdimg->dxtcompression == 1) && !(txdimg->image_flags & 8192)) {
		img->colourType = EColourType_DXT1;
	} else if(txdimg->dxt_cc == ID_DXT2 && !(txdimg->image_flags & 8192)) {
		img->colourType = EColourType_DXT2;
	} else if((txdimg->dxt_cc == ID_DXT3 || txdimg->image_flags & 768 ||txdimg->dxtcompression == 3) && !(txdimg->image_flags & 8192)) {
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

	switch(img->colourType) {
		case EColourType_DXT1:
		case EColourType_DXT2:
		case EColourType_DXT3:
		case EColourType_DXT5:
			read_size = txdimg->data_size;
			break;
		case EColourType_8BPP_256Palette:
			img->palette = (uint32_t*)malloc(256 * sizeof(uint32_t));
			fread(img->palette,sizeof(uint32_t),256,fd);
		default:
			read_size = img->width*img->height*(txdimg->BitsPerPixel/8);
			break;
	}

	img->rbga_data = (void *)malloc(read_size);
	fread(img->rbga_data,read_size,1,fd);

	if(img->num_mipmaps > 0) {
		img->mipmaps = (void **)malloc(img->num_mipmaps*sizeof(void *));
		for(int i=0;i<img->num_mipmaps;i++) {
			uint32_t mipmap_len;
			fread(&mipmap_len,sizeof(uint32_t),1,fd);
			img->mipmaps[i] = (void *)malloc(mipmap_len);
			fread(img->mipmaps[i],mipmap_len,1,fd);
		}
	}
	return img;
}

bool gta_rw_import_txd(ImportOptions* opts) {
	char out_path[FILENAME_MAX+1];
	memset(&out_path,0,sizeof(out_path));
	FILE *fd = fopen(opts->path, "rb");
	if(!fd) return false;
	int len = file_len(fd);
	printf("Reading: %s\n",opts->path);
	TXDFileHeader head;
	TXDRecordInfo record;
	TXDImgHeader img;

	ExportOptions expOpts;
	memset(&expOpts,0,sizeof(expOpts));
	expOpts.type = FileType_Texture;
	expOpts.path = (const char *)&out_path;
	CTexture *tex = new CTexture();
	expOpts.dataClass = (void *)tex;

	int tex_count;

	fread(&head,1,sizeof(head),fd);
	fread(&record,1,sizeof(record),fd);

	tex_count = record.texturecount;
	if(tex_count > 0) {
		fread(&img,1,sizeof(img),fd);
		sprintf(out_path,"%s/%s.png",opts->outpath,img.name);
		if(img.TXDVersion == 0x00325350) {
			printf("Skipping PS2 TXD\n");
			goto end;
		}
		Img* aimg = get_img_and_read(&img,fd);
		decompress(aimg, img.name);
		tex->setDimensions(aimg->width,aimg->height);
		tex->setColourData(aimg->colourType,(void *)aimg->rbga_data);
		tex->setPalette(aimg->palette); //will be NULL if not foudn but its all good!
		sprintf(out_path,"%s/%s.png",opts->outpath,img.name);
		opts->exporter(&expOpts);
		free_img(aimg);
		for(int i=0;i<tex_count-1;i++) {
			fread(&record,sizeof(record),1,fd);
			fread(&img,sizeof(img),1,fd);
			sprintf(out_path,"%s/%s.png",opts->outpath,img.name);
			aimg = get_img_and_read(&img,fd);
			decompress(aimg, img.name);
			tex->setDimensions(aimg->width,aimg->height);
			tex->setColourData(aimg->colourType,(void *)aimg->rbga_data);
			tex->setPalette(aimg->palette); //will be NULL if not foudn but its all good!
			opts->exporter(&expOpts);
			free_img(aimg);
		}
	}
	end:
	fclose(fd);
	
	return true;
}

bool gta_rw_export_txd(ExportOptions *expOpts) {
	return false;
}