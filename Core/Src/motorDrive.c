/*
 * motorA.c
 *
 *  Created on: Jun 14, 2023
 *      Author: Mehmet Dincer
 */

#include "motorDrive.h"

uint8_t carDirection = STOP;
uint8_t carTurn = STOPTURN;

extern RCFilter filt;
RCFilter filtLeft;
RCFilter filtRight;

//________________________________________________________
void motorDrive(uint32_t *adcValue)
{
	motorUpDown(adcValue);
	motorRightLeft(adcValue);
}

//________________________________________________________
/* This function configure the direction of the car*/
void motorUpDown(uint32_t *adcValue)
{
	/* if the joystick adc value greater than 2200 it is goes forward direction*/
	if(adcValue[0] > 2200)
	{
		/* if we desire the rotation of car in farward direction we disable this */
		if(carTurn != FORWARDTURN)
		{
			TIM3->CCR1 = 0;
			TIM3->CCR2 = 0;

			TIM2->CCR1 = RCFilter_Update(&filt, (adcValue[0] - 2200));
			TIM2->CCR2 = RCFilter_Update(&filt, (adcValue[0] - 2200));

			carDirection = FORWARD;

			filtLeft = filt;
			filtRight = filt;
		}
	}
	/* if joystick is in idle state reset the PWM values*/
	else if (adcValue[0] > 1900 && adcValue[0] < 2200)
	{
		if(carTurn != STOPTURN)
		{
			TIM3->CCR1 = 0;
			TIM3->CCR2 = 0;

			TIM2->CCR1 = 0;
			TIM2->CCR2 = 0;

			filt.out[0] = 0;
			filt.out[1] = 0;

			carDirection = STOP;
		}
	}
	/* if adc value if less than 1900 so direction will be backward*/
	else
	{
		if(carTurn != BACKWARDTURN)
		{
			TIM2->CCR1 = 0;
			TIM2->CCR2 = 0;

			TIM3->CCR1 = RCFilter_Update(&filt, (1900 - adcValue[0]));
			TIM3->CCR2 = RCFilter_Update(&filt, (1900 - adcValue[0]));

			carDirection = BACKWARD;

			filtLeft = filt;
			filtRight = filt;
		}
	}
}

//________________________________________________________
/*This function configure the turning setting of the car*/
void motorRightLeft(uint32_t *adcValue)
{
	/*if car direction is forward*/
	if(carDirection == FORWARD)
	{
		/*forward right setting*/
		if(adcValue[1] > 2200)
		{
			carTurn = FORWARDTURN;
			float ratio = (adcValue[1] - 2200.0f) / 1896.0 / 2.0f; //2.0f == %50 ratio

			uint32_t adcDirection = adcValue[0] - 2200;
			uint32_t leftMotor = adcDirection + adcDirection * ratio;
			uint32_t rightMotor = adcDirection - adcDirection * ratio;

			leftMotor = (leftMotor > 1896) ? 1896 : leftMotor;
			rightMotor = (rightMotor < 0) ? 0: rightMotor;

			TIM2->CCR2 = RCFilter_Update(&filtLeft, leftMotor);
			TIM2->CCR1 = RCFilter_Update(&filtRight, rightMotor);

			TIM3->CCR1 = 0;
			TIM3->CCR2 = 0;
		}
		/*forward idle state setting*/
		else if(adcValue[1] >1900 && adcValue[1] < 2200)
		{
			carTurn = STOP;
		}
		/*forward left state setting*/
		else if(adcValue[1] < 1900)
		{
			carTurn = FORWARDTURN;
			float ratio = (1900.0f - adcValue[1]) / 1900.0f / 2.0f; //2.0f == %50 ratio
			// divided by two because the sensitivity is setting to %50
			uint32_t adcDirection = adcValue[0] - 2200;
			uint32_t leftMotor = adcDirection - adcDirection * ratio;
			uint32_t rightMotor = adcDirection + adcDirection * ratio;

			leftMotor = (leftMotor < 0) ? 0 : leftMotor;
			rightMotor = (rightMotor > 1896) ? 1896 : rightMotor;

			TIM2->CCR2 = RCFilter_Update(&filtLeft, leftMotor);
			TIM2->CCR1 = RCFilter_Update(&filtRight, rightMotor);

			TIM3->CCR1 = 0;
			TIM3->CCR2 = 0;
		}
	}
	/*if car is stop mode*/
	else if(carDirection == STOP)
	{
		if(adcValue[1] > 2200)
		{
			carTurn = STOPTURN;
			float ratio = (adcValue[1] - 2200.0) / 1896.0; // %100 ratio
			uint32_t leftMotorAdc = 1896 * ratio; //turn right
			uint32_t rightMotorAdc = 1900 * ratio; //turn left

			TIM2->CCR2 = RCFilter_Update(&filt, leftMotorAdc);
			TIM3->CCR2 = 0;

			TIM3->CCR1 = RCFilter_Update(&filt, rightMotorAdc);
			TIM2->CCR1 = 0;
		}
		else if(adcValue[1] > 1900 && adcValue[1] < 2200)
		{
			carTurn = STOP;
		}
		else if(adcValue[1] < 1900)
		{
			carTurn = STOPTURN;
			float ratio = (1900 - adcValue[1]) / 1900.0; // %100 ratio
			uint32_t leftMotorAdc = 1900 * ratio; //turn right
			uint32_t rightMotorAdc = 1900 * ratio; //turn right

			TIM3->CCR2 = RCFilter_Update(&filt, rightMotorAdc);
			TIM2->CCR2 = 0;

			TIM2->CCR1 = RCFilter_Update(&filt, leftMotorAdc);
			TIM3->CCR1 = 0;
		}
	}
	/* if car direction is in backward*/
	else if (carDirection == BACKWARD)
	{
		/* car moving direction backward right*/
		if(adcValue[1] > 2200)
		{
			carTurn = BACKWARDTURN;
			float ratio = (adcValue[1] - 2200.0f) / 1896.0 / 2.0f; //2.0f == %50 ratio

			uint32_t adcDirection = 1900 - adcValue[0];
			uint32_t leftMotor = adcDirection + adcDirection * ratio;
			uint32_t rightMotor = adcDirection - adcDirection * ratio;

			leftMotor = (leftMotor > 1900) ? 1900 : leftMotor;
			rightMotor = (rightMotor < 0) ? 0: rightMotor;

			TIM3->CCR2 = RCFilter_Update(&filtLeft, leftMotor);
			TIM3->CCR1 = RCFilter_Update(&filtRight, rightMotor);

			TIM2->CCR1 = 0;
			TIM2->CCR2 = 0;
		}
		else if(adcValue[1] > 1900 && adcValue[1] < 2200)
		{
			carTurn = STOP;
		}
		else if(adcValue[1] < 1900)
		{
			carTurn = BACKWARDTURN;
			float ratio = (1900.0f - adcValue[1]) / 1900.0f / 2.0f ; //2.0f == %50 ratio
			// divided by two because the sensitivity is setting to %50
			uint32_t adcDirection = 1900 - adcValue[0];
			uint32_t leftMotor = adcDirection - adcDirection * ratio;
			uint32_t rightMotor = adcDirection + adcDirection * ratio;

			leftMotor = (leftMotor < 0) ? 0 : leftMotor;
			rightMotor = (rightMotor > 1900) ? 1900 : rightMotor;

			TIM3->CCR2 = RCFilter_Update(&filtLeft, leftMotor);
			TIM3->CCR1 = RCFilter_Update(&filtRight, rightMotor);

			TIM2->CCR1 = 0;
			TIM2->CCR2 = 0;
		}
	}
}










