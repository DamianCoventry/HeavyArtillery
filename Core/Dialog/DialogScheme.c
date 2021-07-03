//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Friday, 5 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "DialogScheme.h"
#include "Font.h"
#include "Log.h"
#include "Error.h"

#include <stdlib.h>
#include <string.h>


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

DialogScheme_t* DialogScheme;


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static DialogScheme_t Schemes[DIALOGSCHEME_MAX];
static HDC m_hDC;


//-----------------------------------------------------------------------------
// Name: DialogSchemeInitialise()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogSchemeInitialise(HDC hDC)
{
    m_hDC = hDC;

    LogWrite("DialogSchemeInitialise() beginning...\n");

    Schemes[0].Name                           = "Default";
    Schemes[0].Fonts.TitleFocus               = FontCreate("Tahoma", 16, FW_BOLD, hDC, nullptr);
    Schemes[0].Fonts.TitleNoFocus             = FontCreate("Tahoma", 16, FW_BOLD, hDC, nullptr);
    Schemes[0].Fonts.ControlNormal            = FontCreate("Tahoma", 12, FW_NORMAL, hDC, nullptr);
    Schemes[0].Fonts.ControlHighlight         = FontCreate("Tahoma", 12, FW_NORMAL, hDC, nullptr);
    Schemes[0].Fonts.ControlDisabled          = FontCreate("Tahoma", 12, FW_NORMAL, hDC, nullptr);

    Schemes[0].Colors.FontTitleFocus.R        = 0.8f;
    Schemes[0].Colors.FontTitleFocus.G        = 0.8f;
    Schemes[0].Colors.FontTitleFocus.B        = 0.9f;

    Schemes[0].Colors.FontTitleNoFocus.R      = 0.5f;
    Schemes[0].Colors.FontTitleNoFocus.G      = 0.5f;
    Schemes[0].Colors.FontTitleNoFocus.B      = 0.5f;

    Schemes[0].Colors.FontControlNormal.R     = 0.0f;
    Schemes[0].Colors.FontControlNormal.G     = 0.0f;
    Schemes[0].Colors.FontControlNormal.B     = 0.5f;

    Schemes[0].Colors.FontControlHighlight.R  = 0.8f;
    Schemes[0].Colors.FontControlHighlight.G  = 0.8f;
    Schemes[0].Colors.FontControlHighlight.B  = 0.8f;

    Schemes[0].Colors.FontControlDisabled.R   = 0.4f;
    Schemes[0].Colors.FontControlDisabled.G   = 0.4f;
    Schemes[0].Colors.FontControlDisabled.B   = 0.4f;

    Schemes[0].Colors.DialogBack.R            = 0.600f;
    Schemes[0].Colors.DialogBack.G            = 0.600f;
    Schemes[0].Colors.DialogBack.B            = 0.600f;

    Schemes[0].Colors.TitleFocus.R            = 0.000f;
    Schemes[0].Colors.TitleFocus.G            = 0.000f;
    Schemes[0].Colors.TitleFocus.B            = 0.700f;

    Schemes[0].Colors.TitleNoFocus.R          = 0.700f;
    Schemes[0].Colors.TitleNoFocus.G          = 0.700f;
    Schemes[0].Colors.TitleNoFocus.B          = 0.700f;

    Schemes[0].Colors.Raised.R                = 0.750f;
    Schemes[0].Colors.Raised.G                = 0.750f;
    Schemes[0].Colors.Raised.B                = 0.750f;

    Schemes[0].Colors.Sunken.R                = 0.300f;
    Schemes[0].Colors.Sunken.G                = 0.300f;
    Schemes[0].Colors.Sunken.B                = 0.300f;

    Schemes[0].Colors.Focus.R                 = 0.700f;
    Schemes[0].Colors.Focus.G                 = 0.700f;
    Schemes[0].Colors.Focus.B                 = 0.000f;

    Schemes[0].Colors.ControlBack.R           = 0.750f;
    Schemes[0].Colors.ControlBack.G           = 0.750f;
    Schemes[0].Colors.ControlBack.B           = 0.750f;

    if (nullptr == Schemes[0].Fonts.TitleFocus) return 0;
    if (nullptr == Schemes[0].Fonts.TitleNoFocus) return 0;
    if (nullptr == Schemes[0].Fonts.ControlNormal) return 0;
    if (nullptr == Schemes[0].Fonts.ControlHighlight) return 0;
    if (nullptr == Schemes[0].Fonts.ControlDisabled) return 0;
    /*
        Schemes[1].Name                           = "Rainy Day";
        Schemes[1].Fonts.TitleFocus               = FontCreate( "MS Sans Serif", 10, FW_BOLD, hDC, nullptr );
        Schemes[1].Fonts.TitleNoFocus             = FontCreate( "MS Sans Serif", 10, FW_BOLD, hDC, nullptr );
        Schemes[1].Fonts.ControlNormal            = FontCreate( "MS Sans Serif",  8, FW_NORMAL, hDC, nullptr );
        Schemes[1].Fonts.ControlHighlight         = FontCreate( "MS Sans Serif",  8, FW_NORMAL, hDC, nullptr );
        Schemes[1].Fonts.ControlDisabled          = FontCreate( "MS Sans Serif",  8, FW_NORMAL, hDC, nullptr );

        Schemes[1].Colors.FontTitleFocus.R        = 1.0f;
        Schemes[1].Colors.FontTitleFocus.G        = 1.0f;
        Schemes[1].Colors.FontTitleFocus.B        = 1.0f;

        Schemes[1].Colors.FontTitleNoFocus.R      = 0.5f;
        Schemes[1].Colors.FontTitleNoFocus.G      = 0.5f;
        Schemes[1].Colors.FontTitleNoFocus.B      = 0.5f;

        Schemes[1].Colors.FontControlNormal.R     = 0.1f;
        Schemes[1].Colors.FontControlNormal.G     = 0.1f;
        Schemes[1].Colors.FontControlNormal.B     = 0.1f;

        Schemes[1].Colors.FontControlHighlight.R  = 0.8f;
        Schemes[1].Colors.FontControlHighlight.G  = 0.8f;
        Schemes[1].Colors.FontControlHighlight.B  = 0.8f;

        Schemes[1].Colors.FontControlDisabled.R   = 0.6f;
        Schemes[1].Colors.FontControlDisabled.G   = 0.6f;
        Schemes[1].Colors.FontControlDisabled.B   = 0.6f;

        Schemes[1].Colors.DialogBack.R            = 0.300f;
        Schemes[1].Colors.DialogBack.G            = 0.400f;
        Schemes[1].Colors.DialogBack.B            = 0.500f;

        Schemes[1].Colors.TitleFocus.R            = 0.000f;
        Schemes[1].Colors.TitleFocus.G            = 0.000f;
        Schemes[1].Colors.TitleFocus.B            = 0.700f;

        Schemes[1].Colors.TitleNoFocus.R          = 0.600f;
        Schemes[1].Colors.TitleNoFocus.G          = 0.600f;
        Schemes[1].Colors.TitleNoFocus.B          = 0.650f;

        Schemes[1].Colors.Raised.R                = 0.500f;
        Schemes[1].Colors.Raised.G                = 0.600f;
        Schemes[1].Colors.Raised.B                = 0.700f;

        Schemes[1].Colors.Sunken.R                = 0.300f;
        Schemes[1].Colors.Sunken.G                = 0.400f;
        Schemes[1].Colors.Sunken.B                = 0.500f;

        Schemes[1].Colors.Focus.R                 = 0.200f;
        Schemes[1].Colors.Focus.G                 = 0.200f;
        Schemes[1].Colors.Focus.B                 = 0.700f;

        Schemes[1].Colors.ControlBack.R           = 0.600f;
        Schemes[1].Colors.ControlBack.G           = 0.600f;
        Schemes[1].Colors.ControlBack.B           = 0.650f;



        Schemes[2].Name                           = "Wheat";
        Schemes[2].Fonts.TitleFocus               = FontCreate( "Verdana",         10, FW_BOLD, hDC, nullptr );
        Schemes[2].Fonts.TitleNoFocus             = FontCreate( "Verdana",         10, FW_BOLD, hDC, nullptr );
        Schemes[2].Fonts.ControlNormal            = FontCreate( "MS Sans Serif",    8, FW_NORMAL, hDC, nullptr );
        Schemes[2].Fonts.ControlHighlight         = FontCreate( "MS Sans Serif",    8, FW_NORMAL, hDC, nullptr );
        Schemes[2].Fonts.ControlDisabled          = FontCreate( "MS Sans Serif",    8, FW_NORMAL, hDC, nullptr );

        Schemes[2].Colors.FontTitleFocus.R        = 1.0f;
        Schemes[2].Colors.FontTitleFocus.G        = 1.0f;
        Schemes[2].Colors.FontTitleFocus.B        = 1.0f;

        Schemes[2].Colors.FontTitleNoFocus.R      = 0.5f;
        Schemes[2].Colors.FontTitleNoFocus.G      = 0.5f;
        Schemes[2].Colors.FontTitleNoFocus.B      = 0.5f;

        Schemes[2].Colors.FontControlNormal.R     = 0.2f;
        Schemes[2].Colors.FontControlNormal.G     = 0.2f;
        Schemes[2].Colors.FontControlNormal.B     = 0.2f;

        Schemes[2].Colors.FontControlHighlight.R  = 0.8f;
        Schemes[2].Colors.FontControlHighlight.G  = 0.8f;
        Schemes[2].Colors.FontControlHighlight.B  = 0.8f;

        Schemes[2].Colors.FontControlDisabled.R   = 0.5f;
        Schemes[2].Colors.FontControlDisabled.G   = 0.5f;
        Schemes[2].Colors.FontControlDisabled.B   = 0.5f;

        Schemes[2].Colors.DialogBack.R            = 0.500f;
        Schemes[2].Colors.DialogBack.G            = 0.462f;
        Schemes[2].Colors.DialogBack.B            = 0.400f;

        Schemes[2].Colors.TitleFocus.R            = 0.325f;
        Schemes[2].Colors.TitleFocus.G            = 0.262f;
        Schemes[2].Colors.TitleFocus.B            = 0.200f;

        Schemes[2].Colors.TitleNoFocus.R          = 0.625f;
        Schemes[2].Colors.TitleNoFocus.G          = 0.562f;
        Schemes[2].Colors.TitleNoFocus.B          = 0.500f;

        Schemes[2].Colors.Raised.R                = 0.700f;
        Schemes[2].Colors.Raised.G                = 0.6620f;
        Schemes[2].Colors.Raised.B                = 0.600f;

        Schemes[2].Colors.Sunken.R                = 0.400f;
        Schemes[2].Colors.Sunken.G                = 0.350f;
        Schemes[2].Colors.Sunken.B                = 0.300f;

        Schemes[2].Colors.Focus.R                 = 0.700f;
        Schemes[2].Colors.Focus.G                 = 0.700f;
        Schemes[2].Colors.Focus.B                 = 0.000f;

        Schemes[2].Colors.ControlBack.R           = 0.750f;
        Schemes[2].Colors.ControlBack.G           = 0.750f;
        Schemes[2].Colors.ControlBack.B           = 0.750f;



        Schemes[3].Name                           = "Slate";
        Schemes[3].Fonts.TitleFocus               = FontCreate( "Courier New", 10,   FW_BOLD, hDC, nullptr );
        Schemes[3].Fonts.TitleNoFocus             = FontCreate( "Courier New", 10,   FW_BOLD, hDC, nullptr );
        Schemes[3].Fonts.ControlNormal            = FontCreate( "Verdana",      7, FW_NORMAL, hDC, nullptr );
        Schemes[3].Fonts.ControlHighlight         = FontCreate( "Verdana",      7, FW_NORMAL, hDC, nullptr );
        Schemes[3].Fonts.ControlDisabled          = FontCreate( "Verdana",      7, FW_NORMAL, hDC, nullptr );

        Schemes[3].Colors.FontTitleFocus.R        = 0.8f;
        Schemes[3].Colors.FontTitleFocus.G        = 1.0f;
        Schemes[3].Colors.FontTitleFocus.B        = 0.8f;

        Schemes[3].Colors.FontTitleNoFocus.R      = 0.5f;
        Schemes[3].Colors.FontTitleNoFocus.G      = 0.5f;
        Schemes[3].Colors.FontTitleNoFocus.B      = 0.5f;

        Schemes[3].Colors.FontControlNormal.R     = 0.2f;
        Schemes[3].Colors.FontControlNormal.G     = 0.25f;
        Schemes[3].Colors.FontControlNormal.B     = 0.2f;

        Schemes[3].Colors.FontControlHighlight.R  = 0.8f;
        Schemes[3].Colors.FontControlHighlight.G  = 0.9f;
        Schemes[3].Colors.FontControlHighlight.B  = 0.8f;

        Schemes[3].Colors.FontControlDisabled.R   = 0.4f;
        Schemes[3].Colors.FontControlDisabled.G   = 0.5f;
        Schemes[3].Colors.FontControlDisabled.B   = 0.4f;

        Schemes[3].Colors.DialogBack.R            = 0.500f;
        Schemes[3].Colors.DialogBack.G            = 0.600f;
        Schemes[3].Colors.DialogBack.B            = 0.500f;

        Schemes[3].Colors.TitleFocus.R            = 0.100f;
        Schemes[3].Colors.TitleFocus.G            = 0.400f;
        Schemes[3].Colors.TitleFocus.B            = 0.100f;

        Schemes[3].Colors.TitleNoFocus.R          = 0.600f;
        Schemes[3].Colors.TitleNoFocus.G          = 0.700f;
        Schemes[3].Colors.TitleNoFocus.B          = 0.600f;

        Schemes[3].Colors.Raised.R                = 0.600f;
        Schemes[3].Colors.Raised.G                = 0.700f;
        Schemes[3].Colors.Raised.B                = 0.600f;

        Schemes[3].Colors.Sunken.R                = 0.000f;
        Schemes[3].Colors.Sunken.G                = 0.300f;
        Schemes[3].Colors.Sunken.B                = 0.000f;

        Schemes[3].Colors.Focus.R                 = 0.700f;
        Schemes[3].Colors.Focus.G                 = 0.700f;
        Schemes[3].Colors.Focus.B                 = 0.400f;

        Schemes[3].Colors.ControlBack.R           = 0.750f;
        Schemes[3].Colors.ControlBack.G           = 0.750f;
        Schemes[3].Colors.ControlBack.B           = 0.750f;
    */
    DialogScheme = &Schemes[0];

    LogWrite("DialogSchemeInitialise() done\n");

    return 1;
}


