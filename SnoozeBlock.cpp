/***********************************************************************************
 * Low Power Library for Teensy LC/3.x
 * Copyright (c) 2014, Colin Duffy https://github.com/duff2013
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
 ************************************************************************************
 *  SnoozeBlock.h
 *  Teensy 3.x/LC
 *
 ***********************************************************************************/
#include "SnoozeBlock.h"

SnoozeBlock * SnoozeBlock::root_block[] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
SnoozeBlock * SnoozeBlock::root_class_address[] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
uint8_t SnoozeBlock::global_block_count = 0;
uint8_t SnoozeBlock::current_block = 0;
DMAMEM int SnoozeBlock::source = -1;
DMAMEM SLEEP_MODE SnoozeBlock::mode = RUN;
