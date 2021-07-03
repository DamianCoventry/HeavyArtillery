//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "DialogProcEdit.h"
#include "DialogProcDefault.h"
#include "Dialog.h"
#include "DialogScheme.h"
#include "DialogDraw.h"
#include "Input.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define DIRECTINPUT_VERSION         0x0800
#include <dinput.h>
#include <stdlib.h>


//-----------------------------------------------------------------------------
// private data
//-----------------------------------------------------------------------------

static char ASCIILowerCase[ 256 ];
static char ASCIIUpperCase[ 256 ];


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: AdjustFirstChar()
// Desc:
//-----------------------------------------------------------------------------

static void AdjustFirstChar(Dialog_t* Dialog, Edit_t* Edit)
{
    if (Edit->FirstChar > Edit->CursorPos)
        Edit->CursorPos = Edit->FirstChar;
    else if (Edit->FirstChar == Edit->CursorPos)
        return;

    float NumPixelsWide = Dialog->W - Edit->OffsetX - Edit->OffsetX;

    // Handle the Cursor going out the right side of the edit box
    for (;;)
    {
        auto Text = Edit->Text.substr(Edit->FirstChar, Edit->CursorPos - Edit->FirstChar);

        auto TextPixelWidth = FontPixelWidth(DialogScheme->Fonts.ControlNormal, Text, Text.length());
        if (TextPixelWidth <= NumPixelsWide)
            break;

        Edit->FirstChar++;
    }
}


//-----------------------------------------------------------------------------
// Name: IsWhiteSpace()
// Desc:
//-----------------------------------------------------------------------------

static bool IsWhiteSpace(Edit_t* Edit, size_t Position)
{
    if (Position >= Edit->Text.length())
        return false;
    return Edit->Text[Position] == ' ' || Edit->Text[Position] == '\t' ||
           Edit->Text[Position] == '\r' || Edit->Text[Position] == '\n';
}


//-----------------------------------------------------------------------------
// Name: IsCursorOnWhiteSpace()
// Desc:
//-----------------------------------------------------------------------------

static bool IsCursorOnWhiteSpace(Edit_t* Edit)
{
    return IsWhiteSpace(Edit, Edit->CursorPos);
}


//-----------------------------------------------------------------------------
// Name: MoveCursorPosToPrevWord()
// Desc:
//-----------------------------------------------------------------------------

static void MoveCursorPosToPrevWord(Edit_t* Edit)
{
    // Move off the current word if the cursor is on the first character of the current word.
    if (Edit->CursorPos > 0 && !IsCursorOnWhiteSpace(Edit) && IsWhiteSpace(Edit, Edit->CursorPos - 1))
        --Edit->CursorPos;

    while (Edit->CursorPos > 0 && IsCursorOnWhiteSpace(Edit))
        --Edit->CursorPos;

    while (Edit->CursorPos > 0 && !IsCursorOnWhiteSpace(Edit))
        --Edit->CursorPos;

    if (Edit->CursorPos > 0)
        ++Edit->CursorPos;
}


//-----------------------------------------------------------------------------
// Name: MoveCursorPosToNextWord()
// Desc:
//-----------------------------------------------------------------------------

static void MoveCursorPosToNextWord(Edit_t* Edit)
{
    while (Edit->CursorPos < Edit->Text.length() && !IsCursorOnWhiteSpace(Edit))
        ++Edit->CursorPos;

    while (Edit->CursorPos < Edit->Text.length() && IsCursorOnWhiteSpace(Edit))
        ++Edit->CursorPos;
}


//-----------------------------------------------------------------------------
// Name: KeyToASCII()
// Desc:
//-----------------------------------------------------------------------------

char KeyToASCII(size_t index, size_t shift)
{
    if (index < 0 || index > 255)
        return 0;

    return shift ? ASCIIUpperCase[ index ] : ASCIILowerCase[ index ];
}


//-----------------------------------------------------------------------------
// Name: KeySetupASCII()
// Desc:
//-----------------------------------------------------------------------------

