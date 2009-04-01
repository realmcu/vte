/*================================================================================================*/
/**
    @file   DoubleOptionsForm.cpp
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

#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include "DoubleOptionsForm.h"

DoubleOptionsForm::DoubleOptionsForm(QWidget *parent) : QDialog(parent)
{
    min = 0;
    max = 0; 
    decimals = 0;
    
    QGridLayout *grid = new QGridLayout(this, 4, 2, 5);

    QLabel *label = new QLabel(this);
    label->setText("Min value can be entered:");
    grid->addWidget(label, 0, 0, Qt::AlignRight);

    pDoubleMinEdit = new QLineEdit(this);
    grid->addWidget(pDoubleMinEdit, 0, 1);

    label = new QLabel(this);
    label->setText("Max value can be entered:");
    grid->addWidget(label, 1, 0, Qt::AlignRight);

    pDoubleMaxEdit = new QLineEdit(this);
    grid->addWidget(pDoubleMaxEdit, 1, 1);

    label = new QLabel(this);
    label->setText("Maximum number of decimal places the number may have:");
    grid->addWidget(label, 2, 0, Qt::AlignRight);

    pDoubleDecimalsEdit = new QLineEdit(this);
    grid->addWidget(pDoubleDecimalsEdit, 2, 1);

    QPushButton *ok_button = new QPushButton("OK", this);
    connect(ok_button, SIGNAL(clicked()), this, SLOT(accept()));

    QPushButton *cancel_button = new QPushButton("Cancel", this);
    grid->addWidget(cancel_button, 3, 1);
    connect(cancel_button, SIGNAL(clicked()), this, SLOT(reject()));

    ok_button->resize(cancel_button->width(), cancel_button->height());
    grid->addWidget(ok_button, 3, 0, Qt::AlignRight);

    setEditorsData();
}

void DoubleOptionsForm::accept()
{
    bool ok, all_ok;
    min = pDoubleMinEdit->text().toDouble(&all_ok);
    max = pDoubleMaxEdit->text().toDouble(&ok);
    all_ok &= ok;
    decimals = pDoubleDecimalsEdit->text().toInt(&ok);
    all_ok &= ok;
    if (!all_ok)
    {
        QMessageBox::critical(this, "Invalid parameters",
            "Incorrect data has been input in Min or Max value or Step fields!");
        return;
    }
    QDialog::accept();
}

void DoubleOptionsForm::setData(int min, int max, int decimals)
{
    this->min = min;
    this->max = max;
    this->decimals = decimals;
    setEditorsData();
}

void DoubleOptionsForm::setEditorsData()
{
    pDoubleMinEdit->setText(QString::number(min));
    pDoubleMaxEdit->setText(QString::number(max));
    pDoubleDecimalsEdit->setText(QString::number(decimals));
}
