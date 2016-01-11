typedef struct {
		void *mp_start;
		uint32_t length;
} BufferRange;
class BufferMap {
	/*
		a class for specifiying multiple ranges within different buffers, and merging them into one
	*/	
	public:
		BufferMap(uint32_t num_buffers);
		void SetBuffer(BufferRange *buffer, int index);
		BufferMap* GetBuffer(int index);
	private:
		uint32_t m_num_buffers;
		BufferRange *mp_ranges;

};
struct {
	char m_type:3; //0-2 = axises, 3 = leaf
	union {
		struct m_leaf {
			BufferMap *mp_faces_buffer_map;
		};
		struct m_node {
			BSPNode *mp_left;
			BSPNode *mp_right;
		};
	}
} BSPNode;
class BSPTree {
	public:
		BSPTree(float *vertices, uint32_t num_verts, uint32_t *indices, uint32_t num_indices);
		BSPTree();
		void initalize();
		void setVertices(float *vertices, uint32_t num_verts);
		void setIndices(uint32_t *indices, uint32_t num_verts);
		
		BSPNode *getNodes(uint32_t &num_nodes);
	private:
		BSPNode *m_nodes;
		uint32_t m_num_nodes;
		
		float *mp_vertices;
		uint32_t m_num_verts;
		
		uint32_t *indices;
		uint32_t m_num_indices;
}