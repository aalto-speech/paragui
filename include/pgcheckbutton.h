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
    Update Date:      $Date: 2002/04/27 15:36:54 $
    Source File:      $Source: /sources/paragui/paragui/include/pgcheckbutton.h,v $
    CVS/RCS Revision: $Revision: 1.5 $
    Status:           $State: Exp $
*/

/** \file pgcheckbutton.h
	Header file of the PG_CheckButton class.
*/

#ifndef PG_CHECKBUTTON_H
#define PG_CHECKBUTTON_H

#include "pgradiobutton.h"

/**
	@author Alexander Pipelka
 
	@short A single check button.
*/

class DECLSPEC PG_CheckButton : public PG_RadioButton {
public:

	/**
	construct a PG_CheckButton object
	@param parent	Pointer to the parent widget of NULL
	@param id id of the checkbutton (can be 0 if you don't need a unique id)
	@param r position of the checkbutton
	@param text inital text of the chekbutton
	@param style widget style (loaded from theme) to use
	*/
	PG_CheckButton(PG_Widget* parent, int id, const PG_Rect& r, const char* text, const char* style="CheckButton");

	/** */
	~PG_CheckButton();

	/**
	Set the state of the checkbutton to unpressed.
	*/
	void SetUnpressed();

protected:

	/** */
	bool eventMouseButtonUp(const SDL_MouseButtonEvent* my_widgetButton);
};

#endif // PG_CHECKBUTTON_H
