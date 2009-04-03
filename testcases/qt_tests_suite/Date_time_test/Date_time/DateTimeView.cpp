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

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/



#include "DateTimeView.h"
#include "DateTimeForm.h"
#include <qpushbutton.h>
#include <qlayout.h>
#include <qmessagebox.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DateTimeView::DateTimeView(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* grid =  new QVBoxLayout(this, 2, 8 );
	grid->addStretch();

    pDateTimeStringLabel = new QLabel(this);
    grid->addWidget(pDateTimeStringLabel, 0, Qt::AlignHCenter);

	QDateTime dt = QDateTime::currentDateTime();
	pDateTimeEdit = new QDateTimeEdit(dt, this);

	grid->addWidget(pDateTimeEdit);
	connect(pDateTimeEdit, SIGNAL(valueChanged(const QDateTime&)),
		    this, SLOT(dateTimeChanged()));

	grid->addStretch();
	QLabel* label = new QLabel(this);
	label->setText("Enter new Date/Time :");
	grid->addWidget(label,0, Qt::AlignHCenter);

	sFormat = "yyyy-MM-dd hh:mm:mm";
	pNewDateTimeEdit = new QLineEdit(dt.toString(sFormat), this);
	grid->addWidget(pNewDateTimeEdit);

	QPushButton *button = new QPushButton(this);
	button->setText("Set Date/Time");
	grid->addWidget(button);
	connect(button, SIGNAL(clicked()), this, SLOT(setNewDateTime()));

	grid->addStretch();

	label = new QLabel(this);
	label->setText("Enter new format");
	grid->addWidget(label,0, Qt::AlignHCenter);
	
	pDateTimeFormatEdit = new QLineEdit(sFormat, this);
	grid->addWidget(pDateTimeFormatEdit, 5, 0);

	button = new QPushButton(this);
	button->setText(" Set Format ");
	grid->addWidget(button);
	connect(button, SIGNAL(clicked()), this, SLOT(setFormat()));

	grid->addStretch();

	
	dateTimeChanged();
}

void DateTimeView::dateTimeChanged()
{
	QString sdt = pDateTimeEdit->dateTime().toString(sFormat);
	if (sdt.isNull())
	{
		pDateTimeStringLabel->setText("Enter valid format!");
	}
	else
	{
		pDateTimeStringLabel->setText(sdt);
	}
	pDateTimeStringLabel->repaint();
}

void DateTimeView::setNewDateTime()
{
	QString text = pNewDateTimeEdit->text();
	QDateTime dt = QDateTime::fromString(text, Qt::ISODate);
	if (dt.isValid() == FALSE)
	{
		QMessageBox::warning(this, "Warning", "Date/Time is invalid, but QDateTime::setDateTime will be called anyway\nUse ISO8601 extended format (YYYY-MM-DD, or with time, YYYY-MM-DDTHH:MM:SS)");
	}
	pDateTimeEdit->setDateTime(dt);
	pDateTimeEdit->repaint();
}

void DateTimeView::setFormat()
{
	sFormat = pDateTimeFormatEdit->text();
	dateTimeChanged();
}
