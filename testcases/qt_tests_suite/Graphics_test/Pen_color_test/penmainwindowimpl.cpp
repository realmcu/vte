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
#include "penmainwindowimpl.h"
#include <qmenubar.h>
#include <qpopupmenu.h>


PenMainWindowImpl::PenMainWindowImpl( QWidget* parent, const char* name, WFlags f )
	: PenMainWindow( parent, name, f )
{
	setCaption("Pen");

	// Add your code
	QPopupMenu *fileMenu = new QPopupMenu( this );
    fileMenu->insertItem( "&Exit",  this, SLOT(exitPass()), CTRL+Key_Q );
    fileMenu->insertItem( "&Qiut", this, SLOT(exitFail()), CTRL+Key_C );
 
    QPopupMenu *helpMenu = new QPopupMenu( this );
    helpMenu->insertItem( "&About", this, SLOT(About()), Key_F1);


    QMenuBar *menu = new QMenuBar( this );
    menu->insertItem( "&File", fileMenu );
    menu->insertItem( "&Help", helpMenu );

}
