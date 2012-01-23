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
    Update Date:      $Date: 2012/04/27 18:19:24 $
    Source File:      $Source: /share/puhe/cvsroot/paragui/include/pginfo.h,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

/** \file pginfo.h
    Paragui Library related functions.
    @author Alexander Pipelka
*/

#include "paragui.h"
#include <string>

/**
	@short Paragui Library related functions.

       This namespace contains functions returning Paragui specific information.
	   Note that this is information about the Paragui Project itself such as the versioning
	   information.
*/

namespace PG_Info {

	/**
	Get the current version of ParaGUI
	@return version information
	*/
	DECLSPEC const char* GetVersion();

	/**
	Get the URL of the ParaGUI homepage
	@return hompage URL
	*/
	DECLSPEC const char* GetHomepage();

	/**
	Get ParaGUI's licence information
	@return licence information string
	*/
	DECLSPEC const char* GetLicence();

	/**
	Log all information to the console
	*/
	DECLSPEC void PrintInfo();
}
