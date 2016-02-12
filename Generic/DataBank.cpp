#include "DataBank.h"
#include <string.h>
#include <stdlib.h>
#include <memory.h>
CDataBank::CDataBank() {
	alloc_data_sets(10);
}
CDataBank::CDataBank(int num_data_sets) {
	m_num_data_sets = num_data_sets;
	alloc_data_sets();
}
void CDataBank::SetDataUInt8(int index, uint8_t *mData, int num_data_sets) {
	
}
void CDataBank::SetDataUInt16(int index, uint16_t *mData, int num_data_sets) {
	if(index < 0) index = 0;
	mp_data_array[index].type =	EDataType_UInt16;
	mp_data_array[index].num_elements =	num_data_sets;
	mp_data_array[index].sUnion.uInt16Data = (uint16_t *)malloc(num_data_sets * sizeof(uint16_t));
	memcpy(mp_data_array[index].sUnion.uInt16Data,mData, num_data_sets * sizeof(uint16_t));
}
void CDataBank::SetDataUInt32(int index, uint32_t *mData, int num_data_sets) {
	if(index < 0) index = 0;
	mp_data_array[index].type =	EDataType_UInt32;
	mp_data_array[index].num_elements =	num_data_sets;
	mp_data_array[index].sUnion.uInt32Data = (uint32_t *)malloc(num_data_sets * sizeof(uint32_t));
	memcpy(mp_data_array[index].sUnion.uInt32Data,mData, num_data_sets * sizeof(uint32_t));
}
void CDataBank::SetDataFloat(int index, float *mData, int num_data_sets) {

}
sGenericDataArray *CDataBank::GetData(int index) {
	if(index < 0) index = 0;
	return &mp_data_array[index];

}
void CDataBank::ConvertToCoordinateSystem(ECoordinateSystem system) {

}
float *CDataBank::GetVertexHead(int index) {
	return &mp_data_array[index].sUnion.mVectors[0][Math::X];
}
uint32_t *CDataBank::GetUInt32Head(int index) {
	if(index < 0) index = 0;
	return mp_data_array[index].sUnion.uInt32Data;	
}
void CDataBank::SetDataVector(int index, float *verts, int m_num_vertices, int num_elements) {
	Math::Vector *vectors = new Math::Vector[m_num_vertices];
	for(int i=0;i<m_num_vertices;i++) {
		float x = 0.0,y = 0.0,z = 0.0,w = 1.0;
		if(num_elements >= 1)
			x = verts[0];
		if(num_elements >= 2)
			y = verts[1];
		if(num_elements >= 3)
			z = verts[2];
		if(num_elements >= 4)
			w = verts[3];
		vectors[i] = Math::Vector(x,y,z,w);
		verts += num_elements;
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

void CDataBank::SetNumDataSets(int num) {
	int old_size = m_num_data_sets;
	
	m_num_data_sets = num;
	mp_data_array = (sGenericDataArray *)realloc(mp_data_array, sizeof(sGenericDataArray) * m_num_data_sets);

	//memset(&mp_data_array[old_size+1], 0, sizeof(sGenericDataArray) * (m_num_data_sets - old_size));
}
int CDataBank::GetNumDataSets(int index) {
	if(index == -1) {
		return m_num_data_sets;
	}
	return mp_data_array[index].num_elements;
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
void		CDataPackage::SetNumBanks(int index, int num) {
	CDataBank *bank = GetDataBank(index);
	bank->SetNumDataSets(num);
}
int 		CDataPackage::GetNumElements(int index, int slot) const {
	CDataBank *bank = GetDataBank(index);
	if(!bank)
		return -1;
	return bank->GetNumDataSets(slot);
}