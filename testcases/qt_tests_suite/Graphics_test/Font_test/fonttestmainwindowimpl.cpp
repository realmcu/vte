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
Alexey P           17/05/2004      ?????????   Initial version 

Irina Inkina       27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#include "fonttestmainwindowimpl.h"



FontTestMainWindowImpl::FontTestMainWindowImpl( QWidget* parent, const char* name, WFlags f )
	: FontTestMainWindow( parent, name, f )
{
	setCaption("FontTest");
	// Add your code
	testString = "This is a TEST string";

//    setDefault();

	QAction * fileExitPassAction, * fileExitFailAction;
	QAction * fontChooseAction;


    fileExitPassAction = new QAction("ExitPass", "PassE&xit", CTRL+Key_X, this, "exitpass");
    connect( fileExitPassAction, SIGNAL(activated()), this, SLOT(exitPass()));

    fileExitFailAction = new QAction("ExitFail", "Fail&Exit", CTRL+Key_E, this, "exitfail" );
    connect( fileExitFailAction, SIGNAL(activated()), this, SLOT(exitFail()));

    QPopupMenu * file = new QPopupMenu( this );
    menuBar()->insertItem( "&File", file );
    fileExitPassAction->addTo( file );
    fileExitFailAction->addTo( file );

    fontChooseAction = new QAction("Choose Font", "Choose &Font", CTRL+Key_F, this, "choosefont" );
    connect( fontChooseAction, SIGNAL(activated()), this, SLOT(chooseFont()));

    QPopupMenu * font = new QPopupMenu( this );
    menuBar()->insertItem( "F&ont", font );
	menuBar()->insertSeparator();
    fontChooseAction->addTo( font );


	setFixedSize(240,320);

}


void FontTestMainWindowImpl::chooseFont()
{
    bool ok;
    QFont oldfont = curFont;
    curFont = QFontDialog::getFont(&ok, oldfont, this);

    if (ok) setFont(curFont);
    else    curFont = oldfont;
}

void FontTestMainWindowImpl::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
	qApp->exit(0);
}

void FontTestMainWindowImpl::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
	qApp->exit(1);
}


void FontTestMainWindowImpl::paintEvent( QPaintEvent* e)
{
	setFixedSize(240,320);// it should be done in constructor
	QPainter p(this);

	p.setFont(curFont);
	p.drawText(10,280,20,10,AlignAuto|DontClip,testString);

	p.setFont(QFont("Times", 10, QFont::Bold));
	p.drawText(10,40,"Font's characteristics:");
	p.drawText(10,260,"Test String:");

//	p.setFont(QFont(curFont.family(), 12));
	p.setFont(QFont("Arial", 8));
	p.drawText(10,60,"family():");
	p.drawText(10,80,"rawName():");
	p.drawText(10,100,"defaultFamily():");
	p.drawText(10,120,"lastResortFamily():");
	p.drawText(10,140,"lastResortFont():");
	p.drawText(10,160,"pointSize():");
	p.drawText(10,180,"key():");
	p.drawText(10,220,"toString():");

	p.setFont(QFont("Arial", 8 ));
	p.drawText(120,60,curFont.family());
	p.drawText(120,80,curFont.rawName());
	p.drawText(120,100,curFont.defaultFamily());
	p.drawText(120,120,curFont.lastResortFamily());
	p.drawText(120,140,curFont.lastResortFont());
	p.drawText(120,160,QString::number(curFont.pointSize()));
	p.drawText(30,200,curFont.key());
	p.drawText(30,240,curFont.toString());	
}