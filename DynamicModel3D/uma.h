/*
 * uma.h
 *
 *  Created on: Aug 7, 2016
 *      Author: sled
 */

#ifndef UMA_H_
#define UMA_H_

void uma_init();
void uma_reset();
void uma_buffer_shift();
dyn_state_t *uma_analyze_buffer_data();
dyn_state_t *uma_update(dyn_state_t *state);


#endif /* UMA_H_ */
