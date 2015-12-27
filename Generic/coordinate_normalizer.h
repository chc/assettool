#ifndef _COORDINATE_NORMALIZER_H
#define _COORDINATE_NORMALIZER_H

#include <main.h>
#include <Generic/CGeneric.h>
void convert_xyz_from_to(ECoordinateSystem from, ECoordinateSystem to, float *buffer, uint32_t num_verts);
#endif //_COORDINATE_NORMALIZER_H