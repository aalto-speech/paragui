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
   Update Date:      $Date: 2004/02/21 13:58:06 $
   Source File:      $Source: /sources/paragui/paragui/src/widgets/pgwidget.cpp,v $
   CVS/RCS Revision: $Revision: 1.4.4.22.2.8 $
   Status:           $State: Exp $
 */

#include <cstring>
#include <stdarg.h>

#include "pgwidget.h"
#include "pgapplication.h"
#include "pglog.h"
#include "pgdraw.h"
#include "pglayout.h"
#include "pgtheme.h"

/**
	calculate the minimum of 2 values
*/
#define PG_MAX(a, b)	((a<b) ? b : a)

/**
	calculate the maximum of 2 values
*/
#define PG_MIN(a, b)	((a<b) ? a : b)


bool PG_Widget::bBulkUpdate = false;
PG_RectList PG_Widget::widgetList;
int PG_Widget::my_ObjectCounter = 0;

class PG_WidgetDataInternal {
public:
	PG_WidgetDataInternal() {};

	PG_Font* font;

	bool quitModalLoop;
	bool dirtyUpdate;
	
	PG_Widget* widgetParent;
	PG_RectList* childList;

	char* userdata;
	int userdatasize;

	PG_Point ptDragStart;
	PG_Rect rectClip;

	int id;
	bool mouseInside;
	int fadeSteps;
	bool haveTooltip;
	bool visible;
	bool firstredraw;
	Uint8 transparency;
	bool havesurface;

	Uint16 widthText;
	Uint16 heightText;

	bool inDestruct;
	string name;
	
	bool hidden;
};

#define TXT_HEIGHT_UNDEF 0xFFFF

PG_Widget::PG_Widget(PG_Widget* parent, const PG_Rect& rect) :
	PG_Rect(rect),
	my_srfObject(NULL)
{
	InitWidget(parent, false);
}

PG_Widget::PG_Widget(PG_Widget* parent, const PG_Rect& rect, bool bObjectSurface) :
	PG_Rect(rect),
	my_srfObject(NULL)
{
	InitWidget(parent, bObjectSurface);
}

void PG_Widget::InitWidget(PG_Widget* parent, bool bObjectSurface) {

	my_internaldata = new PG_WidgetDataInternal;
	
	my_internaldata->inDestruct = false;
	my_internaldata->font = NULL;
	my_internaldata->dirtyUpdate = false;
	my_internaldata->widgetParent = parent;
	my_internaldata->id = -1;
	my_internaldata->transparency = 0;
	my_internaldata->quitModalLoop = false;
	my_internaldata->visible = false;
	my_internaldata->hidden = false;
	my_internaldata->firstredraw = true;
	my_internaldata->childList = NULL;
	my_internaldata->haveTooltip = false;
	my_internaldata->fadeSteps = 10;
	my_internaldata->mouseInside = false;
	my_internaldata->userdata = NULL;
	my_internaldata->userdatasize = 0;
	my_internaldata->widthText = TXT_HEIGHT_UNDEF;
	my_internaldata->heightText = TXT_HEIGHT_UNDEF;

	my_internaldata->havesurface = bObjectSurface;

	//Set default font
	if(PG_Application::DefaultFont != NULL) {
		my_internaldata->font = new PG_Font(
					PG_Application::DefaultFont->GetName(),
					PG_Application::DefaultFont->GetSize());
	}
	else {
		PG_LogWRN("Unable to get default font! Did you load a theme ?");
	}

	my_srfScreen = PG_Application::GetScreen();

	if(my_internaldata->havesurface) {
		my_srfObject = PG_Draw::CreateRGBSurface(w, h);
	}

	// ??? - How can i do this better - ???
	char buffer[15];
	sprintf(buffer, "Object%d", ++my_ObjectCounter);
	my_internaldata->name = buffer;

	// default border colors
	my_colorBorder[0][0].r = 255;
	my_colorBorder[0][0].g = 255;
	my_colorBorder[0][0].b = 255;

	my_colorBorder[0][1].r = 239;
	my_colorBorder[0][1].g = 239;
	my_colorBorder[0][1].b = 239;

	my_colorBorder[1][0].r = 89;
	my_colorBorder[1][0].g = 89;
	my_colorBorder[1][0].b = 89;

	my_colorBorder[1][1].r = 134;
	my_colorBorder[1][1].g = 134;
	my_colorBorder[1][1].b = 134;

	my_text = "";

	if (my_internaldata->widgetParent != NULL) {
		my_xpos = my_internaldata->widgetParent->my_xpos + my_xpos;
		my_ypos = my_internaldata->widgetParent->my_ypos + my_ypos;
		my_internaldata->widgetParent->AddChild(this);
	}

	my_internaldata->rectClip = *this;

	AddToWidgetList();
}

void PG_Widget::RemoveAllChilds() {

	// remove all child widgets
	if(my_internaldata->childList != NULL) {

		PG_Rect* i = static_cast<PG_Widget*>(my_internaldata->childList->first());
		while(i != NULL) {
			PG_Widget* w = static_cast<PG_Widget*>(i);
			i = i->next;

			RemoveChild(w);
			delete w;
		}
		my_internaldata->childList->clear();
	}

}

PG_Widget::~PG_Widget() {

	my_internaldata->inDestruct = true;

	if(!my_internaldata->havesurface && my_srfObject) {
		PG_LogWRN("DrawObject declared without a surface has unexpectedly born one ?");
	}
	PG_Application::UnloadSurface(my_srfObject);
	my_srfObject = NULL;

	Hide();

	RemoveAllChilds();

	// remove myself from my parent's childlist (if any parent)

	if (GetParent() != NULL) {
		GetParent()->RemoveChild(this);
	}
    
	RemoveFromWidgetList();

	// remove childlist
	delete my_internaldata->childList;
	my_internaldata->childList = NULL;

	if (my_internaldata->userdata != NULL) {
		delete[] my_internaldata->userdata;
	}
	
	// remove the font
	delete my_internaldata->font;
	
	// remove my private data
	delete my_internaldata;

	//cout << "Removed widget '" << GetName() << "'" << endl;
}

void PG_Widget::RemoveFromWidgetList() {
	widgetList.Remove(this);
}

