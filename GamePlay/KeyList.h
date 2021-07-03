//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Monday, 1 July 2002
//
//========================================================================================


#ifndef _KEYLIST_H_
#define _KEYLIST_H_

#define MAXKEYSINLIST       64


//----------------------------------------------------------------------------------------
// public structures
//----------------------------------------------------------------------------------------

typedef struct KeyDesc_s
{
    const std::string KeyName;
    const std::string Description;
}
KeyDesc_t;

typedef struct KeyList_s
{
    size_t KeyCount;
    KeyDesc_t Keys[ MAXKEYSINLIST ];
}
KeyList_t;


//----------------------------------------------------------------------------------------
// public data
//----------------------------------------------------------------------------------------

extern KeyList_t g_MouseLookKeys;
extern KeyList_t g_MouseLookWaitKeys;
extern KeyList_t g_WaitForShotKeys;
extern KeyList_t g_GetShotInfoKeys;
extern KeyList_t g_TankMoveKeys;


#endif
