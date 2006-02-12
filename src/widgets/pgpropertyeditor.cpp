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
    Update Date:      $Date: 2006/02/12 19:07:32 $
    Source File:      $Source: /sources/paragui/paragui/src/widgets/Attic/pgpropertyeditor.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

#include <algorithm>

#include "pgpropertyeditor.h"
#include "pglabel.h"


PG_PropertyEditor :: PG_PropertyEditor ( PG_Widget *parent, const PG_Rect &rect, const std::string &style, int labelWidthPercentage )
		: PG_ScrollWidget( parent, rect, style ), styleName( style ), ypos ( 0 ), lineHeight(25), lineSpacing(2), labelWidth(labelWidthPercentage) {}
;

std::string PG_PropertyEditor :: GetStyleName( const std::string& widgetName ) {
	return styleName;
};

void PG_PropertyEditor :: Reload() {
	for ( PropertyFieldsType::iterator i = propertyFields.begin(); i != propertyFields.end(); ++i )
		(*i)->Reload();
};


bool PG_PropertyEditor :: Valid() {
	for ( PropertyFieldsType::iterator i = propertyFields.begin(); i != propertyFields.end(); ++i )
		if ( ! (*i)->Valid() )
			return false;
	return true;
};

bool PG_PropertyEditor :: Apply() {
   for ( PropertyFieldsType::iterator i = propertyFields.begin(); i != propertyFields.end(); ++i ) {
      if ( ! (*i)->Valid() ) {
         (*i)->Focus();
		    return false;
      }
   }

	for ( PropertyFieldsType::iterator i = propertyFields.begin(); i != propertyFields.end(); ++i )
		(*i)->Apply();

	return true;
};

PG_Rect PG_PropertyEditor :: RegisterProperty( const std::string& name, PG_PropertyEditorField* propertyEditorField ) {
	propertyFields.push_back( propertyEditorField );

	int w = Width() - my_widthScrollbar - 2 * lineSpacing;

	PG_Label* label = new PG_Label( this, PG_Rect( 0, ypos, w * labelWidth / 100 - 1, lineHeight), name );
	label->LoadThemeStyle( styleName, "Label" );
	PG_Rect r  ( w * labelWidth / 100 , ypos, w * ( 100 - labelWidth ) / 100  - 1, lineHeight );
	ypos += lineHeight + lineSpacing;
	return r;
};

PG_PropertyEditor :: ~PG_PropertyEditor() {
	for ( PropertyFieldsType::iterator i = propertyFields.begin(); i != propertyFields.end(); ++i )
		delete *i;
};

