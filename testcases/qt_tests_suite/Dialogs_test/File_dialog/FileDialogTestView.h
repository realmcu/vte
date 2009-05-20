/*================================================================================================*/
/**
    @file   FileDialogTestView.h
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
#include <qwidget.h>

class QLineEdit;
class QCheckBox;
class QString;

class FileDialogTestView : public QWidget
{
    Q_OBJECT
public:
    FileDialogTestView();

public slots:
    void exitFail();
    void exitPass();


private slots:
    void getOpenFileName();
    void getSaveFileName();
    void getExistingDirectory();
    void getOpenFileNames();

private:
    QLineEdit *pCaptionEdit;
    QLineEdit *pStartWithEdit;
    QLineEdit *pFilterEdit;
    QLineEdit *pDirEdit;
    QCheckBox *pDirOnlyChBox;
    QCheckBox *pResolveSymlinksChBox;

private:
    QString get_startWith();
    QString get_filter();
    QString get_caption();
    bool get_resolveSymlinks();
    bool get_dirOnly();
    QString get_dir();

    void contextMenuEvent( QContextMenuEvent * );

};