void CreateKeyASCIITable(void)
{
    ASCIILowerCase[ DIK_1 ] = '1';
    ASCIILowerCase[ DIK_2 ] = '2';
    ASCIILowerCase[ DIK_3 ] = '3';
    ASCIILowerCase[ DIK_4 ] = '4';
    ASCIILowerCase[ DIK_5 ] = '5';
    ASCIILowerCase[ DIK_6 ] = '6';
    ASCIILowerCase[ DIK_7 ] = '7';
    ASCIILowerCase[ DIK_8 ] = '8';
    ASCIILowerCase[ DIK_9 ] = '9';
    ASCIILowerCase[ DIK_0 ] = '0';
    ASCIILowerCase[ DIK_MINUS ] = '-';
    ASCIILowerCase[ DIK_EQUALS ] = '=';
    ASCIILowerCase[ DIK_Q ] = 'q';
    ASCIILowerCase[ DIK_W ] = 'w';
    ASCIILowerCase[ DIK_E ] = 'e';
    ASCIILowerCase[ DIK_R ] = 'r';
    ASCIILowerCase[ DIK_T ] = 't';
    ASCIILowerCase[ DIK_Y ] = 'y';
    ASCIILowerCase[ DIK_U ] = 'u';
    ASCIILowerCase[ DIK_I ] = 'i';
    ASCIILowerCase[ DIK_O ] = 'o';
    ASCIILowerCase[ DIK_P ] = 'p';
    ASCIILowerCase[ DIK_LBRACKET ] = '[';
    ASCIILowerCase[ DIK_RBRACKET ] = ']';
    ASCIILowerCase[ DIK_A ] = 'a';
    ASCIILowerCase[ DIK_S ] = 's';
    ASCIILowerCase[ DIK_D ] = 'd';
    ASCIILowerCase[ DIK_F ] = 'f';
    ASCIILowerCase[ DIK_G ] = 'g';
    ASCIILowerCase[ DIK_H ] = 'h';
    ASCIILowerCase[ DIK_J ] = 'j';
    ASCIILowerCase[ DIK_K ] = 'k';
    ASCIILowerCase[ DIK_L ] = 'l';
    ASCIILowerCase[ DIK_SEMICOLON ] = ';';
    ASCIILowerCase[ DIK_APOSTROPHE ] = '\'';
    ASCIILowerCase[ DIK_GRAVE ] = '`';
    ASCIILowerCase[ DIK_BACKSLASH ] = '\\';
    ASCIILowerCase[ DIK_Z ] = 'z';
    ASCIILowerCase[ DIK_X ] = 'x';
    ASCIILowerCase[ DIK_C ] = 'c';
    ASCIILowerCase[ DIK_V ] = 'v';
    ASCIILowerCase[ DIK_B ] = 'b';
    ASCIILowerCase[ DIK_N ] = 'n';
    ASCIILowerCase[ DIK_M ] = 'm';
    ASCIILowerCase[ DIK_COMMA ] = ',';
    ASCIILowerCase[ DIK_PERIOD ] = '.';
    ASCIILowerCase[ DIK_SLASH ] = '/';
    ASCIILowerCase[ DIK_MULTIPLY ] = '*';
    ASCIILowerCase[ DIK_SPACE ] = ' ';
    ASCIILowerCase[ DIK_NUMPAD7 ] = '7';
    ASCIILowerCase[ DIK_NUMPAD8 ] = '8';
    ASCIILowerCase[ DIK_NUMPAD9 ] = '9';
    ASCIILowerCase[ DIK_SUBTRACT ] = '-';
    ASCIILowerCase[ DIK_NUMPAD4 ] = '4';
    ASCIILowerCase[ DIK_NUMPAD5 ] = '5';
    ASCIILowerCase[ DIK_NUMPAD6 ] = '6';
    ASCIILowerCase[ DIK_ADD ] = '+';
    ASCIILowerCase[ DIK_NUMPAD1 ] = '1';
    ASCIILowerCase[ DIK_NUMPAD2 ] = '2';
    ASCIILowerCase[ DIK_NUMPAD3 ] = '3';
    ASCIILowerCase[ DIK_NUMPAD0 ] = '0';
    ASCIILowerCase[ DIK_DECIMAL ] = '.';

    ASCIIUpperCase[ DIK_1 ] = '!';
    ASCIIUpperCase[ DIK_2 ] = '@';
    ASCIIUpperCase[ DIK_3 ] = '#';
    ASCIIUpperCase[ DIK_4 ] = '$';
    ASCIIUpperCase[ DIK_5 ] = '%';
    ASCIIUpperCase[ DIK_6 ] = '^';
    ASCIIUpperCase[ DIK_7 ] = '&';
    ASCIIUpperCase[ DIK_8 ] = '*';
    ASCIIUpperCase[ DIK_9 ] = '(';
    ASCIIUpperCase[ DIK_0 ] = ')';
    ASCIIUpperCase[ DIK_MINUS ] = '_';
    ASCIIUpperCase[ DIK_EQUALS ] = '+';
    ASCIIUpperCase[ DIK_Q ] = 'Q';
    ASCIIUpperCase[ DIK_W ] = 'W';
    ASCIIUpperCase[ DIK_E ] = 'E';
    ASCIIUpperCase[ DIK_R ] = 'R';
    ASCIIUpperCase[ DIK_T ] = 'T';
    ASCIIUpperCase[ DIK_Y ] = 'Y';
    ASCIIUpperCase[ DIK_U ] = 'U';
    ASCIIUpperCase[ DIK_I ] = 'I';
    ASCIIUpperCase[ DIK_O ] = 'O';
    ASCIIUpperCase[ DIK_P ] = 'P';
    ASCIIUpperCase[ DIK_LBRACKET ] = '{';
    ASCIIUpperCase[ DIK_RBRACKET ] = '}';
    ASCIIUpperCase[ DIK_A ] = 'A';
    ASCIIUpperCase[ DIK_S ] = 'S';
    ASCIIUpperCase[ DIK_D ] = 'D';
    ASCIIUpperCase[ DIK_F ] = 'F';
    ASCIIUpperCase[ DIK_G ] = 'G';
    ASCIIUpperCase[ DIK_H ] = 'H';
    ASCIIUpperCase[ DIK_J ] = 'J';
    ASCIIUpperCase[ DIK_K ] = 'K';
    ASCIIUpperCase[ DIK_L ] = 'L';
    ASCIIUpperCase[ DIK_SEMICOLON ] = ':';
    ASCIIUpperCase[ DIK_APOSTROPHE ] = '"';
    ASCIIUpperCase[ DIK_GRAVE ] = '~';
    ASCIIUpperCase[ DIK_BACKSLASH ] = '|';
    ASCIIUpperCase[ DIK_Z ] = 'Z';
    ASCIIUpperCase[ DIK_X ] = 'X';
    ASCIIUpperCase[ DIK_C ] = 'C';
    ASCIIUpperCase[ DIK_V ] = 'V';
    ASCIIUpperCase[ DIK_B ] = 'B';
    ASCIIUpperCase[ DIK_N ] = 'N';
    ASCIIUpperCase[ DIK_M ] = 'M';
    ASCIIUpperCase[ DIK_COMMA ] = '<';
    ASCIIUpperCase[ DIK_PERIOD ] = '>';
    ASCIIUpperCase[ DIK_SLASH ] = '?';
    ASCIIUpperCase[ DIK_MULTIPLY ] = '*';
    ASCIIUpperCase[ DIK_SPACE ] = ' ';
    ASCIIUpperCase[ DIK_NUMPAD7 ] = '7';
    ASCIIUpperCase[ DIK_NUMPAD8 ] = '8';
    ASCIIUpperCase[ DIK_NUMPAD9 ] = '9';
    ASCIIUpperCase[ DIK_SUBTRACT ] = '-';
    ASCIIUpperCase[ DIK_NUMPAD4 ] = '4';
    ASCIIUpperCase[ DIK_NUMPAD5 ] = '5';
    ASCIIUpperCase[ DIK_NUMPAD6 ] = '6';
    ASCIIUpperCase[ DIK_ADD ] = '+';
    ASCIIUpperCase[ DIK_NUMPAD1 ] = '1';
    ASCIIUpperCase[ DIK_NUMPAD2 ] = '2';
    ASCIIUpperCase[ DIK_NUMPAD3 ] = '3';
    ASCIIUpperCase[ DIK_NUMPAD0 ] = '0';
    ASCIIUpperCase[ DIK_DECIMAL ] = '.';
}


