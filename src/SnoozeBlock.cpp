#include "SnoozeBlock.h"

SnoozeBlock * SnoozeBlock::root_block[] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
SnoozeBlock * SnoozeBlock::root_class_address[] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
uint8_t SnoozeBlock::global_block_count = 0;
uint8_t SnoozeBlock::current_block = 0;
uint8_t SnoozeBlock::sleepType = 0;
