/*================================================================================================*/
/**
    @file   FileDialogTestView.cpp
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
#include <qapplication.h>
#include <qcursor.h>
#include <qpopupmenu.h>

#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qstringlist.h>
#include "FileDialogTestView.h"

static QString null_if_empty(QString& s);
static QString content_or_null(QString& s);
static QString strlist_to_string(QStringList& sl);

FileDialogTestView::FileDialogTestView()
{
    QGridLayout *grid = new QGridLayout(this, 5, 2, 5);
//-----

    QLabel *label = new QLabel("Caption:", this);
    grid->addWidget(label, 0, 0, Qt::AlignRight);

    pCaptionEdit = new QLineEdit(this);
    grid->addWidget(pCaptionEdit, 0, 1);

	QFrame *f = new QFrame(this);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	grid->addMultiCellWidget(f,1,1,0,1);
//-----
    
    label = new QLabel("startWith:", this);
    grid->addWidget(label, 2, 0, Qt::AlignRight);	
    pStartWithEdit = new QLineEdit(this);
    grid->addWidget(pStartWithEdit, 2, 1);

	
	QPushButton *button = new QPushButton("getOpenFileName", this);
    grid->addMultiCellWidget(button, 3, 3, 0,  1);
    connect(button, SIGNAL(clicked()), this, SLOT(getOpenFileName()));


	f = new QFrame(this);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	grid->addMultiCellWidget(f,4,4,0,1);
//-----

    label = new QLabel("filter:", this);
    grid->addWidget(label, 5, 0, Qt::AlignRight);

    pFilterEdit = new QLineEdit(this);
    grid->addWidget(pFilterEdit, 5, 1);

    button = new QPushButton("getSaveFileName", this);
    grid->addMultiCellWidget(button, 6, 6, 0, 1);
    connect(button, SIGNAL(clicked()), this, SLOT(getSaveFileName()));

	f = new QFrame(this);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	grid->addMultiCellWidget(f,7,7,0,1);

//---


    label = new QLabel("Initial dir:", this);
    grid->addWidget(label, 8, 0, Qt::AlignRight);

    pDirEdit = new QLineEdit(this);
    grid->addWidget(pDirEdit, 8, 1);

    pDirOnlyChBox = new QCheckBox("dirOnly", this);
    grid->addMultiCellWidget(pDirOnlyChBox, 10, 10, 0, 1);

    button = new QPushButton("getExistingDirectory", this);
    grid->addMultiCellWidget(button, 9, 9, 0,1 );
    connect(button, SIGNAL(clicked()), this, SLOT(getExistingDirectory()));

	f = new QFrame(this);
	f->setFrameStyle(QFrame::HLine | QFrame::Sunken);
	grid->addMultiCellWidget(f,11,11,0,1);

//-----
	pResolveSymlinksChBox = new QCheckBox("resolveSymlinks", this);
    grid->addMultiCellWidget(pResolveSymlinksChBox, 13, 13, 0, 1);

    button = new QPushButton("getOpenFileNames", this);
    grid->addMultiCellWidget(button, 12, 12, 0, 1);
    connect(button, SIGNAL(clicked()), this, SLOT(getOpenFileNames()));

    setFixedSize(240,320);
}

void FileDialogTestView::contextMenuEvent( QContextMenuEvent * )
{
   QColor x2(202,202,202);
   QColorGroup g2(black,x2,x2.light(),x2.dark(),x2.dark(120),white/*black*/,white);
   QPalette p2(g2,g2,g2);


    QPopupMenu*	contextMenu = new QPopupMenu( this );
    Q_CHECK_PTR( contextMenu );
    QLabel *caption = new QLabel( "<font color=darkblue><b>"
	" M e n u</b></font>", this );
//        QLabel *caption = new QLabel( "<font color=darkblue><u><b>"
//	" Menu</b></u></font>", this );

    caption->setFrameStyle (QLabel::Panel|QLabel::Raised);
    caption->setAlignment( Qt::AlignCenter );
    caption->setPalette(p2);
