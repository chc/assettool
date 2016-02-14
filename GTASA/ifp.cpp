#include <main.h>
#include <GTASA/CGTASA.h>
#include <Generic/CGame.h>
#include <Vector.h>
#include <GTASA/ifp.h>
#include <Generic/CKeyframeSequence.h>
#include <Generic/CKeyframeSeqCollection.h>
#include <Generic/CKeyframeCollection.h>
#include <crc32.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define ANIM_V2_FOURCC 0x33504e41

typedef struct {
	uint32_t magic; //ANP3
	uint32_t end_address;
	char script_name[24];
	uint32_t anim_count;
} IFPHeader;
typedef struct {
	char name[24];
	uint32_t object_count;
	uint32_t frame_size;
	uint32_t unknown;
} AnimHeader;
typedef struct {
	char name[24];
	uint32_t type;
	uint32_t num_frames;
	uint32_t bone_id;
} ObjectInfo;
typedef struct {
	int16_t quat[4];
	uint16_t time; //seconds
	int16_t trans[3];
} AnimRootFrame;
typedef struct {
	int16_t quat[4];
	uint16_t time; //seconds
} AnimChildFrame;
void print_anim_info(AnimHeader *anim) {
	printf("Anim Info: \n");
	printf("Name: %s (%08X)\n", anim->name,crc32(0, anim->name, strlen(anim->name)));
	printf("Objects: %d\n", anim->object_count);
	printf("Frame size: %d\n", anim->frame_size);
	//printf("Unknown: %d\n", anim->unknown);
	printf("\n\n\n");
}
void print_ifp_header(IFPHeader *head) {
	printf("IFP Header: \n");
	printf("Magic: 0x%08X\n", head->magic);
	printf("Anim count: %d\n", head->anim_count);
	printf("Script name: %s\n", head->script_name);
	printf("End: %08X\n", head->end_address);
	printf("\n\n\n");
}
void print_anim_root_frame(AnimRootFrame *frame) {
	glm::quat quat = glm::quat(frame->quat[3] / 4096.0, frame->quat[0] / 4096.0, frame->quat[1] / 4096.0, frame->quat[2] / 4096.0);
	glm::vec3 trans = glm::vec3(frame->trans[0] / 1024.0, frame->trans[1] / 1024.0, frame->trans[2] / 1024.0);
	printf("Frame Info: \n");
	printf("Trans: %f %f %f\n", trans.x, trans.y, trans.z);
	printf("Quat: %f %f %f %f\n", quat.x, quat.y, quat.z, quat.w);
	printf("Time: %d\n", frame->time);
	printf("Frame time: %f\n", frame->time / GTA_FRAMES_PER_SECOND);
	printf("\n\n\n");
}
void print_anim_child_frame(AnimChildFrame *frame) {
	float div = 4096.0;
	glm::quat quat = glm::quat((frame->quat[3] / div), (frame->quat[0] / div), (frame->quat[1] / div), (frame->quat[2] / div));
	printf("Frame Info: \n");
	printf("Quat: %f %f %f %f\n", quat.x, quat.y, quat.z, quat.w);
	printf("Time: %d\n", frame->time);
	printf("Frame time: %f\n", frame->time / GTA_FRAMES_PER_SECOND);
	printf("\n\n\n");
}
void print_object_info(ObjectInfo *obj) {
	printf("Object Info: \n");
	printf("Name: %s\n", obj->name);
	printf("Type: %d\n", obj->type);
	printf("Frame Count: %d\n", obj->num_frames);
	printf("Bone ID: %d\n", obj->bone_id);
	printf("\n\n\n");
}
bool gta_rw_export_ifp(ExportOptions *expOpts) {
	return false;
}

void get_sequence_from_frame(CKeyframeSequence *keyframe, AnimRootFrame *frame) {
	float div = 4096.0;
	glm::quat quat = glm::quat((frame->quat[3] / div), (frame->quat[0] / div), (frame->quat[1] / div), (frame->quat[2] / div));
	glm::vec3 trans = glm::vec3((frame->trans[0] / 1024.0), (frame->trans[1] / 1024.0), (frame->trans[2] / 1024.0));
	keyframe->setRotation(quat);
	keyframe->setPosition(trans);
	keyframe->setBeginFrame(frame->time / GTA_FRAMES_PER_SECOND);
}
bool gta_rw_import_ifp(ImportOptions* impOpts) {
	FILE *fd = fopen(impOpts->path, "rb");
	IFPHeader head;
	AnimHeader anim;

	Core::Vector<CKeyframeCollection *> collections;
	CKeyframeSeqCollection *collection = NULL;
	fread(&head, sizeof(IFPHeader), 1, fd);
	if(head.magic != ANIM_V2_FOURCC) 
		return false;
	print_ifp_header(&head);
	for (int i = 0; i < head.anim_count; i++) {
		fread(&anim, sizeof(AnimHeader), 1, fd);
		print_anim_info(&anim);
		CKeyframeCollection *key_col = new CKeyframeCollection();
		key_col->setCollectionIdentifier(crc32(0, anim.name, strlen(anim.name)));
		printf("key checksum: %s 0x%08X\n", anim.name, key_col->getCollectionIdentifier());
		for (int j = 0; j < anim.object_count; j++) {
			ObjectInfo obj;
			fread(&obj, sizeof(ObjectInfo), 1, fd);
			print_object_info(&obj);
			collection = new CKeyframeSeqCollection();
			uint32_t checksum = crc32(0, obj.name, strlen(obj.name));
			printf("Obj checksum: %s 0x%08X\n", obj.name, checksum);
			collection->setCollectionIdentifier(checksum);
			collection->setBoneID(obj.bone_id);
			for (int k = 0; k < obj.num_frames; k++) {
				if (obj.type == 4) {
					AnimRootFrame frame;
					fread(&frame, sizeof(AnimRootFrame), 1, fd);
					CKeyframeSequence *seq = new CKeyframeSequence();
					get_sequence_from_frame(seq, &frame);
					collection->add(seq);
					print_anim_root_frame(&frame);
				}
				else {
					AnimChildFrame frame;
					AnimRootFrame fake_root;
					fread(&frame, sizeof(AnimChildFrame), 1, fd);

					memcpy(&fake_root.quat, &frame.quat, sizeof(int16_t) * 4);
					memset(&fake_root.trans, 0, sizeof(int16_t) * 3);
					fake_root.time = frame.time;
					CKeyframeSequence *seq = new CKeyframeSequence();
					get_sequence_from_frame(seq, &fake_root);
					collection->add(seq);
					print_anim_child_frame(&frame);
				}
			}
			key_col->add(collection);
		}
		collections.add(key_col);
	}

	ExportOptions opts;
	memset(&opts, 0, sizeof(opts));


	opts.dataClass = &collections;
	opts.srcPath = impOpts->path;
	opts.args = impOpts->expArgs;
	opts.path = impOpts->outpath;
	impOpts->exporter(&opts);
	return true;
}