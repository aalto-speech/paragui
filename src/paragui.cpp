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
    Update Date:      $Date: 2004/11/17 21:34:23 $
    Source File:      $Source: /sources/paragui/paragui/src/paragui.cpp,v $
    CVS/RCS Revision: $Revision: 1.3.6.3.2.3 $
    Status:           $State: Exp $
*/

#include "pginfo.h"
#include "paragui.h"
#include "pglog.h"

namespace PG_Info {

	static std::string Version = VERSION;
	static std::string Homepage = "http://www.paragui.org";
	static std::string Licence = "LGPL (Lesser General Public Licence)";

	const std::string& GetVersion() {
		return Version;
	};

	const std::string& GetHomepage() {
		return Homepage;
	};

	const std::string& GetLicence() {
		return Licence;
	};

	void PrintInfo() {
		PG_LogMSG("ParaGUI Information:");
		PG_LogMSG("Version: %s", GetVersion().c_str());
		PG_LogMSG("URL: %s", GetHomepage().c_str());
		PG_LogMSG("Licence: %s", GetLicence().c_str());
	}
};
