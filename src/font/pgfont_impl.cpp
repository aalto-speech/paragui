#include "pglog.h"
#include "facecache.h"
#include "pgfont.h"

struct PG_FontDataInternal {
	SDL_Color color;
	int alpha;
	int style;

	int size;
	int index;
	std::string name;

	Uint32 dummy1;
	Uint32 dummy2;

	PG_FontFaceCacheItem* FaceCache;

};

PG_Font::PG_Font(const char* fontfile, int size, int index) {
	my_internaldata = new PG_FontDataInternal;
	my_internaldata->FaceCache = NULL;

	my_internaldata->name = fontfile;
	my_internaldata->size = size;
	my_internaldata->index = index;
	my_internaldata->color.r = 255;
	my_internaldata->color.g = 255;
	my_internaldata->color.b = 255;
	my_internaldata->alpha = 255;
	my_internaldata->style = 0;

	my_internaldata->FaceCache = PG_FontEngine::LoadFontFace(fontfile, size, index);
	
	if(my_internaldata->FaceCache == NULL) {
		PG_LogERR("Unable to create font (name=\"%s\", size=\"%i\", index=\"%i\"", fontfile, size, index);
	}
}

PG_Font::~PG_Font() {
	delete my_internaldata;
}

int PG_Font::GetFontAscender() {
	return my_internaldata->FaceCache ?
		FT_CEIL(FT_MulFix(my_internaldata->FaceCache->Face->ascender, my_internaldata->FaceCache->Face->size->metrics.y_scale)) : 0;
}

int PG_Font::GetFontDescender() {
	return my_internaldata->FaceCache ?
		FT_CEIL(FT_MulFix(my_internaldata->FaceCache->Face->descender, my_internaldata->FaceCache->Face->size->metrics.y_scale)) : 0;
}

int PG_Font::GetFontHeight() {
	return my_internaldata->FaceCache ?
		FT_CEIL(FT_MulFix(my_internaldata->FaceCache->Face->height, my_internaldata->FaceCache->Face->size->metrics.y_scale)) : 0;
}

void PG_Font::SetColor(const SDL_Color& c) {
	my_internaldata->color = c;
}

SDL_Color PG_Font::GetColor() {
	return my_internaldata->color;
}
	
void PG_Font::SetAlpha(int a) {
	my_internaldata->alpha = a;
}

int PG_Font::GetAlpha() {
	return my_internaldata->alpha;
}
	
void PG_Font::SetStyle(int s) {
	my_internaldata->style = s;
}

int PG_Font::GetStyle() {
	return my_internaldata->style;
}
	
bool PG_Font::SetName(const char* fontfile) {
	my_internaldata->name = fontfile;
	my_internaldata->FaceCache = PG_FontEngine::LoadFontFace(
			fontfile,
			GetSize(),
			GetIndex());
	
	if(my_internaldata->FaceCache == NULL) {
		PG_LogERR("Unable to create font (name=\"%s\", size=\"%i\", index=\"%i\"",
			GetName(),
			GetSize(),
			GetIndex());
	}

	return (my_internaldata->FaceCache != NULL);
}

const char* PG_Font::GetName() {
	return my_internaldata->name.c_str();
}

void PG_Font::SetSize(int s) {
	my_internaldata->size = s;
	my_internaldata->FaceCache = PG_FontEngine::LoadFontFace(
			GetName(),
			GetSize(),
			GetIndex());
	
	
	if(my_internaldata->FaceCache == NULL) {
		PG_LogERR("Unable to create font (name=\"%s\", size=\"%i\", index=\"%i\"",
			GetName(),
			GetSize(),
			GetIndex());
	}

}

int PG_Font::GetSize() {
	return my_internaldata->size;
}

PG_FontFaceCacheItem* PG_Font::GetFaceCache(int index) {
	return my_internaldata->FaceCache;
}

void PG_Font::SetIndex(int index) {
	my_internaldata->index = index;
}

int PG_Font::GetIndex() {
	return my_internaldata->index;
}

/*void PG_Font::SetFaceCache(PG_FontFaceCacheItem* cache, int index) {
	my_internaldata->FaceCache = cache;
}*/
