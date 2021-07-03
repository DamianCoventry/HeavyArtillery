//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Tuesday, 25 June 2002
//
//========================================================================================


extern int InitialiseTimer(void);
extern void StampInitialTime(void);
extern void StampLoopTime(void);
extern bool IsTimeForFrame();
extern void BeginFrame(void);
extern void EndFrame(void);
extern std::string GetFPS(void);
extern LONGLONG GetNowMs();
