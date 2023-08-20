/*
 * motorA.h
 *
 *  Created on: Jun 14, 2023
 *      Author: Mehmet Dincer
 */

#ifndef INC_MOTORDRIVE_H_
#define INC_MOTORDRIVE_H_


#include "RCFilter.h"
#include "stm32f4xx_hal.h"

#define STOP				(0x00u)			// This 3 macros using to determine the
#define FORWARD				(0x01u)			// direction of the car
#define BACKWARD 			(0x02u)


#define STOPTURN			(0x01u)
#define FORWARDTURN			(0x02u)
#define BACKWARDTURN		(0x03u)

void motorDrive(uint32_t *adcValue);
void motorUpDown(uint32_t *adcValue);		// Direction setting
void motorRightLeft(uint32_t *adcValue);	// Turning settting



#endif /* INC_MOTORDRIVE_H_ */
