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

Irina Inkina                27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
#include <qwidget.h>

class QLineEdit;
class QComboBox;

class MsgBoxView : public QWidget
{
    Q_OBJECT
public slots:
    void keyPressEvent(QKeyEvent *e);
    void exitFail();
    void exitPass();

 public:
 int VT_rv;
    MsgBoxView::MsgBoxView();
    virtual ~MsgBoxView();

private slots:
    void showInformation();
    void showQuestion();
    void showWarning();
    void showCritical();
    void showAbout();
    void showAboutQt();

private:
    QLineEdit* pCaptionEdit;
    QLineEdit* pTextEdit;
    QComboBox* b0Combo;
    QComboBox* b1Combo;
    QComboBox* b2Combo;

    typedef struct T_BUTTONS_DATA{
        bool isCorrect; // is structure contains correct data
        bool type;      // FALSE - list of int, TRUE - list of QString
        union {
            struct {
                int button0;
                int button1;
                int button2;
            }   ints;
            struct {
                QString *button0Text;
                QString *button1Text;
                QString *button2Text;
            }   strings;
        }   buttons;

//        ~T_BUTTONS_DATA();
    }   BUTTONS_DATA;

    BUTTONS_DATA buttons_data;

    bool fillButtonsData();
    void contextMenuEvent( QContextMenuEvent * );

};
