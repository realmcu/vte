/*================================================================================================*/
/**
    @file   DateTimeView.h

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

// DateTimeView.h: interface for the DateTimeView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATETIMEVIEW_H__353FE2C6_AC26_4B89_9427_DD7069B340F0__INCLUDED_)
#define AFX_DATETIMEVIEW_H__353FE2C6_AC26_4B89_9427_DD7069B340F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qwidget.h>
#include <qlabel.h>
#include <qdatetimeedit.h>
#include <qlineedit.h>
#include <qstring.h>

class DateTimeView : public QWidget  
{
	Q_OBJECT
public:
	DateTimeView(QWidget* parent);

private slots:
	void dateTimeChanged();
	void setNewDateTime();
	void setFormat();

private:
    QLabel* pDateTimeStringLabel;
	QDateTimeEdit* pDateTimeEdit;
	QLineEdit* pNewDateTimeEdit;
	QLineEdit* pDateTimeFormatEdit;
	QString sFormat;
};

#endif // !defined(AFX_DATETIMEVIEW_H__353FE2C6_AC26_4B89_9427_DD7069B340F0__INCLUDED_)
