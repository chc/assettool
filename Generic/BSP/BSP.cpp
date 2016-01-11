#include <stdio.h>
#include "BSP.h"
const int				BSPNode::s_bsp_node_size = 8;	// size of instance on game side
const int				BSPLeaf::s_bsp_leaf_size = 8;
BSPNode::BSPNode() {
	mp_less_branch = NULL;
	mp_greater_branch = NULL;
	m_less_branch_idx = -1;
	m_greater_branch_idx = -1;
	m_array_offset = 0;
	m_split_axis = 0;
	m_split_point = 0.0f;
}

BSPNode::~BSPNode()
{
	if (mp_less_branch)
	{
		delete mp_less_branch;
	}

	if (mp_greater_branch)
	{
		delete mp_greater_branch;
	}
}

int BSPNode::CountBSPNodes( BSPNode *p_bsp_node )
{
	if (p_bsp_node)
	{
		return 1 + CountBSPNodes(p_bsp_node->mp_less_branch) + CountBSPNodes(p_bsp_node->mp_greater_branch);
	} else {
		return 0;
	}
}

int BSPNode::AssignNodesToArray( BSPNode *p_bsp_node, BSPNode **p_bsp_array,
									   int & cur_array_idx, int & cur_array_offset, bool root_node )
{
	if (p_bsp_node)
	{
		//int assigned_index = cur_array_idx;

		// If root of tree, assign node to array
		if (root_node)
		{
			p_bsp_array[cur_array_idx++] = p_bsp_node;
			p_bsp_node->m_array_offset = cur_array_offset;
			cur_array_offset += p_bsp_node->InstanceSize();
		}

		// By assigning the branches together, we can guarantee that the nodes are contiguous (and need only one pointer) 
		if (p_bsp_node->mp_less_branch)
		{
			//Utils::Assert(p_bsp_node->mp_greater_branch != NULL, "Only less BSP branch is not NULL");

			p_bsp_array[cur_array_idx++] = p_bsp_node->mp_less_branch;
			p_bsp_node->mp_less_branch->m_array_offset = cur_array_offset;
			p_bsp_node->m_less_branch_idx = cur_array_offset;
			cur_array_offset += p_bsp_node->mp_less_branch->InstanceSize();
		}
		if (p_bsp_node->mp_greater_branch)
		{
			//Utils::Assert(p_bsp_node->mp_less_branch != NULL, "Only greater BSP branch is not NULL");

			p_bsp_array[cur_array_idx++] = p_bsp_node->mp_greater_branch;
			p_bsp_node->mp_greater_branch->m_array_offset = cur_array_offset;
			p_bsp_node->m_greater_branch_idx = cur_array_offset;
			cur_array_offset += p_bsp_node->mp_greater_branch->InstanceSize();
		}

		// Do recursion
		AssignNodesToArray(p_bsp_node->mp_less_branch, p_bsp_array, cur_array_idx, cur_array_offset, false);
		AssignNodesToArray(p_bsp_node->mp_greater_branch, p_bsp_array, cur_array_idx, cur_array_offset, false);

		//return assigned_index;
		return p_bsp_node->m_array_offset;
	} else {
		return -1;
	}
}



BSPLeaf::BSPLeaf()
{
	mp_face_idx_array = NULL;
	m_num_faces = 0;
	m_split_axis = 3;
}

BSPLeaf::~BSPLeaf()
{
	if (mp_face_idx_array)
	{
		delete[] mp_face_idx_array;
	}
}