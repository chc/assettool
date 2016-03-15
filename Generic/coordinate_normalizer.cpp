#include <main.h>
#include <Generic/coordinate_normalizer.h>

void convert_xyz_from_to(ECoordinateSystem from, ECoordinateSystem to, float *buffer, uint32_t num_verts) {
	float *p = buffer;
	for(int i=0;i<num_verts;i++) {
		float x,y,z;
		switch(from) {
			/*
				Normalize to right handed
			*/
			case ECoordinateSystem_Left_XZY:
				x = p[0];
				y = p[2];
				z = p[1];
				break;
			case ECoordinateSystem_Left:
				x = p[0];
				y = p[1];
				z = -p[2];
				break;
			case ECoordinateSystem_Right: //should invert or w/e if right/left but for now w/e
				x = p[0];
				y = p[1];
				z = p[2];
				break;
		}
		/*
			Take right handed values and convert to whatever
		*/
		switch(to) {
			case ECoordinateSystem_Left:
			case ECoordinateSystem_Right: //should invert or w/e if right/left but for now w/e
				*p++ = x;
				*p++ = y;
				*p++ = z;
			break;
			case ECoordinateSystem_Left_XZY:
				*p++ = x;
				*p++ = z;
				*p++ = y;
				break;
		}
	}
}

void convert_uvw_from_to(ECoordinateSystem from, ECoordinateSystem to, float *buffer, uint32_t num_verts) {
	float *p = buffer;
	for(int i=0;i<num_verts;i++) {
		float x,y,z;
		switch(from) {
			/*
				Normalize to right handed
			*/
			case ECoordinateSystem_Left_XZY:
				x = p[0];
				y = p[1];
				z = p[2];
				break;
			case ECoordinateSystem_Left:
				x = p[0];
				y = p[1];
				z = -p[2];
				break;
			case ECoordinateSystem_Right: //should invert or w/e if right/left but for now w/e
				x = p[0];
				y = p[1];
				z = p[2];
				break;
		}
		/*
			Take right handed values and convert to whatever
		*/
		switch(to) {
			case ECoordinateSystem_Left:
			case ECoordinateSystem_Right: //should invert or w/e if right/left but for now w/e
				*p++ = x;
				*p++ = y;
				*p++ = z;
			break;
			case ECoordinateSystem_Left_XZY:
				*p++ = x;
				*p++ = z;
				*p++ = y;
				break;
		}
	}
}

void convert_to_primitive(CMeshPrimType from, CMeshPrimType to, uint32_t *indices, uint32_t num_indices, uint32_t **out_indices, uint32_t *out_num_indices) {
	uint32_t *list_indices = (uint32_t*)malloc(num_indices * 3 * sizeof(uint32_t));
	uint32_t *p = list_indices;
	uint32_t out_num = 0;
	for(unsigned int c = 0;c<num_indices-2;c++) {
		*p++ = indices[c];
		if( c & 1) {
			*p++ = indices[c+2];
			*p++ = indices[c+1];
		} else {
			*p++ = indices[c+1];
			*p++ = indices[c+2];
		}
		out_num += 3;
	}
	*out_indices = list_indices;
	*out_num_indices = out_num;
}