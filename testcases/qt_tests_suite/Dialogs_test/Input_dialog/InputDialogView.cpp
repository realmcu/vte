/*================================================================================================*/
/**
    @file   InputDialogView.cpp
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

Irina Inkina            27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#include <qcursor.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>

#include <qwidget.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qinputdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include "IntegerOptionsForm.h"
#include "DoubleOptionsForm.h"
#include "InputDialogView.h"

const static char *list[] = 
{
    "Line one",
    "Line two",
    "Line three",
    "Line four",
    "Line five",
    "Line six"
};

const static char *echo_modes[] = 
{
    "Normal",
    "NoEcho",
    "Password",
    0
};

InputDialogView::InputDialogView() : QWidget()
{
    m_IntMin = -2147483647;
    m_IntMax = 2147483647;
    m_IntStep = 1;

    m_DoubleMin = -2147483647;
    m_DoubleMax = 2147483647;
    m_DoubleDecimals = 1;

    QGridLayout *grid = new QGridLayout( this, 7,2 , 5 );

    QPushButton *button = new QPushButton(this);
    button->setText("Input Text");
    grid->addWidget(button, 0, 0);
    connect(button, SIGNAL(clicked()), this, SLOT(InputTextClicked()));

    QLabel *label = new QLabel(this);
    label->setText("Echo Mode: ");
    grid->addWidget(label, 1,0, Qt::AlignRight);

    pEchoModeCombo = new QComboBox(FALSE, this);
    pEchoModeCombo->insertStrList(echo_modes);
    grid->addWidget(pEchoModeCombo, 1, 1);

    button = new QPushButton(this);
    button->setText("Input Integer");
    grid->addWidget(button, 3, 0);
    connect(button, SIGNAL(clicked()), this, SLOT(InputIntegerClicked()));

    button = new QPushButton("Options...", this);
    grid->addWidget(button, 3, 1);
    connect(button, SIGNAL(clicked()), this, SLOT(IntOptsClicked()));

/*
    label = new QLabel(this);
    label->setText("Min value:");
    grid->addWidget(label, 1, 1, Qt::AlignRight);

    pIntMinEdit = new QLineEdit("-2147483647", this);
    grid->addWidget(pIntMinEdit, 1, 2);

    label = new QLabel(this);
    label->setText("Max value:");
    grid->addWidget(label, 1, 3, Qt::AlignRight);

    pIntMaxEdit = new QLineEdit("2147483647", this);
    grid->addWidget(pIntMaxEdit, 1, 4);

    label = new QLabel(this);
    label->setText("Step:");
    grid->addWidget(label, 1, 5, Qt::AlignRight);

    pIntStepEdit = new QLineEdit("1", this);
    grid->addWidget(pIntStepEdit, 1, 6);
*/
    button = new QPushButton(this);
    button->setText("Input Double");
    grid->addWidget(button, 5, 0);
    connect(button, SIGNAL(clicked()), this, SLOT(InputDoubleClicked()));

/*
    label = new QLabel(this);
    label->setText("Min value:");
    grid->addWidget(label, 2, 1, Qt::AlignRight);

    pDoubleMinEdit = new QLineEdit("-2147483647", this);
    grid->addWidget(pDoubleMinEdit, 2, 2);

    label = new QLabel(this);
    label->setText("Max value:");
    grid->addWidget(label, 2, 3, Qt::AlignRight);

    pDoubleMaxEdit = new QLineEdit("2147483647", this);
    grid->addWidget(pDoubleMaxEdit, 2, 4);

    label = new QLabel(this);
    label->setText("Decimals:");
    grid->addWidget(label, 2, 5, Qt::AlignRight);

    pDoubleDecimalsEdit = new QLineEdit("1", this);
    grid->addWidget(pDoubleDecimalsEdit, 2, 6);
*/
    button = new QPushButton("Options...", this);
    grid->addWidget(button, 5, 1);
    connect(button, SIGNAL(clicked()), this, SLOT(DblOptsClicked()));

    button = new QPushButton(this);
    button->setText("Select Item");
    grid->addWidget(button, 7, 0);
    connect(button, SIGNAL(clicked()), this, SLOT(SelectItemClicked()));

    pIsListEditableCheckbox = new QCheckBox("Editable", this);
    grid->addWidget(pIsListEditableCheckbox, 7, 1);
    
    setMaximumSize(240,320);
}





void InputDialogView::InputTextClicked()
{
    QLineEdit::EchoMode echo_mode = QLineEdit::Normal;
    switch(pEchoModeCombo->currentItem())
    {
    case 0: 
        echo_mode = QLineEdit::Normal;
        break;
    case 1:
        echo_mode = QLineEdit::NoEcho;
        break;
    case 2:
        echo_mode = QLineEdit::Password;
        break;
    }
    bool ok;
    QString result = QInputDialog::getText(
        "Testing QInputDialog::getText()", "Enter a String:", echo_mode,
        QString::null, &ok, this);
    if (ok)
    {
        QMessageBox::information(this, "Entered text", 
            QString("Text entered:\n") + result);
    }
    else
    {
        QMessageBox::critical(this, "Error entering data", "getText() returned FALSE");
    }
}