void PG_Widget::AddToWidgetList() {
	if(!GetParent()) {
		widgetList.Add(this);
	}
}

/** Check if we can accept the event */

bool PG_Widget::AcceptEvent(const SDL_Event * event) {

	if (!IsVisible() || IsHidden()) {
		return false;
	}

	switch (event->type) {
		case SDL_MOUSEMOTION:
			if ((event->motion.x < my_internaldata->rectClip.my_xpos) ||
				(event->motion.x > (my_internaldata->rectClip.my_xpos + my_internaldata->rectClip.my_width - 1))) {
				if (my_internaldata->mouseInside) {
					my_internaldata->mouseInside = false;
					eventMouseLeave();
				}
				return false;
			}
			if ((event->motion.y < my_internaldata->rectClip.my_ypos) ||
				(event->motion.y > (my_internaldata->rectClip.my_ypos + my_internaldata->rectClip.my_height - 1))) {
				if (my_internaldata->mouseInside) {
					my_internaldata->mouseInside = false;
					eventMouseLeave();
				}
				return false;
			}
			if (!my_internaldata->mouseInside) {
				my_internaldata->mouseInside = true;
				eventMouseEnter();
				return true;
			}
			break;

		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
			if ((event->button.x < my_internaldata->rectClip.my_xpos) ||
				(event->button.x > (my_internaldata->rectClip.my_xpos + my_internaldata->rectClip.my_width - 1)))
				return false;

			if ((event->button.y < my_internaldata->rectClip.my_ypos) ||
				(event->button.y > (my_internaldata->rectClip.my_ypos + my_internaldata->rectClip.my_height - 1)))
				return false;

			break;
	}

	return true;		// accept the event as default
}


/**  */
void PG_Widget::eventMouseEnter() {}


/**  */
void PG_Widget::eventMouseLeave() {
	my_internaldata->mouseInside = false;

	if(GetParent()) {
		GetParent()->eventMouseLeave();
	}
}

/**  */
void PG_Widget::eventShow() {}

/**  */
void PG_Widget::eventHide() {}

/**  */
PG_Point PG_Widget::ClientToScreen(int sx, int sy) {
	return PG_Point(sx + my_xpos, sy + my_ypos);
}

PG_Point PG_Widget::ScreenToClient(int x, int y) {
	return PG_Point(x - my_xpos, y - my_ypos);
}

void PG_Widget::AddChild(PG_Widget * child) {

    if (!child)
        return;
    
	// remove our new child from previous lists
	if(child->GetParent()) {
		child->GetParent()->RemoveChild(child);
	}
	else {
		child->RemoveFromWidgetList();
	}

	child->my_internaldata->widgetParent = this;

	if (my_internaldata->childList == NULL) {
		my_internaldata->childList = new PG_RectList;
	}

	my_internaldata->childList->Add(child);
}

bool PG_Widget::MoveWidget(int x, int y) {

	if (GetParent() != NULL) {
		x += GetParent()->my_xpos;
		y += GetParent()->my_ypos;
	}
	if(x == my_xpos && y == my_ypos) {
		// Optimization: We haven't moved, so do nothing.
		return false;
	}

	if(!IsVisible()) {
		MoveRect(x, y);
		return true;
	}

	// delta x,y
	int dx = x - my_xpos;
	int dy = y - my_ypos;

	// calculate vertical update rect

	PG_Rect vertical(0, 0, abs(dx), my_height + abs(dy));

	if(dx >= 0) {
		vertical.my_xpos = my_xpos;
	} else {
		vertical.my_xpos = my_xpos + my_width + dx;
	}

	vertical.my_ypos = my_ypos;

	// calculate vertical update rect

	PG_Rect horizontal(0, 0, my_width + abs(dx), abs(dy));

	horizontal.my_xpos = my_xpos;

	if(dy >= 0) {
		horizontal.my_ypos = my_ypos;
	} else {
		horizontal.my_ypos = my_ypos + my_height + dy;
	}

	// move rectangle and store new background
	MoveRect(x, y);

	if(vertical.my_xpos + vertical.my_width > my_srfScreen->w) {
		vertical.my_width = my_srfScreen->w - vertical.my_xpos;
	}
	if(vertical.my_ypos + vertical.my_height > my_srfScreen->h) {
		vertical.my_height = my_srfScreen->h - vertical.my_ypos;
	}

	if(horizontal.my_xpos + horizontal.my_width > my_srfScreen->w) {
		horizontal.my_width = my_srfScreen->w- horizontal.my_xpos;
	}
	if(horizontal.my_ypos + horizontal.my_height > my_srfScreen->h) {
		horizontal.my_height = my_srfScreen->h - horizontal.my_ypos;
	}

	if(!PG_Application::GetBulkMode()) {
		UpdateRect(vertical);
		UpdateRect(horizontal);
		UpdateRect(my_internaldata->rectClip);
		PG_Application::LockScreen();
		SDL_Rect rects[3] = {my_internaldata->rectClip, vertical, horizontal};
		SDL_UpdateRects(my_srfScreen, 3, rects);
		PG_Application::UnlockScreen();
	}

	return true;
}

bool PG_Widget::MoveWidget(const PG_Rect& r) {
	MoveWidget(r.x, r.y);
	SizeWidget(r.w, r.h);
	return true;
}

bool PG_Widget::SizeWidget(Uint16 w, Uint16 h) {
	bool v = IsVisible();

	if (v) {
		SetVisible(false);
	}

	if (my_internaldata->firstredraw != true) {
		RestoreBackground();
	}

	// create new widget drawsurface
	if(my_srfObject) {
		PG_Application::UnloadSurface(my_srfObject);

		if(w > 0 && h > 0) {
			my_srfObject = PG_Draw::CreateRGBSurface(w, h);
		}
		else {
			my_srfObject = NULL;
		}
	}

	eventSizeWindow(w, h);
	eventSizeWidget(w, h);

	my_width = w;
	my_height = h;

	if (v) {
		SetVisible(true);
	}
	return true;
}
/**  */
bool PG_Widget::ProcessEvent(const SDL_Event * event, bool bModal) {

	bool processed = false;
	// do i have a capturehook set ? (modal)
	if(bModal) {
		// i will send that event to my children

		if(my_internaldata->childList != NULL) {
			PG_Widget* list = static_cast<PG_Widget*>(my_internaldata->childList->first());

			while (!processed && (list != NULL)) {
				processed = list->ProcessEvent(event, true);
				list = static_cast<PG_Widget*>(list->next);
			}
		}

		if(processed) {
			return processed;
		}
    }

	// let me see if i can process it myself

	if(PG_MessageObject::ProcessEvent(event)) {
		return true;
	}

	if(bModal) {
		return processed;
	}

	// ask my parent to process the event

	if(GetParent()) {
		if(GetParent()->ProcessEvent(event)) {
			return true;
		}
	}

	return false;
}

