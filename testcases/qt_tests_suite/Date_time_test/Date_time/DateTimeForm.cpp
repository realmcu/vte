/*================================================================================================*/
/**
    @file   DateTimeForm.cpp

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
Konstantin L.                 17/05/2004      ?????????   Initial version 
Irina Inkina                  27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

#include "DateTimeForm.h"
#include <qlayout.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qapplication.h>
#include "DateTimeView.h"
#include <qmessagebox.h>

DateTimeForm::DateTimeForm()
{
   // If we used a QMainWindow we could use its built-in menuBar().
    fileMenu = new QPopupMenu( this );
    Q_CHECK_PTR( fileMenu );
    fileMenu->insertItem( "&Exit - Test Pass",  this, SLOT(exitPass()), CTRL+Key_Q );
    fileMenu->insertItem( "&Qiut - Test Fail", this, SLOT(exitFail()), CTRL+Key_C );

	formatMenu = new QPopupMenu(this);
	Q_CHECK_PTR(formatMenu);
	formatMenu->insertItem("&Date Format", this, SLOT(changeDateFormat()), CTRL+Key_D);
	formatMenu->insertItem("&Time Format", this, SLOT(changeTimeFormat()), CTRL+Key_T);
	
	helpMenu = new QPopupMenu(this);
	Q_CHECK_PTR(formatMenu);
	helpMenu->insertItem("&Help", this, SLOT(showHelp()), CTRL+Key_H);

	QMenuBar *menu = new QMenuBar( this );
    Q_CHECK_PTR( menu );
    menu->insertItem( "&File", fileMenu );
	menu->insertItem("&Help", helpMenu);
//	menu->insertItem( "F&ormat", formatMenu);

	DateTimeView* pView = new DateTimeView(this);
	setCentralWidget(pView);
	pView->show();

	setFixedSize(240,320);

	assistant = new QAssistantClient( "", this );
}

void DateTimeForm::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
	qApp->exit(0);
}

void DateTimeForm::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
	qApp->exit(1);
}

void DateTimeForm::changeDateFormat()
{
}

void DateTimeForm::changeTimeFormat()
{
}



void DateTimeForm::showHelp()
{
	assistant->showPage("index.html");
}
