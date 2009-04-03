/*================================================================================================*/
/**
    @file   DateTime.cpp

    @brief  LTP Motorola template.
*/
/*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Konstantin L.                17/05/2004      ?????????   Initial version 

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/


// DateTime.cpp : Defines the entry point for the console application.
//

#include <qapplication.h>
#include "DateTimeForm.h"

int Date_time_main(int argc, char* argv[])
{
    QApplication app( argc, argv );

	DateTimeForm fm;
	app.setMainWidget(&fm);
    fm.setCaption("Date Time element test");
    fm.show();
    app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
	return app.exec();

	return 0;
}
