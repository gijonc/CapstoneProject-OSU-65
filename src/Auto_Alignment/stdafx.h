#ifndef _STDAFX_H_
#define _STDAFX_H_
/*
	File: stdafx.h
	
	include file for standard system include files,
	or project specific include files that are used frequently, but
	are changed infrequently
*/

#include <stdint.h>
#include <Arduino.h>
#include <Wire.h>

// TODO: reference additional headers your program requires here
#include "debug_config.h"
#include "quaternion.h"
#include "data_utils.h"
#include "quaternionFilters.h"
#include "quaternion_module.h"
#include "MPU9250.h"

#endif