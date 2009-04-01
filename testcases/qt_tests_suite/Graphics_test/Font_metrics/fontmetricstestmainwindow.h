/****************************************************************************
** Form interface generated from reading ui file '.\fontmetricstestmainwindow.ui'
**
** Created: Thu May 13 11:40:18 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.2   edited Nov 24 13:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef FONTMETRICSTESTMAINWINDOW_H
#define FONTMETRICSTESTMAINWINDOW_H

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

class FontMetricsTestMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    FontMetricsTestMainWindow( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~FontMetricsTestMainWindow();


protected:

protected slots:
    virtual void languageChange();

};

#endif // FONTMETRICSTESTMAINWINDOW_H
