/****************************************************************************
** $Id: tabdialog.cpp,v 1.1.1.1 2008/04/14 09:01:38 b06080 Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "tabdialog.h"

#include <qvbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qdatetime.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlistbox.h>
#include <qapplication.h>
#include <qmessagebox.h>

TabDialog::TabDialog( QWidget *parent, const char *name, const QString &_filename )
    : QTabDialog( parent, name ), filename( _filename ), fileinfo( filename )
{
    setupTab1();
    setupTab2();
    setupTab3();
    
    Test_Q = new QPushButton(this,"Test_Q" );
    Test_Q->setGeometry( QRect( 8, 289, 120, 36 ) );
//    Test_Q->setGeometry( QRect( 8, height()-18, 130, 38 ) );
    Test_Q->setBackgroundColor( QColor( 94, 128, 180 ) );
    QFont ButtonGroup_font(  Test_Q->font() );
    ButtonGroup_font.setPointSize( 12);
    ButtonGroup_font.setBold( TRUE );
    Test_Q->setText("Qiut - Pass");
    Test_Q->setFont(ButtonGroup_font);             //
    
   connect( this, SIGNAL( applyButtonPressed() ),this, SLOT( exitFail() ) );
   connect( Test_Q, SIGNAL( clicked() ), this, SLOT( exitPass() ) );
//   connect( this, SIGNAL( applyButtonPressed() ), qApp, SLOT( quit() ) );
   
}

void TabDialog::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
	qApp->exit(0);
}

void TabDialog::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
	qApp->exit(1);
}



void TabDialog::setupTab1()
{
    QVBox *tab1 = new QVBox( this );
    tab1->setMargin( 5 );

    (void)new QLabel( "Filename:", tab1 );
    QLineEdit *fname = new QLineEdit( filename, tab1 );
    fname->setFocus();

    (void)new QLabel( "Path:", tab1 );
    QLineEdit *path = new QLineEdit( fileinfo.dirPath( TRUE ), tab1 );
    path->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    (void)new QLabel( "Size:", tab1 );
    ulong kb = (ulong)(fileinfo.size()/1024);
    QLabel *size = new QLabel( QString( "%1 KB" ).arg( kb ), tab1 );
    size->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    (void)new QLabel( "Last Read:", tab1 );
    QLabel *lread = new QLabel( fileinfo.lastRead().toString(), tab1 );
    lread->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    (void)new QLabel( "Last Modified:", tab1 );
    QLabel *lmodif = new QLabel( fileinfo.lastModified().toString(), tab1 );
    lmodif->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    addTab( tab1, "General" );
}

void TabDialog::setupTab2()
{
    QVBox *tab2 = new QVBox( this );
    tab2->setMargin( 5 );

    QButtonGroup *bg = new QButtonGroup( 1, QGroupBox::Horizontal, "Permissions", tab2 );

    QCheckBox *readable = new QCheckBox( "Readable", bg );
    if ( fileinfo.isReadable() )
        readable->setChecked( TRUE );

    QCheckBox *writable = new QCheckBox( "Writeable", bg );
    if ( fileinfo.isWritable() )
        writable->setChecked( TRUE );

    QCheckBox *executable = new QCheckBox( "Executable", bg );
    if ( fileinfo.isExecutable() )
        executable->setChecked( TRUE );

    QButtonGroup *bg2 = new QButtonGroup( 2, QGroupBox::Horizontal, "Owner", tab2 );

    (void)new QLabel( "Owner", bg2 );
    QLabel *owner = new QLabel( fileinfo.owner(), bg2 );
    owner->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    (void)new QLabel( "Group", bg2 );
    QLabel *group = new QLabel( fileinfo.group(), bg2 );
    group->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    addTab( tab2, "Permissions" );
}

void TabDialog::setupTab3()
{
	/*
    QVBox *tab3 = new QVBox( this );
    tab3->setMargin( 5 );
    tab3->setSpacing( 5 );
    
    (void)new QLabel( QString( "Open %1 with:" ).arg( filename ), tab3 );

    QListBox *prgs = new QListBox( tab3 );
    for ( unsigned int i = 0; i < 30; i++ ) {
        QString prg = QString( "Application %1" ).arg( i );
        prgs->insertItem( prg );
    }
    prgs->setCurrentItem( 3 );

    (void)new QCheckBox( QString( "Open files with the extension '%1' always with this application" ).arg( fileinfo.extension() ), tab3 );

    addTab( tab3, "Applications" );
	*/
}
