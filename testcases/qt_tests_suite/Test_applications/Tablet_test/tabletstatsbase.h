/****************************************************************************
** Form interface generated from reading ui file 'tabletstatsbase.ui'
**
** Created: Thu May 6 11:01:33 2004
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.2   edited Nov 24 13:47 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef TABLETSTATSBASE_H
#define TABLETSTATSBASE_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class MyOrientation;
class StatsCanvas;
class QLabel;

class TabletStatsBase : public QWidget
{
    Q_OBJECT

public:
    TabletStatsBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~TabletStatsBase();

    QLabel* TextLabel1;
    QLabel* lblXPos;
    QLabel* TextLabel3;
    QLabel* lblYPos;
    QLabel* TextLabel5;
    QLabel* lblPressure;
    QLabel* TextLabel7;
    QLabel* lblDev;
    QLabel* TextLabel9;
    QLabel* TextLabel10;
    QLabel* lblXTilt;
    QLabel* TextLabel12;
    QLabel* lblYTilt;
    MyOrientation* orient;
    StatsCanvas* statCan;

protected:
    QHBoxLayout* TabletStatsBaseLayout;
    QVBoxLayout* Layout7;
    QHBoxLayout* Layout5;
    QHBoxLayout* Layout4;
    QHBoxLayout* Layout3;
    QHBoxLayout* Layout2;
    QHBoxLayout* Layout1;

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;
    QPixmap image1;

};

#endif // TABLETSTATSBASE_H
