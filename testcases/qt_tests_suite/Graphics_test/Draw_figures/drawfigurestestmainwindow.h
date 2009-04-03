/****************************************************************************
** Form interface generated from reading ui file '.\drawfigurestestmainwindow.ui'
**
** Created: Thu May 20 15:01:19 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.2   edited Nov 24 13:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DRAWFIGURESTESTMAINWINDOW_H
#define DRAWFIGURESTESTMAINWINDOW_H

#include <qvariant.h>
#include <qmainwindow.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;

class DrawFiguresTestMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    DrawFiguresTestMainWindow( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~DrawFiguresTestMainWindow();


protected:

protected slots:
    virtual void languageChange();

};

#endif // DRAWFIGURESTESTMAINWINDOW_H
