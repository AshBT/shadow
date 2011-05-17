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

#include <string.h>
#include <stdlib.h>

#include "global.h"
#include "hashtable.h"
#include "hash.h"

hashtable_tp hashtable_create(unsigned int buckets, float growth_factor) {
	hashtable_tp ht = malloc(sizeof(*ht));

	if(!ht)
		printfault(EXIT_NOMEM,"Out of memory: hashtable_create");

	ht->population = 0;
	ht->growth_factor = growth_factor;
	ht->num_buckets = buckets == 0 ? 8 : buckets;
	ht->buckets = malloc(sizeof(*ht->buckets)*ht->num_buckets);

	if(!ht->buckets)
		printfault(EXIT_NOMEM,"Out of memory: hashtable_create");

	memset(ht->buckets, 0, sizeof(*ht->buckets)*ht->num_buckets);

	return ht;
}

void hashtable_destroy(hashtable_tp ht) {
	int i;
	for(i=0; i<ht->num_buckets; i++) {
		if(ht->buckets[i].btree) {
			btree_destroy(ht->buckets[i].btree);
			ht->buckets[i].btree = NULL;
		}
	}

	free(ht->buckets);
	free(ht);
}

/*
void hashtable_set(hashtable_tp ht, char * key, void * value) {
	hashtable_set(ht, adler32(key), value);
}*/

void hashtable_set(hashtable_tp ht, unsigned int key, void * value) {
	if(ht == NULL || ht->num_buckets == 0)
		return;
	hashtable_bucket_tp bucket = &ht->buckets[hashtable_hashfunction(key) % ht->num_buckets];

	if(value == NULL)
		hashtable_remove(ht, key);
	else {
		if(!bucket->btree) {
			if(!bucket->single_data) {
				bucket->single_key = key;
				bucket->single_data = value;
			} else {
				bucket->btree = btree_create(2);
				btree_insert(bucket->btree, bucket->single_key, bucket->single_data);
				btree_insert(bucket->btree, key, value);
			}

		} else
			btree_insert(bucket->btree, key, value);
		ht->population++;

		if(ht->population >= (ht->num_buckets * ht->growth_factor))
			/* we're running out of space. LETS GROW! */
			hashtable_rehash(ht,ht->num_buckets * 2);
	}
}

void hashtable_rehash(hashtable_tp ht, int newsize) {
	int i,j,tmpkey;
	void * tmpdata;
	hashtable_t tmpht;

	tmpht.population = 0;
	tmpht.num_buckets = newsize;
	tmpht.buckets = malloc(sizeof(*ht->buckets)*newsize);
	memset(tmpht.buckets, 0, sizeof(*ht->buckets)*newsize);
	tmpht.growth_factor = 1.0f;

	if(!tmpht.buckets)
		printfault(EXIT_NOMEM, "Out of memory: hashtable_rehash %d\n", sizeof(*ht->buckets)*newsize);

	for(i=0; i<ht->num_buckets; i++) {
		if(ht->buckets[i].btree) {
			for(j = 0; j<btree_get_size(ht->buckets[i].btree); j++) {
				tmpdata = btree_get_index(ht->buckets[i].btree, j, &tmpkey);
				hashtable_set(&tmpht, tmpkey, tmpdata);
			}
			btree_destroy(ht->buckets[i].btree);
			ht->buckets[i].btree = NULL;
		} else if(ht->buckets[i].single_data)
			hashtable_set(&tmpht, ht->buckets[i].single_key, ht->buckets[i].single_data);
	}

	free(ht->buckets);
	ht->buckets = tmpht.buckets;
	ht->num_buckets = tmpht.num_buckets;
	ht->population = tmpht.population;

	return ;
}

void hashtable_walk(hashtable_tp ht, hashtable_walk_callback_tp cb) {
	int i;
	for(i=0; i<ht->num_buckets; i++) {
		if(ht->buckets[i].btree)
			btree_walk(ht->buckets[i].btree, cb);
		else if(ht->buckets[i].single_data)
			(*cb)(ht->buckets[i].single_data, ht->buckets[i].single_key);
	}
}

void hashtable_walk_param(hashtable_tp ht, hashtable_walk_param_callback_tp cb, void* param) {
	int i;
	for(i=0; i<ht->num_buckets; i++) {
		if(ht->buckets[i].btree)
			btree_walk_param(ht->buckets[i].btree, cb, param);
		else if(ht->buckets[i].single_data)
			(*cb)(ht->buckets[i].single_data, ht->buckets[i].single_key, param);
	}
}

void * hashtable_get(hashtable_tp ht, unsigned int key) {
	if(ht == NULL || ht->num_buckets == 0)
		return NULL;

	hashtable_bucket_tp bucket = &ht->buckets[hashtable_hashfunction(key) % ht->num_buckets];

	if(!bucket)
		return NULL;

	if(!bucket->btree) {
		if(bucket->single_key == key)
			return bucket->single_data;
		else
			return NULL;
	} else
		return btree_get(bucket->btree, key);
}

void * hashtable_remove(hashtable_tp ht, unsigned int key) {
	if(ht == NULL || ht->num_buckets == 0)
		return NULL;

	hashtable_bucket_tp bucket = &ht->buckets[hashtable_hashfunction(key) % ht->num_buckets];

	if(!bucket)
		return NULL;

	void * r = NULL;
	if(!bucket->btree && bucket->single_key == key) {
		r = bucket->single_data;
		bucket->single_data = NULL;
	} else if(bucket->btree) {
		r = btree_remove(bucket->btree, key);
		if(btree_get_size(bucket->btree) < 2) {
			bucket->single_data = btree_get_head(bucket->btree, &bucket->single_key);
			btree_destroy(bucket->btree);
			bucket->btree = NULL;
		}
	}

	if(r) {
		ht->population--;

		if(ht->population < ht->growth_factor * ht->num_buckets / 2)
			hashtable_rehash(ht, ht->num_buckets / 2);
	}

	return r;
}
