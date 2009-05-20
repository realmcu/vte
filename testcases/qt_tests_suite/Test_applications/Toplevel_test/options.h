/****************************************************************************
** Form interface generated from reading ui file 'options.ui'
**
** Created: Thu May 6 10:59:27 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.2   edited Nov 24 13:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
/*================================================================================================*/
/**
    @file   options.h

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

#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qguardedptr.h>
#include <qvbox.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QButtonGroup;
class QRadioButton;
class QCheckBox;
class QGroupBox;
class QLineEdit;
class QToolButton;
class QSlider;
class QLabel;
class QPushButton;

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    OptionsDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~OptionsDialog();

    QButtonGroup* bgBorder;
    QRadioButton* rbBorderNormal;
    QRadioButton* rbBorderDialog;
    QButtonGroup* bgTitle;
    QCheckBox* cbTitleSystem;
    QCheckBox* cbTitleMinimize;
    QCheckBox* cbTitleMaximize;
    QCheckBox* cbTitleContext;
    QButtonGroup* bgBehavior;
    QCheckBox* cbBehaviorTaskbar;
    QCheckBox* cbBehaviorStays;
    QCheckBox* cbBehaviorPopup;
    QCheckBox* cbBehaviorTool;
    QCheckBox* cbBehaviorModal;
    QGroupBox* gbProperties;
    QLineEdit* leCaption;
    QLineEdit* leIcon;
    QToolButton* tbPick;
    QSlider* slTransparency;
    QLabel* textLabel3;
    QLabel* textLabel2;
    QLabel* textLabel1;
    QPushButton* pbApply;
    QPushButton* pbClose;
    QPushButton* pbClose_E;


public slots:
    virtual void apply();
     void exitPass();
     void exitFail();
protected:
    QVBoxLayout* OptionsDialogLayout;
    QHBoxLayout* layout5;
    QVBoxLayout* layout4;
    QVBoxLayout* bgBorderLayout;
    QVBoxLayout* bgTitleLayout;
    QVBoxLayout* layout3;
    QSpacerItem* spacer2;
    QVBoxLayout* bgBehaviorLayout;
    QGridLayout* gbPropertiesLayout;
    QHBoxLayout* layout2;
    QHBoxLayout* layout1;
    QSpacerItem* spacer1;

protected slots:
    virtual void languageChange();

    virtual void pickIcon();


private:
    QGuardedPtr<QVBox> widget;

};

#endif // OPTIONSDIALOG_H
