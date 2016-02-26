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
CDataBank::~CDataBank() {
	for(int i=0;i<m_num_data_sets;i++) {
		if(mp_data_array[i].num_elements > 0) {
			switch(mp_data_array[i].type) {
				case EDataType_UInt32:
					free(mp_data_array[i].sUnion.uInt32Data);
				break;
				case EDataType_Vector:
					free(mp_data_array[i].sUnion.mVectors);
				break;
				case EDataType_UInt16:
					free(mp_data_array[i].sUnion.uInt16Data);
				break;
			}
		}
	}
	free(mp_data_array);
    mp_data_array = NULL;
    m_num_data_sets = 0;
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
	return glm::value_ptr(mp_data_array[index].sUnion.mVectors[0]);
}
uint32_t *CDataBank::GetUInt32Head(int index) {
	if(index < 0) index = 0;
	return mp_data_array[index].sUnion.uInt32Data;	
}
void CDataBank::SetDataVector(int index, float *verts, int m_num_vertices, int num_elements) {
	glm::vec4 *vectors = new glm::vec4[m_num_vertices];
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
		vectors[i] = glm::vec4(x,y,z,w);
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
	mp_data_array = (sGenericDataArray *)realloc(NULL, sizeof(sGenericDataArray) * m_num_data_sets);
    memset(mp_data_array, 0, sizeof(sGenericDataArray) * m_num_data_sets);
}
void CDataBank::free_data_sets() {

}

void CDataBank::SetNumDataSets(int num) {
    if(m_num_data_sets >= num) return;
	int old_size = m_num_data_sets;
	m_num_data_sets = num;
	mp_data_array = (sGenericDataArray *)realloc(mp_data_array, sizeof(sGenericDataArray) * m_num_data_sets);
    for(int i=0;i<num;i++) {
    	mp_data_array[i].num_elements = 0;
    }
    //memset(&mp_data_array[old_size], 0, sizeof(sGenericDataArray) * (m_num_data_sets - old_size));
}
int CDataBank::GetNumDataSets(int index) const {
	if(index == -1) {
		return m_num_data_sets;
	}
	return mp_data_array[index].num_elements;
}

////////////////////////////////////////////////////////////////////
//Data Packages
CDataPackage::CDataPackage(int num_data_sets) {
	m_num_data_banks = num_data_sets;
	mp_data_banks = new CDataBank[num_data_sets];
}
CDataPackage::~CDataPackage() {
	if(mp_data_banks) {
		delete[] mp_data_banks;
	}
}
CDataBank *	CDataPackage::GetDataBank(int index) const {
	return &mp_data_banks[index];
}
void		CDataPackage::SetNumBanks(int index, int num) {
	CDataBank *bank = GetDataBank(index);
	bank->SetNumDataSets(num);
}
int 		CDataPackage::GetNumElements(int index, int slot) const {
	const CDataBank *bank = GetDataBank(index);
	if(!bank)
		return -1;
	return bank->GetNumDataSets(slot);
}