void InputDialogView::InputIntegerClicked()
{
    bool ok;
/*
    bool all_ok;
    int min = pIntMinEdit->text().toInt(&all_ok);
    int max = pIntMaxEdit->text().toInt(&ok);
    all_ok &= ok;
    int step = pIntStepEdit->text().toInt(&ok);
    all_ok &= ok;
    if (!all_ok)
    {
        QMessageBox::critical(this, "Invalid parameters",
            "Incorrect data has been input in Min or Max value or Step fields!");
        return;
    }
*/
    int result = QInputDialog::getInteger(
        "Testing QInputDialog::getInteger()", "Enter an Integer:", 
        0, m_IntMin, m_IntMax, m_IntStep, &ok, this);
    if (ok)
    {
        QMessageBox::information(this, "Entered Integer", 
            QString("Number entered: ") + QString::number(result));
    }
    else
    {
        QMessageBox::critical(this, "Error entering data", "getInteger() returned FALSE");
    }
}

void InputDialogView::InputDoubleClicked()
{
    bool ok;
/*
    bool all_ok;
    int min = pDoubleMinEdit->text().toDouble(&all_ok);
    int max = pDoubleMaxEdit->text().toDouble(&ok);
    all_ok &= ok;
    int decs = pDoubleDecimalsEdit->text().toInt(&ok);
    all_ok &= ok;
    if (!all_ok)
    {
        QMessageBox::critical(this, "Invalid parameters",
            "Incorrect data has been input in Min or Max value or Decimals fields!");
        return;
    }
*/
    
    double result = QInputDialog::getDouble(
        "Testing QInputDialog::getDouble()", "Enter a Double:", 
        0, m_DoubleMin, m_DoubleMax, m_DoubleDecimals, &ok, this);
    if (ok)
    {
        QMessageBox::information(this, "Entered Double", 
            QString("Number entered: ") + QString::number(result));
    }
    else
    {
        QMessageBox::critical(this, "Error entering data", "getDouble() returned FALSE");
    }
}

void InputDialogView::SelectItemClicked()
{
    QStringList data;
//    QString s("You will select from list of elements:");
    for (int i = 0; i < sizeof(list)/sizeof(*list); i++)
    {
//        s += '\n';
//        s += list[i];
        data << list[i];
    }
//    QMessageBox::information(this, "Select Item", s);

    bool ok;
    QString result = QInputDialog::getItem(
        "Testing QInputDialog::getItem()", "Select an Item from a list:", 
        data, 0, pIsListEditableCheckbox->isOn(), &ok, this);
    if (ok)
    {
        QMessageBox::information(this, "Selected Item", 
            QString("Item entered:\n") + result);
    }
    else
    {
        QMessageBox::critical(this, "Error entering data", "getItem() returned FALSE");
    }
}

void InputDialogView::IntOptsClicked()
{
    IntegerOptionsForm iof(this);
    iof.setData(m_IntMin, m_IntMax, m_IntStep);
    if (iof.exec())
    {
        m_IntMin = iof.min;
        m_IntMax = iof.max;
        m_IntStep = iof.step;
    }
}

void InputDialogView::DblOptsClicked()
{
    DoubleOptionsForm dof(this);
    dof.setData(m_DoubleMin, m_DoubleMax, m_DoubleDecimals);
    if (dof.exec())
    {
        m_DoubleMin = dof.min;
        m_DoubleMax = dof.max;
        m_DoubleDecimals = dof.decimals;
    }
}

extern "C"{
    #include "test.h"
}


extern char *TCID;

void InputDialogView::keyPressEvent(QKeyEvent *e)
 {
    if((e->key() == Qt::Key_F11)&&(e->state() & ControlButton )){
	VT_rv=TPASS;
	e->accept();
	close();
	return;
    }
    if((e->key() == Qt::Key_F12)&&(e->state() & ControlButton)){
	VT_rv=TFAIL;
	e->accept();
	close();
	return;
    }
    e->ignore();
}

void InputDialogView::contextMenuEvent( QContextMenuEvent * )
{
   QColor x2(202,202,202);
   QColorGroup g2(black,x2,x2.light(),x2.dark(),x2.dark(120),white/*black*/,white);
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
    contextMenu->insertItem( "&Quit - pass",  this, SLOT(exitPass()), CTRL+Key_F11);//CTRL+Key_F11
    contextMenu->insertItem( "E&xit - fail",  this, SLOT(exitFail()), CTRL+Key_F12);//CTRL+Key_F12
    contextMenu->exec( QCursor::pos() );
    delete contextMenu;
}


void InputDialogView::exitPass()
{

	VT_rv=TPASS;
	close();
        
}

void InputDialogView::exitFail()
{
  VT_rv=TFAIL;
	close();
 
}
