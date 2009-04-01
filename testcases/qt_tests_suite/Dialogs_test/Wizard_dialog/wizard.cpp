/****************************************************************************
** $Id: wizard.cpp,v 1.1.1.1 2008/04/14 09:01:38 b06080 Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
/*================================================================================================*/
/**
    @file   wizard.cpp

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


#include "wizard.h"

#include <qcursor.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>

#include <qwidget.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qvalidator.h>
#include <qapplication.h>

Wizard::Wizard( QWidget *parent, const char *name )
    : QWizard( parent, name, TRUE )
{
    setupPage1();
    setupPage2();
   setupPage3();

    key->setFocus();
    setFixedWidth(310);
}


void Wizard::setupPage1()
{
   QColor x2(202,202,202);
   QColorGroup g2(black,x2,x2.light(),x2.dark(),x2.dark(120),white,white);
   QPalette p2(g2,g2,g2);

    QFont _font;
    _font.setBold( TRUE);
    _font.setPointSize( 10);

    page1 = new QHBox( this );
    page1->setSpacing(8);

    QLabel *info = new QLabel( page1 );
    info->setFont(_font );
//    info->setBackgroundColor( QColor( 94, 128, 180 ) );
    info->setFrameStyle (QLabel::Panel|QLabel::Sunken);//Raised);
    info->setPalette( p2 );
    info->setMargin( 11 );
//    info->setPalette( yellow );
    info->setText( "Enter your personal\n"
                   "key here.\n\n"
                   "Your personal key\n"
                   "consists of 4 digits" );
//    info->setMaximumWidth( info->sizeHint().width() );

    QVBox *page = new QVBox( page1 );

    QHBox *row1 = new QHBox( page );

    (void)new QLabel( "Key:", row1 );

    key = new QLineEdit( row1 );
    key->setMaxLength( 4 );
    key->setValidator( new QIntValidator( 1000, 9999, key ) );
  
    connect( key, SIGNAL( textChanged( const QString & ) ),
	     this, SLOT( keyChanged( const QString & ) ) );
  
    addPage( page1, "Personal Key" );

    setNextEnabled( page1, FALSE );
    setHelpEnabled( page1, FALSE );
}

void Wizard::setupPage2()
{
   QColor x2(202,202,202);
   QColorGroup g2(black,x2,x2.light(),x2.dark(),x2.dark(120),white,white);
   QPalette p2(g2,g2,g2);

    QFont _font;
    _font.setBold( TRUE);
    _font.setPointSize( 10);
 
    page2 = new QHBox( this );
    page2->setSpacing(8);
   page2->setGeometry(0,0,width()-150,height()-30 );
    page2->setFont( _font );

    QLabel *info = new QLabel( page2 );
    info->setFrameStyle (QLabel::Panel|QLabel::Sunken);//Raised);
    info->setMargin( 6 );
    QFont l_font(  info->font() );
    l_font.setBold( TRUE);
    l_font.setPointSize( 10);
    info->setFont( l_font );
    info->setBackgroundColor( QColor( 94, 128, 180 ) );         
    info->setText(( "Enter your\npersonal\ndata here\n\nThe required fields\nare First Name\nLast Name\n,and E-Mail."));
    info->setAlignment( AlignLeft | WordBreak);
//    info->setPalette(p2);
  
  /*

    QLabel *info = new QLabel( page2 );
    info->setMargin( 11 );
    info->setPalette( yellow );
    info->setText( "\n"
                   "Enter your personal\n"
                   "data here.\n\n"
                   "The required fields are\n"
                   "First Name, Last Name \n"
                   "and E-Mail.\n" );
    info->setMaximumWidth( info->sizeHint().width() ); 
*/
    QVBox *page = new QVBox( page2 );

    QHBox *row1 = new QHBox( page );
    QHBox *row2 = new QHBox( page );
    QHBox *row3 = new QHBox( page );
    QHBox *row4 = new QHBox( page );
    QHBox *row5 = new QHBox( page );

    QLabel *label1 = new QLabel( " First Name: ", row1 );
    label1->setAlignment( Qt::AlignVCenter );
    QLabel *label2 = new QLabel( " Last Name: ", row2 );
    label2->setAlignment( Qt::AlignVCenter );
    QLabel *label3 = new QLabel( " Address: ", row3 );
    label3->setAlignment( Qt::AlignVCenter );
    QLabel *label4 = new QLabel( " Phone Number: ", row4 );
    label4->setAlignment( Qt::AlignVCenter );
    QLabel *label5 = new QLabel( " E-Mail: ", row5 );
    label5->setAlignment( Qt::AlignVCenter );

    label1->setMinimumWidth( label4->sizeHint().width() );
    label2->setMinimumWidth( label4->sizeHint().width() );
    label3->setMinimumWidth( label4->sizeHint().width() );
    label4->setMinimumWidth( label4->sizeHint().width() );
    label5->setMinimumWidth( label4->sizeHint().width() );

    firstName = new QLineEdit( row1 );
    firstName->setMinimumWidth( 10);
    lastName = new QLineEdit( row2 );
    lastName->setMinimumWidth( 10);
    address = new QLineEdit( row3 );
    address->setMinimumWidth( 10);
    phone = new QLineEdit( row4 );
    phone->setMinimumWidth( 10);
    email = new QLineEdit( row5 );
    email->setMinimumWidth( 10);

    connect( firstName, SIGNAL( textChanged( const QString & ) ),
	     this, SLOT( dataChanged( const QString & ) ) );
    connect( lastName, SIGNAL( textChanged( const QString & ) ),
	     this, SLOT( dataChanged( const QString & ) ) );
    connect( email, SIGNAL( textChanged( const QString & ) ),
	     this, SLOT( dataChanged( const QString & ) ) );

    addPage( page2, "Personal Data" );

    setHelpEnabled( page2, FALSE );  
}

