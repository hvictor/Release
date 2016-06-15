#ifndef __SERIALIZATION_H
#define __SERIALIZATION_H

#include "../FastMemory/fast_mem_pool.h"

void open_serialization_channel(char *file_name);
void close_serialization_channel();
void serialize_frame_data(FrameData *frame_data);
bool deserialize_next_frame_data(FrameData *dst);
void open_deserialization_channel(char *file_name);
void close_deserialization_channel();

#endif
