/****************************************************************************
** $Id: table.cpp,v 1.1.1.1 2008/04/14 09:01:46 b06080 Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/*================================================================================================*/
/**
    @file   table.cpp

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

#include "table.h"

#include <qapplication.h>
#include <qtable.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qmessagebox.h>

// Qt logo: static const char *qtlogo_xpm[]
#include "qtlogo.xpm"

// Table size

const int numRows = 30;
const int numCols = 10;

// The program starts here.



Table::Table(QWidget* parent, const char* name) :
    QWidget(parent, name)
{
    QMenuBar *menu = new QMenuBar( this );
    menu->setGeometry(0,0,240,15);
    Q_CHECK_PTR( menu );
    QPopupMenu *file = new QPopupMenu;

    Q_CHECK_PTR( file );
    file->insertSeparator();
    file->insertItem( "&Quit - pass", this, SLOT(exitPass()), CTRL+Key_Q );
    file->insertItem( "E&xit - fail", this, SLOT(exitFail()), CTRL+Key_X );

    menu->insertItem( "&File", file );


    QTable *table=new QTable( numRows, numCols ,this);
    table->setGeometry(0,30,240,290);
    table->setFrameStyle((QFrame::WinPanel|QFrame::Raised));    

/*    QTable table( numRows, numCols ,this);
    table.setGeometry(0,0,200,300);
 */
    QHeader *header = table->horizontalHeader();
    header->setLabel( 0, QObject::tr( "Tiny" ), 40 );
    header->setLabel( 1, QObject::tr( "Checkboxes" ) );
    header->setLabel( 5, QObject::tr( "Combos" ) );
    header->setMovingEnabled(TRUE);
 
    QImage img( qtlogo_xpm );
    QPixmap pix = img.scaleHeight( table->rowHeight(3) );
    table->setPixmap( 3, 2, pix );
    table->setText( 3, 2, "A Pixmap" );

    QStringList comboEntries;
    comboEntries << "one" << "two" << "three" << "four";

    for ( int i = 0; i < numRows; ++i ){
	QComboTableItem * item = new QComboTableItem( table, comboEntries, FALSE );
	item->setCurrentItem( i % 4 );
	table->setItem( i, 5, item );
    }
    for ( int j = 0; j < numRows; ++j )
	table->setItem( j, 1, new QCheckTableItem( table, "Check me" ) );
  
}
void Table::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
  qApp->exit(1);
}

void Table::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");

qApp->exit(0);
}

int Table_main( int argc, char **argv )
{
    QApplication a( argc, argv );
    Table tb;
    a.setMainWidget(&tb);
    tb.resize(240,320);
    tb.show();
    return a.exec();

}
/*
int Table_main( int argc, char **argv )
{
    QApplication app( argc, argv );

    QTable table( numRows, numCols );
    table.setGeometry(0,0,240,320);
 
    QHeader *header = table.horizontalHeader();
    header->setLabel( 0, QObject::tr( "Tiny" ), 40 );
    header->setLabel( 1, QObject::tr( "Checkboxes" ) );
    header->setLabel( 5, QObject::tr( "Combos" ) );
    header->setMovingEnabled(TRUE);

    QImage img( qtlogo_xpm );
    QPixmap pix = img.scaleHeight( table.rowHeight(3) );
    table.setPixmap( 3, 2, pix );
    table.setText( 3, 2, "A Pixmap" );

    QStringList comboEntries;
    comboEntries << "one" << "two" << "three" << "four";

    for ( int i = 0; i < numRows; ++i ){
	QComboTableItem * item = new QComboTableItem( &table, comboEntries, FALSE );
	item->setCurrentItem( i % 4 );
	table.setItem( i, 5, item );
    }
    for ( int j = 0; j < numRows; ++j )
	table.setItem( j, 1, new QCheckTableItem( &table, "Check me" ) );

    app.setMainWidget( &table );
    table.show();
    return app.exec();
}
  */
  