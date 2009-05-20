/*================================================================================================*/
/**
    @file   IntegerOptionsForm.h
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
Roman Holodov           17/05/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

#include <qdialog.h>

class QLineEdit;

class IntegerOptionsForm : public QDialog
{
    Q_OBJECT
public:
    IntegerOptionsForm(QWidget* parent);
    void setData(int min, int max, int step);

    int min;
    int max;
    int step;

protected slots:
    virtual void accept();

private:
    QLineEdit *pIntMinEdit;
    QLineEdit *pIntMaxEdit;
    QLineEdit *pIntStepEdit;

    void setEditorsData();
};