/****************************************************************************
** $Id: buttongroups.cpp,v 1.1.1.1 2008/04/14 09:01:44 b06080 Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/*================================================================================================*/
/**
    @file   buttongroups.cpp

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

#include "buttongroups.h"
#include <qcursor.h>

#include <qpopupmenu.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qapplication.h>

/*
 * Constructor
 *
 * Creates all child widgets of the ButtonGroups window
 */

ButtonsGroups::ButtonsGroups( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
	QApplication::setFont(QFont("Arial",8),true);
    // Create Widgets which allow easy layouting
    QVBoxLayout *vbox = new QVBoxLayout( this, 11, 6 );
    QHBoxLayout *box1 = new QHBoxLayout( vbox );
    QHBoxLayout *box2 = new QHBoxLayout( vbox );

    // ------- first group

    // Create an exclusive button group
    QButtonGroup *bgrp1 = new QButtonGroup( 1, QGroupBox::Horizontal, "Radiobutins", this);
    box1->addWidget( bgrp1 );
    bgrp1->setExclusive( TRUE );

    // insert 3 radiobuttons
    QRadioButton *rb11 = new QRadioButton( "&Radiobutton", bgrp1 );
    rb11->setChecked( TRUE );
    (void)new QRadioButton( "R&adiobutton", bgrp1 );
    (void)new QRadioButton( "Ra&diobutton", bgrp1 );

    // ------- second group

    // Create a non-exclusive buttongroup
    QButtonGroup *bgrp2 = new QButtonGroup( 1, QGroupBox::Horizontal, "Checkboxes", this );
    box1->addWidget( bgrp2 );
    bgrp2->setExclusive( FALSE );

    // insert 3 checkboxes
    (void)new QCheckBox( "&Checkbox", bgrp2 );
    QCheckBox *cb12 = new QCheckBox( "C&heckbox", bgrp2 );
    cb12->setChecked( TRUE );
    QCheckBox *cb13 = new QCheckBox( "Triple &State", bgrp2 );
    cb13->setTristate( TRUE );
    cb13->setChecked( TRUE );

    // ------------ third group

    // create a buttongroup which is exclusive for radiobuttons and non-exclusive for all other buttons
    QButtonGroup *bgrp3 = new QButtonGroup( 1, QGroupBox::Horizontal, "Mixed", this );
    box2->addWidget( bgrp3 );
    bgrp3->setRadioButtonExclusive( TRUE );

    // insert three radiobuttons
    rb21 = new QRadioButton( "Rad&iobutton", bgrp3 );
    rb22 = new QRadioButton( "Radi&obutton", bgrp3 );
    rb23 = new QRadioButton( "Radio&button", bgrp3 );
    rb23->setChecked( TRUE );

    // insert a checkbox...
    state = new QCheckBox( "E&nable", bgrp3 );
    state->setChecked( TRUE );
    // ...and connect its SIGNAL clicked() with the SLOT slotChangeGrp3State()
    connect( state, SIGNAL( clicked() ), this, SLOT( slotChangeGrp3State() ) );

    // ------------ fourth group

    // create a groupbox which layouts its childs in a columns
    QGroupBox *bgrp4 = new QButtonGroup( 1, QGroupBox::Horizontal, "Buttons", this );
    box2->addWidget( bgrp4 );

    // insert four pushbuttons...
    (void)new QPushButton( "&Push Button", bgrp4, "push" );

    // now make the second one a toggle button
    QPushButton *tb2 = new QPushButton( "&Toggle Button", bgrp4, "toggle" );
    tb2->setToggleButton( TRUE );
    tb2->setOn( TRUE );

    // ... and make the third one a flat button
    QPushButton *tb3 = new QPushButton( "&Flat Button", bgrp4, "flat" );
    tb3->setFlat(TRUE);

    // .. and the fourth a button with a menu
    QPushButton *tb4 = new QPushButton( "Popup Button", bgrp4, "popup" );
    QPopupMenu *menu = new QPopupMenu(tb4);
    menu->insertItem("Item1", 0);
    menu->insertItem("Item2", 1);
    menu->insertItem("Item3", 2);
    menu->insertItem("Item4", 3);
    tb4->setPopup(menu);
    
    setFixedSize(240,320);   
}

/*
 * SLOT slotChangeGrp3State()
 *
 * enables/disables the radiobuttons of the third buttongroup
 */

void ButtonsGroups::slotChangeGrp3State()
{
    rb21->setEnabled( state->isChecked() );
    rb22->setEnabled( state->isChecked() );
    rb23->setEnabled( state->isChecked() );
}

extern "C"{
    #include "test.h"
}


extern char *TCID;

void ButtonsGroups::keyPressEvent(QKeyEvent *e){
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

void ButtonsGroups::contextMenuEvent( QContextMenuEvent * )
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
//    caption->setBackgroundColor( QColor( 94, 128, 180 ) );

    contextMenu->setFrameStyle (QLabel::WinPanel|QLabel::Raised);
    contextMenu->insertItem( caption );
    contextMenu->insertItem( "&Quit - pass",  this, SLOT(exitPass()), CTRL+Key_F11);//CTRL+Key_F11
    contextMenu->insertItem( "E&xit - fail",  this, SLOT(exitFail()), CTRL+Key_F12);//CTRL+Key_F12
    contextMenu->exec( QCursor::pos() );
    delete contextMenu;
}


void ButtonsGroups::exitPass()
{

	VT_rv=TPASS;
	close();

}

void ButtonsGroups::exitFail()
{
  VT_rv=TFAIL;
	close();

}

