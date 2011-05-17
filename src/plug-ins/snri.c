/**
 * The Shadow Simulator
 *
 * Copyright (c) 2010-2011 Rob Jansen <jansen@cs.umn.edu>
 * Copyright (c) 2006-2009 Tyson Malchow <tyson.malchow@gmail.com>
 *
 * This file is part of Shadow.
 *
 * Shadow is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Shadow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shadow.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "log_codes.h"
#include "snricall_codes.h"
#include "snri.h"

int (*_snricall_fpref)(int, ...);

int snri_gettime(struct timeval *t) {
	return _snricall(SNRICALL_GETTIME, t);
}

int snri_timer_create(int milli_delay, snri_timer_callback_fp callback_function, void * cb_arg) {
	int timer_id;
	if(_snricall(SNRICALL_CREATE_TIMER, milli_delay, callback_function, cb_arg, &timer_id) == SNRICALL_SUCCESS)
		return timer_id;
	else
		return SNRICALL_ERROR;
}

int snri_timer_destroy(int timer_id) {
	return _snricall(SNRICALL_DESTROY_TIMER, timer_id);
}

int snri_exit() {
	return _snricall(SNRICALL_EXIT);
}

int snri_log_binary(int level, char * data, int data_size) {
	return _snricall(SNRICALL_LOG_BINARY, level, data, data_size);
}

int snri_resolve_name(char* name, in_addr_t* addr_out) {
	return _snricall(SNRICALL_RESOLVE_NAME, name, addr_out);
}

int snri_resolve_addr(in_addr_t addr, char* name_out, int name_out_len) {
	return _snricall(SNRICALL_RESOLVE_ADDR, addr, name_out, name_out_len);
}

int snri_resolve_minbw(in_addr_t addr, unsigned int* bw_KBps_out) {
	return _snricall(SNRICALL_RESOLVE_BW, addr, bw_KBps_out);
}

int snri_getip(in_addr_t* addr_out) {
	return _snricall(SNRICALL_GETIP, addr_out);
}

int snri_gethostname(char* name_out, int name_out_len) {
	in_addr_t ip;
	if(snri_getip(&ip) == SNRICALL_ERROR) {
		return SNRICALL_ERROR;
	} else {
		return snri_resolve_addr(ip, name_out, name_out_len);
	}
}

int snri_socket_is_readable(int sockd) {
	int bool = 0;
	if(_snricall(SNRICALL_SOCKET_IS_READABLE, sockd, &bool) == SNRICALL_ERROR) {
		return SNRICALL_ERROR;
	} else {
		return bool;
	}
}

int snri_socket_is_writable(int sockd) {
	int bool = 0;
	if(_snricall(SNRICALL_SOCKET_IS_WRITABLE, sockd, &bool) == SNRICALL_ERROR) {
		return SNRICALL_ERROR;
	} else {
		return bool;
	}
}

int snri_set_loopexit_fn(snri_timer_callback_fp fn) {
	return _snricall(SNRICALL_SET_LOOPEXIT_FN, fn);
}
