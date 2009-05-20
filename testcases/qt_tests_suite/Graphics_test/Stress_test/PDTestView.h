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
Konstantin L           17/05/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#include <qwidget.h>

class QMenuBar;
class QProgressDialog;

class ProgressDialogTestView : public QWidget
{
    Q_OBJECT
public:
    ProgressDialogTestView();

private slots:
    void Start();
	void Exit();
	void Quit();
	void testr();

private:
    QMenuBar *menubar;

    int n;
private:
    QProgressDialog* newProgressDialog(const char *label, int steps);
};