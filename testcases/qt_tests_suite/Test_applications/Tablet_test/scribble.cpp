/****************************************************************************
** $Id: scribble.cpp,v 1.1.1.1 2008/04/14 09:01:41 b06080 Exp $
**
** Copyright ( C ) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/*================================================================================================*/
/**
    @file   scribble.cpp

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

#include "canvas.h"
#include "scribble.h"
#include <qmessagebox.h>

#include <qapplication.h>
#include <qevent.h>
#include <qpainter.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include <qtooltip.h>
#include <qrect.h>
#include <qpoint.h>
#include <qcolordialog.h>
#include <qfiledialog.h>
#include <qcursor.h>
#include <qimage.h>
#include <qstrlist.h>
#include <qpopupmenu.h>
#include <qintdict.h>
#include <qmenubar.h>
#include <qlabel.h>




Scribble::Scribble( QWidget *parent, const char *name )
    : QMainWindow( parent, name )
{
    canvas = new Canvas( this );
    setCentralWidget( canvas );

    QToolBar *tools = new QToolBar( this );

    eSave = new QToolButton( QPixmap(), "Save", "Save as PNG image", this, SLOT( slotFile() ), tools );
    eSave->setText( "File..." );

    tools->addSeparator();
  
    bSave = new QToolButton( QPixmap(), "Save", "Save as PNG image", this, SLOT( slotSave() ), tools );
    bSave->setText( "Save as..." );

    tools->addSeparator();

    bPColor = new QToolButton( QPixmap(), "Choose Pen Color", "Choose Pen Color", this, SLOT( slotColor() ), tools );
    bPColor->setText( "Choose Pen Color..." );

    tools->addSeparator();

    
    bPWidth = new QSpinBox( 1, 20, 1, tools );
    QToolTip::add( bPWidth, "Choose Pen Width" );
    connect( bPWidth, SIGNAL( valueChanged( int ) ), this, SLOT( slotWidth( int ) ) );
    bPWidth->setValue( 3 );

    tools->addSeparator();

    bClear = new QToolButton( QPixmap(), "Clear Screen", "Clear Screen", this, SLOT( slotClear() ), tools );
    bClear->setText( "Clear Screen" );
}

/*

Scribble::Scribble( QWidget *parent, const char *name )
    : QMainWindow( parent, name )
{
    canvas = new Canvas( this );
    setCentralWidget( canvas );

    QToolBar *tools = new QToolBar( this );

    bSave = new QPushButton( "Save as...", tools );

    tools->addSeparator();

    bPColor = new QPushButton( "Choose Pen Color...", tools );
    //    bPColor->setText( "Choose Pen Color..." );

    tools->addSeparator();

    bPWidth = new QSpinBox( 1, 20, 1, tools );
    QToolTip::add( bPWidth, "Choose Pen Width" );
    connect( bPWidth, SIGNAL( valueChanged( int ) ), this, SLOT( slotWidth( int ) ) );
    bPWidth->setValue( 3 );

    tools->addSeparator();

    bClear = new QPushButton( "Clear Screen", tools );
    QObject::connect( bSave, SIGNAL( clicked() ), this, SLOT( slotSave() ) );
    QObject::connect( bPColor, SIGNAL( clicked() ), this, SLOT( slotColor() ) );
    QObject::connect( bClear, SIGNAL( clicked() ), this, SLOT( slotClear() ) );
		
}

  */
void Scribble::slotSave()
{
    QPopupMenu *menu = new QPopupMenu( 0 );
    QIntDict<QString> formats;
    formats.setAutoDelete( TRUE );

    for ( unsigned int i = 0; i < QImageIO::outputFormats().count(); i++ ) {
	QString str = QString( QImageIO::outputFormats().at( i ) );
	formats.insert( menu->insertItem( QString( "%1..." ).arg( str ) ), new QString( str ) );
    }

    menu->setMouseTracking( TRUE );
    int id = menu->exec( bSave->mapToGlobal( QPoint( 0, bSave->height() + 1 ) ) );

    if ( id != -1 ) {
	QString format = *formats[ id ];

	QString filename = QFileDialog::getSaveFileName( QString::null, QString( "*.%1" ).arg( format.lower() ), this );
	if ( !filename.isEmpty() )
	    canvas->save( filename, format );
    }
  
    delete menu;
}


void Scribble::slotFile()
{ 
    QPopupMenu *menu = new QPopupMenu( this );
    menu->insertItem( "&Quit - pass", this, SLOT(exitPass()), CTRL+Key_F11);
    menu->insertItem( "E&xit - fail", this, SLOT(exitFail()), CTRL+Key_F12);
    menu->setMouseTracking( TRUE );
    menu->exec( eSave->mapToGlobal( QPoint( 0, eSave->height() + 1 ) ) );

    delete menu; 
}

void Scribble::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
  qApp->exit(0);
}

void Scribble::exitFail()
{
 	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
  qApp->exit(1);
}

void Scribble::slotColor()
{
    QColor c = QColorDialog::getColor( canvas->penColor(), this );
    canvas->setPenColor( c );
}

void Scribble::slotWidth( int w )
{
    canvas->setPenWidth( w );
}

void Scribble::slotClear()
{
    canvas->clearScreen();
}
