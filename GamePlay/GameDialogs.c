//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"
#include <windows.h>

#include "LoadingScreen.h"

#include "DialogFlyBy.h"
#include "DialogMain.h"
#include "DialogHelp.h"
#include "DialogAppOptions.h"
#include "DialogGameOptions.h"
#include "DialogNewGame.h"
#include "DialogPlayer.h"
#include "DialogBot.h"
#include "DialogLevelStart.h"
#include "DialogShop.h"
#include "DialogProperties.h"
#include "DialogGameToolbar.h"
#include "DialogPopupGame.h"
#include "DialogPopupHelp.h"
#include "DialogPopupTank.h"
#include "DialogPopupTools.h"
#include "DialogPopupView.h"
#include "DialogCamera.h"
#include "DialogShotInfo.h"
#include "DialogKeys.h"
#include "DialogTankMove.h"
#include "DialogWeapon.h"
#include "DialogPlayOrder.h"
#include "DialogMyTank.h"
#include "DialogFindTank.h"
#include "DialogBeginTurn.h"
#include "DialogBotToolbar.h"
#include "DialogStats.h"

#include "Log.h"


//-----------------------------------------------------------------------------
// Name: CreateDialogs()
// Desc: 
//-----------------------------------------------------------------------------

size_t CreateDialogs( void )
{
    LogWrite( "CreateDialogs() creating...\n" );

    BeginLoadingScreen( TextureGame, "Creating Dialogs...", 0, 28 );

    DialogFlyByCreate();        UpdateLoadingScreen( 0 );
    DialogMainCreate();         UpdateLoadingScreen( 1 );
    DialogHelpCreate();         UpdateLoadingScreen( 2 );
    DialogAppOptionsCreate();   UpdateLoadingScreen( 3 );
    DialogGameOptionsCreate();  UpdateLoadingScreen( 4 );
    DialogNewGameCreate();      UpdateLoadingScreen( 5 );
    DialogPlayerCreate();       UpdateLoadingScreen( 6 );
    DialogBotCreate();          UpdateLoadingScreen( 7 );
    DialogLevelStartCreate();   UpdateLoadingScreen( 8 );
    DialogShopCreate();         UpdateLoadingScreen( 9 );
    DialogPropertiesCreate();   UpdateLoadingScreen( 10 );
    DialogGameToolbarCreate();  UpdateLoadingScreen( 11 );
    DialogPopupGameCreate();    UpdateLoadingScreen( 12 );
    DialogPopupHelpCreate();    UpdateLoadingScreen( 13 );
    DialogPopupTankCreate();    UpdateLoadingScreen( 14 );
    DialogPopupToolsCreate();   UpdateLoadingScreen( 15 );
    DialogPopupViewCreate();    UpdateLoadingScreen( 16 );
    DialogCameraCreate();       UpdateLoadingScreen( 17 );
    DialogShotInfoCreate();     UpdateLoadingScreen( 18 );
    DialogKeysCreate();         UpdateLoadingScreen( 19 );
    DialogTankMoveCreate();     UpdateLoadingScreen( 20 );
    DialogPlayOrderCreate();    UpdateLoadingScreen( 21 );
    DialogWeaponCreate();       UpdateLoadingScreen( 22 );
    DialogMyTankCreate();       UpdateLoadingScreen( 23 );
    DialogFindTankCreate();     UpdateLoadingScreen( 24 );
    DialogBeginTurnCreate();    UpdateLoadingScreen( 25 );
    DialogBotToolbarCreate();   UpdateLoadingScreen( 26 );
    DialogStatsCreate();        UpdateLoadingScreen( 27 );

    EndLoadingScreen( "Creating Dialogs...Done", 28 );

    LogWrite( "CreateDialogs() created\n" );

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DeleteDialogs()
// Desc: 
//-----------------------------------------------------------------------------

void DeleteDialogs( void )
{
    LogWrite( "DeleteDialogs() deleting...\n" );

    DialogSend( DialogBotToolbar, WM_DESTROY, 0, 0 );
    DialogSend( DialogBeginTurn, WM_DESTROY, 0, 0 );
    DialogSend( DialogFindTank, WM_DESTROY, 0, 0 );
    DialogSend( DialogMyTank, WM_DESTROY, 0, 0 );
    DialogSend( DialogWeapon, WM_DESTROY, 0, 0 );
    DialogSend( DialogPlayOrder, WM_DESTROY, 0, 0 );
    DialogSend( DialogTankMove, WM_DESTROY, 0, 0 );
    DialogSend( DialogKeys, WM_DESTROY, 0, 0 );
    DialogSend( DialogShotInfo, WM_DESTROY, 0, 0 );
    DialogSend( DialogCamera, WM_DESTROY, 0, 0 );
    DialogSend( DialogPopupView, WM_DESTROY, 0, 0 );
    DialogSend( DialogPopupTools, WM_DESTROY, 0, 0 );
    DialogSend( DialogPopupTank, WM_DESTROY, 0, 0 );
    DialogSend( DialogPopupHelp, WM_DESTROY, 0, 0 );
    DialogSend( DialogPopupGame, WM_DESTROY, 0, 0 );
    DialogSend( DialogGameToolbar, WM_DESTROY, 0, 0 );
    DialogSend( DialogProperties, WM_DESTROY, 0, 0 );
    DialogSend( DialogShop, WM_DESTROY, 0, 0 );
    DialogSend( DialogLevelStart, WM_DESTROY, 0, 0 );
    DialogSend( DialogBot, WM_DESTROY, 0, 0 );
    DialogSend( DialogPlayer, WM_DESTROY, 0, 0 );
    DialogSend( DialogNewGame, WM_DESTROY, 0, 0 );
    DialogSend( DialogGameOptions, WM_DESTROY, 0, 0 );
    DialogSend( DialogAppOptions, WM_DESTROY, 0, 0 );
    DialogSend( DialogHelp, WM_DESTROY, 0, 0 );
    DialogSend( DialogMain, WM_DESTROY, 0, 0 );
    DialogSend( DialogFlyBy, WM_DESTROY, 0, 0 );

    LogWrite( "DeleteDialogs() deleted\n" );
}