bool PG_Widget::RemoveChild(PG_Widget * child) {
	if(my_internaldata->childList == NULL || child == NULL) {
		return false;
	}

	return my_internaldata->childList->Remove(child);
}

bool PG_Widget::IsMouseInside() {
	PG_Point p;
	int x, y;
	
	SDL_GetMouseState(&x, &y);
	p.x = static_cast<Sint16>(x);
	p.y = static_cast<Sint16>(y);
	my_internaldata->mouseInside = IsInside(p);

	return my_internaldata->mouseInside;
}

/**  */
bool PG_Widget::Redraw(bool update) {
	PG_Rect r(0, 0, my_width, my_height);

	if(my_srfObject != NULL) {
		eventDraw(my_srfObject, r);
	}

	if(my_internaldata->childList != NULL) {
		for(PG_Widget* i = static_cast<PG_Widget*>(my_internaldata->childList->first()); i != NULL; i = static_cast<PG_Widget*>(i->next)) {
			i->Redraw(false);
		}
	}

	if (update) {
		Update();
	}
	return true;
}

void PG_Widget::SetVisible(bool visible) {

	if(IsHidden()) {
		return;
	}
	
	// Attempt to make object visible
	if(visible) {
		if(my_internaldata->visible) {			// Object already visible
			return;
		} else {					// Display object
			my_internaldata->visible = visible;
			if(my_internaldata->firstredraw) {
				Redraw(false);
				my_internaldata->firstredraw = false;
			}
		}

	}

	// Attempt to make object invisible
	if(!visible) {
		if(!my_internaldata->visible) {			// Object is already invisible
			return;
		} else {					// Hide object
			RestoreBackground();
			my_internaldata->visible = visible;
		}
	}

	if(my_internaldata->childList != NULL) {
		for(PG_Widget* i = static_cast<PG_Widget*>(my_internaldata->childList->first()); i != NULL; i = static_cast<PG_Widget*>(i->next)) {
			i->SetVisible(visible);
			if(!i->IsHidden()) {
				if(visible) {
					i->eventShow();
				} else {
					i->eventHide();
				}
			}
		}
	}
}

/**  */
void PG_Widget::Show(bool fade) {

	if(GetParent() == NULL) {
		widgetList.BringToFront(this);
	}
	else {
		GetParent()->GetChildList()->BringToFront(this);
	}

	SetHidden(false);
	SetVisible(true);
	
	eventShow();

	PG_Widget* parent = GetParent();
	if(parent != NULL && !parent->IsVisible()) {
		return;
	}
	
	if (fade) {
		FadeIn();
	}

	if(IsMouseInside()) {
		eventMouseEnter();
	}

	SDL_SetClipRect(my_srfScreen, NULL);
	Update();

	return;
}

/**  */
void PG_Widget::Hide(bool fade) {

	if(!IsVisible()) {
		SetHidden(true);
		eventHide();
		return;
	}

	RecalcClipRect();

	if(!my_internaldata->inDestruct) {
		eventMouseLeave();
	}

	if (fade) {
		FadeOut();
	}

	SetVisible(false);
	eventHide();

	ReleaseCapture();
	ReleaseInputFocus();

	SDL_SetClipRect(my_srfScreen, NULL);

	if(!PG_Application::GetBulkMode()) {
		//RestoreBackground();
		UpdateRect(my_internaldata->rectClip);
	}

	if(!PG_Application::GetBulkMode()) {
		PG_Application::LockScreen();
		SDL_UpdateRects(my_srfScreen, 1, &my_internaldata->rectClip);
		PG_Application::UnlockScreen();
	}

	SetHidden(true);

	return;
}

/**  */
void PG_Widget::MoveRect(int x, int y) {
	int dx = x - my_xpos;
	int dy = y - my_ypos;

	// recalc cliprect
	RecalcClipRect();

	my_xpos = x;
	my_ypos = y;
	my_internaldata->rectClip.my_xpos += dx;
	my_internaldata->rectClip.my_ypos += dy;

	// recalc cliprect
	RecalcClipRect();

	if(my_internaldata->childList != NULL) {
		for(PG_Widget* i = static_cast<PG_Widget*>(my_internaldata->childList->first()); i != NULL; i = static_cast<PG_Widget*>(i->next)) {
			i->MoveRect(i->my_xpos + dx, i->my_ypos + dy);
		}
	}

	eventMoveWindow(x, y);
	eventMoveWidget(x, y);
}

void PG_Widget::Blit(bool recursive, bool restore) {
	static PG_Rect src;
	static PG_Rect dst;
	
	if(!my_internaldata->visible || my_internaldata->hidden) {
		return;
	}

	// recalc clipping rectangle
	RecalcClipRect();

	// don't draw a null rect
	if(my_internaldata->rectClip.w == 0 || my_internaldata->rectClip.h == 0) {
		return;
	}

	PG_Application::LockScreen();

	// restore the background
	if(restore) {
		RestoreBackground(&my_internaldata->rectClip);
	}

	// get source & destination rectangles
	src.SetRect(my_internaldata->rectClip.x - my_xpos, my_internaldata->rectClip.y - my_ypos, my_internaldata->rectClip.w, my_internaldata->rectClip.h);
	dst = my_internaldata->rectClip;

	// call the blit handler
	eventBlit(my_srfObject, src, dst);

	// should we draw our children
	if(recursive) {
		// draw the children-list
		if(my_internaldata->childList != NULL) {
			my_internaldata->childList->Blit(my_internaldata->rectClip);
		}
	}
	
	PG_Application::UnlockScreen();
}