//    caption->setBackgroundColor( QColor( 94, 128, 180 ) );

    contextMenu->setFrameStyle (QLabel::WinPanel|QLabel::Raised);
    contextMenu->insertItem( caption );
    contextMenu->insertItem( "&Quit - pass",  this, SLOT(exitPass()), CTRL+Key_Q );
    contextMenu->insertItem( "E&xit - fail",  this, SLOT(exitFail()), CTRL+Key_X );
    contextMenu->exec( QCursor::pos() );
    delete contextMenu;
}


void FileDialogTestView::exitPass()
{
	QMessageBox::information(this, "Test Pass", "Exiting with test pass");
	qApp->exit(0);
}

void FileDialogTestView::exitFail()
{
	QMessageBox::information(this, "Test Fail", "Exiting with test fail");
	qApp->exit(1);
}





static QString selFilter;
static QString res;

	void FileDialogTestView::getOpenFileName()
	{
		res = QFileDialog::getOpenFileName(get_startWith(), 
						get_filter(), this, 0, get_caption(), 
						&selFilter, get_resolveSymlinks());

		QMessageBox::information(this, "QFileDialog::getOpenFileName() result",
			QString("Returned value is: ") + 
				content_or_null( res ) + QString("\nselectedFilter is: ") + selFilter
		);
	}

void FileDialogTestView::getSaveFileName()
{
	res =	QFileDialog::getSaveFileName(get_startWith(),
                    get_filter(), this, 0, get_caption(), 
                    &selFilter, get_resolveSymlinks());	
    QMessageBox::information(this, "QFileDialog::getSaveFileName() result", 
        QString("Returned value is: ") + 
            content_or_null(res) + QString("\nselectedFilter is: ") + selFilter
    );
}

void FileDialogTestView::getExistingDirectory()
{
	res =  QFileDialog::getExistingDirectory(get_dir(),
                    this, 0, get_caption(), get_dirOnly(), 
                    get_resolveSymlinks());

    QMessageBox::information(this, "QFileDialog::getExistingDirectory() result",
        QString("Returned value is: ") + content_or_null(res) 
    );
}

void FileDialogTestView::getOpenFileNames()
{
	QStringList list = QFileDialog::getOpenFileNames(get_filter(),
					get_dir(), this, 0, get_caption(), &selFilter,
					get_resolveSymlinks());
	res = strlist_to_string(list);
    QMessageBox::information(this, "QFileDialog::getOpenFileNames() result",
        QString("Returned value is: ") +
            content_or_null(res)
		    + QString("\nselectedFilter is: ") + selFilter
       );
}

QString FileDialogTestView::get_startWith()
{
    QString str = pStartWithEdit->text();
	return null_if_empty(res );

}

QString FileDialogTestView::get_filter()
{
    QString str = pFilterEdit->text();
    return null_if_empty(str);

}

QString FileDialogTestView::get_caption()
{
	QString str = pCaptionEdit->text();
    return null_if_empty(str);
}

bool FileDialogTestView::get_resolveSymlinks()
{
    return pResolveSymlinksChBox->isOn();
}

bool FileDialogTestView::get_dirOnly()
{
    return pDirOnlyChBox->isOn();
}

QString FileDialogTestView::get_dir()
{
	QString str = pDirEdit->text();
    return null_if_empty(str);
}

static QString null_if_empty(QString& s)
{
    if (!s.isEmpty())
    {
        return s;
    }
    else 
    {
        return QString::null;
    }
}

static QString content_or_null(QString& s)
{
    if (s.isNull())
    {
        return QString("QString::null");
    }
    else
    {
        return "\"" + s + "\"";
    }
}

static QString strlist_to_string(QStringList& sl)
{
    return sl.join("\n");
}
