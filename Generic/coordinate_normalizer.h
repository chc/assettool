#ifndef _COORDINATE_NORMALIZER_H
#define _COORDINATE_NORMALIZER_H

#include <main.h>
#include <Generic/CGeneric.h>
#include <Generic/CMesh.h>
void convert_xyz_from_to(ECoordinateSystem from, ECoordinateSystem to, float *buffer, uint32_t num_verts);
void convert_uvw_from_to(ECoordinateSystem from, ECoordinateSystem to, float *buffer, uint32_t num_verts);
void convert_to_primitive(CMeshPrimType from, CMeshPrimType to, uint32_t *indices, uint32_t num_indices, uint32_t **out_indices, uint32_t *out_num_indices);
#endif //_COORDINATE_NORMALIZER_H