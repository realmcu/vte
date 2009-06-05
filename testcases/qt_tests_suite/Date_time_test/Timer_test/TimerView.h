/*================================================================================================*/
/**
    @file   TimerView.h

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
#ifndef XTIMERVIEW_H
#define XTIMERVIEW_H

#include <qwidget.h>
#include <qtimer.h>
#include <qerrormessage.h>

class QLineEdit;
class QPushButton;
class QTimer;
class QLabel;

class TimerView : public QWidget
{
    Q_OBJECT
public:
    TimerView();
    virtual ~TimerView();

public slots:

   void exitFail();
   void exitPass();

private slots:
    void startStopClicked();
    void timerTimedOut();
    void changeClicked();
    void updateIsActiveLabel();
    void resetTimeoutsCounter();
    void singleShot();
    void catchSingleShot();
    void updateTimerIDLabel();

private:
	int m_bTimerStarted;
    QTimer m_Timer;
    int m_TimeoutsCounter;
    QLineEdit *pTimerValueEdit;
    QPushButton *pStartStopButton;
    QLineEdit *pChangeValueEdit;
    QLabel *pIsActiveLabel;
    QLabel *pTimeoutsCounterLabel;
    QLineEdit *pSingleShotValueEdit;
    QPushButton *pSingleShotButton;
    QLabel *pTimerIDLabel;
    QErrorMessage *pTimerExpiredMessage;

private:
   	void setStartStopButtonText();
    void updateTimeoutsCounterLabel();
    void contextMenuEvent( QContextMenuEvent * );
    
};

#endif //
