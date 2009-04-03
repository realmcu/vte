/*================================================================================================*/
/**
    @file   DateTimeForm.h

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
Konstantin L.            17/05/2004      ?????????   Initial version 
Irina Inkina             27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

#include <qwidget.h>
#include <qlabel.h>
#include <qmenubar.h>
#include <qmainwindow.h>
#include <qpopupmenu.h>
#include <qassistantclient.h>

class DateTimeForm : public QMainWindow
{
  Q_OBJECT
public:
	DateTimeForm();

private:
	QPopupMenu *fileMenu;
	QPopupMenu *formatMenu;
	QPopupMenu *helpMenu;

	QAssistantClient *assistant;

public slots:
	void exitPass();
	void exitFail();
	void changeDateFormat();
	void changeTimeFormat();
	void showHelp();
};