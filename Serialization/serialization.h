#ifndef __SERIALIZATION_H
#define __SERIALIZATION_H

#include "../FastMemory/fast_mem_pool.h"
#include "../StaticModel/GroundModel.h"
#include "../StaticModel/NetModel.h"
#include "../StaticModel/TennisFieldStaticModel.h"

void open_serialization_channel(char *file_name);
void close_serialization_channel();
void serialize_static_model();
void deserialize_static_model();
void serialize_frame_data(FrameData *frame_data);
bool deserialize_next_frame_data(FrameData *dst);
void open_deserialization_channel(char *file_name);
void close_deserialization_channel();

void open_serialization_channel_async(char *fileName);
void serialize_frame_data_async(FrameData *frame_data);

#endif
