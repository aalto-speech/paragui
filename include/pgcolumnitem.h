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
    Update Date:      $Date: 2003/12/02 15:27:57 $
    Source File:      $Source: /sources/paragui/paragui/include/pgcolumnitem.h,v $
    CVS/RCS Revision: $Revision: 1.3.6.1.2.1 $
    Status:           $State: Exp $
*/

/** \file pgcolumnitem.h
	Header file of the PG_ColumnItem class.
*/

#ifndef PG_COLUMNITEM_H
#define PG_COLUMNITEM_H

#include "pglistboxitem.h"
#include <vector>
#include <string>

/**
	@author Alexander Pipelka
	
	@short ListBoxItem with multiple columns
 
	This is a multi-column item that can be inserted into the PG_ListBox
*/

class DECLSPEC PG_ColumnItem : public PG_ListBoxItem {
public:

	/**
	Construct a new PG_ColumnItem object.
	@param columns number of columns this item should hold.
	@param height height of the item in pixels.
	@param userdata pointer to userdata this item should refer to.
	*/
	PG_ColumnItem(Uint32 columns, Uint32 height, void* userdata = NULL);

	/** */
	~PG_ColumnItem();

	/**
	Set the width of a column (in pixels).
	@param column index of the column to change (starting at 0).
	@param width new width of the column (in pixels).
	*/
	void SetColumnWidth(Uint32 column, Uint32 width);

	/**
	Get the width of a column
	@param column index of the column.
	@return width of the column.
	*/
	int GetColumnWidth(Uint32 column);

	/**
	Set the text of a column
	@param column index of the column to change (starting at 0).
	@param text new text of the column.
	*/
	void SetColumnText(Uint32 column, const char* text);

	/**
	Get the text of a column.
	@param column index of the column.
	@return text of the column.
	*/
	const char* GetColumnText(Uint32 column);

	/*
	Return the number of columns in this item.
	@return number of columns.
	*/
	int GetColumnCount();

protected:

	/** */
	void eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst);

private:

	Uint32 my_columncount;

	vector <Uint32> my_columnwidth;
	vector <string> my_columntext;
};

#endif	// PG_COLUMNITEM_H
