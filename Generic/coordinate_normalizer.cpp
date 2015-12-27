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