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
Konstantin L           17/05/2004      ?????????   Initial version 

Irina Inkina           27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qprogressdialog.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qcolor.h>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qdatetime.h>
#include "PDTestView.h"
#include <qtimer.h>
#include "stdlib.h"


ProgressDialogTestView::ProgressDialogTestView() : QWidget()
{
  	n = 10000;

    menubar = new QMenuBar(this);
    Q_CHECK_PTR(menubar);

    QPopupMenu *file = new QPopupMenu();
    Q_CHECK_PTR(file);
    menubar->insertItem("&File", file);
    file->insertItem("&Start!", this, SLOT(Start()), CTRL+Key_S);
    file->insertItem( "&Exit - Test Pass",  this, SLOT(Exit()), CTRL+Key_Q );
    file->insertItem( "&Qiut - Test Fail", this, SLOT(Quit()), CTRL+Key_C );

	
	setFixedSize(240,320);

    QTimer::singleShot( 1000, this, SLOT(testr())  );

	
}
void ProgressDialogTestView::Exit()
{
	qApp->exit(0);
}
void ProgressDialogTestView::Quit()
{
	qApp->exit(1);
}



void ProgressDialogTestView::Start()
{
    bool ok;
    n = QInputDialog::getInteger("Input number of boxes",
        "Input number of boxes that will be drawn",
        1000, 0, 2147483647, 1000, &ok, this);
    if (!ok)
    {
        QMessageBox::critical(this, "Input error", "Incorrect data entered!");
        return;
    }

	testr();
}

QProgressDialog* ProgressDialogTestView::newProgressDialog(const char *label, int steps)
{
    return NULL;
}


void ProgressDialogTestView::testr()
{


	QProgressDialog* lpb = new QProgressDialog(
        "Drawing rectangles", "Cancel", n, this, "progress", TRUE);
    lpb->setMinimumDuration(0);
	lpb->setCaption("Please Wait");

	QPainter p(this);
    QTime t;
    t.start();
    int i;
if(n!=1)    
	for ( i=0; i<n; i++) 
    {
//1		if(100*(i/100)==i)
    		lpb->setProgress(i);
    	if ( lpb->wasCancelled() )
    		break;

    	QColor c(rand()%255, rand()%255, rand()%255);
    	int x = rand()%(width()-8);
    	int y = rand()%(height()-8);
    	int w = rand()%(width()-x);
    	int h = rand()%(height()-y);
    	p.fillRect(x,y,w,h,c);
	}
    int elapsed = t.elapsed();
    QMessageBox::information(this, "Information",
        QString::number(i) + " boxes drawn.\nTime elapsed (msec): " 
        + QString::number(elapsed));

	p.fillRect(0, 0, width(), height(), backgroundColor());

    delete lpb;
}