/**  */
void PG_Widget::Update(bool doBlit) {
	static PG_Rect src;
	static PG_Rect dst;
	
	if(PG_Application::GetBulkMode()) {
		return;
	}

	if(!my_internaldata->visible || my_internaldata->hidden) {
		return;
	}

	// recalc cliprect
	RecalcClipRect();

	if(my_internaldata->rectClip.w == 0 || my_internaldata->rectClip.h == 0) {
		return;
	}

	PG_Application::LockScreen();

	// BLIT
	if(doBlit) {

		SDL_SetClipRect(my_srfScreen, &my_internaldata->rectClip);
		RestoreBackground(&my_internaldata->rectClip);

		src.SetRect(my_internaldata->rectClip.x - my_xpos, my_internaldata->rectClip.y - my_ypos, my_internaldata->rectClip.w, my_internaldata->rectClip.h);
		dst = my_internaldata->rectClip;

		eventBlit(my_srfObject, src, dst);

		if(my_internaldata->childList != NULL) {
			my_internaldata->childList->Blit(my_internaldata->rectClip);
		}

		// check if other children of my parent overlap myself
		if(GetParent() != NULL) {
			PG_RectList* children = GetParent()->GetChildList();
			if(children) {
				children->Blit(my_internaldata->rectClip, this->next);
			}
		}

		// find the toplevel widget
		PG_Widget* obj = GetToplevelWidget();
		widgetList.Blit(my_internaldata->rectClip, obj->next);

	}

	// Update screen surface
#ifdef DEBUG
	PG_LogDBG("UPD: x:%d y:%d w:%d h:%d",dst.x,dst.y,dst.w,dst.h);
#endif // DEBUG
	
	SDL_UpdateRects(my_srfScreen, 1, &my_internaldata->rectClip);

	SDL_SetClipRect(my_srfScreen, NULL);
	PG_Application::UnlockScreen();
}

/**  */
void PG_Widget::SetChildTransparency(Uint8 t) {
	if(my_internaldata->childList == NULL) {
		return;
	}

	for(PG_Widget* i = static_cast<PG_Widget*>(my_internaldata->childList->first()); i != NULL; i = static_cast<PG_Widget*>(i->next)) {
		i->SetTransparency(t);
	}
	Update();
}

void PG_Widget::StartWidgetDrag() {
	int x, y;
	
	SDL_GetMouseState(&x, &y);
	my_internaldata->ptDragStart.x = static_cast<Sint16>(x);
	my_internaldata->ptDragStart.y = static_cast<Sint16>(y);
	
	my_internaldata->ptDragStart.x -= my_xpos;
	my_internaldata->ptDragStart.y -= my_ypos;
}

void PG_Widget::WidgetDrag(int x, int y) {

	x -= my_internaldata->ptDragStart.x;
	y -= my_internaldata->ptDragStart.y;

	if(x < 0)
		x=0;
	if(y < 0)
		y=0;
	if(x > (my_srfScreen->w - my_width -1))
		x = (my_srfScreen->w - my_width -1);
	if(y > (my_srfScreen->h - my_height -1))
		y = (my_srfScreen->h - my_height -1);

	MoveWidget(x,y);
}

void PG_Widget::EndWidgetDrag(int x, int y) {
	WidgetDrag(x,y);
	my_internaldata->ptDragStart.x = 0;
	my_internaldata->ptDragStart.y = 0;
}

void PG_Widget::HideAll() {
	for(PG_Widget* i = static_cast<PG_Widget*>(widgetList.first()); i != NULL; i = static_cast<PG_Widget*>(i->next)) {
		i->Hide();
	}
}

void PG_Widget::BulkUpdate() {
	bBulkUpdate = true;

	for(PG_Widget* i = static_cast<PG_Widget*>(widgetList.first()); i != NULL; i = static_cast<PG_Widget*>(i->next)) {
		if(i->IsVisible()) {
			i->Update();
		}
	}

	bBulkUpdate = false;
}

void PG_Widget::BulkBlit() {
	bBulkUpdate = true;
	widgetList.Blit();
	PG_Application::DrawCursor();
	bBulkUpdate = false;
}

void PG_Widget::LoadThemeStyle(const char* widgettype, const char* objectname) {
	PG_Theme* t = PG_Application::GetTheme();
	PG_Color c;

	const char *font = t->FindFontName(widgettype, objectname);
	int fontsize = t->FindFontSize(widgettype, objectname);
	PG_Font::Style fontstyle = t->FindFontStyle(widgettype, objectname);

	if(font != NULL)
		SetFontName(font, true);

	if (fontsize > 0)
		SetFontSize(fontsize, true);

	if (fontstyle >= 0)
		SetFontStyle(fontstyle, true);

	c = GetFontColor();
	t->GetColor(widgettype, objectname, "textcolor", c);
	SetFontColor(c);

	t->GetColor(widgettype, objectname, "bordercolor0", my_colorBorder[0][0]);
	t->GetColor(widgettype, objectname, "bordercolor1", my_colorBorder[1][0]);
}

void PG_Widget::LoadThemeStyle(const char* widgettype) {}

void PG_Widget::FadeOut() {
	PG_Rect r(0, 0, my_width, my_height);

	// blit the widget to screen (invisible)
	Blit();

	// create a temp surface
	SDL_Surface* srfFade = PG_Draw::CreateRGBSurface(my_width, my_height);

	// blit the widget to temp surface
	PG_Draw::BlitSurface(my_srfScreen, *this, srfFade, r);

	int d = (255-my_internaldata->transparency)/ my_internaldata->fadeSteps;
	if(!d) {
		d = 1;
	} // minimum step == 1
	
	PG_Application::LockScreen();
	
	for(int i=my_internaldata->transparency; i<255; i += d) {
		RestoreBackground(NULL, true);
		SDL_SetAlpha(srfFade, SDL_SRCALPHA, 255-i);
		SDL_BlitSurface(srfFade, NULL, my_srfScreen, this);
		SDL_UpdateRects(my_srfScreen, 1, &my_internaldata->rectClip);
	}

	RestoreBackground(NULL, true);
	SDL_SetAlpha(srfFade, SDL_SRCALPHA, 0);
	SDL_BlitSurface(srfFade, NULL, my_srfScreen, this);
	SetVisible(false);
	PG_Application::UnlockScreen();

	Update(false);

	PG_Application::UnloadSurface(srfFade);
}

