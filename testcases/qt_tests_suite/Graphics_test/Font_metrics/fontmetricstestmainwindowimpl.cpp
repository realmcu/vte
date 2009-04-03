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

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#include "fontmetricstestmainwindowimpl.h"


FontMetricsTestMainWindowImpl::FontMetricsTestMainWindowImpl( QWidget* parent, const char* name, WFlags f )
	: FontMetricsTestMainWindow( parent, name, f )
{
	setCaption("FontMetricsTest");

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


//1	setFixedSize(240,320);

}


void FontMetricsTestMainWindowImpl::chooseFont()
{
    bool ok;
    QFont oldfont = curFont;
    curFont = QFontDialog::getFont(&ok, oldfont, this);

    if (ok) setFont(curFont);
    else    curFont = oldfont;
}

void FontMetricsTestMainWindowImpl::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
	qApp->exit(0);
}

void FontMetricsTestMainWindowImpl::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
	qApp->exit(1);
}


void FontMetricsTestMainWindowImpl::paintEvent( QPaintEvent* e)
{
	setFixedSize(240,320);// it should be done in constructor
	QPainter p(this);

	p.setFont(curFont);
	p.drawText(10,270,20,10,AlignAuto|DontClip,testString);

	QFontMetrics fm(curFont);




//	p.setFont(QFont(curFont.family(), 12));
	p.setFont(QFont("Times", 10, QFont::Bold));
	p.drawText(10,40,"Font Metrics:");
	p.drawText(10,250,"Test String:");

	p.setFont(QFont("Arial", 8));

	p.drawText(10,60,"ascent()");
	p.drawText(10,75,"descent()");
	p.drawText(10,90,"height()");
	p.drawText(10,105,"leading()");
	p.drawText(10,120,"lineSpacing()");
	p.drawText(10,135,"minLeftBearing()");
	
	p.drawText(170,60,QString::number(fm.ascent()));
	p.drawText(170,75,QString::number(fm.descent()));
	p.drawText(170,90,QString::number(fm.height()));
	p.drawText(170,105,QString::number(fm.leading()));
	p.drawText(170,120,QString::number(fm.lineSpacing()));
	p.drawText(170,135,QString::number(fm.minLeftBearing()));

	p.drawText(10,150,"minRightBearing()");
	p.drawText(10,165,"maxWidth()");
	p.drawText(10,180,"underlinePos()");
	p.drawText(10,195,"overlinePos()");
	p.drawText(10,210,"strikeOutPos()");
	p.drawText(10,225,"lineWidth()");

	p.drawText(170,150,QString::number(fm.minRightBearing()));
	p.drawText(170,165,QString::number(fm.maxWidth()));
	p.drawText(170,180,QString::number(fm.underlinePos()));
	p.drawText(170,195,QString::number(fm.overlinePos()));
	p.drawText(170,210,QString::number(fm.strikeOutPos()));
	p.drawText(170,225,QString::number(fm.lineWidth()));

}

