/*
 * DynamicModelDataSerializer.h
 *
 *  Created on: Jun 15, 2016
 *      Author: sled
 */

#ifndef DYNAMICMODELDATASERIALIZER_H_
#define DYNAMICMODELDATASERIALIZER_H_

#include "../Configuration/Configuration.h"
#include "../TargetPredator/TargetPredator.h"
#include "../DynamicModel3D/DynamicModel.h"
#include "../Common/data_types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <list>
#include <algorithm>

using namespace std;

typedef struct
{
	list<pred_state_t> data;
	bool dynamic_model_3d_data_avail;
} DynamicModelData2D;

typedef struct
{
	list<dyn_state_t> data;
} DynamicModelData3D;

class DynamicModelDataSerializer {
public:
	static DynamicModelDataSerializer *getInstance();
	virtual ~DynamicModelDataSerializer();
	void openSerializationStream(char *fileName);
	void closeSerializationStream();
	void serializeDynamicModelData(DynamicModelData2D data_2d, DynamicModelData3D data_3d);

private:
	DynamicModelDataSerializer();
};

#endif /* DYNAMICMODELDATASERIALIZER_H_ */
