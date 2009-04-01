/****************************************************************************
** $Id: lineedits.cpp,v 1.1.1.1 2008/04/14 09:01:46 b06080 Exp $
**
** Copyright (C) 1992-2003 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/*================================================================================================*/
/**
    @file   lineedits.cpp

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

#include "lineedits.h"

#include <qlineedit.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qvalidator.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qcursor.h>
#include <qpopupmenu.h>

/*
 * Constructor
 *
 * Creates child widgets of the LineEdits widget
 */


#include <qapplication.h>

LineEdits::LineEdits( QWidget *parent, const char *name )
    : QGroupBox( 0, Horizontal, "Line edits", parent, name )
{
	QApplication::setFont(QFont("Arial",10),true);
    setMargin( 10 );

    QVBoxLayout* box = new QVBoxLayout( layout() );

    QHBoxLayout *row1 = new QHBoxLayout( box );
    row1->setMargin( 5 );

    // Create a Label
    QLabel* label = new QLabel( "Echo Mode: ", this);
    row1->addWidget( label );

    // Create a Combobox with three items...
    combo1 = new QComboBox( FALSE, this );
    row1->addWidget( combo1 );
    combo1->insertItem( "Normal" );
    combo1->insertItem( "Password" );
    combo1->insertItem( "No Echo" );
    // ...and connect the activated() SIGNAL with the slotEchoChanged() SLOT to be able
    // to react when an item is selected
    connect( combo1, SIGNAL( activated( int ) ), this, SLOT( slotEchoChanged( int ) ) );

    // insert the first LineEdit
    lined1 = new QLineEdit( this );
    box->addWidget( lined1 );

    // another widget which is used for layouting
    QHBoxLayout *row2 = new QHBoxLayout( box );
    row2->setMargin( 5 );

    // and the second label
    label = new QLabel( "Validator: ", this );
    row2->addWidget( label );

    // A second Combobox with again three items...
    combo2 = new QComboBox( FALSE, this );
    row2->addWidget( combo2 );
    combo2->insertItem( "No Validator" );
    combo2->insertItem( "Integer Validator" );
    combo2->insertItem( "Double Validator" );
    // ...and again the activated() SIGNAL gets connected with a SLOT
    connect( combo2, SIGNAL( activated( int ) ), this, SLOT( slotValidatorChanged( int ) ) );

    // and the second LineEdit
    lined2 = new QLineEdit( this );
    box->addWidget( lined2 );

    // yet another widget which is used for layouting
    QHBoxLayout *row3 = new QHBoxLayout( box );
    row3->setMargin( 5 );

    // we need a label for this too
    label = new QLabel( "Alignment: ", this );
    row3->addWidget( label );

    // A combo box for setting alignment
    combo3 = new QComboBox( FALSE, this );
    row3->addWidget( combo3 );
    combo3->insertItem( "Left" );
    combo3->insertItem( "Centered" );
    combo3->insertItem( "Right" );
    // ...and again the activated() SIGNAL gets connected with a SLOT
    connect( combo3, SIGNAL( activated( int ) ), this, SLOT( slotAlignmentChanged( int ) ) );

    // and the third lineedit
    lined3 = new QLineEdit( this );
    box->addWidget( lined3 );

    // exactly the same for the fourth
    QHBoxLayout *row4 = new QHBoxLayout( box );
    row4->setMargin( 5 );

    // we need a label for this too
    label = new QLabel( "Input mask: ", this );
    row4->addWidget( label );

    // A combo box for choosing an input mask
    combo4 = new QComboBox( FALSE, this );
    row4->addWidget( combo4 );
    combo4->insertItem( "No mask" );
    combo4->insertItem( "Phone number" );
    combo4->insertItem( "ISO date" );
    combo4->insertItem( "License key" );

    // ...this time we use the activated( const QString & ) signal
    connect( combo4, SIGNAL( activated( int ) ),
	     this, SLOT( slotInputMaskChanged( int ) ) );

    // and the fourth lineedit
    lined4 = new QLineEdit( this );
    box->addWidget( lined4 );

    // last widget used for layouting
    QHBox *row5 = new QHBox( this );
    box->addWidget( row5 );
    row5->setMargin( 5 );

    // last label
    (void)new QLabel( "Read-Only: ", row5 );

    // A combo box for setting alignment
    combo5 = new QComboBox( FALSE, row5 );
    combo5->insertItem( "False" );
    combo5->insertItem( "True" );
    // ...and again the activated() SIGNAL gets connected with a SLOT
    connect( combo5, SIGNAL( activated( int ) ), this, SLOT( slotReadOnlyChanged( int ) ) );

    // and the last lineedit
    lined5 = new QLineEdit( this );
    box->addWidget( lined5 );

    // give the first LineEdit the focus at the beginning
    lined1->setFocus();
}

