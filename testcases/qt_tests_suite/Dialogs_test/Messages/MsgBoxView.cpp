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
Roman Holodov           17/05/2004      ?????????   Initial version 

Irina Inkina            27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#include <qcursor.h>
#include <qpopupmenu.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qlistbox.h>
#include "MsgBoxView.h"

const static char *DefaultButtons[] =
{
    "",
    "None",
    "QMessageBox::NoButton",
    "QMessageBox::Ok",
    "QMessageBox::Cancel",
    "QMessageBox::Yes",
    "QMessageBox::No",
    "QMessageBox::Abort",
    "QMessageBox::Retry",
    "QMessageBox::Ignore",
    "QMessageBox::YesAll",
    "QMessageBox::NoAll",
    0
};

MsgBoxView::MsgBoxView() : QWidget()
{
    QGridLayout *grid = new QGridLayout(this, 12, 2, 4);
	grid->setColStretch(0,1);
	grid->setColStretch(1,1);


    QLabel *label = new QLabel("Caption: ", this);
    grid->addWidget(label, 0, 0, Qt::AlignRight);

    pCaptionEdit = new QLineEdit("Message Box", this);
    grid->addWidget(pCaptionEdit, 0, 1);

    label = new QLabel("Text", this);
    grid->addWidget(label, 1, 0, Qt::AlignRight);

    pTextEdit = new QLineEdit("Message Text", this);
    grid->addWidget(pTextEdit, 1, 1);

	QFrame *f = new QFrame(this);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	grid->addMultiCellWidget(f,2,2,0,1);


//-------
    QGridLayout *grid1 = new QGridLayout(0, 5, 2, 1, 1, "buttons");

  
	label = new QLabel("button0:", this);
    grid1->addWidget(label, 1, 0, Qt::AlignRight);

    b0Combo = new QComboBox(TRUE, this);
    grid1->addWidget(b0Combo, 1, 1);
    b0Combo->insertStrList(DefaultButtons);
    b0Combo->setCurrentItem(1);


    label = new QLabel("button1:", this);
    grid1->addWidget(label, 2, 0, Qt::AlignRight);

    b1Combo = new QComboBox(TRUE, this);
    grid1->addWidget(b1Combo, 2, 1);
    b1Combo->insertStrList(DefaultButtons);
    b1Combo->setCurrentItem(1);


    label = new QLabel("button2:", this);
    grid1->addWidget(label, 3, 0, Qt::AlignRight);

	b2Combo = new QComboBox(TRUE, this);
    grid1->addWidget(b2Combo, 3, 1);
    b2Combo->insertStrList(DefaultButtons);
    b2Combo->setCurrentItem(1);



	grid->addMultiCellLayout(grid1,3,6,0,1);
//------

	f = new QFrame(this);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	grid->addMultiCellWidget(f,7,7,0,1);


    QPushButton *button = new QPushButton("information", this);
    grid->addWidget(button, 8, 0);
    connect(button, SIGNAL(clicked()), this, SLOT(showInformation()));


    button = new QPushButton("warning", this);
    grid->addWidget(button, 8, 1);
    connect(button, SIGNAL(clicked()), this, SLOT(showWarning()));

	
    button = new QPushButton("critical", this);
    grid->addWidget(button, 9, 0);
    connect(button, SIGNAL(clicked()), this, SLOT(showCritical()));


    button = new QPushButton("question", this);
    grid->addWidget(button, 9, 1);
    connect(button, SIGNAL(clicked()), this, SLOT(showQuestion()));


    button = new QPushButton("about", this);
    grid->addWidget(button, 10, 0);
    connect(button, SIGNAL(clicked()), this, SLOT(showAbout()));

    button = new QPushButton("aboutQt", this);
    grid->addWidget(button, 10, 1);
    connect(button, SIGNAL(clicked()), this, SLOT(showAboutQt()));


	setMaximumWidth(240);
}

/*
MsgBoxView::T_BUTTONS_DATA::~T_BUTTONS_DATA()
{
    if (isCorrect && type)
    {
        if (buttons.strings.button0Text != NULL)
        {
            delete buttons.strings.button0Text;
        }
        if (buttons.strings.button1Text != NULL)
        {
            delete buttons.strings.button1Text;
        }
        if (buttons.strings.button2Text != NULL)
        {
            delete buttons.strings.button2Text;
        }
    }
}
*/

MsgBoxView::~MsgBoxView()
{
}

void MsgBoxView::showInformation()
{
    int result;
    fillButtonsData();
    if (!buttons_data.isCorrect)
    {
        return;
    }
    if (buttons_data.type)
    {
        result = QMessageBox::information(this, 
            pCaptionEdit->text(),
            pTextEdit->text(),
            *buttons_data.buttons.strings.button0Text,
            *buttons_data.buttons.strings.button1Text,
            *buttons_data.buttons.strings.button2Text
            );
    }
    else
    {
        result = QMessageBox::information(this,
            pCaptionEdit->text(),
            pTextEdit->text(),
            buttons_data.buttons.ints.button0,
            buttons_data.buttons.ints.button1,
            buttons_data.buttons.ints.button2
            );
    }

    QMessageBox::information(this, "Result", 
        QString("QMessageBox returned ") + QString::number(result));
}

