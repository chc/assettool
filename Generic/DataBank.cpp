#include "DataBank.h"
#include <string.h>
#include <stdlib.h>
#include <memory.h>
CDataBank::CDataBank() {

}
CDataBank::CDataBank(int num_data_sets) {
	m_num_data_sets = num_data_sets;
	alloc_data_sets();
}
void CDataBank::SetDataUInt8(int index, uint8_t *mData, int num_data_sets) {
	
}
void CDataBank::SetDataUInt16(int index, uint16_t *mData, int num_data_sets) {

}
void CDataBank::SetDataUInt32(int index, uint32_t *mData, int num_data_sets) {
	mp_data_array[index].type =	EDataType_Float;
	mp_data_array[index].num_elements =	num_data_sets;
	mp_data_array[index].sUnion.uInt32Data = (uint32_t *)malloc(num_data_sets * sizeof(uint32_t));
	memcpy(mp_data_array[index].sUnion.uInt32Data,mData, num_data_sets * sizeof(uint32_t));
}
void CDataBank::SetDataFloat(int index, float *mData, int num_data_sets) {

}
sGenericDataArray *CDataBank::GetData(int index) {
	return &mp_data_array[index];

}
void CDataBank::ConvertToCoordinateSystem(ECoordinateSystem system) {

}
float *CDataBank::GetVertexHead(int index) {
	return &mp_data_array[index].sUnion.mVectors[0][Math::X];
}
uint32_t *CDataBank::GetUInt32Head(int index) {
return &mp_data_array[index].sUnion.uInt32Data[0];	
}
void CDataBank::SetDataVector(int index, float *verts, int m_num_vertices) {
	alloc_data_sets(m_num_vertices);

	Math::Vector *vectors = new Math::Vector[m_num_vertices];
	for(int i=0;i<m_num_vertices;i++) {
		vectors[i] = Math::Vector(verts[0], verts[1], verts[2], 1.0);
		verts += 3;
	}

	mp_data_array[index].type =	EDataType_Vector;
	mp_data_array[index].num_elements = m_num_vertices;
	mp_data_array[index].sUnion.mVectors = vectors;
	//mp_data_banks[index].type = EDataType_Vector;

}
void CDataBank::alloc_data_sets(int size) {
	if(!size) {
		size = m_num_data_sets;
	}
	m_num_data_sets = size;

	mp_data_array = (sGenericDataArray *)malloc(sizeof(sGenericDataArray) * m_num_data_sets);
	memset(mp_data_array, 0, sizeof(sGenericDataArray) * m_num_data_sets);
}
void CDataBank::free_data_sets() {

}

////////////////////////////////////////////////////////////////////
//Data Packages

CDataPackage::CDataPackage(int num_data_sets) {
	m_data_banks = num_data_sets;
	mp_data_banks = new CDataBank[num_data_sets];
}
CDataPackage::~CDataPackage() {
	if(mp_data_banks) {
		free(mp_data_banks);
	}
}
CDataBank *	CDataPackage::GetDataBank(int index) {
	return &mp_data_banks[index];
}