#ifndef _BSP_H
#define _BSP_H
////////////////////////////////////////////////////////////////
// Axis-Aligned BSP tree node
//

// Precision for fixed point split point value
#define COLLISION_SUB_INCH_PRECISION 16.0f
#define COLLISION_RECIPROCAL_SUB_INCH_PRECISION 0.0625f

class BSPNode
{
public:
	// Constants
	enum
	{
		NUM_AXIS_BITS = 2,			// Number of bits used in fixed split_point for the axis identification
	};

						BSPNode();
	virtual				~BSPNode();

	virtual int			InstanceSize() const { return s_bsp_node_size; }

	static int			CountBSPNodes( BSPNode *p_bsp_node );		// recursively count nodes
	static int			AssignNodesToArray( BSPNode *p_bsp_node, BSPNode **p_bsp_array,
											int & cur_array_idx, int & cur_array_offset, bool root_node = true );

	char				m_split_axis;		// the axis it is split on (0 = X, 1 = Y, 2 = Z, 3 = Leaf)
	float				m_split_point;		// the point on the axis

	BSPNode *			mp_less_branch;		// branches
	BSPNode *			mp_greater_branch;
	int					m_less_branch_idx;	// index into exported node array
	int					m_greater_branch_idx;
	int					m_array_offset;		// offset of this node in the export array

private:
	static const int	s_bsp_node_size;
};

////////////////////////////////////////////////////////////////
// Axis-Aligned BSP tree leaf
//
class BSPLeaf : public BSPNode
{
public:
						BSPLeaf();
	virtual				~BSPLeaf();

	virtual int			InstanceSize() const { return s_bsp_leaf_size; }

	unsigned short		m_num_faces;
	unsigned short *	mp_face_idx_array;

private:
	static const int	s_bsp_leaf_size;
};
#endif //_BSP_H