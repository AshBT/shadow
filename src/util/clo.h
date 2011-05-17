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

#ifndef _clo_h
#define _clo_h

struct CLO_entry {
	int id;
	char option;
	char fulloption[20];
	char transitive;
	char desc[200];
};

#define CLO_OKAY 1
#define CLO_BAD 0
#define CLO_USAGE 2

int parse_clo(int argc, char * argv[], struct CLO_entry cloentries[], int (*clo_handler)(char*,int,void*), void *v);

#endif