//-----------------------------------------------------------------------------
// Name: DialogSchemeShutdown()
// Desc:
//-----------------------------------------------------------------------------

void DialogSchemeShutdown(void)
{
    size_t i;

    for (i = 0; i < DIALOGSCHEME_MAX; i++)
    {
        if (Schemes[i].Fonts.TitleFocus)
            FontDelete(Schemes[i].Fonts.TitleFocus);
        if (Schemes[i].Fonts.TitleNoFocus)
            FontDelete(Schemes[i].Fonts.TitleNoFocus);
        if (Schemes[i].Fonts.ControlNormal)
            FontDelete(Schemes[i].Fonts.ControlNormal);
        if (Schemes[i].Fonts.ControlHighlight)
            FontDelete(Schemes[i].Fonts.ControlHighlight);
        if (Schemes[i].Fonts.ControlDisabled)
            FontDelete(Schemes[i].Fonts.ControlDisabled);
    }
}


//-----------------------------------------------------------------------------
// Name: DialogSchemeSet()
// Desc:
//-----------------------------------------------------------------------------

void DialogSchemeSet(const std::string& Name)
{
    size_t i;

    for (i = 0; i < DIALOGSCHEME_MAX; i++)
    {
        if (_stricmp(Name.c_str(), Schemes[i].Name.c_str()) == 0)
        {
            DialogScheme = &Schemes[i];
            return;
        }
    }
}


//-----------------------------------------------------------------------------
// Name: DialogSchemeGetFirst()
// Desc:
//-----------------------------------------------------------------------------

std::string DialogSchemeGetFirst(size_t* Index)
{
    (*Index) = 0;
    return Schemes[(*Index)].Name;
}


//-----------------------------------------------------------------------------
// Name: DialogSchemeGetNext()
// Desc:
//-----------------------------------------------------------------------------

std::string DialogSchemeGetNext(size_t* Index)
{
    (*Index)++;
    if ((*Index) >= DIALOGSCHEME_MAX)
        return nullptr;

    return Schemes[(*Index)].Name;
}


//-----------------------------------------------------------------------------
// Name: DialogSchemeGetNext()
// Desc:
//-----------------------------------------------------------------------------

void DialogSchemeReloadFonts(void)
{
    DialogSchemeShutdown();
    DialogSchemeInitialise(m_hDC);
}
