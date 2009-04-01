/****************************************************************************
** $Id: lifedlg.cpp,v 1.1.1.1 2008/04/14 09:01:44 b06080 Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/*================================================================================================*/
/**
    @file   lifedlg.cpp

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
Irina Inkina                27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

#include "lifedlg.h"
#include <qapplication.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qslider.h>
#include <qcombobox.h>
#include <qdatetime.h>
#include <stdlib.h>

#include "patterns.cpp"
#include <qcursor.h>
#include <qpopupmenu.h>
#include <qapplication.h>
#include <qmessagebox.h>

// A simple timer which has a pause and a setSpeed slot

LifeTimer::LifeTimer( QWidget *parent ) : QTimer( parent ), interval( 500 )
{
    start( interval );
}


void LifeTimer::pause( bool stopIt )
{
    if ( stopIt )
	stop();
    else
	start( interval );
}


void LifeTimer::setSpeed( int speed )
{
    interval = MAXSPEED - speed; 
    if ( isActive() )
	changeInterval( interval );
}


// A top-level container widget to organize the others

LifeDialog::LifeDialog( int scale, QWidget * parent, const char * name )
    : QWidget( parent, name )
{
    QFont f;
    f.setPointSize(10);
    qb_1 = new QPushButton( "Quit!", this );
    qb = new QPushButton( "Exit!", this );
    cb = new QComboBox( this, "comboBox" );
    life = new LifeWidget(scale, this);
    life->move( SIDEBORDER, TOPBORDER );
    qb->setFont(f);
    qb_1->setFont(f);
    qb->setGeometry(5,24,50, 20 );
    qb_1->setGeometry(5,2,50, 20 );
    qb->setFocus();
    
//11    connect( qb, SIGNAL(clicked()), qApp, SLOT(quit()) );

    connect( qb, SIGNAL(clicked()), this, SLOT(exitPass()) );
    connect( qb_1, SIGNAL(clicked()), this, SLOT(exitFail()) );
//11 qb->setGeometry( SIDEBORDER-15, SIDEBORDER, qb->sizeHint().width()-15, 25 );

    timer = new LifeTimer( this );

    connect( timer, SIGNAL(timeout()), life, SLOT(nextGeneration()) );
    pb = new QPushButton( "Pause", this );
    pb->setToggleButton( TRUE );
    connect( pb, SIGNAL(toggled(bool)), timer, SLOT(pause(bool)) );
    pb->resize( pb->sizeHint().width(), 25 );
    pb->move( width() - SIDEBORDER - pb->width(), SIDEBORDER );

    sp = new QLabel( "Speed:", this );
    sp->adjustSize();
    sp->setMinimumSize( 20, 15 );
    sp->move( SIDEBORDER, 45 );
    scroll = new QSlider( 0, LifeTimer::MAXSPEED, 50,
			     LifeTimer::MAXSPEED / 2,
			     QSlider::Horizontal, this );
    connect( scroll, SIGNAL(valueChanged(int)),
	     timer,  SLOT(setSpeed(int)) );
  
    scroll->move( sp->width() + 2 * SIDEBORDER, 45 );
    scroll->resize( 200, 15 );

    life->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    life->show();

    srand( QTime(0,0,0).msecsTo(QTime::currentTime()) );
    int sel =  rand() % NPATS;
    getPattern( sel );
     
//1    cb->move( 2*SIDEBORDER + qb->width(), SIDEBORDER);
    cb->move( 2*SIDEBORDER + 65, SIDEBORDER);
    cb->insertItem( "Glider Gun " );
    cb->insertItem( "Figure Eight " );
    cb->insertItem( "Pulsar " );
    cb->insertItem( "Barber Pole P2 " );
    cb->insertItem( "Achim P5 " );
    cb->insertItem( "Hertz P4 " );
    cb->insertItem( "Tumbler " );
    cb->insertItem( "Pulse1 P4" );
    cb->insertItem( "Shining Flower P5 " );
    cb->insertItem( "Pulse2 P6 " );
    cb->insertItem( "Pinwheel, Clock P4 " );
    cb->insertItem( "Pentadecatholon " );
    cb->insertItem( "Piston " );
    cb->insertItem( "Piston2 " );
    cb->insertItem( "Switch Engine " );
    cb->insertItem( "Gears (Gear, Flywheel, Blinker) " );
    cb->insertItem( "Turbine8 " );
    cb->insertItem( "P16 " );
    cb->insertItem( "Puffer " );
    cb->insertItem( "Escort " );
    cb->insertItem( "Dart Speed 1/3 " );
    cb->insertItem( "Period 4 Speed 1/2 " );
    cb->insertItem( "Another Period 4 Speed 1/2 " );
    cb->insertItem( "Smallest Known Period 3 Spaceship Speed 1/3 " );
    cb->insertItem( "Turtle Speed 1/3 " );
    cb->insertItem( "Smallest Known Period 5 Speed 2/5 " );
    cb->insertItem( "Sym Puffer " );
    cb->insertItem( "], Near Ship, Pi Heptomino " );
    cb->insertItem( "R Pentomino " );
    cb->setAutoResize( FALSE );
    cb->setCurrentItem( sel );
    cb->show();
    connect( cb, SIGNAL(activated(int)), SLOT(getPattern(int)) );
       
    QSize s;
    s = life->minimumSize();
    setMinimumSize( s.width() + 2 * SIDEBORDER, 
		    s.height() + TOPBORDER + SIDEBORDER );
    s = life->maximumSize();
    setMaximumSize( s.width() + 2 * SIDEBORDER, 
		    s.height() + TOPBORDER + SIDEBORDER );
    s = life->sizeIncrement();
    setSizeIncrement( s.width(), s.height() );
  
    resize( QMIN(512, qApp->desktop()->width()),
	    QMIN(480, qApp->desktop()->height()) );

}

void LifeDialog::exitPass()
{
 QMessageBox::information(this, "Test Pass", "Exiting with test pass");
 qApp->exit(0); 
}

void LifeDialog::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
  qApp->exit(1);
}

void LifeDialog::contextMenuEvent( QContextMenuEvent * )
{
   QColor x2(202,202,202);
   QColorGroup g2(black,x2,x2.light(),x2.dark(),x2.dark(120),white/*black*/,white);
   QPalette p2(g2,g2,g2);


    QPopupMenu*	contextMenu = new QPopupMenu( this );
    Q_CHECK_PTR( contextMenu );
    QLabel *caption = new QLabel( "<font color=darkblue><b>"
	" M e n u</b></font>", this );

    caption->setFrameStyle (QLabel::Panel|QLabel::Raised);
    caption->setAlignment( Qt::AlignCenter );
    caption->setPalette(p2);
    caption->setBackgroundColor( QColor( 94, 128, 180 ) );

    contextMenu->setFrameStyle (QLabel::WinPanel|QLabel::Raised);
    contextMenu->insertItem( caption );
    contextMenu->insertItem( "&Quit - pass",  this, SLOT(exitPass()), CTRL+Key_Q );
    contextMenu->insertItem( "E&xit - fail",  this, SLOT(exitFail()), CTRL+Key_X );
    contextMenu->exec( QCursor::pos() );
    delete contextMenu;
}




void LifeDialog::resizeEvent( QResizeEvent * e )
{
    life->resize( e->size() - QSize( 2 * SIDEBORDER, TOPBORDER + SIDEBORDER ));
    pb->move( e->size().width() - SIDEBORDER - pb->width(), SIDEBORDER );
    scroll->resize( e->size().width() - sp->width() - 3 * SIDEBORDER,
		    scroll->height() );
    cb->resize( width() - 4*SIDEBORDER -65- pb->width()  , 25 );
//1    cb->resize( width() - 4*SIDEBORDER - qb->width() - pb->width()  , 25 );
}


// Adapted from xlock, see pattern.cpp for copyright info.

void LifeDialog::getPattern( int pat )
{
    life->clear();
    int i = pat % NPATS;
    int col;
    int * patptr = &patterns[i][0];
    while ( (col = *patptr++) != 127 ) {
	int row = *patptr++;
	col += life->maxCol() / 2;
	row += life->maxRow() / 2;
	life->setPoint( col, row );
    }
}
