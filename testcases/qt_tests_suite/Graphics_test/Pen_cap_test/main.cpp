/*================================================================================================*/
/**
    @file   
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
Alexey P           17/05/2004      ?????????   Initial version 

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#include <qapplication.h>
#include "pencapmainwindow.h"


int Pen_cap_test_main( int argc, char** argv )
{
	QApplication app( argc, argv );

	PenCapMainWindow mainwindow;
        mainwindow.setFixedSize(240,320);
//	mainwindow.setGeometry(120, 100, 0, 0);
	app.setMainWidget(&mainwindow);

	mainwindow.show();

	return app.exec();
}

