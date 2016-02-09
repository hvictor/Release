/*
 * gpu_flow_recogu8.h
 *
 *  Created on: Jan 24, 2016
 *      Author: sled
 */

#ifndef GPU_FLOW_RECOGU8_H_
#define GPU_FLOW_RECOGU8_H_

// GPU optical flow regions binary map
unsigned char *gpu_flow_recogu8_bitmap(unsigned char *u8data, unsigned char *u8res, unsigned int N, unsigned int thresh);


#endif /* GPU_FLOW_RECOGU8_H_ */
