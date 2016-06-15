/*
 * DynamicModelDataSerializer.cpp
 *
 *  Created on: Jun 15, 2016
 *      Author: sled
 */

#include "DynamicModelDataSerializer.h"

DynamicModelDataSerializer *DynamicModelDataSerializer::getInstance()
{
	static DynamicModelDataSerializer *inst = 0;

	if (inst == 0)
	{
		inst = new DynamicModelDataSerializer();
	}

	return inst;
}

DynamicModelDataSerializer::DynamicModelDataSerializer()
{

}

DynamicModelDataSerializer::~DynamicModelDataSerializer()
{

}

void DynamicModelDataSerializer::openSerializationStream(char *fileName)
{

}

void DynamicModelDataSerializer::closeSerializationStream()
{

}

void DynamicModelDataSerializer::serializeDynamicModelData(DynamicModelData2D data_2d, DynamicModelData3D data_3d)
{

}
