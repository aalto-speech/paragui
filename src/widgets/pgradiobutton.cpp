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
    Update Date:      $Date: 2004/04/28 13:27:02 $
    Source File:      $Source: /sources/paragui/paragui/src/widgets/pgradiobutton.cpp,v $
    CVS/RCS Revision: $Revision: 1.3.6.4.2.8 $
    Status:           $State: Exp $
*/

#include "pgradiobutton.h"
#include "pgapplication.h"
#include "pgtheme.h"
#include "pgbutton.h"

PG_RadioButton::PG_RadioButton(PG_Widget* parent, const PG_Rect& r, const char* text, PG_RadioButton* firstOfGroup, int id, const char* style) : PG_ThemeWidget(parent, r) {
	PG_Rect rectButton;
	PG_Rect rectLabel;

	SetID(id);

	my_groupFirst = (firstOfGroup == NULL) ? this : firstOfGroup;
	my_groupNext = NULL;

	my_isPressed = false;
	my_storeMarker = false;
	my_hoverTransparency = 128;

	if(text == NULL) {
		rectButton.SetRect(0, 0, r.my_width, r.my_height);
	} else {
		rectButton.SetRect(0, 0, r.my_height, r.my_height);
	}

	my_widgetButton = new PG_Button(this, rectButton);
	my_widgetButton->SetToggle(true);
	my_widgetButton->EnableReceiver(false);
	my_widgetButton->sigClick.connect(slot(*this, &PG_RadioButton::handleButtonClick));

	rectLabel.SetRect(rectButton.my_width, 0, r.my_width - rectButton.my_width, r.my_height);
	my_widgetLabel = new PG_Label(this, rectLabel, text, style);

	// load default first
	if(strcmp(style, "RadioButton") != 0) {
		LoadThemeStyle("RadioButton");
	}

	// load inherited style
	LoadThemeStyle(style);

	SetTransparency(255);

	AddToGroup(this);

	if(firstOfGroup == NULL) {
		SetPressed();
	}
}

void PG_RadioButton::LoadThemeStyle(const char* widgettype) {
	PG_Theme* t = PG_Application::GetTheme();

	t->GetProperty(widgettype, "RadioButton", "transparency", my_hoverTransparency);

	PG_ThemeWidget::LoadThemeStyle(widgettype, "RadioButton");

	my_widgetButton->LoadThemeStyle(widgettype, "CheckButton");
	my_widgetLabel->LoadThemeStyle(widgettype);
}

PG_RadioButton::~PG_RadioButton() {
	delete my_widgetLabel;
	delete my_widgetButton;
}

void PG_RadioButton::eventMouseEnter() {
	SetTransparency(my_hoverTransparency);
	Update();
	PG_ThemeWidget::eventMouseEnter();
}

void PG_RadioButton::eventMouseLeave() {
	SetTransparency(255);
	Update();
	PG_ThemeWidget::eventMouseLeave();
}

bool PG_RadioButton::eventMouseButtonUp(const SDL_MouseButtonEvent* my_widgetButton) {
	PG_RadioButton* list = my_groupFirst;
	Uint16 mx;
	Uint16 my;
	if(my_groupFirst != NULL) {

		if(my_widgetButton->button == 4) {		
			while(list->my_groupNext != NULL) {
				if(list->my_groupNext == this) {
					mx = (Uint16)(list->my_xpos + ((my_widgetButton->x * 1.0 - list->my_xpos) / list->my_groupNext->my_width) * list->my_width);
					my = (Uint16)list->my_ypos;
					SDL_WarpMouse(mx, my);
					break;
				}
				list = list->my_groupNext;
			}									
			return true;
		}

		if(my_widgetButton->button == 5) {		
			do {				
				if(list == this && list->my_groupNext != NULL) {
					mx = (Uint16)(list->my_groupNext->my_xpos + ((my_widgetButton->x * 1.0 - list->my_groupNext->my_xpos) / list->my_width) * list->my_groupNext->my_width);
					my = (Uint16)list->my_groupNext->my_ypos;
					SDL_WarpMouse(mx, my);
					break;
				}
				list = list->my_groupNext;
			} while(list != NULL);
			return true;
		}		
	}

	SetPressed();	
	return true;
}

bool PG_RadioButton::handleButtonClick(PG_Button* button) {
	SetPressed();
	return true;
}

void PG_RadioButton::SetPressed() {

	if(my_isPressed) {
		my_widgetButton->SetPressed(true);
		return;
	}

	PG_RadioButton* list = my_groupFirst;

	while(list != NULL) {
		if (list->my_isPressed) {
			list->ReleaseButton();
		}
		list = list->my_groupNext;
	}

	my_widgetButton->SetPressed(true);
	my_isPressed = true;

	Update();

	// Notify parent
	sigClick(this, true);
}

bool PG_RadioButton::GetPressed() {
	return my_isPressed;
}

void PG_RadioButton::ReleaseButton() {
	my_widgetButton->SetPressed(false);
	my_isPressed = false;
}

void PG_RadioButton::AddToGroup(PG_RadioButton* w) {
	PG_RadioButton* list = my_groupFirst;

	while(list->my_groupNext != NULL) {
		list = list->my_groupNext;
	}

	list->my_groupNext = w;
	w->my_groupFirst = my_groupFirst;
	w->my_groupNext = NULL;
}

void PG_RadioButton::SetText(const char* text) {
	my_widgetLabel->SetText(text);
}

const char* PG_RadioButton::GetText() {
	return my_widgetLabel->GetText();
}

void PG_RadioButton::SetAlignment(PG_Label::TextAlign a) {
	my_widgetLabel->SetAlignment(a);
}

void PG_RadioButton::SetFontColor(const PG_Color& Color) {
	my_widgetLabel->SetFontColor(Color);
}

void PG_RadioButton::SetSizeByText(int Width, int Height, const char *Text) {
	if (Text == NULL) {
		Text = my_widgetLabel->GetText();
	}

	my_widgetButton->SetSizeByText();
	my_widgetLabel->SetSizeByText(0, 0, Text);

	my_widgetLabel->MoveWidget(my_widgetButton->my_width, 0);
	SizeWidget(my_widgetButton->my_width + my_widgetLabel->my_width + Width, PG_MAX(my_widgetButton->my_height + Height, my_widgetLabel->my_height));
	my_widgetButton->MoveWidget(0, (my_height - my_widgetButton->my_height) >> 1);	
}
