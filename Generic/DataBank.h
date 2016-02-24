#ifndef _DATABANK_H
#define _DATABANK_H
#include <main.h>
#include <Generic/CGeneric.h>
#include <stdint.h>
typedef struct {
	void *value;
	uint32_t identifier;
} DataMapEntry;

enum EDataType {

	EDataType_String_ASCII,
	EDataType_String_UTF8,
	EDataType_UInt8,
	EDataType_UInt16,
	EDataType_UInt32,
	EDataType_UInt64,

	EDataType_Double,
	EDataType_Float,

	EDataType_Vector,
	EDataType_Matrix,
};

//A Single piece of generic data
struct sGenericData {
	EDataType type;
	union {
		const char *mString;
		uint32_t 	uInt32Data;
	} sUnion;
};

//An array of generic data
struct sGenericDataArray {
	EDataType type;
	ECoordinateSystem coordSystem;
	uint32_t num_elements;
	bool copied;
	union {
		uint8_t *uInt8Data;
		uint16_t *uInt16Data;
		uint32_t *uInt32Data;
		uint64_t *uInt64Data;
		float *fData;
		double *dblData;
		glm::vec4 *mVectors;
		glm::mat4 *mMatrices;
		sGenericDataArray *sArrayData;
	} sUnion;
};

class CDataPackage;
class CDataBank {
	public:
		friend class CDataPackage;
		CDataBank();
		CDataBank(int num_data_sets);
		~CDataBank();
		//mutators
		void ConvertToCoordinateSystem(ECoordinateSystem system);
		//setters
		void SetDataUInt8(int index, uint8_t *mData, int num_data_sets);
		void SetDataUInt16(int index, uint16_t *mData, int num_data_sets);
		void SetDataUInt32(int index, uint32_t *mData, int num_data_sets);
		void SetDataFloat(int index, float *mData, int num_data_sets);
		void SetDataVector(int index, float *verts, int m_num_vertices, int num_elements = 3);
		void SetDataMatrix(int index, float *matarices, int m_num_matrices); //4x4 matrices only
		//getters
		float *GetVertexHead(int index);
		uint32_t *GetUInt32Head(int index);
		float *GetFloatHead(int index);

		sGenericDataArray *GetData(int index);

		void SetNumDataSets(int num);
		int GetNumDataSets(int index = -1) const;
	
		void alloc_data_sets(int size = 0);
        void free_data_sets();
        private:
        sGenericDataArray *mp_data_array;
		int m_num_data_sets; 
};

/*
	1 data bank per data group
		data group examples:
		vertices
		normals
		UVs
			set 1
			set 2
			set 3
			...
		weights
			float weights - "set 1"
			bone indices - "set 2"
*/
class CDataPackage {
public:
	CDataPackage(int num_data_sets);
	~CDataPackage();
	CDataBank *	GetDataBank(int index) const;
	void		SetNumBanks(int index, int num);
	int 		GetNumElements(int index, int slot = -1) const;
private:
	int m_num_data_banks;
	CDataBank *mp_data_banks;
};

#endif //_DATABANK_H