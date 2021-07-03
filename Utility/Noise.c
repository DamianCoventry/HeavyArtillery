//========================================================================================
//
// Heavy Artillery
// Copyright (c) 2002, All rights reserved
//
// Damian Coventry
// Thursday, 27 June 2002
//
//========================================================================================


#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define BUFFER_SIZE             64
#define MAX_MOUNTAIN_HEIGHT     4096
#define MAX_CENTER_HEIGHT       2048

#define DTOR                    0.017453292519943295769236907684886f

#define Index( _X, _Y )         ( (_Y) * BUFFER_SIZE + (_X) )


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static void CreateBaseNoise( short *Buffer );
static void CreateEdgeMountains( short *Buffer, unsigned short BorderSize );
static void CreateCenterHills( short *Buffer );
static void CreateMountainRange( short *Buffer, unsigned short X1, unsigned short Y1, unsigned short X2, unsigned short Y2, unsigned short Radius );

static void AirBrushMountain( short *Buffer, unsigned short X, unsigned short Y, unsigned short Radius, unsigned short MaxHeight );
static void AirBrush( short *Buffer, unsigned short X, unsigned short Y, unsigned short Radius, unsigned short MaxHeight );

static void Smooth( short *Buffer );
static unsigned short GetSides( short *Buffer, unsigned short X, unsigned short Y );
static unsigned short GetDiagonals( short *Buffer, unsigned short X, unsigned short Y );

static void SetEdges( short *Buffer, unsigned short Value );
static void InverseBuffer( short *Buffer );


//-----------------------------------------------------------------------------
// Name: NoiseCanyon()
// Desc: 
//-----------------------------------------------------------------------------

void NoiseCanyon( short *Buffer, void (*Progress)( unsigned short, unsigned short ) )
{
    unsigned short i;

    Progress( 0, 4 );
    CreateBaseNoise( Buffer );

    Progress( 1, 0 );
    CreateEdgeMountains( Buffer, BUFFER_SIZE / 8 );

    Progress( 1, 0 );
    CreateCenterHills( Buffer );

    Progress( 1, 0 );
    for( i = 0; i < 3; i++ )
        Smooth( Buffer );

    Progress( 2, 0 );
}


//-----------------------------------------------------------------------------
// Name: NoiseIsland()
// Desc: 
//-----------------------------------------------------------------------------

void NoiseIsland( short *Buffer )
{
    unsigned short i;

    CreateBaseNoise( Buffer );
    CreateEdgeMountains( Buffer, BUFFER_SIZE / 8 );
    CreateCenterHills( Buffer );

    InverseBuffer( Buffer );
    //SetEdges( Buffer, 0 );

    for( i = 0; i < 3; i++ )
        Smooth( Buffer );
}


//-----------------------------------------------------------------------------
// Name: CreateBaseNoise()
// Desc: 
//-----------------------------------------------------------------------------

static void CreateBaseNoise( short *Buffer )
{
    unsigned short Y, X;

    for( Y = 0; Y < BUFFER_SIZE; Y++ )
        for( X = 0; X < BUFFER_SIZE; X++ )
            Buffer[ Index( X, Y ) ] = rand() % 512;
}


//-----------------------------------------------------------------------------
// Name: CreateEdgeMountains()
// Desc: 
//-----------------------------------------------------------------------------

static void CreateEdgeMountains( short *Buffer, unsigned short BorderSize )
{
    unsigned short Radius = BorderSize / 2;

    // create 4 mountain ranges along the edge of the buffer

    CreateMountainRange( Buffer, Radius, Radius, BUFFER_SIZE-BorderSize-Radius, Radius, Radius );
    CreateMountainRange( Buffer, BUFFER_SIZE-Radius, Radius, BUFFER_SIZE-Radius, BUFFER_SIZE-BorderSize-Radius, Radius );

    CreateMountainRange( Buffer, BUFFER_SIZE-Radius, BUFFER_SIZE-Radius, BorderSize+Radius, BUFFER_SIZE-Radius, Radius );
    CreateMountainRange( Buffer, Radius, BUFFER_SIZE-Radius, Radius, Radius+BorderSize, Radius );
}


//-----------------------------------------------------------------------------
// Name: CreateCenterHills()
// Desc: 
//-----------------------------------------------------------------------------

static void CreateCenterHills( short *Buffer )
{
    unsigned short i, Count;
    unsigned short Offset, Size;
    unsigned short Radius;

    // add some mountains to the buffer

    Count = rand()%16+32;

    for( i = 0; i < Count; i++ )
    {
        Radius  = rand() % 8 + 4;
        Size    = 48 - ( Radius * 2 );
        Offset  = Size / 2;

        AirBrushMountain( Buffer,
            rand() % Size + Offset, rand() % Size + Offset,
            Radius, MAX_CENTER_HEIGHT );
    }
}


//-----------------------------------------------------------------------------
// Name: CreateMountainRange()
// Desc: 
//-----------------------------------------------------------------------------

static void CreateMountainRange( short *Buffer, unsigned short X1, unsigned short Y1, unsigned short X2, unsigned short Y2, unsigned short Radius )
{
    unsigned short Step, StepMax;
    unsigned short X, Y;
    unsigned short DeltaX, DeltaY;

    StepMax = rand() % 16 + 32;

    DeltaX = ( X2 - X1 ) / StepMax;
    DeltaY = ( Y2 - Y1 ) / StepMax;

    X = X1;
    Y = Y1;

    for( Step = 0; Step < 2*StepMax; Step++ )
    {
        AirBrushMountain( Buffer, X+(rand()%5-2), Y+(rand()%5-2), Radius, MAX_MOUNTAIN_HEIGHT );

        X += DeltaX;
        Y += DeltaY;
    }
}


