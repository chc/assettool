#include <THPS/CTHPSScene.h>
#include <Generic/CMesh.h>
#include <Generic/CMaterial.h>
#include <Generic/ScenePack.h>

#include <shaderlib/shaderasm.h>
#include <shaderlib/GLSLBuilder.h>

int get_mesh_count(std::vector<LibTHPS::Sector *> secs) {
	int c = 0;
	std::vector<LibTHPS::Sector *>::iterator it = secs.begin();
	while(it != secs.end()) {

		LibTHPS::Sector *sec = *it;
		c += sec->getMeshes().size();	
		it++;
	}
	return c;
}
ETextureAddresingMode tex_address_mode_from_thps_uvaddress(uint32_t filter) {
	switch(filter) {
		default:
		case 0:
			return ETextureAddressMode_Wrap;
		case 1:
			return ETextureAddressMode_Clamp;
		case 2:
			return ETextureAddressMode_Border;
	}
}
uint32_t get_material_shader_flags(LibTHPS::Material *mat, uint32_t *ignore_bf, EBlendMode *blend_modes, bool *mcm) {
	// Integrate the ignore vertex alpha flags.
	uint32_t code = 0;
	*ignore_bf = mat->GetIgnoreVertexAlphaPasses();
	code |= ( *ignore_bf << 25 );

	for(int i=0;i<mat->m_passes;i++) {
		LibTHPS::materialTexInfo texinfo = mat->getTexture(i);
		if( !*mcm )
		{
			if(( texinfo.m_colour[0] != 0.5f ) || ( texinfo.m_colour[1] != 0.5f ) || ( texinfo.m_colour[2] != 0.5f ))
				*mcm = true;
		}
		blend_modes[i] = (EBlendMode)(texinfo.m_reg_alpha & LibTHPS::Material::BLEND_MODE_MASK);

		code |= ( blend_modes[i] << ( 5 * ( i + 1 )));
	}
	// Check also to see if material color modulation is required.
	if( mcm )
	{
		code |= ( 1 << 30 );
	}
	return code;
}
void setup_material_shader(LibTHPS::Material *mat, CMaterial *out) {
	EBlendMode blend_modes[4];
	uint32_t code;
	uint32_t ignore_bf;
	bool mcm;
	get_material_shader_flags(mat, &ignore_bf, (EBlendMode *)&blend_modes, &mcm);

	char shader_buffer[4096];
	memset(&shader_buffer,0,sizeof(shader_buffer));

	switch(mat->m_passes) {
		case 1:
			break;
		case 2: 
		{
			/*
			vec1-2 = tex colours
			vec 3-4 = temp colours
			vec5 = misc temp stuff
			*/
			strcat(shader_buffer,"stex vec0 tex0 uv0\n");
			strcat(shader_buffer,"stex vec1 tex1 uv1\n");

			if(mcm) {
				// Modulate texture0 and texture 1 color with pass 0 and pass 1 material color, and place into t0.rgb and t1.rgb.
				strcat(shader_buffer,"mul vec2.rgb vec0.rgb mat0.diffuse.rgb | clamp(0.0,1.0)\n");
				strcat(shader_buffer,"mul vec3.rgb vec1.rgb mat1.diffuse.rgb | clamp(0.0,1.0)\n");

				// Modulate result color with vertex color.
				strcat(shader_buffer,"mul vec0.rgb vec2.rgb vertcol.rgb | clamp(0.0,1.0) x(4.0)");
				strcat(shader_buffer,"mul vec1.rgb vec3.rgb vertcol.rgb | clamp(0.0,1.0) x(4.0)");
				if(ignore_bf == 0x00) {
					// Pass0 modulates with vertex alpha. Pass1 modulates with vertex alpha.
					strcat(shader_buffer,"mul vec0.a vec0.a vertcol.a | clamp(0.0,1.0) x(2.0)\n");
					strcat(shader_buffer,"mul vec1.a vec1.a vertcol.a | clamp(0.0,1.0) x(2.0)\n");
				} else if(ignore_bf == 0x01) {
					// Pass0 modulates with constant alpha. Pass1 modulates with vertex alpha.
					strcat(shader_buffer,"mul vec0.a vec0.a const.a | clamp(0.0,1.0) x(2.0)\n");
					strcat(shader_buffer,"mul vec1.a vec1.a vertcol.a | clamp(0.0,1.0) x(2.0)\n");
				} else if(ignore_bf == 0x02) {
					// Pass0 modulates with vertex alpha. Pass1 modulates with constant alpha.
					strcat(shader_buffer,"mul vec0.a vec0.a vertcol.a | clamp(0.0,1.0) x(2.0)\n");
					strcat(shader_buffer,"mul vec1.a vec1.a const0.a | clamp(0.0,1.0) x(2.0)\n");
				} else if(ignore_bf == 0x03) {
					// Pass0 modulates with constant alpha. Pass1 modulates with constant alpha.
					strcat(shader_buffer,"mul vec0.a vec0.a const.a | clamp(0.0,1.0) x(2.0)\n");
					strcat(shader_buffer,"mul vec1.a vec1.a const.a | clamp(0.0,1.0) x(2.0)\n");
				}
			} else {
				// Modulate texture0 and texture1 color with vertex color.
				strcat(shader_buffer, "mul vec0.rgb vec0.rgb vertcol.rgb | clamp(0.0,1.0) x(2.0)\n");
				strcat(shader_buffer, "mul vec1.rgb vec1.rgb vertcol.rgb | clamp(0.0,1.0) x(2.0)\n");
				if( ignore_bf == 0x00 ) {
					// Pass0 modulates with vertex alpha. Pass1 modulates with vertex alpha.
					strcat(shader_buffer, "mul vec0.a vec0.a vertcol.a | clamp(0.0,1.0) x(2.0)\n");
					strcat(shader_buffer, "mul vec1.a vec1.a vertcol.a | clamp(0.0,1.0) x(2.0)\n");
				} else if(ignore_bf == 0x01) {
					// Pass0 modulates with constant alpha. Pass1 modulates with vertex alpha.
					strcat(shader_buffer, "mul vec0.a vec0.a const0.a | clamp(0.0,1.0) x(2.0)\n");
					strcat(shader_buffer, "mul vec1.a vec1.a vertcol.a | clamp(0.0,1.0) x(2.0)\n");
				} else if(ignore_bf == 0x02) {
					// Pass0 modulates with vertex alpha. Pass1 modulates with constant alpha.
					strcat(shader_buffer, "mul vec0.a vec0.a vertcol.a | clamp(0.0,1.0) x(2.0)\n");
					strcat(shader_buffer, "mul vec1.a vec1.a const0.a | clamp(0.0,1.0) x(2.0)\n");
				} else if(ignore_bf == 0x03) {
					// Pass0 modulates with constant alpha. Pass1 modulates with constant alpha.
					strcat(shader_buffer, "mul vec0.a vec0.a const0.a | clamp(0.0,1.0) x(2.0)\n");
					strcat(shader_buffer, "mul vec1.a vec1.a const0.a | clamp(0.0,1.0) x(2.0)\n");
				}
			}
			strcat(shader_buffer, "mov outcol vec0\n");
			switch(blend_modes[1]) {
				case EBlendMode_Add:
					strcat(shader_buffer, "mul vec5.rgb vec1.rgb vec1.a\n");
					strcat(shader_buffer, "add outCol.rgb vec5.rgb vec0.rgb | clamp(0.0,1.0)\n");
					//strcat(shader_buffer, "finalColour.rgb = clamp((texcolour[1].rgb*texcolour[1].a)+texcolour[0].rgb,0.0,1.0);\n");
					break;
				case EBlendMode_Add_Fixed:
					strcat(shader_buffer, "mul vec5.rgb vec1.rgb mat.diffuse[1].a\n");
					strcat(shader_buffer, "add outCol.rgb vec5.rgb vec0.rgb | clamp(0.0,1.0)\n");
					break;
				case EBlendMode_Subtract:
					strcat(shader_buffer, "mul vec5.rgb vec1.rgb -vec1.a\n");
					strcat(shader_buffer, "out outCol.rgb vec5.rgb vec0.rgb | clamp(0.0,1.0)\n");
				break;
				case EBlendMode_Subtract_Fixed:
					strcat(shader_buffer, "mul vec5.rgb vec1.rgb -mat1.diffuse.a\n");
					strcat(shader_buffer, "out outCol.rgb vec5.rgb vec0.rgb | clamp(0.0,1.0)\n");
				break;
				case EBlendMode_Blend:
					//strcat(shader_buffer, "finalColour.rgb = vec3(mix(texcolour[0],texcolour[1],texcolour[1].a));\n");
					//strcat(shader_buffer, "gl_FragColor = clamp(texcolour[0]*texcolour[1],0.0,1.0);\n");
					strcat(shader_buffer, "lerp outCol.rgb vec0.rgb vec1.rgb mat1.diffuse.a\n");
				break;
				case EBlendMode_Blend_Fixed:
					strcat(shader_buffer, "lerp outCol.rgb vec0.rgb vec1.rgb mat1.diffuse.a\n");
					//strcat(shader_buffer, "finalColour.rgb = vec3(mix(texcolour[0],texcolour[1],materialColour[1].a));\n");
				break;
				case EBlendMode_Modulate:
					strcat(shader_buffer, "mul outCol.rgb vec0.rgb vec1.a\n");
					//strcat(shader_buffer, "finalColour.rgb = (texcolour[0].rgb*texcolour[1].a);\n");
					//strcat(shader_buffer, "gl_FragColor = clamp(texcolour[0]*texcolour[1],0.0,1.0);\n");
				break;
				case EBlendMode_Modulate_Fixed:
					//strcat(shader_buffer, "finalColour.rgb = (texcolour[0].rgb*materialColour[1].a);\n");
					//strcat(shader_buffer, "texcolour[0].rgb = clamp(finalColour.rgb,0.0,1.0);\n");
					strcat(shader_buffer, "mul outCol.rgb vec0.rgb mat1.diffuse.a\n");
				break; //removed in xbox version for some reason
				case EBlendMode_Brighten:
					strcat(shader_buffer, "mul vec5.rgb vec0.rgb vec1.a\n");
					strcat(shader_buffer, "add outCol.rgb vec5.rgb vec0.rgb | clamp(0.0,1.0)\n");
					//strcat(shader_buffer, "finalColour.rgb = (texcolour[0].rgb*texcolour[1].a) + texcolour[0].rgb;\n");
					//strcat(shader_buffer, "gl_FragColor = clamp(texcolour[0]*texcolour[1],0.0,1.0);\n");
				break;
				case EBlendMode_Brighten_Fixed:
					strcat(shader_buffer, "mul vec5.rgb vec0.rgb mat0.diffuse.a\n");
					strcat(shader_buffer, "add outCol.rgb vec5.rgb vec0.rgb | clamp(0.0,1.0)\n");
					//strcat(shader_buffer, "finalColour.rgb = (texcolour[0].rgb*materialColour[1].a) + texcolour[0].rgb;\n");
				break;
				case EBlendMode_Blend_Previous_Mask:
					//strcat(shader_buffer, "finalColour.rgb = vec3(mix(texcolour[0].rgb,texcolour[1].rgb,texcolour[0].a));\n");
					strcat(shader_buffer, "lerp outCol.rgb vec0.rgb vec1.rgb vec0.a\n");
				break;
				case EBlendMode_Blend_Inverse_Previous_Mask:
					strcat(shader_buffer, "lerp outCol.rgb vec1.rgb vec0.rgb vec0.a\n");
					//strcat(shader_buffer, "finalColour.rgb = vec3(mix(texcolour[1].rgb,texcolour[0].rgb,texcolour[0].a));\n");
				break;
			}
			break;
		}
	}
	ShaderASM::run_shader_mem(shader_buffer, strlen(shader_buffer));
	out->setShaderCode((uint8_t *)ShaderASM::g_asmState.opcodeBuffer, ShaderASM::g_asmState.opcodeWriteIDX);
}
void thps_material_to_cmaterial(LibTHPS::Material *mat, CMaterial *out) {
	out->setIdentifierChecksum(mat->getChecksum());

	setup_material_shader(mat, out);

	for(int i=0;i<mat->m_passes;i++) {
		LibTHPS::materialTexInfo matinfo = mat->getTexture(i);
		if(matinfo.m_texture_checksum == 0) break;
		out->setTextureChecksum(matinfo.m_texture_checksum, i);
		out->setBlendMode(EBlendMode_Modulate, i);
		out->setTextureAddressMode(tex_address_mode_from_thps_uvaddress(matinfo.m_u_address), tex_address_mode_from_thps_uvaddress(matinfo.m_v_address), i);
	}	
}
bool thps_xbx_import_scn(ImportOptions* opts) {
	LibTHPS::Scene *scn = new LibTHPS::Scene(opts->path, LibTHPS::Platform_Xbox);

	//load materials
	std::vector<LibTHPS::Material *> mats = scn->getMaterialList();
	std::vector<LibTHPS::Material *>::iterator it2 = mats.begin();
	CMaterial **out_mats = (CMaterial **)malloc(mats.size() * sizeof(CMaterial **));
	int i = 0;
	while(it2 != mats.end()) {
		LibTHPS::Material *mat = *it2;
		out_mats[i] = new CMaterial();
		thps_material_to_cmaterial(mat, out_mats[i]);
		i++;
		it2++;
	}

	std::vector<LibTHPS::Sector *> secs = scn->getSectorList();

	int mesh_count = get_mesh_count(secs);

	CMesh **out_meshes = (CMesh **)malloc(sizeof(CMesh *) * secs.size());
	
	std::vector<LibTHPS::Sector *>::iterator it = secs.begin();

	i = 0;

	//load sector stuff
	while(it != secs.end()) {

		LibTHPS::Sector *sec = *it;

		out_meshes[i] = new CMesh();
		out_meshes[i]->setNumVerts(sec->getNumVerticies());
		out_meshes[i]->setVerticies(sec->getPositions());
		out_meshes[i]->setNormals(sec->getNormals());
		out_meshes[i]->setGroupId(sec->getChecksum());
		out_meshes[i]->setColours(sec->getColours());

		float *uvws = (float *)malloc(sizeof(float) * 3 * sec->getNumVerticies());
		float *_uvs = sec->getTexCoords();
		float **uv_buffers = (float **)malloc(sizeof(float *) * sec->getNumTexChannels());
		float **uv_buffers_cursor = (float **)malloc(sizeof(float *) * sec->getNumTexChannels());
		float *p = _uvs;
		for(int j=0;j<sec->getNumTexChannels();j++) {
			uv_buffers[j] = (float *)malloc(sizeof(float)*3*sec->getNumVerticies());
			uv_buffers_cursor[j] = uv_buffers[j];
		}
		for(int j=0;j<sec->getNumVerticies();j++) {
			for(int k=0;k<sec->getNumTexChannels();k++) {
				memcpy(uv_buffers_cursor[k], p, sizeof(float)*2);
				uv_buffers_cursor[k][2] = 0.0;
				uv_buffers_cursor[k] += 3;
				p += 2;
			}
		}
	
		for(int k=0;k<sec->getNumTexChannels();k++) {
			out_meshes[i]->setUVWs(uv_buffers[k], k);
			free(uv_buffers[k]);
		}
		free(uv_buffers_cursor);
		free(uv_buffers);
		free(uvws);

		//load sector indices stuff
		std::vector<LibTHPS::Mesh *> meshes = sec->getMeshes();
		std::vector<LibTHPS::Mesh *>::iterator it2 = meshes.begin();

		out_meshes[i]->setIndexLevels(meshes.size());
		out_meshes[i]->setPrimType(CMeshPrimType_TriangleStrips);
		out_meshes[i]->setCoordinateSystem(ECoordinateSystem_Left_XZY);
		uint32_t j = 0;
		while(it2 != meshes.end()) {
			LibTHPS::Mesh *mesh = *it2;
			uint16_t *indices, *x;
			
			uint32_t num_indices = mesh->getCombinedIndices(&indices);
			x = indices;
			uint32_t *out_indices = (uint32_t *)malloc(num_indices * sizeof(uint32_t));
			uint32_t *p = out_indices;
			for(int c=0;c<num_indices;c++) {
				*p++ = *x++;
			}

			CMaterial *mat = CMaterial::findMaterialByChecksum(out_mats,mats.size() , mesh->getMaterialChecksum());
			out_meshes[i]->setIndexMaterial(mat, j);
			out_meshes[i]->setUseIndexedMaterials(true);
			out_meshes[i]->setIndices(out_indices, num_indices, j++);
			free(out_indices);
			free(indices);
			it2++;
		}

		i++;
		it++;
	}

	delete scn;

	ScenePack pack;
	memset(&pack, 0, sizeof(pack));
	pack.m_meshes = out_meshes;
	pack.num_meshes = secs.size();
	pack.m_materials = out_mats;
	pack.num_materials = mats.size();

	ExportOptions expopts;
	memset(&expopts,0,sizeof(expopts));
	
	expopts.dataClass = &pack;
	expopts.srcPath = opts->path;
	expopts.args = opts->expArgs;
	expopts.path = opts->outpath;
	opts->exporter(&expopts);
	return true;
}
bool thps_xbx_export_scn(ExportOptions* opts) {
	return false;
}