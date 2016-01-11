#include "BSP.h"
#include <Generic/CCollision.h>
BSPNode *		create_bsp_tree(const COLBBox & bbox, unsigned short *p_face_indexes, int num_faces, COLTriangleMesh *mesh, int level = 1);
BSPLeaf *		create_bsp_leaf(unsigned short *p_face_indexes, int num_faces);
bool				calc_split_faces(int axis, float axis_distance, unsigned short *p_face_indexes,
									 int num_faces, int & less_faces, int & greater_faces, COLTriangleMesh *mesh, 
									 unsigned short *p_less_face_indexes = NULL, unsigned short *p_greater_face_indexes = NULL);