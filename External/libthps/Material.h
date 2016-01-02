#ifndef _LIBTHPS_MATERIAL_H
#define _LIBTHPS_MATERIAL_H
#include "LibTHPS.h"
#include "texture.h"

#include <vector>

namespace LibTHPS {

#define MAX_PASSES 4

// Material Flags
#define MATFLAG_UV_WIBBLE					(1<<0)
#define MATFLAG_VC_WIBBLE					(1<<1)
#define MATFLAG_TEXTURED					(1<<2)
#define MATFLAG_ENVIRONMENT					(1<<3)
#define MATFLAG_DECAL						(1<<4)
#define MATFLAG_SMOOTH						(1<<5)
#define MATFLAG_TRANSPARENT					(1<<6)
#define MATFLAG_PASS_COLOR_LOCKED			(1<<7)
#define MATFLAG_SPECULAR					(1<<8)		// Specular lighting is enabled on this material (Pass0).
#define MATFLAG_BUMP_SIGNED_TEXTURE			(1<<9)		// This pass uses an offset texture which needs to be treated as signed data.
#define MATFLAG_BUMP_LOAD_MATRIX			(1<<10)		// This pass requires the bump mapping matrix elements to be set up.
#define MATFLAG_PASS_TEXTURE_ANIMATES		(1<<11)		// This pass has a texture which animates.
#define MATFLAG_PASS_IGNORE_VERTEX_ALPHA	(1<<12)		// This pass should not have the texel alpha modulated by the vertex alpha.
#define MATFLAG_EXPLICIT_UV_WIBBLE			(1<<14)		// Uses explicit uv wibble (set via script) rather than calculated.
#define MATFLAG_WATER_EFFECT				(1<<27)		// This material should be processed to provide the water effect.
#define MATFLAG_NO_MAT_COL_MOD				(1<<28)		// No material color modulation required (all passes have m.rgb = 0.5).

struct sUVWibbleParams
{
	float	m_UVel;
	float	m_VVel;
	float	m_UFrequency;
	float	m_VFrequency;
	float	m_UAmplitude;
	float	m_VAmplitude;
	float	m_UPhase;
	float	m_VPhase;
	float	m_UVMatrix[4];		// This value is written to dynamically. The first two values are rotation, the second two are translation.
};

struct sTextureWibbleKeyframe
{
	uint32_t			m_time;
	//Texture	*mp_texture;
	uint32_t	m_texture_checksum;
};

struct sTextureWibbleParams
{
	uint32_t					m_num_keyframes[MAX_PASSES];
	uint32_t					m_phase[MAX_PASSES];
	uint32_t					m_num_iterations[MAX_PASSES];
	sTextureWibbleKeyframe		*mp_keyframes[MAX_PASSES];
};

struct sVCWibbleKeyframe
{
	int			m_time;
	uint32_t	m_color;
};

struct sVCWibbleParams
{
	uint32_t				m_num_keyframes;
	int					m_phase;
	sVCWibbleKeyframe	*mp_keyframes;
};


typedef struct {
	uint32_t m_texture_checksum;
	uint32_t m_flags;
	bool m_has_colour;
	float m_colour[4];
	uint32_t m_reg_alpha;

	uint32_t m_u_address;
	uint32_t m_v_address;

	float m_envmap_tiling[2];
	uint32_t m_filtering_mode;

	sUVWibbleParams m_uv_wibble_params;

	sVCWibbleParams *mp_wibble_vc_params; //only used for first pass

	uint32_t m_mag;
	uint32_t m_min;
	uint32_t m_k;
	uint32_t m_l;

	
} materialTexInfo;


class Material {
	
public:
	Material(FILE *fd, EPlatform platform);
	~Material();
	static std::vector<Material *> loadMaterialsFromScene(FILE *fd, EPlatform platform);

	uint32_t getChecksum();
	float *getSpecularColour();

	materialTexInfo getTexture(int pass);
	uint32_t GetIgnoreVertexAlphaPasses();

	static const uint32_t BLEND_MODE_MASK	= 0x00FFFFFFUL;
//private:
	void loadFromFile(FILE *fd);
	uint32_t m_checksum;
	uint32_t m_name_checksum;
	uint32_t m_passes;
	uint32_t m_alpha_cutoff; //8 bit in thps
	bool m_sorted;
	float m_draw_order;
	bool m_single_sided;
	bool m_no_backface_culling;
	uint32_t m_zbias;
	
	bool m_grassify;
	
	float m_grass_height;
	int m_grass_layers;

	float m_specular_colour[4];

	materialTexInfo m_texture_info[4];

	bool m_uv_wibble;

	uint32_t	*mp_wibble_vc_colours;

	uint32_t m_num_sequences;

	sTextureWibbleParams	*mp_wibble_texture_params;
	bool					m_texture_wibble;

};

}
#endif //_LIBTHPS_MATERIAL_H