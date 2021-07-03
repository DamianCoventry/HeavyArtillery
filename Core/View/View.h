//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Tuesday, 25 June 2002
//
//========================================================================================


#include "Vector.h"


extern void SetupViewTransform( void );
extern void ExtractViewFrustumPlanes( void );
extern Plane2_t *GetViewFrustum( void );

extern void SetFirstPersonView( size_t PlayerIDTank, size_t PlayerIDBarrell );
extern void SetThirdPersonView( size_t PlayerIDTank, size_t PlayerIDBarrell );
extern void SetChaseView( size_t PlayerIDTank, size_t PlayerIDBarrell );
extern void SetBirdsEyeView( size_t PlayerIDTank, size_t PlayerIDBarrell );
extern void SetMouseLookView( size_t PlayerIDTank, size_t PlayerIDBarrell );

extern void UpdateFirstPersonView( void );
extern void UpdateThirdPersonView( void );
extern void UpdateChaseView( void );
extern void UpdateBirdsEyeView( void );

extern void (*UpdateView)( void );

extern bool IsLineOfSight(const Vector_t& From, const Vector_t& To);
extern bool IsConeOfSight(const Vector_t& From, const Vector_t& To, float ConeBaseRadius, size_t NumConeBasePoints, size_t& NumCollidingPoints);
extern bool IsCylinderOfSight(const Vector_t& From, const Vector_t& To, float CylinderRadius, size_t NumCylinderPoints, size_t& NumCollidingPoints);

extern void ResetLineOfSightDebugInfo();