/*
 * SLOT slotEchoChanged( int i )
 *
 * i contains the number of the item which the user has been chosen in the
 * first Combobox. According to this value, we set the Echo-Mode for the
 * first LineEdit.
 */

void LineEdits::slotEchoChanged( int i )
{
    switch ( i ) {
    case 0:
	lined1->setEchoMode( QLineEdit::Normal );
        break;
    case 1:
	lined1->setEchoMode( QLineEdit::Password );
        break;
    case 2:
	lined1->setEchoMode( QLineEdit::NoEcho );
        break;
    }

    lined1->setFocus();
}

/*
 * SLOT slotValidatorChanged( int i )
 *
 * i contains the number of the item which the user has been chosen in the
 * second Combobox. According to this value, we set a validator for the
 * second LineEdit. A validator checks in a LineEdit each character which
 * the user enters and accepts it if it is valid, else the character gets
 * ignored and not inserted into the lineedit.
 */

void LineEdits::slotValidatorChanged( int i )
{
    switch ( i ) {
    case 0:
	lined2->setValidator( 0 );
        break;
    case 1:
	lined2->setValidator( new QIntValidator( lined2 ) );
        break;
    case 2:
	lined2->setValidator( new QDoubleValidator( -999.0, 999.0, 2,
						    lined2 ) );
        break;
    }

    lined2->setText( "" );
    lined2->setFocus();
}


/*
 * SLOT slotAlignmentChanged( int i )
 *
 * i contains the number of the item which the user has been chosen in
 * the third Combobox.  According to this value, we set an alignment
 * third LineEdit.
 */

void LineEdits::slotAlignmentChanged( int i )
{
    switch ( i ) {
    case 0:
	lined3->setAlignment( QLineEdit::AlignLeft );
        break;
    case 1:
	lined3->setAlignment( QLineEdit::AlignCenter );
        break;
    case 2:
	lined3->setAlignment( QLineEdit::AlignRight );
        break;
    }

    lined3->setFocus();
}

/*
 * SLOT slotInputMaskChanged( const QString &mask )
 *
 * i contains the number of the item which the user has been chosen in
 * the third Combobox.  According to this value, we set an input mask on
 * third LineEdit.
 */

void LineEdits::slotInputMaskChanged( int i )
{
    switch( i ) {
    case 0:
	lined4->setInputMask( QString::null );
	break;
    case 1:
	lined4->setInputMask( "+99 99 99 99 99;_" );
	break;
    case 2:
	lined4->setInputMask( "0000-00-00" );
	lined4->setText( "00000000" );
	lined4->setCursorPosition( 0 );
	break;
    case 3:
	lined4->setInputMask( ">AAAAA-AAAAA-AAAAA-AAAAA-AAAAA;#" );
	break;
    }
    lined4->setFocus();
}

/*
 * SLOT slotReadOnlyChanged( int i )
 *
 * i contains the number of the item which the user has been chosen in
 * the fourth Combobox.  According to this value, we toggle read-only.
 */

void LineEdits::slotReadOnlyChanged( int i )
{
    switch ( i ) {
    case 0:
	lined5->setReadOnly( FALSE );
        break;
    case 1:
	lined5->setReadOnly( TRUE );
        break;
    }

    lined5->setFocus();
}


extern "C"{
    #include "test.h"
}


extern char *TCID;

void LineEdits::keyPressEvent(QKeyEvent *e){
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

void LineEdits::contextMenuEvent( QContextMenuEvent * )
{
   QColor x2(202,202,202);
   QColorGroup g2(black,x2,x2.light(),x2.dark(),x2.dark(120),white,white);
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


void LineEdits::exitPass()
{

	VT_rv=TPASS;
	close();

}

void LineEdits::exitFail()
{
  VT_rv=TFAIL;
	close();

}

