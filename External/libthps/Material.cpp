#include "Material.h"


namespace LibTHPS {

Material::Material(FILE *fd, EPlatform platform) {
	memset(&m_specular_colour, 0, sizeof(m_specular_colour));
	memset(&m_texture_info, 0, sizeof(m_texture_info));
	mp_wibble_texture_params = NULL;
	loadFromFile(fd);
}
Material::~Material() {
}
uint32_t Material::getChecksum() {
	return m_checksum;
}
float *Material::getSpecularColour() {
	return m_specular_colour;
}
void Material::loadFromFile(FILE *fd) {
	fread(&m_checksum, sizeof(uint32_t), 1, fd);
	fread(&m_name_checksum, sizeof(uint32_t), 1, fd);
	fread(&m_passes, sizeof(uint32_t), 1, fd); //number of texture layers

	fread(&m_alpha_cutoff,sizeof(uint32_t), 1, fd);

	fread(&m_sorted, sizeof(bool), 1, fd);
	fread(&m_draw_order, sizeof(float), 1, fd);

	fread(&m_single_sided, sizeof(bool), 1, fd);

	fread(&m_no_backface_culling, sizeof(bool), 1, fd);

	fread(&m_zbias, sizeof(uint32_t), 1, fd);

	fread(&m_grassify, sizeof(bool), 1, fd);

	if(m_grassify) {
		fread(&m_grass_height, sizeof(float), 1, fd);
		fread(&m_grass_layers, sizeof(uint32_t), 1, fd);
	}
	fread(&m_specular_colour[3], sizeof(float), 1, fd);
	if(m_specular_colour[3] > 0.0f) {
		fread(&m_specular_colour, sizeof(float),3, fd);
	}

	for(uint32_t pass = 0;pass < m_passes; pass++) {
		fread(&m_texture_info[pass].m_texture_checksum, sizeof(uint32_t), 1, fd);
		fread(&m_texture_info[pass].m_flags, sizeof(uint32_t), 1, fd);

		fread(&m_texture_info[pass].m_has_colour, sizeof(bool), 1, fd);

		fread(&m_texture_info[pass].m_colour, sizeof(float), 3, fd);

		uint64_t reg_alpha;
		fread(&reg_alpha, sizeof(uint64_t), 1, fd);
		uint32_t blend_mode		= (uint32_t)( reg_alpha & 0xFFFFFFUL );
		uint32_t fixed_alpha		= (uint32_t)( reg_alpha >> 32 );
		m_texture_info[pass].m_reg_alpha = blend_mode | ( fixed_alpha << 24 );


		fread(&m_texture_info[pass].m_u_address, sizeof(uint32_t), 1, fd);
		fread(&m_texture_info[pass].m_v_address, sizeof(uint32_t), 1, fd);

		fread(&m_texture_info[pass].m_envmap_tiling[0], sizeof(float), 2, fd);
		fread(&m_texture_info[pass].m_filtering_mode, sizeof(uint32_t), 1, fd);

		if(m_texture_info[pass].m_flags & MATFLAG_UV_WIBBLE) {
			m_uv_wibble = true;
			fread(&m_texture_info[pass].m_uv_wibble_params, sizeof(float), 8, fd);
		}
		// Read vertex color wibble data.
		if(( pass == 0 ) && ( m_texture_info[0].m_flags & MATFLAG_VC_WIBBLE ))
		{
			fread(&m_num_sequences, sizeof(uint32_t), 1, fd);
			m_texture_info[0].mp_wibble_vc_params = (sVCWibbleParams *)malloc(sizeof(sVCWibbleParams) * m_num_sequences);
			mp_wibble_vc_colours = (uint32_t*)malloc(m_num_sequences * sizeof(uint32_t));
			for(uint32_t seq = 0;seq < m_num_sequences;seq++) {
				fread(&m_texture_info[0].mp_wibble_vc_params[seq].m_num_keyframes, sizeof(uint32_t), 1, fd);
				fread(&m_texture_info[0].mp_wibble_vc_params[seq].m_phase, sizeof(uint32_t), 1, fd);
				m_texture_info[0].mp_wibble_vc_params[seq].mp_keyframes = (sVCWibbleKeyframe *)malloc(m_texture_info[0].mp_wibble_vc_params[seq].m_num_keyframes * sizeof(sVCWibbleKeyframe));
				
				fread(m_texture_info[0].mp_wibble_vc_params[seq].mp_keyframes, sizeof(sVCWibbleKeyframe), m_texture_info[0].mp_wibble_vc_params[seq].m_num_keyframes, fd);
			}
		}
		// Read texture wibble data.
		if( m_texture_info[pass].m_flags & MATFLAG_PASS_TEXTURE_ANIMATES )
		{
			if(mp_wibble_texture_params == NULL) {
				mp_wibble_texture_params = (sTextureWibbleParams *)malloc(sizeof(sTextureWibbleParams));

				memset(mp_wibble_texture_params, 0, sizeof(sTextureWibbleParams));

				m_texture_wibble = true;
			}
			uint32_t num_keyframes, period, iterations, phase;
			fread(&num_keyframes, sizeof(uint32_t), 1, fd);
			fread(&period, sizeof(uint32_t), 1, fd);
			fread(&iterations, sizeof(uint32_t), 1, fd);
			fread(&phase, sizeof(uint32_t), 1, fd);

			mp_wibble_texture_params->m_num_keyframes[pass] = num_keyframes;
			mp_wibble_texture_params->m_phase[pass] = period;
			mp_wibble_texture_params->m_num_iterations[pass] = iterations;
			mp_wibble_texture_params->mp_keyframes[pass] = (sTextureWibbleKeyframe *)malloc(sizeof(sTextureWibbleKeyframe)*num_keyframes);

			for(int x=0;x<num_keyframes;x++) {
				fread(&mp_wibble_texture_params->mp_keyframes[pass][x].m_time, sizeof(uint32_t), 1, fd);
				fread(&mp_wibble_texture_params->mp_keyframes[pass][x].m_texture_checksum, sizeof(uint32_t), 1, fd);
			}
		}
		//if(m_texture_info[pass].m_texture_checksum) {
			fread(&m_texture_info[pass].m_mag, sizeof(uint32_t), 1, fd);
			fread(&m_texture_info[pass].m_min, sizeof(uint32_t), 1, fd);
			fread(&m_texture_info[pass].m_k, sizeof(uint32_t), 1, fd);
			fread(&m_texture_info[pass].m_l, sizeof(uint32_t), 1, fd);
		//}
	}

}
std::vector<Material *> Material::loadMaterialsFromScene(FILE *fd, EPlatform platform) {


	uint32_t num_materials;
	std::vector<Material *> mats;

	fread(&num_materials,sizeof(uint32_t),1,fd);
	
	mats.reserve(num_materials);
	for(uint32_t i=0;i<num_materials;i++) {
		Material *mat = new Material(fd, platform);
		mats.push_back(mat);
	}
	return mats;
}

materialTexInfo Material::getTexture(int pass) {
	return m_texture_info[pass];
}
uint32_t Material::GetIgnoreVertexAlphaPasses() {
{
	// Return a bitfield with a bit set for any pass that is flagged to ignore vertex alpha.
	uint32_t bf = 0;

	for( uint32_t p = 0; p < m_passes; ++p )
	{
		if( m_texture_info[p].m_flags & MATFLAG_PASS_IGNORE_VERTEX_ALPHA )
		{
			bf |= ( 1 << p );
		}
	}
	
	return bf;
}

}

}