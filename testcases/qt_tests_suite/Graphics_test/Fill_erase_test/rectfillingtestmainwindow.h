/****************************************************************************
** Form interface generated from reading ui file '.\rectfillingtestmainwindow.ui'
**
** Created: Fri May 14 13:20:18 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.2   edited Nov 24 13:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef RECTFILLINGTESTMAINWINDOW_H
#define RECTFILLINGTESTMAINWINDOW_H

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

class RectFillingTestMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    RectFillingTestMainWindow( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~RectFillingTestMainWindow();


protected:

protected slots:
    virtual void languageChange();

};

#endif // RECTFILLINGTESTMAINWINDOW_H