//-----------------------------------------------------------------------------
// Name: AirBrushMountain()
// Desc: 
//-----------------------------------------------------------------------------

static void AirBrushMountain( short *Buffer, unsigned short X, unsigned short Y, unsigned short MaxRadius, unsigned short MaxHeight )
{
    unsigned short i, Radius;
    unsigned short Count;

    // create a mountain by airbrushing a random amount at the
    // given radius, then decrease the radius and loop. stop the
    // loop when the radius is small enough.

    Count = (rand()%25+25);

    for( i = 0; i < Count; i++ )
    {
        for( Radius = MaxRadius; Radius > 1; Radius -= 2 )
            AirBrush( Buffer, X, Y, Radius, MaxHeight );
    }
}


//-----------------------------------------------------------------------------
// Name: AirBrush()
// Desc: 
//-----------------------------------------------------------------------------

static void AirBrush( short *Buffer, unsigned short X, unsigned short Y, unsigned short Radius, unsigned short MaxHeight )
{
    unsigned short i, MaxPixels;
    float Angle;
    unsigned short OffsetX, OffsetY;
    unsigned short NewX, NewY;

    // add a random amount of pixels to the buffer in a
    // circular pattern.  check for and handle overflow.

    MaxPixels = (rand()%3+3);

    for( i = 0; i < MaxPixels; i++ )
    {
        for( Angle = 0.0f; Angle < 360.0f; Angle += 45.0f )
        {
            OffsetX = (unsigned short)( (float)((unsigned short)rand()%Radius) * (float)sin( DTOR * Angle ) );
            OffsetY = (unsigned short)( (float)((unsigned short)rand()%Radius) * (float)cos( DTOR * Angle ) );

            NewX = X+OffsetX;
            NewY = Y+OffsetY;

            if( NewX >= 0 && NewX < BUFFER_SIZE && NewY >= 0 && NewY < BUFFER_SIZE )
            {
                Buffer[ Index( NewX, NewY ) ] += (rand()%4+4);

                if( Buffer[ Index( NewX, NewY ) ] > MaxHeight )
                    Buffer[ Index( NewX, NewY ) ] = MaxHeight;
            }
        }
    }
}


//-----------------------------------------------------------------------------
// Name: Smooth()
// Desc: 
//-----------------------------------------------------------------------------

static void Smooth( short *Buffer )
{
    unsigned short X, Y;
    unsigned short Center, Sides, Diags;

    for( Y = 0; Y < BUFFER_SIZE; Y++ )
    {
        for( X = 0; X < BUFFER_SIZE; X++ )
        {
            Sides = GetSides( Buffer, X, Y );
            Diags = GetDiagonals( Buffer, X, Y );
            Center = Buffer[ Index( X, Y ) ];

            Buffer[ Index( X, Y ) ] = ( Center / 4 ) + ( Sides / 8 ) + ( Diags / 16 );
        }
    }
}


//-----------------------------------------------------------------------------
// Name: GetSides()
// Desc: 
//-----------------------------------------------------------------------------

static unsigned short GetSides( short *Buffer, unsigned short X, unsigned short Y )
{
    unsigned short Sides = 0;

    if( X > 0 )
        Sides += Buffer[ Index( X-1, Y ) ];

    if( X < BUFFER_SIZE - 1 )
        Sides += Buffer[ Index( X+1, Y ) ];

    if( Y > 0 )
        Sides += Buffer[ Index( X, Y-1 ) ];

    if( Y < BUFFER_SIZE - 1 )
        Sides += Buffer[ Index( X, Y+1 ) ];

    return Sides;
}


//-----------------------------------------------------------------------------
// Name: GetDiagonals()
// Desc: 
//-----------------------------------------------------------------------------

static unsigned short GetDiagonals( short *Buffer, unsigned short X, unsigned short Y )
{
    unsigned short Diags = 0;

    if( X > 0 )
    {
        if( Y > 0 )
            Diags += Buffer[ Index( X-1, Y-1 ) ];

        if( Y < BUFFER_SIZE - 1 )
            Diags += Buffer[ Index( X-1, Y+1 ) ];
    }

    if( X < BUFFER_SIZE - 1 )
    {
        if( Y > 0 )
            Diags += Buffer[ Index( X+1, Y-1 ) ];

        if( Y < BUFFER_SIZE - 1 )
            Diags += Buffer[ Index( X+1, Y+1 ) ];
    }

    return Diags;
}


//-----------------------------------------------------------------------------
// Name: InverseBuffer()
// Desc: 
//-----------------------------------------------------------------------------

static void InverseBuffer( short *Buffer )
{
    unsigned short X, Y;

    for( Y = 0; Y < BUFFER_SIZE; Y++ )
        for( X = 0; X < BUFFER_SIZE; X++ )
            Buffer[ Index( X, Y ) ] = MAX_MOUNTAIN_HEIGHT - Buffer[ Index( X, Y ) ];
}


//-----------------------------------------------------------------------------
// Name: SetEdges()
// Desc: 
//-----------------------------------------------------------------------------

static void SetEdges( short *Buffer, unsigned short Value )
{
    unsigned short X, Y;

    for( Y = 0; Y < BUFFER_SIZE; Y++ )
    {
        Buffer[ Index( 0, Y ) ] = Value;
        Buffer[ Index( BUFFER_SIZE - 1, Y ) ] = Value;
    }
    for( X = 0; X < BUFFER_SIZE; X++ )
    {
        Buffer[ Index( X, 0 ) ] = Value;
        Buffer[ Index( X, BUFFER_SIZE - 1 ) ] = Value;
    }
}
