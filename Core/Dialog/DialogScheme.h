//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Friday, 5 April 2002
//-----------------------------------------------------------------------------


#ifndef _dlg_scheme_h_
#define _dlg_scheme_h_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Font.h"

#define DIALOGSCHEME_MAX        4


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

#ifndef _COLOR_STRUCT_
#define _COLOR_STRUCT_

typedef struct Color_s
{
    float R, G, B, A;
}
Color_t;

#endif

typedef struct DialogScheme_s
{
    std::string Name;

    struct Fonts_s
    {
        Font_t* TitleFocus;
        Font_t* TitleNoFocus;
        Font_t* ControlNormal;
        Font_t* ControlHighlight;
        Font_t* ControlDisabled;
    }
    Fonts;

    struct Colors_s
    {
        Color_t FontTitleFocus;
        Color_t FontTitleNoFocus;
        Color_t FontControlNormal;
        Color_t FontControlHighlight;
        Color_t FontControlDisabled;

        Color_t DialogBack;
        Color_t TitleFocus;
        Color_t TitleNoFocus;
        Color_t Raised;
        Color_t Sunken;
        Color_t Focus;
        Color_t ControlBack;
    }
    Colors;
}
DialogScheme_t;


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern DialogScheme_t* DialogScheme;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

size_t DialogSchemeInitialise(HDC hDC);
void DialogSchemeShutdown(void);

void DialogSchemeReloadFonts(void);

void DialogSchemeSet(const std::string& Name);

std::string DialogSchemeGetFirst(size_t* Index);
std::string DialogSchemeGetNext(size_t* Index);


#endif