void PG_Widget::FadeIn() {

	// blit the widget to screen (invisible)
	SDL_SetClipRect(my_srfScreen, NULL);
	Blit();

	PG_Rect src(
	    0,
	    0,
	    (my_xpos < 0) ? my_width + my_xpos : my_width,
	    (my_ypos < 0) ? my_height + my_ypos : my_height);

	// create a temp surface
	SDL_Surface* srfFade = PG_Draw::CreateRGBSurface(w, h);

	PG_Application::LockScreen();
	
	// blit the widget to temp surface
	PG_Draw::BlitSurface(my_srfScreen, my_internaldata->rectClip, srfFade, src);

	int d = (255-my_internaldata->transparency)/ my_internaldata->fadeSteps;

	if(!d) {
		d = 1;
	} // minimum step == 1
	for(int i=255; i>my_internaldata->transparency; i -= d) {
		RestoreBackground(NULL, true);
		SDL_SetAlpha(srfFade, SDL_SRCALPHA, 255-i);
		PG_Draw::BlitSurface(srfFade, src, my_srfScreen, my_internaldata->rectClip);
		SDL_UpdateRects(my_srfScreen, 1, &my_internaldata->rectClip);
	}

	PG_Application::UnlockScreen();

	Update();

	PG_Application::UnloadSurface(srfFade);
}

void PG_Widget::SetFadeSteps(int steps) {
	my_internaldata->fadeSteps = steps;
}

bool PG_Widget::Action(KeyAction action) {
	int x = my_xpos + my_width / 2;
	int y = my_ypos + my_height / 2;

	switch(action) {
		case ACT_ACTIVATE:
			SDL_WarpMouse(x,y);
			eventMouseEnter();
			break;

		case ACT_DEACTIVATE:
			eventMouseLeave();
			break;

		case ACT_OK:
			SDL_MouseButtonEvent button;
			button.button = 1;
			button.x = x;
			button.y = y;
			eventMouseButtonDown(&button);
			SDL_Delay(200);
			eventMouseButtonUp(&button);
			Action(ACT_ACTIVATE);
			break;

		default:
			break;
	}

	return false;
}

bool PG_Widget::RestoreBackground(PG_Rect* clip, bool force) {

	if(my_internaldata->dirtyUpdate && (my_internaldata->transparency == 0) && !force) {
		return false;
	}
	
	if(PG_Application::GetBulkMode()) {
		return false;
	}

	if(clip == NULL) {
		clip = &my_internaldata->rectClip;
	}

	if(GetParent() == NULL) {
		PG_Application::RedrawBackground(*clip);

		if(widgetList.first() != this) {
			SDL_SetClipRect(my_srfScreen, clip);
			widgetList.Blit(*clip, widgetList.first(), this);
		}
		return true;
	}

	GetParent()->RestoreBackground(clip);
	SDL_SetClipRect(my_srfScreen, clip);
	GetParent()->Blit(false, false);

	return true;
}

PG_Widget* PG_Widget::FindWidgetFromPos(int x, int y) {
	PG_Point p;
	p.x = x;
	p.y = y;
	bool finished = false;

	PG_Widget* toplevel = widgetList.IsInside(p);
	PG_Widget* child = NULL;

	if(!toplevel) {
		return NULL;
	}

	while(!finished) {

		if(toplevel->GetChildList()) {
			child = toplevel->GetChildList()->IsInside(p);

			if(child) {
				toplevel = child;
				child = NULL;
			} else {
				finished = true;
			}

		} else {
			finished = true;
		}
	}

	return toplevel;
}

void PG_Widget::UpdateRect(const PG_Rect& r) {
	if(PG_Application::GetBulkMode()) {
		return;
	}

	SDL_Surface* screen = PG_Application::GetScreen();

	PG_Application::LockScreen();
	PG_Application::RedrawBackground(r);
	SDL_SetClipRect(screen, (PG_Rect*)&r);
	widgetList.Blit(r);
	SDL_SetClipRect(screen, NULL);
	PG_Application::UnlockScreen();
}

void PG_Widget::UpdateScreen() {
	UpdateRect(
	    PG_Rect(0, 0, PG_Application::GetScreenWidth(), PG_Application::GetScreenHeight())
	);
}

bool PG_Widget::IsInFrontOf(PG_Widget* widget) {
	PG_Widget* w1 = NULL;
	PG_Widget* w2 = NULL;
	PG_RectList* list = &widgetList;

	// do both widgets have the same parent ?
	if((GetParent() != NULL) && (GetParent() == widget->GetParent())) {
		w1 = this;
		w2 = widget;
		list = GetParent()->GetChildList();
	} else {
		w1 = this->GetToplevelWidget();
		w2 = widget->GetToplevelWidget();
	}

	return (w1->index > w2->index);
}

PG_Widget* PG_Widget::GetToplevelWidget() {
	if(GetParent() == NULL) {
		return this;
	}

	return GetParent()->GetToplevelWidget();
}

void PG_Widget::SendToBack() {
	if(GetParent() == NULL) {
		widgetList.SendToBack(this);
	} else {
		GetParent()->GetChildList()->SendToBack(this);
	}
	Update();
}

void PG_Widget::BringToFront() {
	if(GetParent() == NULL) {
		widgetList.BringToFront(this);
	} else {
		GetParent()->GetChildList()->BringToFront(this);
	}
	Update();
}

void PG_Widget::RecalcClipRect() {
	static PG_Rect pr;

	if (my_internaldata->widgetParent != NULL) {
		pr = *(my_internaldata->widgetParent->GetClipRect());
	} else {
		pr.SetRect(
		    0,
		    0,
		    PG_Application::GetScreenWidth(),
		    PG_Application::GetScreenHeight());
	}

	PG_Rect ir = IntersectRect(pr);
	SetClipRect(ir);
}

bool PG_Widget::LoadLayout(const char *name) {
	bool rc = PG_Layout::Load(this, name, NULL, NULL);
	Update();
	return rc;
}

