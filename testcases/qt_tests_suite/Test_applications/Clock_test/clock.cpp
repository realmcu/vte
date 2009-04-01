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

Irina Inkina            27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#include "clock.h"
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qobject.h>
#include <qapplication.h>
#include <qcursor.h>
#include <qlabel.h>

Clock::Clock(QWidget *parent,const char *name){

	analogClock=new AnalogClock(this);
	digitalClock=new DigitalClock(this);

	digitalClock->hide();

	childClock=analogClock;

//	QPopupMenu *clockStyleMenu=new QPopupMenu(this);
//	clockStyleMenu->insertItem("&Analog clock",this,SLOT(switchToAnalog()));
//	clockStyleMenu->insertItem("&Digital clock",this,SLOT(switchToDigital()));
//	QMenuBar *menu=new QMenuBar(this);
//	menu->insertItem("&Style",clockStyleMenu);
//	menu->insertItem("&About",this,SLOT(about()));
	connect(analogClock,SIGNAL(switchStyle()),this,SLOT(switchToDigital()));
	connect(digitalClock,SIGNAL(switchStyle()),this,SLOT(switchToAnalog()));
}

void Clock::resizeEvent(QResizeEvent *e){
	QWidget::resizeEvent(e);
	childClock->setGeometry(rect());
}

void Clock::switchToAnalog(){
	if(childClock==analogClock)return;
	digitalClock->hide();
	childClock=analogClock;
	childClock->setGeometry(rect());
	childClock->show();
}

void Clock::switchToDigital(){
	if(childClock==digitalClock)return;
	analogClock->hide();
	childClock=digitalClock;
	childClock->setGeometry(rect());
	childClock->show();
}

void Clock::about(){
	QMessageBox::about(this,"Clock","This is a simple expample.");
}


void Clock::contextMenuEvent( QContextMenuEvent * )
{
   QColor x2(202,202,202);
   QColorGroup g2(black,x2,x2.light(),x2.dark(),x2.dark(120),white/*black*/,white);
   QPalette p2(g2,g2,g2);


    QPopupMenu*	contextMenu = new QPopupMenu( this );
    Q_CHECK_PTR( contextMenu );
    QLabel *caption = new QLabel( "<font color=darkblue><b>"
	" M e n u</b></font>", this );
//        QLabel *caption = new QLabel( "<font color=darkblue><u><b>"
//	" Menu</b></u></font>", this );

    caption->setFrameStyle (QLabel::Panel|QLabel::Raised);
    caption->setAlignment( Qt::AlignCenter );
    caption->setPalette(p2);
//    caption->setBackgroundColor( QColor( 94, 128, 180 ) );

    contextMenu->setFrameStyle (QLabel::WinPanel|QLabel::Raised);
    contextMenu->insertItem( caption );
    contextMenu->insertItem( "&Quit - pass",  this, SLOT(exitPass()), CTRL+Key_Q );
    contextMenu->insertItem( "E&xit - fail",  this, SLOT(exitFail()), CTRL+Key_X );
    contextMenu->exec( QCursor::pos() );
    delete contextMenu;
}


void Clock::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
	qApp->exit(0);
}

void Clock::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
	qApp->exit(1);
}


