#ifndef BRUSHTESTMAINWINDOW_H
#define BRUSHTESTMAINWINDOW_H

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

class BrushTestMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    BrushTestMainWindow( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
virtual   ~BrushTestMainWindow();

protected:

protected slots:
    virtual void languageChange();

};

#endif // BRUSHTESTMAINWINDOW_H