bool PG_Widget::LoadLayout(const char *name, void (* WorkCallback)(int now, int max)) {
	bool rc = PG_Layout::Load(this, name, WorkCallback, NULL);
	Update();
	return rc;

}

bool PG_Widget::LoadLayout(const char *name, void (* WorkCallback)(int now, int max),void *UserSpace) {
	bool rc = PG_Layout::Load(this, name, WorkCallback, UserSpace);
	Update();
	return rc;
}

void PG_Widget::SetUserData(void *userdata, int size) {
	my_internaldata->userdata = new char[size];
	memcpy(my_internaldata->userdata, userdata, size);
	my_internaldata->userdatasize = size;
}

int PG_Widget::GetUserDataSize() {
	return my_internaldata->userdatasize;
}

void PG_Widget::GetUserData(void *userdata) {
	if (my_internaldata->userdata == NULL)
		return;
		
	memcpy(userdata, my_internaldata->userdata, my_internaldata->userdatasize);
}

void PG_Widget::ReleaseUserData() {
	if (my_internaldata->userdata != NULL)
		delete[] my_internaldata->userdata;
	my_internaldata->userdatasize = 0;
}

void PG_Widget::AddText(const char* text, bool update) {
	my_text += text;
	my_internaldata->widthText = TXT_HEIGHT_UNDEF;
	my_internaldata->heightText = TXT_HEIGHT_UNDEF;

	//TO-DO : Optimalize this !!! - because of widget functions overloading SetText()
	if (update) {
		SetText(GetText());
	}
}

void PG_Widget::SetText(const char* text) {

	my_internaldata->widthText = TXT_HEIGHT_UNDEF;
	my_internaldata->heightText = TXT_HEIGHT_UNDEF;

	if(text == NULL) {
		my_text = "";
		return;
	}

	my_text = string(text);
	Update();
}

void PG_Widget::SetTextFormat(const char* text, ...) {
	va_list ap;
	va_start(ap, text);
	char temp[256];

	if(text == NULL) {
		my_text = "";
		return;
	}

	if(text[0] == 0) {
		my_text = "";
		return;
	}

	vsprintf(temp, text, ap);
	SetText(temp);
	va_end(ap);
}

void PG_Widget::SetFontColor(const PG_Color& Color) {
	my_internaldata->font->SetColor(Color);
}

void PG_Widget::SetFontAlpha(int Alpha, bool bRecursive) {
	my_internaldata->font->SetAlpha(Alpha);

	if(!bRecursive || (GetChildList() == NULL)) {
		return;
	}

	for(PG_Widget* i = static_cast<PG_Widget*>(GetChildList()->first()); i != NULL; i = static_cast<PG_Widget*>(i->next)) {
		i->SetFontAlpha(Alpha, true);
	}
}

void PG_Widget::SetFontStyle(PG_Font::Style Style, bool bRecursive) {
	my_internaldata->font->SetStyle(Style);

	if(!bRecursive || (GetChildList() == NULL)) {
		return;
	}

	for(PG_Widget* i = static_cast<PG_Widget*>(GetChildList()->first()); i != NULL; i = static_cast<PG_Widget*>(i->next)) {
		i->SetFontStyle(Style, true);
	}
}

int PG_Widget::GetFontSize() {
	return my_internaldata->font->GetSize();
}

void PG_Widget::SetFontSize(int Size, bool bRecursive) {
	my_internaldata->font->SetSize(Size);

	if(!bRecursive || (GetChildList() == NULL)) {
		return;
	}

	for(PG_Widget* i = static_cast<PG_Widget*>(GetChildList()->first()); i != NULL; i = static_cast<PG_Widget*>(i->next)) {
		i->SetFontSize(Size, true);
	}

}

void PG_Widget::SetFontIndex(int Index, bool bRecursive) {
//	my_internaldata->font->SetIndex(Index);
}

void PG_Widget::SetFontName(const char *Name, bool bRecursive) {
	my_internaldata->font->SetName(Name);

	if(!bRecursive || (GetChildList() == NULL)) {
		return;
	}

	for(PG_Widget* i = static_cast<PG_Widget*>(GetChildList()->first()); i != NULL; i = static_cast<PG_Widget*>(i->next)) {
		i->SetFontName(Name, true);
	}

}

void PG_Widget::SetSizeByText(int Width, int Height, const char *Text) {
	Uint16 w,h;
	int baselineY;
	
	if (Text == NULL) {
		Text = my_text.c_str();
	}

	if (!PG_FontEngine::GetTextSize(Text, my_internaldata->font, &w, &h, &baselineY)) {
		return;
	}

	if (my_width == 0 && my_height > 0 && Width == 0) {
 		my_width = w;
 		my_ypos += (my_height - h - baselineY) / 2;
 		my_height = h + baselineY;
 	}
 	else if (my_height == 0 && my_width > 0 && Height == 0) {
 		my_xpos += (my_width - w) / 2;
 		my_width = w;
 		my_height = h + baselineY;
 	}
 	else {
		my_width = w + Width;
		my_height = h + Height + baselineY;
	}

}

void PG_Widget::SetFont(PG_Font* font) {
	if(my_internaldata->font != NULL) {
		delete my_internaldata->font;
	}
	
	my_internaldata->font = new PG_Font(font->GetName(), font->GetSize());
}

void PG_Widget::GetTextSize(Uint16& w, Uint16& h, const char* text) {
	if(text == NULL) {
		if(my_internaldata->widthText != TXT_HEIGHT_UNDEF) {
			w = my_internaldata->widthText;
			h = my_internaldata->heightText;
			return;
		}
		text = my_text.c_str();
	}

	GetTextSize(w, h, text, my_internaldata->font);

	if(text == NULL) {
		my_internaldata->widthText = w;
		my_internaldata->heightText = h;
	}
}

void PG_Widget::GetTextSize(Uint16& w, Uint16& h, const char* text, PG_Font* font) {
	PG_FontEngine::GetTextSize(text, font, &w);
	h = font->GetFontHeight();
}

int PG_Widget::GetTextWidth() {

	if(my_internaldata->widthText != TXT_HEIGHT_UNDEF) {
		return my_internaldata->widthText;
	}
		
	GetTextSize(my_internaldata->widthText, my_internaldata->heightText);

	return my_internaldata->widthText;
}

