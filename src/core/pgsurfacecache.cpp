/*
    ParaGUI - crossplatform widgetset
    Copyright (C) 2000,2001,2002  Alexander Pipelka
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
 
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
    Alexander Pipelka
    pipelka@teleweb.at
 
    Last Update:      $Author: braindead $
    Update Date:      $Date: 2009/05/06 14:13:59 $
    Source File:      $Source: /sources/paragui/paragui/src/core/pgsurfacecache.cpp,v $
    CVS/RCS Revision: $Revision: 1.2.4.4.2.7 $
    Status:           $State: Exp $
*/

#include "paragui.h"

#include <iostream>
#include <cstring>
#include <string>
#include <cassert>
#include <map>

#include "pgsurfacecache.h"
#include "pglog.h"

#define MY_SURFACEMAP ((pg_surfacemap_t*)my_surfacemap)
#define MY_SURFACEINDEX ((pg_surfacemap_index_t*)my_surfacemap_index)

typedef std::map<std::string, pg_surface_cache_t*> pg_surfacemap_t;
typedef std::map<unsigned long, pg_surface_cache_t*> pg_surfacemap_index_t;

typedef pg_surfacemap_t::iterator pg_surfacemap_iter_t;
typedef pg_surfacemap_index_t::iterator pg_surfacemap_index_iter_t;

PG_SurfaceCache::PG_SurfaceCache() {
	my_surfacemap = (void*)new(pg_surfacemap_t);
	my_surfacemap_index = (void*)new(pg_surfacemap_index_t);
}

PG_SurfaceCache::~PG_SurfaceCache() {
	Cleanup();

	delete MY_SURFACEMAP;
	delete MY_SURFACEINDEX;

	my_surfacemap = NULL;
	my_surfacemap_index = NULL;
}

void PG_SurfaceCache::Cleanup() {

	if(my_surfacemap == NULL) {
		return;
	}

	pg_surfacemap_iter_t i = MY_SURFACEMAP->begin();
	while(i != MY_SURFACEMAP->end()) {
		pg_surface_cache_t* t = (*i).second;
		if(t != NULL) {
			SDL_FreeSurface(t->surface);
			delete t;
		}
		MY_SURFACEMAP->erase(i);
		i = MY_SURFACEMAP->begin();
	}

	MY_SURFACEMAP->clear();
	MY_SURFACEINDEX->clear();
}

void PG_SurfaceCache::CreateKey(std::string &key, Uint16 w, Uint16 h,
                                PG_Gradient* gradient, SDL_Surface* background,
                                PG_Draw::BkMode bkmode, Uint8 blend) {
	char tmpkey[256];
	char colorkey[10];
	int i=0;

	assert(w != 0 && h != 0);

	snprintf(tmpkey, sizeof(tmpkey), "%04x%04x%08lx%01i%01i",
	        w, h,
	        reinterpret_cast<unsigned long>(background),
	        bkmode,
	        blend);

	if(gradient != NULL) {
		for(i=0; i<4; i++) {
			snprintf(colorkey, sizeof(colorkey), "%02x%02x%02x",
			        gradient->colors[i].r,
			        gradient->colors[i].g,
			        gradient->colors[i].b
			       );

			strcat(tmpkey, colorkey);
		}
	}

	key = tmpkey;
}

pg_surface_cache_t* PG_SurfaceCache::FindByKey(const std::string &key) {
	pg_surfacemap_t::iterator i = MY_SURFACEMAP->find(key);
	if(i == MY_SURFACEMAP->end()) {
		return NULL;
	}
	return (*i).second;
	//return (*MY_SURFACEMAP)[key];
}

pg_surface_cache_t* PG_SurfaceCache::FindBySurface(SDL_Surface* surface) {
	return (*MY_SURFACEINDEX)[reinterpret_cast<unsigned long>(surface)];
}

SDL_Surface* PG_SurfaceCache::FindSurface(const std::string &key) {
	pg_surfacemap_t::iterator i = MY_SURFACEMAP->find(key);
	if(i == MY_SURFACEMAP->end()) {
		return NULL;
	}
	return (*i).second->surface;
	/*pg_surface_cache_t* t = (*MY_SURFACEMAP)[key];

	if(t == NULL) {
		return NULL;
	}

	return t->surface;*/
}

SDL_Surface* PG_SurfaceCache::AddSurface(const std::string &key, SDL_Surface* surface) {
	pg_surface_cache_t* t = NULL;

	if(surface == NULL) {
		return NULL;
	}

	// check if surface already exists
	t = FindByKey(key);

	// handle existing surface
	if(t != NULL) {
		PG_LogDBG("Trying to add surface with existing key!");
		// existing surface has a different pointer
		// than new one
		if(t->surface != surface) {
			PG_LogDBG("New and existing surfacepointers are NOT equal !!!");
			// delete new surface (avoid mem-leak)
			SDL_FreeSurface(surface);
		}

		// increase refcount and return cached surface
		t->refcount++;
		return t->surface;
	}

	pg_surface_cache_t* item = new pg_surface_cache_t;
	item->refcount = 1;
	item->surface = surface;
	item->key = key;
	(*MY_SURFACEMAP)[key] = item;
	(*MY_SURFACEINDEX)[reinterpret_cast<unsigned long>(surface)] = item;

	return surface;
}

void PG_SurfaceCache::DeleteSurface(SDL_Surface* surface, bool bDeleteIfNotExists) {

	if(!surface) {
		return;
	}

	pg_surface_cache_t* t = FindBySurface(surface);

	// free unmanaged surface
	if(t == NULL) {
		if(bDeleteIfNotExists) {
			SDL_FreeSurface(surface);
		}
		return;
	}

	// dec reference
	t->refcount--;

	// no more references ?
	if(t->refcount > 0) {
		return;
	}

	MY_SURFACEMAP->erase(t->key);
	MY_SURFACEINDEX->erase(reinterpret_cast<unsigned long>(surface));

	SDL_FreeSurface(t->surface);
	delete t;
}

void PG_SurfaceCache::IncRef(const std::string &key) {
	pg_surface_cache_t* t = FindByKey(key);

	if(t == NULL) {
		return;
	}

	t->refcount++;
}


/*
 * Local Variables:
 * c-basic-offset: 8
 * End:
 */