void MsgBoxView::showQuestion()
{
    int result;
    fillButtonsData();
    if (!buttons_data.isCorrect)
    {
        return;
    }
    if (buttons_data.type)
    {
        result = QMessageBox::question(this, 
            pCaptionEdit->text(),
            pTextEdit->text(),
            *buttons_data.buttons.strings.button0Text,
            *buttons_data.buttons.strings.button1Text,
            *buttons_data.buttons.strings.button2Text
            );
    }
    else
    {
        result = QMessageBox::question(this,
            pCaptionEdit->text(),
            pTextEdit->text(),
            buttons_data.buttons.ints.button0,
            buttons_data.buttons.ints.button1,
            buttons_data.buttons.ints.button2
            );
    }

    QMessageBox::information(this, "Result", 
        QString("QMessageBox returned ") + QString::number(result));
}

void MsgBoxView::showWarning()
{
    int result;
    fillButtonsData();
    if (!buttons_data.isCorrect)
    {
        return;
    }
    if (buttons_data.type)
    {
        result = QMessageBox::warning(this, 
            pCaptionEdit->text(),
            pTextEdit->text(),
            *buttons_data.buttons.strings.button0Text,
            *buttons_data.buttons.strings.button1Text,
            *buttons_data.buttons.strings.button2Text
            );
    }
    else
    {
        result = QMessageBox::warning(this,
            pCaptionEdit->text(),
            pTextEdit->text(),

            buttons_data.buttons.ints.button0,
            buttons_data.buttons.ints.button1,
            buttons_data.buttons.ints.button2
            );
    }

    QMessageBox::information(this, "Result", 
        QString("QMessageBox returned ") + QString::number(result));
}

void MsgBoxView::showCritical()
{
        int result;
    fillButtonsData();
    if (!buttons_data.isCorrect)
    {
        return;
    }
    if (buttons_data.type)
    {
        result = QMessageBox::critical(this, 
            pCaptionEdit->text(),
            pTextEdit->text(),
            *buttons_data.buttons.strings.button0Text,
            *buttons_data.buttons.strings.button1Text,
            *buttons_data.buttons.strings.button2Text
            );
    }
    else
    {
        result = QMessageBox::critical(this,
            pCaptionEdit->text(),
            pTextEdit->text(),
            buttons_data.buttons.ints.button0,
            buttons_data.buttons.ints.button1,
            buttons_data.buttons.ints.button2
            );
    }

    QMessageBox::information(this, "Result", 
        QString("QMessageBox returned ") + QString::number(result));
}

void MsgBoxView::showAbout()
{
    QMessageBox::about(this, 
        pCaptionEdit->text(), pTextEdit->text());
}

void MsgBoxView::showAboutQt()
{
    QMessageBox::aboutQt(this,
        pCaptionEdit->text());
}

static int convertIntButton(int v)
{
    switch (v)
    {
    case 1:
        return 0;
    case 2:
        return QMessageBox::NoButton;
    case 3:
        return QMessageBox::Ok;
    case 4:
        return QMessageBox::Cancel;
    case 5:
        return QMessageBox::Yes;
    case 6:
        return QMessageBox::No;
    case 7:
        return QMessageBox::Abort;
    case 8:
        return QMessageBox::Retry;
    case 9:
        return QMessageBox::Ignore;
    case 10:
        return QMessageBox::YesAll;
    case 11:
        return QMessageBox::NoAll;
    default:
        return 0;
    }
}


bool MsgBoxView::fillButtonsData()
{
    int b0, b1, b2;
    buttons_data.isCorrect = TRUE;
    b0 = b0Combo->currentItem();
    b1 = b1Combo->currentItem();
    b2 = b2Combo->currentItem();
// 1) if at least one is 0, i.e. editable, others should be editable too or not present
// 2) None buttons should be last - as default arguments convension
    if (b0 == 0 || b1 == 0 || b2 == 0)
    {
        if (b0 > 1 || b1 > 1 || b2 > 1)
        {
            buttons_data.isCorrect = FALSE;
        }
        else
        {
            if (b2 != 1 && (b0 == 1 || b1 == 1))
            {
                buttons_data.isCorrect = FALSE;
            }
            else
            {
                if (b1 != 1 && b0 == 1)
                {
                    buttons_data.isCorrect = FALSE;
                }
            }
        }
    }
    if (buttons_data.isCorrect == FALSE)
    {
        QMessageBox::critical(this, "Data Error", "Incorrect data input");
        return FALSE;
    }

    // determine type now
    buttons_data.type = (b0 == 0 || b0 == 1);

    if (buttons_data.type)
    {   // strings
        static QString s0, s1, s2;
        if (b0 == 1)
        {
            s0 = QString::null;
        }
        else
        {
            s0 = b0Combo->lineEdit()->text();
        }
        buttons_data.buttons.strings.button0Text = &s0;

        if (b1 == 1)
        {
            s1 = QString::null;
        }
        else
        {
            s1 = b1Combo->lineEdit()->text();
        }
        buttons_data.buttons.strings.button1Text = &s1;

        if (b2 == 1)
        {
            s2 = QString::null;
        }
        else
        {
            s2 = b0Combo->lineEdit()->text();
        }
        buttons_data.buttons.strings.button2Text = &s2;
    }
    else
    {   // ints
        buttons_data.buttons.ints.button0 = convertIntButton(b0Combo->listBox()->currentItem());
        buttons_data.buttons.ints.button1 = convertIntButton(b1Combo->listBox()->currentItem());
        buttons_data.buttons.ints.button2 = convertIntButton(b2Combo->listBox()->currentItem());
    }

    return TRUE;
}


extern "C"{
    #include "test.h"
}


extern char *TCID;

void MsgBoxView::keyPressEvent(QKeyEvent *e){
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

void MsgBoxView::contextMenuEvent( QContextMenuEvent * )
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


void MsgBoxView::exitPass()
{

	VT_rv=TPASS;
	close();

}

void MsgBoxView::exitFail()
{
  VT_rv=TFAIL;
	close();

}

