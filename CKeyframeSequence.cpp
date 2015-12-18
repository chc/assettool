#include "CKeyframeSequence.h"

CKeyframeSequence::CKeyframeSequence() {
	position = glm::vec3(0.0, 0.0, 0.0);
	rotation = glm::quat();
	for (int i = 0; i < MAX_MESH_TEXTURES; i++) {
		uv_matrices[i] = glm::mat3x3();
	}
	start_frame = 0;
}
CKeyframeSequence::~CKeyframeSequence() {

}
void CKeyframeSequence::setPosition(glm::vec3 position) {
	this->position = position;
}
void CKeyframeSequence::setRotation(glm::quat rotation) {
	this->rotation = rotation;
}
glm::quat CKeyframeSequence::getRotation() {
	return this->rotation;
}
void CKeyframeSequence::setUVMatrix(glm::mat3x3 uv_matrix, int level) {
	uv_matrices[level] = uv_matrix;
}
glm::vec3 CKeyframeSequence::getPosition() {
	return position;
}
glm::mat3x3 CKeyframeSequence::getUVMatrix(int level) {
	return uv_matrices[level];
}
void CKeyframeSequence::setBeginFrame(float frame) {
	start_frame = frame;
}
float CKeyframeSequence::getBeginFrame() {
	return start_frame;
}
