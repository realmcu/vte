/*
 * KAsteroids - Copyright (c) Martin R. Jones 1997
 *
 * Part of the KDE project
 */
/*================================================================================================*/
/**
    @file   Asteroid.h

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
Irina Inkina                27/07/2004      ?????????   Initial version

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

#ifndef __KAST_TOPLEVEL_H__
#define __KAST_TOPLEVEL_H__

#include <qmainwindow.h>
#include <qdict.h>
#include <qmap.h>

#include "view.h"


class KALedMeter;
class QLCDNumber;

class KAstTopLevel : public QMainWindow
{
    Q_OBJECT
public:
    KAstTopLevel( QWidget *parent=0, const char *name=0 );
    virtual ~KAstTopLevel();
public slots:
  void exitFail();
  void exitPass();
    
private:
    void playSound( const char *snd );
    void readSoundMapping();
    void doStats();
    void contextMenuEvent( QContextMenuEvent * );

protected:
    virtual void showEvent( QShowEvent * );
    virtual void hideEvent( QHideEvent * );
    virtual void keyPressEvent( QKeyEvent *event );
    virtual void keyReleaseEvent( QKeyEvent *event );

private slots:
    void slotNewGame();

    void slotShipKilled();
    void slotRockHit( int size );
    void slotRocksRemoved();

    void slotUpdateVitals();

private:
    KAsteroidsView *view;
    QLCDNumber *scoreLCD;
    QLCDNumber *levelLCD;
    QLCDNumber *shipsLCD;

    QLCDNumber *teleportsLCD;
//    QLCDNumber *bombsLCD;
    QLCDNumber *brakesLCD;
    QLCDNumber *shieldLCD;
    QLCDNumber *shootLCD;
    KALedMeter *powerMeter;

    bool   sound;
    QDict<QString> soundDict;

    // waiting for user to press Enter to launch a ship
    bool waitShip;
    bool isPaused;

    int shipsRemain;
    int score;
    int level;
    bool showHiscores;

    enum Action { Launch, Thrust, RotateLeft, RotateRight, Shoot, Teleport,
                    Brake, Shield, Pause, NewGame  };

    QMap<int,Action> actions;
};

#endif

