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
    Update Date:      $Date: 2002/04/15 13:31:31 $
    Source File:      $Source: /sources/paragui/paragui/src/widgets/pgmaskedit.cpp,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/


#include "pgmaskedit.h"

PG_MaskEdit::PG_MaskEdit(PG_Widget* parent, const PG_Rect& r, const char* style) : PG_LineEdit(parent, r, style) {
	my_spacer = '_';
}

void PG_MaskEdit::SetText(const char* new_text) {
	PG_LineEdit::SetText(my_displaymask.c_str());

	// set the displaymask if the string is empty
	if((new_text == NULL) || (new_text[0] == 0)) {
		return;
	}

	SetCursorPos(0);

	// merge the text with the displaymask
	for(Uint32 i=0; i<strlen(new_text); i++) {
		InsertChar(&new_text[i]);
	}
}

void PG_MaskEdit::SetMask(const char* mask) {
	my_mask = mask;
	my_displaymask = mask;

	for(Uint32 i=0; i<my_displaymask.size(); i++) {
		if(my_displaymask[i] == '#') {
			my_displaymask[i] = my_spacer;
		}
	}

	PG_LineEdit::SetText(my_displaymask.c_str());
}

const char* PG_MaskEdit::GetMask() {
	return my_mask.c_str();
}

void PG_MaskEdit::SetSpacer(char c) {
	my_spacer = c;
}

char PG_MaskEdit::GetSpacer() {
	return my_spacer;
}

void PG_MaskEdit::InsertChar(const char* c) {
        if (!c)
	        return;

	// check if we are on a valid position
	while(((Uint32)my_cursorPosition < my_mask.length()) && (my_mask[my_cursorPosition] != '#')) {
		my_cursorPosition++;
	}

	if((Uint32)my_cursorPosition == my_mask.length()) {
		return;
	}

	my_text[my_cursorPosition++] = *c;

	while((my_cursorPosition < (int)my_mask.size()) &&
	        (my_mask[my_cursorPosition] != '#')) {
		my_cursorPosition++;
	}

	SetCursorPos(my_cursorPosition);

}

void PG_MaskEdit::DeleteChar(Uint16 pos) {
	if(my_mask[pos] == '#') {
		my_text[pos] = my_spacer;
	}
}

bool PG_MaskEdit::eventMouseButtonDown(const SDL_MouseButtonEvent* button) {

	// let the parent first process the event
	if(!PG_LineEdit::eventMouseButtonDown(button)) {
		return false;
	}

	// check the cursorposition and find a valid position
	while((my_text[my_cursorPosition] == my_spacer) || (my_mask[my_cursorPosition] != '#')) {
		my_cursorPosition--;
		if(my_cursorPosition < 0) {
			break;
		}
	}

	if(my_cursorPosition >= 0) {
		my_cursorPosition++;
	}

	SetCursorPos(my_cursorPosition);

	return true;
}
