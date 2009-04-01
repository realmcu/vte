/*================================================================================================*/
/**
    @file   canvas.h

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
Irina Inkina           27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <qpopupmenu.h>
#include <qmainwindow.h>
#include <qintdict.h>
#include <qcanvas.h>




class FigureEditor : public QCanvasView {
    Q_OBJECT

public:
    FigureEditor(QCanvas&, QWidget* parent=0, const char* name=0, WFlags f=0);
    void clear();

protected:
    void contentsMousePressEvent(QMouseEvent*);
    void contentsMouseMoveEvent(QMouseEvent*);

signals:
    void status(const QString&);

private:
    QCanvasItem* moving;
    QPoint moving_start;
};



class Main : public QMainWindow {
    Q_OBJECT

public:
     Main(QCanvas&, QWidget* parent=0, const char* name=0, WFlags f=0);
    ~Main();

public slots:
	void exitFail();
	void exitPass();
        void help();

private slots:
    void aboutQt();
    void newView();
    void clear();
    void init();

    void addSprite();
    void addCircle();
    void addHexagon();
    void addPolygon();
    void addSpline();
    void addText();
    void addLine();
    void addRectangle();
    void addMesh();
    void addButterfly();

    void toggleDoubleBuffer();

private:
    QCanvas& canvas;
    FigureEditor *editor;

    QPopupMenu* options;
    QPrinter* printer;
    int dbf_id;
    int z;
};

#endif
