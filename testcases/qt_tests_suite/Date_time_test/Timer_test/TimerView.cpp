/*================================================================================================*/
/**
    @file   DateTimeView.cpp

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
Konstantin L.           17/05/2004      ?????????   Initial version 

Irina Inkina            27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qcursor.h>
#include <qpopupmenu.h>
#include <qapplication.h>

#include "TimerView.h"

TimerView::TimerView() : QWidget()
{
    QGridLayout *grid = new QGridLayout( this, 8, 2, 5 );

	//----------------------
    QLabel *label = new QLabel(this);
    label->setText("Timer interval");
    grid->addWidget(label, 0, 0, Qt::AlignRight);

    pTimerValueEdit = new QLineEdit("10000", this);
    grid->addWidget(pTimerValueEdit, 0, 1);

    m_bTimerStarted = FALSE;

    pStartStopButton = new QPushButton(this);
    setStartStopButtonText();
    grid->addWidget(pStartStopButton, 1,0 );
    connect(pStartStopButton, SIGNAL(clicked()), this, SLOT(startStopClicked()));

	//----------------------

    QPushButton *button = new QPushButton(this);
    button->setText("Change interval");
    grid->addWidget(button, 1, 1);
    connect(button, SIGNAL(clicked()), this, SLOT(changeClicked()));

	//----------------------

    label = new QLabel(this);
    label->setText("Is Active:");
    grid->addWidget(label, 2, 0, Qt::AlignRight);

    pIsActiveLabel = new QLabel(this);
    grid->addWidget(pIsActiveLabel, 2, 1, Qt::AlignLeft);
    updateIsActiveLabel();

//----------------------

    label = new QLabel(this);
    label->setText("Timer ID:");
    grid->addWidget(label, 3, 0, Qt::AlignRight);

    pTimerIDLabel = new QLabel(this);
    grid->addWidget(pTimerIDLabel, 3, 1, Qt::AlignLeft);
    updateTimerIDLabel();

	
	//----------------------
    label = new QLabel(this);
    label->setText("Timeouts count: ");
    grid->addWidget(label, 4, 0, Qt::AlignRight);

    pTimeoutsCounterLabel = new QLabel(this);
    grid->addWidget(pTimeoutsCounterLabel, 4, 1, Qt::AlignLeft);

    m_TimeoutsCounter = 0;
    updateTimeoutsCounterLabel();

/*

    button = new QPushButton(this);
    button->setText("Reset");
    grid->addWidget(button, 3, 1);


    connect(button, SIGNAL(clicked()), this, SLOT(resetTimeoutsCounter()));
*/
	//----------------------

    label = new QLabel(this);
    label->setText("SingleShot interval (msec):");
    grid->addMultiCellWidget(label, 6, 6,0,2);

    pSingleShotValueEdit = new QLineEdit("5000", this);
    grid->addWidget(pSingleShotValueEdit, 7, 0);

    button = new QPushButton(this);
    button->setText("SingeShot!");
    grid->addWidget(button, 7, 1);
    connect(button, SIGNAL(clicked()), this, SLOT(singleShot()));
	/*
    button = new QPushButton(this);
    button->setText("Refresh TimerID");
    grid->addWidget(button, 4, 1);
    connect(button, SIGNAL(clicked()), this, SLOT(updateTimerIDLabel()));
*/

    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(timerTimedOut()));

    pTimerExpiredMessage = new QErrorMessage(this);

	
	setMaximumWidth(240);
}
///////

void TimerView::contextMenuEvent( QContextMenuEvent * )
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
    contextMenu->insertItem( "&Quit - pass",  this, SLOT(exitPass()), CTRL+Key_Q );
    contextMenu->insertItem( "E&xit - fail",  this, SLOT(exitFail()), CTRL+Key_C );
    contextMenu->exec( QCursor::pos() );
    delete contextMenu;
}


void TimerView::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
	qApp->exit(0);
}

void TimerView::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
	qApp->exit(1);
}

///////////////

TimerView::~TimerView()
{
    delete pTimerExpiredMessage;
}

void TimerView::setStartStopButtonText()
{
    if (m_bTimerStarted == FALSE)
    {
        pStartStopButton->setText("Start");
    }
    else
    {
        pStartStopButton->setText("Stop");
    }
    pStartStopButton->repaint();
}

void TimerView::startStopClicked()
{
    if (m_bTimerStarted == FALSE)
    {
        bool res;
        int timer_value = QString(pTimerValueEdit->text()).toInt(&res, 10);
        if (res == FALSE)
        {
            QMessageBox::critical(this, "Number Format Error", 
                "Incorrect number format in Timer Interval Value editor");
            return;
        }
        if (m_Timer.start(timer_value) == 0)
        {
            QMessageBox::critical(this, "Operation Failed", 
                "Timer starting failed");
            return;
        }
        m_bTimerStarted = TRUE;
    }
    else
    {
        m_bTimerStarted = FALSE;
        m_Timer.stop();
    }
    setStartStopButtonText();
	updateIsActiveLabel();
	updateTimerIDLabel();
}

void TimerView::changeClicked()
{
    bool res;
    int timer_value = QString(pTimerValueEdit->text()).toInt(&res, 10);
    if (res == FALSE)
    {
        QMessageBox::critical(this, "Number Format Error", 
            "Incorrect number format in Change Interval Value editor");
        return;
    }
    
    m_Timer.changeInterval(timer_value);

    if ((m_bTimerStarted = m_Timer.isActive()) == FALSE)
    {
        QMessageBox::critical(this, "Operation Failed", 
            "Timer should run but isActive() returned FALSE");
        m_bTimerStarted = FALSE;
    }

    setStartStopButtonText();
	updateIsActiveLabel();
}

void TimerView::timerTimedOut()
{
    m_TimeoutsCounter++;
    updateTimeoutsCounterLabel();
	updateIsActiveLabel();
	updateTimerIDLabel();
    pTimerExpiredMessage->message("Timer timeout event received!");
}

void TimerView::updateIsActiveLabel()
{
    pIsActiveLabel->setText(
        QString((m_Timer.isActive() == FALSE) ? "NO" : "YES"));
    pIsActiveLabel->repaint();
}

void TimerView::resetTimeoutsCounter()
{
    m_TimeoutsCounter = 0;
    updateTimeoutsCounterLabel();
	updateTimerIDLabel();
	updateIsActiveLabel();
}

void TimerView::updateTimeoutsCounterLabel()
{
    pTimeoutsCounterLabel->setText(QString("") + QString::number(m_TimeoutsCounter));
    pTimeoutsCounterLabel->repaint();
}

void TimerView::singleShot()
{
    bool res;
    int timer_value = QString(pSingleShotValueEdit->text()).toInt(&res, 10);
    if (res == FALSE)
    {
        QMessageBox::critical(this, "Number Format Error", 
            "Incorrect number format in Change Interval Value editor");
        return;
    }
    QTimer::singleShot(timer_value, this, SLOT(catchSingleShot()));
}

void TimerView::catchSingleShot()
{
    QMessageBox::information(this, "Catch!", "SingleShot completed!");
}

void TimerView::updateTimerIDLabel()
{
    pTimerIDLabel->setText(QString::number(m_Timer.timerId()));
    pTimerIDLabel->repaint();
}