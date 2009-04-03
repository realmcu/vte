/*================================================================================================*/
/**
    @file   progressbar.cpp

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

#include "progressbar.h"
#include <qcursor.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qlabel.h>

#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qprogressbar.h>
#include <qlayout.h>

#include <qmotifstyle.h>

/*
 * Constructor
 *
 * Creates child widgets of the ProgressBar widget
 */

ProgressBar::ProgressBar( QWidget *parent, const char *name )
    : QButtonGroup( 0, Horizontal, "Progress Bar", parent, name ), timer()
{
    setMargin( 10 );

    QGridLayout* toplayout = new QGridLayout( layout(), 2, 2, 5);

    setRadioButtonExclusive( TRUE );

    // insert three radiobuttons which the user can use
    // to set the speed of the progress and two pushbuttons
    // to start/pause/continue and reset the progress
    slow = new QRadioButton( "S&low", this );
    normal = new QRadioButton( "&Normal", this );
    fast = new QRadioButton( "&Fast", this );
    QVBoxLayout* vb1 = new QVBoxLayout;
    toplayout->addLayout( vb1, 0, 0 );
    vb1->addWidget( slow );
    vb1->addWidget( normal );
    vb1->addWidget( fast );

    // two push buttons, one for start, for for reset.
    start = new QPushButton( "&Start", this );
    reset = new QPushButton( "&Reset", this );
    QVBoxLayout* vb2 = new QVBoxLayout;
    toplayout->addLayout( vb2, 0, 1 );
    vb2->addWidget( start );
    vb2->addWidget( reset );

    // Create the progressbar
    progress = new QProgressBar( 100, this );
    //    progress->setStyle( new QMotifStyle() );
    toplayout->addMultiCellWidget( progress, 1, 1, 0, 1 );

    // connect the clicked() SIGNALs of the pushbuttons to SLOTs
    connect( start, SIGNAL( clicked() ), this, SLOT( slotStart() ) );
    connect( reset, SIGNAL( clicked() ), this, SLOT( slotReset() ) );

    // connect the timeout() SIGNAL of the progress-timer to a SLOT
    connect( &timer, SIGNAL( timeout() ), this, SLOT( slotTimeout() ) );

    // Let's start with normal speed...
    normal->setChecked( TRUE );


    // some contraints
    start->setFixedWidth( 80 );
    setFixedWidth( 200 );
}

/*
 * SLOT slotStart
 *
 * This SLOT is called if the user clicks start/pause/continue
 * button
 */

void ProgressBar::slotStart()
{
    // If the progress bar is at the beginning...
    if ( progress->progress() == -1 ) {
        // ...set according to the checked speed-radiobutton
        // the number of steps which are needed to complete the process
        if ( slow->isChecked() )
            progress->setTotalSteps( 10000 );
        else if ( normal->isChecked() )
            progress->setTotalSteps( 1000 );
        else
            progress->setTotalSteps( 50 );

        // disable the speed-radiobuttons
        slow->setEnabled( FALSE );
        normal->setEnabled( FALSE );
        fast->setEnabled( FALSE );
    }

    // If the progress is not running...
    if ( !timer.isActive() ) {
        // ...start the timer (and so the progress) with a interval of 1 ms...
        timer.start( 1 );
        // ...and rename the start/pause/continue button to Pause
        start->setText( "&Pause" );
    } else { // if the prgress is running...
        // ...stop the timer (and so the prgress)...
        timer.stop();
        // ...and rename the start/pause/continue button to Continue
        start->setText( "&Continue" );
    }
}

/*
 * SLOT slotReset
 *
 * This SLOT is called when the user clicks the reset button
 */

void ProgressBar::slotReset()
{
    // stop the timer and progress
    timer.stop();

    // rename the start/pause/continue button to Start...
    start->setText( "&Start" );
    // ...and enable this button
    start->setEnabled( TRUE );

    // enable the speed-radiobuttons
    slow->setEnabled( TRUE );
    normal->setEnabled( TRUE );
    fast->setEnabled( TRUE );

    // reset the progressbar
    progress->reset();
}

/*
 * SLOT slotTimeout
 *
 * This SLOT is called each ms when the timer is
 * active (== progress is running)
 */

void ProgressBar::slotTimeout()
{
    int p = progress->progress();

#if 1
    // If the progress is complete...
    if ( p == progress->totalSteps() )  {
        // ...rename the start/pause/continue button to Start...
        start->setText( "&Start" );
        // ...and disable it...
        start->setEnabled( FALSE );
        // ...and return
        return;
    }
#endif

    // If the process is not complete increase it
    progress->setProgress( ++p );
}

extern "C"{
    #include "test.h"
}


extern char *TCID;

void ProgressBar::keyPressEvent(QKeyEvent *e){
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

void ProgressBar::contextMenuEvent( QContextMenuEvent * )
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


void ProgressBar::exitPass()
{

	VT_rv=TPASS;
	close();

}

void ProgressBar::exitFail()
{
  VT_rv=TFAIL;
	close();

}