//-----------------------------------------------------------------------------
// Name: DialogProcDefault()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcEdit(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2)
{
    Edit_t* Edit;
    char Key;
    size_t i;
    float X, Y;

    Edit = (Edit_t*)Dialog->Data;

    switch (Msg)
    {
    case WM_CREATE :
    {
        Edit = new Edit_t();

        Edit->Reset();

        Dialog->Data = (void*)Edit;

        Edit->OffsetX = 4;
        Edit->OffsetY = (Dialog->H / 2) - (DialogScheme->Fonts.ControlNormal->Height / 2);

        return DIALOG_MSGHANDLED;
    }
    case WM_PAINT :
    {
        DialogClientToScreen(Dialog, &X, &Y);
        DialogDrawRectControl(X, Y, Dialog->W, Dialog->H);
        auto OrigX = X, OrigY = Y;

        float NumPixelsWide = Dialog->W - Edit->OffsetX - Edit->OffsetX;

        auto Text = Edit->Text.substr(Edit->FirstChar);

        auto CharCount = FontCharCount(DialogScheme->Fonts.ControlNormal, Text, NumPixelsWide);
        if (Text.length() > CharCount)
            Text = Edit->Text.substr(Edit->FirstChar, CharCount);

        FontWrite(DialogScheme->Fonts.ControlNormal, X + Edit->OffsetX, Edit->OffsetY + Y,
                  DialogScheme->Colors.FontControlNormal, Text);

        if (Dialog->Parent->Focus == Dialog)
        {
            // draw the flashing Cursor
            if (++Edit->BlinkCounter >= EDIT_BLINKTHRESHOLD)
            {
                Edit->BlinkCounter = 0;
                Edit->BlinkVisible = !Edit->BlinkVisible;
            }

            if (Edit->BlinkVisible)
            {
                i = Edit->CursorPos - Edit->FirstChar;

                if (i == 0)
                    X += Edit->OffsetX;
                else
                    X += Edit->OffsetX + FontPixelWidth(DialogScheme->Fonts.ControlNormal, Text, i);

                FontWrite(DialogScheme->Fonts.ControlNormal, X, Y + Edit->OffsetY,
                          DialogScheme->Colors.FontControlNormal, "|");
            }
        }

        if (Dialog->Parent->Focus == Dialog)
            DialogDrawRectFocus(OrigX, OrigY, Dialog->W, Dialog->H);

        return DIALOG_MSGHANDLED;
    }
    case WM_CLEAR :
        Edit->FirstChar = 0;
        Edit->CursorPos = 0;
        Edit->Text.clear();
        return DIALOG_MSGHANDLED;

    case WM_KILLFOCUS :
        DialogSend(Dialog->Parent, WM_COMMAND, (void*)EN_UPDATE, Dialog);
        return DIALOG_MSGHANDLED;

    case WM_KEYDOWN :
        //snd_play( snd_click2, 0 );
        switch ((size_t)Param1)
        {
        case DIK_HOME :
            Edit->BlinkCounter = 0;
            Edit->BlinkVisible = 1;
            Edit->FirstChar = 0;
            Edit->CursorPos = 0;
            return DIALOG_MSGHANDLED;

        case DIK_END :
            Edit->BlinkCounter = 0;
            Edit->BlinkVisible = 1;
            Edit->CursorPos = Edit->Text.length();
            AdjustFirstChar(Dialog, Edit);
            return DIALOG_MSGHANDLED;

        case DIK_LEFT :
            Edit->BlinkCounter = 0;
            Edit->BlinkVisible = 1;
            if (Edit->CursorPos > 0)
            {
                if (IsKeyHeldDown(DIK_LCONTROL) || IsKeyHeldDown(DIK_RCONTROL))
                    MoveCursorPosToPrevWord(Edit);
                else
                    --Edit->CursorPos;

                if (Edit->CursorPos < Edit->FirstChar)
                    Edit->FirstChar = Edit->CursorPos;
            }
            return DIALOG_MSGHANDLED;

        case DIK_RIGHT :
            Edit->BlinkCounter = 0;
            Edit->BlinkVisible = 1;
            if (Edit->CursorPos < Edit->Text.length())
            {
                if (IsKeyHeldDown(DIK_LCONTROL) || IsKeyHeldDown(DIK_RCONTROL))
                    MoveCursorPosToNextWord(Edit);
                else
                    ++Edit->CursorPos;

                AdjustFirstChar(Dialog, Edit);
            }
            return DIALOG_MSGHANDLED;

        case DIK_BACK :
            Edit->BlinkCounter = 0;
            Edit->BlinkVisible = 1;
            if (!Edit->Text.empty() && Edit->CursorPos > 0)
            {
                Edit->Text.erase(Edit->CursorPos - 1, 1);
                --Edit->CursorPos;

                if (Edit->CursorPos < Edit->FirstChar)
                {
                    if (Edit->FirstChar < 5)
                        Edit->FirstChar = 0;
                    else
                        Edit->FirstChar -= 5;
                }
                DialogSend(Dialog->Parent, WM_COMMAND, (void*)EN_CHANGE, Dialog);
            }
            return DIALOG_MSGHANDLED;

        case DIK_DELETE :
            Edit->BlinkCounter = 0;
            Edit->BlinkVisible = 1;
            if (!Edit->Text.empty() && Edit->CursorPos < Edit->Text.length())
            {
                Edit->Text.erase(Edit->CursorPos, 1);
                DialogSend(Dialog->Parent, WM_COMMAND, (void*)EN_CHANGE, Dialog);
            }
            return DIALOG_MSGHANDLED;

        default :
            Key = KeyToASCII((size_t)Param1, IsKeyHeldDown(DIK_LSHIFT) || IsKeyHeldDown(DIK_RSHIFT));
            if (!Key)
                return DIALOG_MSGNOTHANDLED;

            Edit->BlinkCounter = 0;
            Edit->BlinkVisible = 1;

            Edit->Text.insert(Edit->CursorPos, 1, Key);
            ++Edit->CursorPos;

            AdjustFirstChar(Dialog, Edit);
            DialogSend(Dialog->Parent, WM_COMMAND, (void*)EN_CHANGE, Dialog);
            return DIALOG_MSGHANDLED;
        }
        break;

    case WM_SETTEXT :
        Edit->Text = *(std::string*)Param1;
        Edit->CursorPos = Edit->Text.length();
        Edit->FirstChar = 0;
        AdjustFirstChar(Dialog, Edit);
        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}
