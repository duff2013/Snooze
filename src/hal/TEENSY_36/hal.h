/***********************************************************************************
 * Low Power Library
 * Copyright (c) 2019, Colin Duffy https://github.com/duff2013
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ***********************************************************************************
 *  hal.h
 *  Teensy 3.6
 *
 * Purpose: Teensy 3.2 HAL
 *
 **********************************************************************************/
#if defined(__MK66FX1M0__)

#ifndef __HAL_H__
#define __HAL_H__

#define TYPE uint8_t
#define REDUCED_CPU_BLOCK( SNOOZE_BLOCK )   \
for ( TYPE __ToDo = set_runlp( SNOOZE_BLOCK );  __ToDo;  __ToDo = set_run( SNOOZE_BLOCK ) )


#ifdef __cplusplus
#include "common.h"
#include "SnoozeTimer.h"
#include "SnoozeAlarm.h"
#include "SnoozeTouch.h"
#include "SnoozeCompare.h"
#include "SnoozeDigital.h"
#include "SnoozeAudio.h"
#include "SnoozeUSBSerial.h"
#include "SnoozeSPI.h"

#endif /* cplusplus */
#endif /* __HAL_H__ */
#endif /* MK64FX512 */
