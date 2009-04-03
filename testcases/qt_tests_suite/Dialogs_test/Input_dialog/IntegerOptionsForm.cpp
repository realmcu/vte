/*================================================================================================*/
/**
    @file   IntegerOptionsForm.cpp
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
#include "IntegerOptionsForm.h"

IntegerOptionsForm::IntegerOptionsForm(QWidget *parent) : QDialog(parent)
{
    min = 0;
    max = 0;
    step = 0;
    
    QGridLayout *grid = new QGridLayout(this, 4, 2, 5);

    QLabel *label = new QLabel(this);
    label->setText("Min value can be entered:");
    grid->addWidget(label, 0, 0, Qt::AlignRight);

    pIntMinEdit = new QLineEdit(this);
    grid->addWidget(pIntMinEdit, 0, 1);

    label = new QLabel(this);
    label->setText("Max value can be entered:");
    grid->addWidget(label, 1, 0, Qt::AlignRight);

    pIntMaxEdit = new QLineEdit(this);
    grid->addWidget(pIntMaxEdit, 1, 1);

    label = new QLabel(this);
    label->setText("Step of value change as the arrow buttons pressed:");
    grid->addWidget(label, 2, 0, Qt::AlignRight);

    pIntStepEdit = new QLineEdit(this);
    grid->addWidget(pIntStepEdit, 2, 1);

    QPushButton *ok_button = new QPushButton("OK", this);
    connect(ok_button, SIGNAL(clicked()), this, SLOT(accept()));

    QPushButton *cancel_button = new QPushButton("Cancel", this);
    grid->addWidget(cancel_button, 3, 1);
    connect(cancel_button, SIGNAL(clicked()), this, SLOT(reject()));

    ok_button->resize(cancel_button->width(), cancel_button->height());
    grid->addWidget(ok_button, 3, 0, Qt::AlignRight);

    setEditorsData();
}

void IntegerOptionsForm::accept()
{
    bool ok, all_ok;
    min = pIntMinEdit->text().toInt(&all_ok);
    max = pIntMaxEdit->text().toInt(&ok);
    all_ok &= ok;
    step = pIntStepEdit->text().toInt(&ok);
    all_ok &= ok;
    if (!all_ok)
    {
        QMessageBox::critical(this, "Invalid parameters",
            "Incorrect data has been input in Min or Max value or Step fields!");
        return;
    }
    QDialog::accept();
}

void IntegerOptionsForm::setData(int min, int max, int step)
{
    this->min = min;
    this->max = max;
    this->step = step;
    setEditorsData();
}

void IntegerOptionsForm::setEditorsData()
{
    pIntMinEdit->setText(QString::number(min));
    pIntMaxEdit->setText(QString::number(max));
    pIntStepEdit->setText(QString::number(step));
}