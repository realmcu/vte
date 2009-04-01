/****************************************************************************
** Form interface generated from reading ui file '.\fonttestmainwindow.ui'
**
** Created: Thu May 13 10:18:09 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.2   edited Nov 24 13:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef FONTTESTMAINWINDOW_H
#define FONTTESTMAINWINDOW_H

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

class FontTestMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    FontTestMainWindow( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~FontTestMainWindow();


protected:

protected slots:
    virtual void languageChange();

};

#endif // FONTTESTMAINWINDOW_H
