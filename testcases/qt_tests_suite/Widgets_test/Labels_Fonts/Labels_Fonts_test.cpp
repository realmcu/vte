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
Roman Holodov           17/05/2004      ?????????   Initial version 

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#include <qapplication.h>
#include "form.h"

extern "C"{
    #include "test.h"
}

int VT_Labels_Fonts_cleanup(){
    return TPASS;
};
int VT_Labels_Fonts_setup(){
    return TPASS;
};


int VT_Labels_Fonts_test(QApplication *app)
{
    Form fm;
    app->setMainWidget(&fm);
    fm.setCaption("Labels_Fonts test [CTRL+F11 - Pass / CTRL+F12 - Fail]");
    fm.show();
//11    fm.showMaximized();
    QObject::connect(app,SIGNAL(lastWindowClosed()),app,SLOT(quit()));
    fm.VT_rv=TFAIL;
    app->exec();
    return fm.VT_rv;
}

