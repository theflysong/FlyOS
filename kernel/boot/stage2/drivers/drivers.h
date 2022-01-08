// SPDX-License-Identifier: GPL-2.0-only
/* -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *   Copyright (C) 2021, 2021 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * Author: Flysong
 *
 * kernel/boot/stage2/drivers/drivers.h
 *
 * Drivers were collected here
 */



#pragma once

#include "driver.h"

extern driver_t vedio_driver;
extern driver_t memory_driver;
extern driver_t a_disk_driver;
extern driver_t keyboard_driver;

PUBLIC void init_drivers(void);
PUBLIC bool terminate_drivers(void);