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
 
    Last Update:      $Author: senarvi $
    Update Date:      $Date: 2012/04/27 18:30:03 $
    Source File:      $Source: /share/puhe/cvsroot/paragui/src/widgets/pgcheckbutton.cpp,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

#include "pgcheckbutton.h"
#include "pgbutton.h"

PG_CheckButton::PG_CheckButton(PG_Widget* parent, const PG_Rect& r, const char* text, int id, const char* style)
        : PG_RadioButton(parent, r, text, NULL, id)
{
	LoadThemeStyle(style);
	my_widgetButton->LoadThemeStyle(style, "CheckKnob");

	SetUnpressed();
}

PG_CheckButton::~PG_CheckButton() {}

bool PG_CheckButton::eventMouseButtonUp(const SDL_MouseButtonEvent* my_widgetButton) {

	if(my_isPressed) {
		SetUnpressed();
	} else {
		SetPressed();
	}

	return true;
}

void PG_CheckButton::SetUnpressed() {
	my_widgetButton->SetPressed(false);
	my_isPressed = false;

	Update();

	// Notify parent
	sigClick(this, false);
}