void Wizard::setupPage3()
{
    QFont l_font;
    l_font.setBold( TRUE);
    l_font.setPointSize( 10);

    page3 = new QHBox( this );
    page3->setSpacing(8);
    page3->setFont( l_font );
    
    QLabel *info = new QLabel( page3 );
    info->setFrameStyle (QLabel::Panel|QLabel::Sunken);
    info->setMargin( 6 );
    info->setFont( l_font );
//    info->setPalette( yellow );
    info->setBackgroundColor( QColor( 94, 128, 180 ) );
    info->setText( "\n"
                   "Look here to see of\n"
                   "the data you entered\n"
                   "is correct. To confirm,\n"
                   "press the [Finish] button\n"
                   "else go back to correct\n"
                   "mistakes." );
    info->setMargin( 11 );
    info->setAlignment( AlignTop|AlignLeft );
    info->setMaximumWidth( info->sizeHint().width() );

    QVBox *page = new QVBox( page3 );

    QHBox *row1 = new QHBox( page );
    QHBox *row2 = new QHBox( page );
    QHBox *row3 = new QHBox( page );
    QHBox *row4 = new QHBox( page );
    QHBox *row5 = new QHBox( page );
    QHBox *row6 = new QHBox( page );

    QLabel *label1 = new QLabel( " Personal Key: ", row1 );
    label1->setAlignment( Qt::AlignVCenter );
    QLabel *label2 = new QLabel( " First Name: ", row2 );
    label2->setAlignment( Qt::AlignVCenter );
    QLabel *label3 = new QLabel( " Last Name: ", row3 );
    label3->setAlignment( Qt::AlignVCenter );
    QLabel *label4 = new QLabel( " Address: ", row4 );
    label4->setAlignment( Qt::AlignVCenter );
    QLabel *label5 = new QLabel( " Phone Number: ", row5 );
    label5->setAlignment( Qt::AlignVCenter );
    QLabel *label6 = new QLabel( " E-Mail: ", row6 );
    label6->setAlignment( Qt::AlignVCenter );

    label1->setMinimumWidth( label1->sizeHint().width() );
    label2->setMinimumWidth( label1->sizeHint().width() );
    label3->setMinimumWidth( label1->sizeHint().width() );
    label4->setMinimumWidth( label1->sizeHint().width() );
    label5->setMinimumWidth( label1->sizeHint().width() );
    label6->setMinimumWidth( label1->sizeHint().width() );

    lKey = new QLabel( row1 );
    lFirstName = new QLabel( row2 );
    lLastName = new QLabel( row3 );
    lAddress = new QLabel( row4 );
    lPhone = new QLabel( row5 );
    lEmail = new QLabel( row6 );

    addPage( page3, "Finish" );

    setFinishEnabled( page3, TRUE );
    setHelpEnabled( page3, FALSE );
}

void Wizard::showPage( QWidget* page )
{
    if ( page == page1 ) {
    } else if ( page == page2 ) {
    } else if ( page == page3 ) {
        lKey->setText( key->text() );
        lFirstName->setText( firstName->text() );
        lLastName->setText( lastName->text() );
        lAddress->setText( address->text() );
        lPhone->setText( phone->text() );
        lEmail->setText( email->text() );
    } 
  
    QWizard::showPage(page);

    if ( page == page1 ) {
        keyChanged( key->text() );
        key->setFocus();
    } else if ( page == page2 ) {
        dataChanged( firstName->text() );
        firstName->setFocus();
    } else if ( page == page3 ) {
        finishButton()->setEnabled( TRUE );
        finishButton()->setFocus();
    }      
}

void Wizard::keyChanged( const QString &text )
{
    QString t = text;
    int p = 0;
    bool on = ( key->validator()->validate(t, p) == QValidator::Acceptable );
    nextButton()->setEnabled( on );
}

void Wizard::dataChanged( const QString & )
{
    if ( !firstName->text().isEmpty() &&
         !lastName->text().isEmpty() &&
         !email->text().isEmpty() )
        nextButton()->setEnabled( TRUE );
    else
        nextButton()->setEnabled( FALSE );
}

void Wizard::contextMenuEvent( QContextMenuEvent * )
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
    contextMenu->insertItem( "&Quit - pass",  this, SLOT(exitPass()), CTRL+Key_Q );
    contextMenu->insertItem( "E&xit - fail",  this, SLOT(exitFail()), CTRL+Key_X );
    contextMenu->exec( QCursor::pos() );
    delete contextMenu;
}


void Wizard::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
	qApp->exit(0);
}

void Wizard::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
	qApp->exit(1);
}



