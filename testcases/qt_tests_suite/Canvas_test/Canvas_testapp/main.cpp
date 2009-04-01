/*================================================================================================*/
/**
    @file   main.cpp

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
Dmitriy Kazachkov           17/05/2004      ?????????   Initial version 

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qapplication.h>
#include <qimage.h>

#include "canvas.h"

#include <stdlib.h>

extern QString butterfly_fn;
extern QString logo_fn;

int Canvas_main(int argc, char** argv)
{
    QApplication app(argc,argv);
    
	butterfly_fn = "butterfly.png";
   
    
    QCanvas canvas(240,320);
    canvas.setAdvancePeriod(30);
    Main m(canvas);
    m.resize(m.sizeHint());
    m.setCaption("Qt Example - Canvas");
    if ( QApplication::desktop()->width() > m.width() + 10
	&& QApplication::desktop()->height() > m.height() +30 )
	m.show();
    else
	m.showMaximized();
    
    QObject::connect( qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()) );
    
    return app.exec();
}

