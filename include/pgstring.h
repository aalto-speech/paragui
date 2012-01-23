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
    Update Date:      $Date: 2012/04/27 18:19:48 $
    Source File:      $Source: /share/puhe/cvsroot/paragui/include/pgstring.h,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

#ifndef PG_STRING_H
#define PG_STRING_H

#include "paragui.h"

#if defined(ENABLE_UNICODE)
#include "ychar.h"
#include "ystring.h"
#define PG_Char YChar
#define PG_String YString
#else
#include <string>
#define PG_Char char
#define PG_String std::string
#endif

#endif	// PG_STRING_H