int PG_Widget::GetTextHeight() {
	return my_internaldata->font->GetFontAscender();
}

void PG_Widget::DrawText(const PG_Rect& rect, const char* text) {
	if(my_srfObject == NULL) {
		PG_FontEngine::RenderText(my_srfScreen, my_internaldata->rectClip, my_xpos+ rect.x, my_ypos + rect.y + GetFontAscender(), text, my_internaldata->font);
	}
	else {
		PG_FontEngine::RenderText(my_srfObject, PG_Rect(0,0,Width(),Height()), rect.x, rect.y + GetFontAscender(), text, my_internaldata->font);
	}
}

void PG_Widget::DrawText(int x, int y, const char* text) {
	DrawText(PG_Rect(x,y,w-x,h-y), text);
}

void PG_Widget::DrawText(int x, int y, const char* text, const PG_Rect& cliprect) {
	if(my_srfObject == NULL) {
		PG_Rect rect = cliprect;
		rect.x += my_xpos;
		rect.y += my_ypos;
//		PG_Rect r = this->IntersectRect(rect);
		PG_FontEngine::RenderText(my_srfScreen, rect, my_xpos + x, my_ypos + y + GetFontAscender(), text, my_internaldata->font);
	}
	else {
//		PG_Rect rect = this->IntersectRect(cliprect);
		PG_FontEngine::RenderText(my_srfObject, cliprect, x, y + GetFontAscender(), text, my_internaldata->font);
	}
}

void PG_Widget::DrawText(const PG_Rect& rect, const char* text, const PG_Color& c) {
	SetFontColor(c);
	DrawText(rect, text);
}

void PG_Widget::DrawText(int x, int y, const char* text, const PG_Color& c) {
	DrawText(PG_Rect(x,y,w,h), text, c);
}

void PG_Widget::QuitModal() {
	eventQuitModal(GetID(), this, 0);
}

bool PG_Widget::WillQuitModal()
{
	return my_internaldata->quitModalLoop;
}

void PG_Widget::StopQuitModal() {
	my_internaldata->quitModalLoop = false;
}

void PG_Widget::RunModal() {
	SDL_Event event;
	my_internaldata->quitModalLoop = false;

	// run while in modal mode
	while(!my_internaldata->quitModalLoop) {
		SDL_WaitEvent(&event);
		PG_Application::ClearOldMousePosition();
		ProcessEvent(&event, true);
		PG_Application::DrawCursor();
	}

	return;
}

bool PG_Widget::eventQuitModal(int id, PG_MessageObject* widget, unsigned long data) {
	my_internaldata->quitModalLoop = true;
	return true;
}

const char* PG_Widget::GetText() {
	return my_text.c_str();
}

void PG_Widget::eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst) {

	// Don't blit an object without a surface

	if(srf == NULL) {
		return;
	}

	// Set alpha
	Uint8 a = 255-my_internaldata->transparency;
	if(a != 0) {
		SDL_SetAlpha(srf, SDL_SRCALPHA, a);

		// Blit widget surface to screen
#ifdef DEBUG
		PG_LogDBG("SRC BLIT: x:%d y:%d w:%d h:%d",src.x,src.y,src.w,src.h);
		PG_LogDBG("DST BLIT: x:%d y:%d w:%d h:%d",dst.x,dst.y,dst.w,dst.h);
#endif // DEBUG

		PG_Draw::BlitSurface(srf, src, my_srfScreen, dst);
	}
}

void PG_Widget::DrawBorder(const PG_Rect& r, int size, bool up) {
	int i0, i1;

	if(!IsVisible()) {
		return;
	}

	i0 = (up) ? 0 : 1;
	i1 = (up) ? 1 : 0;

	// outer frame
	if (size >= 1) {
		DrawHLine(r.x + 0, r.y + 0, r.w, my_colorBorder[i0][0]);
		DrawVLine(r.x + 0, r.y + 0, r.h - 1, my_colorBorder[i0][0]);

		DrawHLine(r.x + 0, r.y + r.h - 1, r.w - 1, my_colorBorder[i1][0]);
		DrawVLine(r.x + r.w - 1, r.y + 1, r.h - 1, my_colorBorder[i1][0]);
	}
	// inner frame
	if (size >= 2) {
		DrawHLine(r.x + 1, r.y + 1, r.w - 1, my_colorBorder[i0][1]);
		DrawVLine(r.x + 1, r.y + 1, r.h - 2, my_colorBorder[i0][1]);

		DrawHLine(r.x + 1, r.y + r.h - 2, r.w - 2, my_colorBorder[i1][1]);
		DrawVLine(r.x + r.w - 2, r.y + 2, r.h - 2, my_colorBorder[i1][1]);
	}
}

void PG_Widget::SetTransparency(Uint8 t, bool bRecursive) {
	my_internaldata->transparency = t;

	if(!bRecursive || (GetChildList() == NULL)) {
		return;
	}

	for(PG_Widget* i = static_cast<PG_Widget*>(GetChildList()->first()); i != NULL; i = static_cast<PG_Widget*>(i->next)) {
		i->SetTransparency(t, true);
	}	
}

void PG_Widget::SetClipRect(PG_Rect& r) {
	my_internaldata->rectClip = r;
}

void PG_Widget::GetClipRects(PG_Rect& src, PG_Rect& dst, const PG_Rect& rect) {

	dst = IntersectRect(my_internaldata->rectClip, rect);

	int dx = dst.my_xpos - rect.my_xpos;
	int dy = dst.my_ypos - rect.my_ypos;

	if(dx < 0) {
		dx = 0;
	}

	if(dy < 0) {
		dy = 0;
	}

	src.my_xpos = dx;
	src.my_ypos = dy;
	src.my_width = dst.my_width;
	src.my_height = dst.my_height;
}

void PG_Widget::SetPixel(int x, int y, const PG_Color& c) {
	static PG_Point p;

	if(my_srfObject == NULL) {
		p.x = my_xpos + x;
		p.y = my_ypos + y;
		if(my_internaldata->rectClip.IsInside(p)) {
			PG_Draw::SetPixel(p.x, p.y, c, my_srfScreen);
		}
	} else {
		PG_Draw::SetPixel(x, y, c, my_srfObject);
	}
}

