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
    Update Date:      $Date: 2002/04/15 14:53:56 $
    Source File:      $Source: /sources/paragui/paragui/src/core/pgmain.cpp,v $
    CVS/RCS Revision: $Revision: 1.1 $
    Status:           $State: Exp $
*/
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <cstdlib>

#include "pgapplication.h"
#include "pglog.h"

int PG_main(int argc, char **argv, PG_Application *app)
{
    if (!app)
        return 1;

    PG_TRY {
        app->Run();
    }

    PG_CATCH_ALL {
        //
        // For now it just aborts...
        //
        // TODO: report more information (will require adding a
        // PG_exception class and handling the standard exceptions in a
        // separate catch block)
        //
        PG_LogERR("Unhandled exception caught in PG_main! "
                  "Aborting execution.");
        exit(1);
    }

    return 0;
}
