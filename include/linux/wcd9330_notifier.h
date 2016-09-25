/*
 * include/linux/input/wcd9330_notifier.h
 *
 * Copyright (c) 2016, jollaman999 <admin@jollaman999.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __LINUX_WCD9330_NOTIFIER_H
#define __LINUX_WCD9330_NOTIFIER_H

#include <linux/notifier.h>

#define TOMTOM_EVENT_PLAYING		0x01
#define TOMTOM_EVENT_STOPPED		0x02

int tomtom_register_client(struct notifier_block *nb);
int tomtom_unregister_client(struct notifier_block *nb);
int tomtom_notifier_call_chain(unsigned long val, void *v);

#endif /* __LINUX_WCD9330_NOTIFIER_H */