void PG_Widget::DrawHLine(int x, int y, int w, const PG_Color& color) {
	static PG_Rect rect;
	SDL_Surface* surface = my_srfObject;
	
	if(my_srfObject == NULL) {
		surface = my_srfScreen;
	}
	
	x += my_xpos;
	y += my_ypos;

	if((y < my_internaldata->rectClip.y) || (y >= (my_internaldata->rectClip.y+my_internaldata->rectClip.h))) {
		return;
	}

	// clip to widget cliprect
	int x0 = PG_MAX(x, my_internaldata->rectClip.x);
	int x1 = PG_MIN(x+w, my_internaldata->rectClip.x+my_internaldata->rectClip.w);
	Uint32 c = color.MapRGB(surface->format);

	int wl = (x1-x0);
	
	if(wl <= 0) {
		return;
	}
	
	if(my_srfObject != NULL) {
		x0 -= my_xpos;
		y -= my_ypos;
	}

	rect.SetRect(x0, y, wl, 1);
	SDL_FillRect(surface, &rect, c);
}

void PG_Widget::DrawVLine(int x, int y, int h, const PG_Color& color) {
	static PG_Rect rect;
	SDL_Surface* surface = my_srfObject;
	
	if(my_srfObject == NULL) {
		surface = my_srfScreen;
	}
	
	x += my_xpos;
	y += my_ypos;

	if((x < my_internaldata->rectClip.x) || (x >= (my_internaldata->rectClip.x+my_internaldata->rectClip.w))) {
		return;
	}
	
	// clip to widget cliprect
	int y0 = PG_MAX(y, my_internaldata->rectClip.y);
	int y1 = PG_MIN(y+h, my_internaldata->rectClip.y+my_internaldata->rectClip.h);
	Uint32 c = color.MapRGB(surface->format);

	int hl = (y1-y0);
	
	if(hl <= 0) {
		return;
	}
	
	if(my_srfObject != NULL) {
		y0 -= my_ypos;
		x -= my_xpos;
	}

	rect.SetRect(x, y0, 1, hl);
	SDL_FillRect(surface, &rect, c);
}

/**  */
void PG_Widget::DrawRectWH(int x, int y, int w, int h, const PG_Color& c) {

	DrawHLine(x, y, w, c);
	DrawHLine(x, y + h - 1, w, c);
	DrawVLine(x, y, h, c);
	DrawVLine(x + w - 1, y, h, c);

}

void PG_Widget::DrawLine(Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, const PG_Color& color, Uint8 width) {
	SDL_Surface* surface = my_srfObject;

	if(surface == NULL) {
		surface = PG_Application::GetScreen();
		x0 += my_xpos;
		y0 += my_ypos;
		x1 += my_xpos;
		y1 += my_ypos;
	}

	PG_Draw::DrawLine(surface, x0, y0, x1, y1, color, width);
}

void PG_Widget::eventDraw(SDL_Surface* surface, const PG_Rect& rect) {
}

void PG_Widget::eventMoveWidget(int x, int y) {
}

void PG_Widget::eventMoveWindow(int x, int y) {
}

void PG_Widget::eventSizeWindow(Uint16 w, Uint16 h) {
}

void PG_Widget::eventSizeWidget(Uint16 w, Uint16 h) {
}

SDL_Surface* PG_Widget::GetWidgetSurface() {
	return my_srfObject;
}

SDL_Surface* PG_Widget::GetScreenSurface() {
	return my_srfScreen;
}

bool PG_Widget::IsVisible() {
	return my_internaldata->visible;
}

PG_Widget* PG_Widget::GetParent() {
	if(my_internaldata == NULL) {
		return NULL;
	}
	return my_internaldata->widgetParent;
}

int PG_Widget::GetID() {
	if(my_internaldata == NULL) {
		return -1;
	}
	return my_internaldata->id;
}

PG_Widget* PG_Widget::FindChild(int id) {
	if(my_internaldata->childList == NULL) {
		return NULL;
	}
	return my_internaldata->childList->Find(id);
}

PG_Widget* PG_Widget::FindChild(const char *name) {
	if(my_internaldata->childList == NULL) {
		return NULL;
	}
	return my_internaldata->childList->Find(name);
}

PG_RectList* PG_Widget::GetChildList() {
	return my_internaldata->childList;
}

int PG_Widget::GetChildCount() {
	return my_internaldata->childList ? my_internaldata->childList->size() : 0;
}

PG_RectList* PG_Widget::GetWidgetList() {
	return &widgetList;
}

void PG_Widget::SetName(const char *name) {
	my_internaldata->name = name;
}

const char* PG_Widget::GetName() {
	return my_internaldata->name.c_str();
}

int PG_Widget::GetFontAscender() {
	return my_internaldata->font->GetFontAscender();
}

int PG_Widget::GetFontHeight() {
	return my_internaldata->font->GetFontHeight();
}

PG_Color PG_Widget::GetFontColor() {
	return my_internaldata->font->GetColor();
}

PG_Font* PG_Widget::GetFont() {
	return my_internaldata->font;
}

Uint8 PG_Widget::GetTransparency() {
	return my_internaldata->transparency;
}

PG_Rect* PG_Widget::GetClipRect() {
	return &my_internaldata->rectClip;
}

bool PG_Widget::IsClippingEnabled() {
	return ((my_internaldata->rectClip.my_width != my_width) || (my_internaldata->rectClip.my_height != my_height));
}

void PG_Widget::GetClipRects(PG_Rect& src, PG_Rect& dst) {
	GetClipRects(src, dst, *this);
}

void PG_Widget::SetID(int id) {
	my_internaldata->id = id;
}

void PG_Widget::SetDirtyUpdate(bool bDirtyUpdate) {
	if(PG_Application::GetDirtyUpdatesDisabled()) {
		my_internaldata->dirtyUpdate = false;
		return;
	}
	
	my_internaldata->dirtyUpdate = bDirtyUpdate;
}

bool PG_Widget::GetDirtyUpdate() {
	return my_internaldata->dirtyUpdate;
}

void PG_Widget::SetHidden(bool hidden) {
	my_internaldata->hidden = hidden;
}

	
bool PG_Widget::IsHidden() {
	return my_internaldata->hidden;
}
