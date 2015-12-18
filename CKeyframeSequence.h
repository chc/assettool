#ifndef _CKEYFRAMESEQUENCE_H
#define _CKEYFRAMESEQUENCE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "CMesh.h"
class CKeyframeSequence {
public:
	CKeyframeSequence();
	~CKeyframeSequence();
	void setPosition(glm::vec3 position);
	void setRotation(glm::quat rotation);
	void setUVMatrix(glm::mat3x3 uv_matrix, int level);
	void setBeginFrame(float frame); //begin frame, reletive to the beginning of the sequence collection

	glm::quat getRotation();
	glm::vec3 getPosition();
	glm::mat3x3 getUVMatrix(int level);
	float getBeginFrame();
private:
	glm::quat rotation;
	glm::vec3 position;
	glm::mat3x3 uv_matrices[MAX_MESH_TEXTURES];
	float start_frame;
};

#endif //_CKEYFRAMESEQUENCE_H