#include "BSPGen.h"
#include <assert.h>
static const int		s_max_face_per_leaf = 20;		// maximum number faces per leaf
static const int		s_max_tree_levels = 8;			// maximum number of levels in a tree

BSPNode *		create_bsp_tree(const COLBBox & bbox, unsigned short *p_face_indexes, int num_faces, COLTriangleMesh *mesh, int level) {
	if ((num_faces <= s_max_face_per_leaf) || (level == s_max_tree_levels)) // Check if this should be a leaf
	{
		return create_bsp_leaf(p_face_indexes, num_faces);
	} else {																// Create Node

		// Find initial splits on the three axis
		int i_mid_split[3];
		float mid_width[3], mid_split[3];
		mid_width[0] = ((bbox.max[0] - bbox.min[0]) * 0.5f);
		mid_width[1] = ((bbox.max[1] - bbox.min[1]) * 0.5f);
		mid_width[2] = ((bbox.max[2] - bbox.min[2]) * 0.5f);
		i_mid_split[0] = (int) (((mid_width[0] + bbox.min[0]) * COLLISION_SUB_INCH_PRECISION) + 0.5f);	// Round to nearest 1/16th
		i_mid_split[1] = (int) (((mid_width[1] + bbox.min[1]) * COLLISION_SUB_INCH_PRECISION) + 0.5f);	// Round to nearest 1/16th
		i_mid_split[2] = (int) (((mid_width[2] + bbox.min[2]) * COLLISION_SUB_INCH_PRECISION) + 0.5f);	// Round to nearest 1/16th
		mid_split[0] = i_mid_split[0] * COLLISION_RECIPROCAL_SUB_INCH_PRECISION;
		mid_split[1] = i_mid_split[1] * COLLISION_RECIPROCAL_SUB_INCH_PRECISION;
		mid_split[2] = i_mid_split[2] * COLLISION_RECIPROCAL_SUB_INCH_PRECISION;

		// Find the weighting of the three potential splits
		int less_faces[3], greater_faces[3];
		calc_split_faces(0, mid_split[0], p_face_indexes, num_faces, less_faces[0], greater_faces[0], mesh);
		calc_split_faces(1, mid_split[1], p_face_indexes, num_faces, less_faces[1], greater_faces[1], mesh);
		calc_split_faces(2, mid_split[2], p_face_indexes, num_faces, less_faces[2], greater_faces[2], mesh);

		// Figure out best split
		int best_axis = -1;
		float best_diff = -1;
		const int duplicate_threshold = (num_faces * 7) / 10;	// tunable
		for (int axis = 0; axis <= 2; axis++)
		{
			float new_diff = (float) abs(less_faces[axis] - greater_faces[axis]);
			int duplicates = less_faces[axis] + greater_faces[axis] - num_faces;

			if (duplicates >= duplicate_threshold)
				continue;

			new_diff += duplicates;				// tunable
			new_diff /= mid_width[axis];		// tunable

			if ((best_axis < 0) || (new_diff < best_diff))
			{
				best_axis = axis;
				best_diff = new_diff;
			}
		}

		if (best_axis < 0)			// Couldn't make a good split, give up
		{
			return create_bsp_leaf(p_face_indexes, num_faces);
		}

		// Allocate new temp arrays for the face indexes
		unsigned short *p_less_face_indexes = new unsigned short[less_faces[best_axis]];
		unsigned short *p_greater_face_indexes = new unsigned short[greater_faces[best_axis]];

		// Now fill in the array
		calc_split_faces(best_axis, mid_split[best_axis], p_face_indexes, num_faces, 
						 less_faces[best_axis], greater_faces[best_axis], mesh,
						 p_less_face_indexes, p_greater_face_indexes);

		// And the new bboxes
		COLBBox less_bbox(bbox), greater_bbox(bbox);
		less_bbox.max[best_axis] = mid_split[best_axis];
		greater_bbox.min[best_axis] = mid_split[best_axis];
	
		// And now calculate the branches
		BSPNode *p_bsp_tree = new BSPNode;
		p_bsp_tree->m_split_axis = best_axis;
		p_bsp_tree->m_split_point = mid_split[best_axis];
		p_bsp_tree->mp_less_branch = create_bsp_tree(less_bbox, p_less_face_indexes, less_faces[best_axis], mesh, level + 1);
		p_bsp_tree->mp_greater_branch = create_bsp_tree(greater_bbox, p_greater_face_indexes, greater_faces[best_axis], mesh, level + 1);

		// Free temp arrays
		delete p_less_face_indexes;
		delete p_greater_face_indexes;

		return p_bsp_tree;
	}
}
BSPLeaf *		create_bsp_leaf(unsigned short *p_face_indexes, int num_faces) {
	BSPLeaf *p_bsp_leaf = new BSPLeaf;

	p_bsp_leaf->m_split_axis = 3;
	p_bsp_leaf->mp_less_branch = NULL;
	p_bsp_leaf->mp_greater_branch = NULL;

	// Make new array in BottomUp memory
	p_bsp_leaf->m_num_faces = num_faces;
	p_bsp_leaf->mp_face_idx_array = new unsigned short[num_faces];
	for (int i = 0; i < num_faces; i++)
	{
		p_bsp_leaf->mp_face_idx_array[i] = p_face_indexes[i];
	}

	return p_bsp_leaf;
}
bool				calc_split_faces(int axis, float axis_distance, unsigned short *p_face_indexes,
									 int num_faces, int & less_faces, int & greater_faces, COLTriangleMesh *mesh, 
									 unsigned short *p_less_face_indexes, unsigned short *p_greater_face_indexes) {
	less_faces = greater_faces = 0;

	for (int i = 0; i < num_faces; i++)
	{
		bool less = false, greater = false;
		//NxFace *p_face = &(mp_CollFaces[p_face_indexes[i]]);

		uint32_t *p_face = &mesh->indices[i * 3];

		// Check the face
		for (int j = 0; j < 3; j++)
		{
			int vidx = p_face[j];
			float axis_val = (&mesh->verticies[vidx * 3])[axis];
			if (axis_val < axis_distance)
			{
				less = true;
			} else if (axis_val >= axis_distance)
			{
				greater = true;
			}
		}

		assert(less || greater);

		// Increment counts and possibly put in new array
		if (less)
		{
			if (p_less_face_indexes)
			{
				p_less_face_indexes[less_faces] = p_face_indexes[i];
			}
			less_faces++;
		}
		if (greater)
		{
			if (p_greater_face_indexes)
			{
				p_greater_face_indexes[greater_faces] = p_face_indexes[i];
			}
			greater_faces++;
		}
	}

	return true;							 
}