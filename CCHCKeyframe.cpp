#include "Iterator.h"
#include "CCHCKeyframe.h"
#include "CKeyframeSequence.h"
#include "CKeyframeCollection.h"
#define CHC_KEYFRAME_VERSION 1
typedef struct {
	uint32_t version;
	uint32_t num_collections;
} CCHCKeyframeCollectionHeader;

enum ECHCKeyframeFlags { //must corrospond to game
	ECHCMeshFlag_HasTranslation = (1 << 0),
	ECHCMeshFlag_HasRotation = (1 << 1),
};
typedef struct {
	uint32_t num_frames;
	uint8_t frame_flags;
	uint32_t checksum;
} CCHCKeyframeCollection;
bool chc_engine_import_keyframe_collection(ImportOptions* opts) {
	return false;
}
bool chc_engine_export_keyframe_collection(ExportOptions* opts) {
	Core::Vector<CKeyframeCollection *> *vec = (Core::Vector<CKeyframeCollection *> *)opts->dataClass;
	FILE *fd = fopen(opts->path, "wb");
	Core::Iterator<Core::Vector<CKeyframeCollection *>, CKeyframeCollection *> it = vec->begin();

	CCHCKeyframeCollectionHeader head;
	head.version = CHC_KEYFRAME_VERSION;
	head.num_collections = vec->size();

	fwrite(&head, sizeof(CCHCKeyframeCollectionHeader), 1, fd);
	while (it != vec->end()) {
		CKeyframeCollection *key_collection = *it;
		Core::Vector<CKeyframeSequence *> frame_collection = key_collection->getCollection();
		CCHCKeyframeCollection coll_head;
		memset(&coll_head, 0, sizeof(coll_head));
		coll_head.frame_flags = ECHCMeshFlag_HasTranslation| ECHCMeshFlag_HasRotation; //just write it all out for now
		coll_head.num_frames = frame_collection.size();
		coll_head.checksum = key_collection->getCollectionIdentifier();
		fwrite(&coll_head, sizeof(coll_head), 1, fd);
		Core::Iterator<Core::Vector<CKeyframeSequence *>, CKeyframeSequence *> it2 = frame_collection.begin();
		while (it2 != frame_collection.end()) {
			CKeyframeSequence *seq = *it2;
			float begin_frame = seq->getBeginFrame();
			glm::vec3 pos = seq->getPosition();
			glm::quat quat = seq->getRotation();
			fwrite(&begin_frame, sizeof(float), 1, fd);
			fwrite(glm::value_ptr(pos), sizeof(float), 3, fd);
			fwrite(glm::value_ptr(quat), sizeof(float), 4, fd);

			it2++;
		}
		it++;
	}
	fclose(fd);
	return true;
}