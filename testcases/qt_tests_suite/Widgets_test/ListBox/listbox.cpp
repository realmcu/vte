/****************************************************************************
** $Id: listbox.cpp,v 1.1.1.1 2008/04/14 09:01:46 b06080 Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/*================================================================================================*/
/**
    @file   listbox.cpp

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

#include "listbox.h"

#include <qcursor.h>
#include <qpopupmenu.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlistbox.h>
#include <qbuttongroup.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qapplication.h>


ListBoxDemo::ListBoxDemo()
    : QWidget( 0, 0 )
{
   setFont(QFont("Arial",8));

QGridLayout * g = new QGridLayout( this, 2, 2, 5);

    g->addWidget( new QLabel( "<b>Configuration:</b>", this ), 0, 0 );
    g->addWidget( new QLabel( "<b>Result:</b>", this ), 0, 1 );

    l = new QListBox( this );
    g->addWidget( l, 1, 1 );
    l->setFocusPolicy( QWidget::StrongFocus );

    QVBoxLayout * v = new QVBoxLayout;
    g->addLayout( v, 1, 0 );

    QRadioButton * b;
    bg = new QButtonGroup( 0 );

    b = new QRadioButton( "Fixed columns",   this );
    bg->insert( b );
    v->addWidget( b );
    b->setChecked( TRUE );
    connect( b, SIGNAL(clicked()), this, SLOT(setNumCols()) );
    QHBoxLayout * h = new QHBoxLayout;
    v->addLayout( h );
    h->addSpacing( 30 );
//    h->addSpacing( 100 );
    h->addWidget( new QLabel( "Columns:", this ) );
    columns = new QSpinBox( this );
    h->addWidget( columns );

    v->addSpacing( 12 );

    b = new QRadioButton( "As fit on screen",  this );
    bg->insert( b );
    v->addWidget( b );
    connect( b, SIGNAL(clicked()), this, SLOT(setColsByWidth()) );

    v->addSpacing( 6 );

    b = new QRadioButton( "Fixed rows",     this );
    bg->insert( b );
    v->addWidget( b );
    connect( b, SIGNAL(clicked()), this, SLOT(setNumRows()) );
    h = new QHBoxLayout;
    v->addLayout( h);
    h->addSpacing( 30 );
//    h->addSpacing( 100 );
    h->addWidget( new QLabel( "Rows:", this ) );
    rows = new QSpinBox( this );
    rows->setEnabled( FALSE );
    h->addWidget( rows );   

//    v->addSpacing( 12 );

    b = new QRadioButton( "As fit on screen",
                          this );
    bg->insert( b );
    v->addWidget( b );
    connect( b, SIGNAL(clicked()), this, SLOT(setRowsByHeight()) );

 //   v->addSpacing( 12 );

    QCheckBox * cb = new QCheckBox( "Variable-height", this );
    cb->setChecked( TRUE );
    connect( cb, SIGNAL(toggled(bool)), this, SLOT(setVariableHeight(bool)) );
    v->addWidget( cb );
    //v->addSpacing( 6 );

    cb = new QCheckBox( "Variable-width", this );
    connect( cb, SIGNAL(toggled(bool)), this, SLOT(setVariableWidth(bool)) );
    v->addWidget( cb );

    cb = new QCheckBox( "Multi-Selection", this );
    connect( cb, SIGNAL(toggled(bool)), this, SLOT(setMultiSelection(bool)) );
    v->addWidget( cb );

    v->addWidget( new QLabel( "<b>Sort Order:</b>", this ) );

h = new QHBoxLayout;
    v->addLayout( h );


    QPushButton *pb = new QPushButton( "A-Z", this );
    connect( pb, SIGNAL( clicked() ), this, SLOT( sortAscending() ) );
    h->addWidget( pb );

    pb = new QPushButton( "Z-A", this );
    connect( pb, SIGNAL( clicked() ), this, SLOT( sortDescending() ) );
    h->addWidget( pb );

    v->addStretch( 100 );

    int i = 0;
    while( ++i <= 2560 )
        l->insertItem( QString::fromLatin1( "Item " ) + QString::number( i ),
                       i );
    columns->setRange( 1, 256 );
    columns->setValue( 1 );
    rows->setRange( 1, 256 );
    rows->setValue( 256 );

    connect( columns, SIGNAL(valueChanged(int)), this, SLOT(setNumCols()) );
    connect( rows, SIGNAL(valueChanged(int)), this, SLOT(setNumRows()) );
//setFixedSize(240,320);
}


ListBoxDemo::~ListBoxDemo()
{
    delete bg;
}


void ListBoxDemo::setNumRows()
{
    columns->setEnabled( FALSE );
    rows->setEnabled( TRUE );
    l->setRowMode( rows->value() );
}


void ListBoxDemo::setNumCols()
{
    columns->setEnabled( TRUE );
    rows->setEnabled( FALSE );
    l->setColumnMode( columns->value() );
}


void ListBoxDemo::setRowsByHeight()
{
    columns->setEnabled( FALSE );
    rows->setEnabled( FALSE );
    l->setRowMode( QListBox::FitToHeight );
}


void ListBoxDemo::setColsByWidth()
{
    columns->setEnabled( FALSE );
    rows->setEnabled( FALSE );
    l->setColumnMode( QListBox::FitToWidth );
}


void ListBoxDemo::setVariableWidth( bool b )
{
    l->setVariableWidth( b );
}


void ListBoxDemo::setVariableHeight( bool b )
{
    l->setVariableHeight( b );
}

void ListBoxDemo::setMultiSelection( bool b )
{
    l->clearSelection();
    l->setSelectionMode( b ? QListBox::Extended : QListBox::Single );
}

void ListBoxDemo::sortAscending()
{
    l->sort( TRUE );
}

void ListBoxDemo::sortDescending()
{
    l->sort( FALSE );
}

extern "C"{
    #include "test.h"
}


extern char *TCID;

void ListBoxDemo::keyPressEvent(QKeyEvent *e){
    if((e->key() == Qt::Key_F11)&&(e->state() & ControlButton )){
	VT_rv=TPASS;
	e->accept();
	close();
	return;
    }
    if((e->key() == Qt::Key_F12)&&(e->state() & ControlButton)){
	VT_rv=TFAIL;
	e->accept();
	close();
	return;
    }
    e->ignore();
}

void ListBoxDemo::contextMenuEvent( QContextMenuEvent * )
{
   QColor x2(202,202,202);
   QColorGroup g2(black,x2,x2.light(),x2.dark(),x2.dark(120),white,white);
   QPalette p2(g2,g2,g2);


    QPopupMenu*	contextMenu = new QPopupMenu( this );
    Q_CHECK_PTR( contextMenu );
    QLabel *caption = new QLabel( "<font color=darkblue><b>"
	" M e n u</b></font>", this );
    QFont f("Arial",10);
    contextMenu->setFont(f);
    caption->setFrameStyle (QLabel::Panel|QLabel::Raised);
    caption->setAlignment( Qt::AlignCenter );
    caption->setPalette(p2);
//    caption->setBackgroundColor( QColor( 94, 128, 180 ) );

    contextMenu->setFrameStyle (QLabel::WinPanel|QLabel::Raised);
    contextMenu->insertItem( caption );
    contextMenu->insertItem( "&Quit - pass",  this, SLOT(exitPass()), CTRL+Key_F11);//CTRL+Key_F11
    contextMenu->insertItem( "E&xit - fail",  this, SLOT(exitFail()), CTRL+Key_F12);//CTRL+Key_F12
    contextMenu->exec( QCursor::pos() );
    delete contextMenu;
}


void ListBoxDemo::exitPass()
{

	VT_rv=TPASS;
	close();

}

void ListBoxDemo::exitFail()
{
  VT_rv=TFAIL;
	close();

}


