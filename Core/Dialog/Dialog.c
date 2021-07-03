//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// FrIDay, 5 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "Dialog.h"
#include "Dialogscheme.h"
#include "Input.h"
#include "Font.h"

#include "DialogProcCheck.h"
#include "DialogProcLabel.h"
#include "DialogProcButton.h"
#include "DialogProcDefault.h"
#include "DialogProcStatic.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/GL.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DIALOG_MAXTIMERS            8

#define CURSOR_WIDTHHALF            20
#define CURSOR_HEIGHTHALF           20


//-----------------------------------------------------------------------------
// private structures
//-----------------------------------------------------------------------------

typedef struct Timer_s
{
    size_t Counter;
    size_t Threshold;
    Dialog_t* Dialog;
}
Timer_t;

typedef struct MouseButton_s
{
    size_t ButtonIndex;
    size_t WindowMessage;
}
MouseButton_t;


//-----------------------------------------------------------------------------
// public Data
//-----------------------------------------------------------------------------

//Dialog_t *DialogFocus;
Dialog_t* DialogCapture;

Dialog_t* DialogHead   = nullptr;
Dialog_t* DialogTail   = nullptr;
Dialog_t* DialogMsgBox = nullptr;

float CursorX;
float CursorY;

float DialogViewportX1;
float DialogViewportY1;
float DialogViewportX2;
float DialogViewportY2;


//-----------------------------------------------------------------------------
// private Data
//-----------------------------------------------------------------------------

static MouseButton_t MouseButtons[] =
{
    { 0, WM_LBUTTONDOWN }
    , { 0, WM_LBUTTONUP }
    , { 1, WM_RBUTTONDOWN }
    , { 1, WM_RBUTTONUP }
    , { 2, WM_MBUTTONDOWN }
    , { 2, WM_MBUTTONUP }
};

static size_t DialogCount = 0;
static Dialog_t Dialogs[ DIALOG_MAXDIALOGS ];

static size_t CursorVisible;

static size_t RetValue;
static Dialog_t* JustEnded;

static Timer_t Timers[ DIALOG_MAXTIMERS ];


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------

static void DialogLink(Dialog_t* Dialog);
static void DialogUnlink(Dialog_t* Dialog);


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

Dialog_t* DialogCreate(const std::string& Name, size_t Flags,
                       float X, float Y, float W, float H,
                       Dialog_t* Parent, DialogProc_t DialogProc, void* Param1, void* Param2)
{
    Dialog_t* Dialog;

    if (DialogCount >= DIALOG_MAXDIALOGS)
        return nullptr;

    Dialog = &Dialogs[ DialogCount ];

    Dialog->Reset();
    Dialog->Name = Name;

    Dialog->X = X;
    Dialog->Y = Y;
    Dialog->W = W;
    Dialog->H = H;

    Dialog->DialogProc = DialogProc;
    Dialog->Parent = Parent;

    if (Dialog->Parent)
    {
        if (Dialog->Parent->ChildCount < DIALOG_MAXCHILDREN)
            Dialog->Parent->Children[ Dialog->Parent->ChildCount++ ] = Dialog;
        else
            Dialog->Parent = nullptr;
    }

    Dialog->Flags = Flags | DIALOG_ENABLED;
    DialogCount++;

    // if it's not a Child and doesn't Have a title bar, then it must
    // be a Popup menu
    if (!(Dialog->Flags & DIALOG_CHILD) && !(Dialog->Flags & DIALOG_TITLE))
    {
        Dialog->Flags |= DIALOG_POPUPMENU;
        Dialog->Flags &= ~DIALOG_MODAL;
    }

    DialogSend(Dialog, WM_CREATE, Param1, Param2);

    return Dialog;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogDelete(Dialog_t* Dialog)
{
    DialogSend(Dialog, WM_DESTROY, nullptr, nullptr);
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogTrackPopup(Dialog_t* Dialog, float X, float Y, void* Param)
{
    Dialog->X = X;
    if (Dialog->X + Dialog->W > DialogViewportX2)
        Dialog->X = DialogViewportX2 - Dialog->W;

    Dialog->Y = Y;
    if (Dialog->Y + Dialog->H > DialogViewportY2)
        Dialog->Y = DialogViewportY2 - Dialog->H - 1.0f;

    DialogShow(Dialog, Param);
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogShow(Dialog_t* Dialog, void* Param)
{
    DialogUnlink(Dialog);
    DialogLink(Dialog);

    Dialog->Flags |= DIALOG_VISIBLE;

    if (Dialog->Flags & DIALOG_CENTER)
    {
        Dialog->X = (DialogViewportX2 / 2) - (Dialog->W / 2);
        Dialog->Y = (DialogViewportY2 / 2) - (Dialog->H / 2);
    }

    DialogSend(Dialog, WM_ACTIVATE, (void*)WA_ACTIVE, Param);
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogBringToTop(Dialog_t* Dialog)
{
    DialogUnlink(Dialog);
    DialogLink(Dialog);
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogHide(Dialog_t* Dialog)
{
    if (!Dialog)
        return;

    DialogUnlink(Dialog);

    Dialog->Flags &= ~DIALOG_VISIBLE;

    DialogSend(Dialog, WM_ACTIVATE, WA_INACTIVE, 0);

    JustEnded = Dialog;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogHideAll(Dialog_t* ExceptThisOne)
{
    size_t i;

    for (i = 0; i < DialogCount; i++)
    {
        if (Dialogs[ i ].Parent == nullptr && ExceptThisOne != &Dialogs[ i ])
            DialogHide(&Dialogs[ i ]);
    }

    JustEnded = nullptr;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogShowCursor(size_t Visible)
{
    CursorVisible = Visible;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogToggle(Dialog_t* Dialog)
{
    Dialog->Flags& DIALOG_VISIBLE ? DialogHide(Dialog) : DialogShow(Dialog, 0);
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DrawActiveDialogs(void)
{
    Dialog_t* Current;

    if (DialogHead)
    {
        glDisable(GL_DEPTH_TEST);

        Current = DialogTail;
        while (Current)
        {
            if (Current->Flags & DIALOG_VISIBLE && !Current->DontDraw)
                DialogSend(Current, WM_PAINT, 0, 0);

            Current = Current->Prev;
        }

        glEnable(GL_DEPTH_TEST);
    }
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

size_t DialogSend(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2)
{
    size_t i;

    if (Dialog)
    {
        if (DIALOG_BROADCAST == Dialog)
        {
            for (i = 0; i < DialogCount; i++)
                if (Dialogs[ i ].DialogProc)
                    Dialogs[ i ].DialogProc(&Dialogs[ i ], Msg, Param1, Param2);
        }
        else if (Dialog->DialogProc)
            return Dialog->DialogProc(Dialog, Msg, Param1, Param2);
    }

    return 0;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

static void DialogLink(Dialog_t* Dialog)
{
    if (DialogHead)
    {
        DialogHead->Prev = Dialog;
        Dialog->Next = DialogHead;
        Dialog->Prev = nullptr;
        DialogHead = Dialog;
    }
    else
    {
        DialogHead = Dialog;
        DialogTail = Dialog;
    }
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

static void DialogUnlink(Dialog_t* Dialog)
{
    Dialog_t* Current;

    Current = DialogHead;
    while (Current)
    {
        if (Dialog == Current)
            break;

        Current = Current->Next;
    }

    if (!Current)
        return;

    if (Current == DialogHead)
    {
        if (Current == DialogTail)
        {
            DialogHead = nullptr;
            DialogTail = nullptr;
        }
        else
        {
            DialogHead = DialogHead->Next;
            DialogHead->Prev = nullptr;
        }
    }
    else if (Current == DialogTail)
    {
        DialogTail = DialogTail->Prev;
        DialogTail->Next = nullptr;
    }
    else
    {
        Current->Next->Prev = Current->Prev;
        Current->Prev->Next = Current->Next;
    }

    Dialog->Next = nullptr;
    Dialog->Prev = nullptr;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogScreenToClient(Dialog_t* Dialog, float* X, float* Y)
{
    float TotalX, TotalY;
    Dialog_t* Current;

    TotalX = TotalY = 0;
    Current = Dialog;

    while (Current->Parent)
    {
        TotalX += Current->X;
        TotalY += Current->Y;
        Current = Current->Parent;
    }

    if (X)
        (*X) -= Current->X + TotalX;

    if (Y)
        (*Y) -= Current->Y + TotalY;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogClientToScreen(Dialog_t* Dialog, float* X, float* Y)
{
    float TotalX, TotalY;
    Dialog_t* Current;

    TotalX = TotalY = 0;
    Current = Dialog;

    while (Current->Parent)
    {
        TotalX += Current->X;
        TotalY += Current->Y;
        Current = Current->Parent;
    }

    if (X)
        (*X) = Current->X + TotalX;

    if (Y)
        (*Y) = Current->Y + TotalY;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

Dialog_t* DialogAtCoords(float X, float Y)
{
    Dialog_t* Current;

    Current = DialogHead;
    while (Current)
    {
        if (!(Current->Flags & DIALOG_CHILD))
        {
            if (X >= Current->X && X < Current->X + Current->W &&
                    Y >= Current->Y && Y < Current->Y + Current->H)
                break;
        }
        Current = Current->Next;
    }

    return Current;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

size_t DialogSetFocus(Dialog_t* Parent, Dialog_t* Child)
{
    if (Parent->Focus)
        DialogSend(Parent->Focus, WM_KILLFOCUS, 0, 0);

    Parent->Focus = Child;
    DialogSend(Parent->Focus, WM_SETFOCUS, 0, 0);

    return 1;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogFocusNext(Dialog_t* Parent)
{
    size_t ChildIndex;
    size_t i;

    if (!Parent->Focus)
    {
        // no Focus is Currently set, choose the first non-label in the list
        for (ChildIndex = 0; ChildIndex < Parent->ChildCount; ChildIndex++)
        {
            if (Parent->Children[ ChildIndex ]->Flags & DIALOG_CANGETFOCUS &&
                    Parent->Children[ ChildIndex ]->Flags & DIALOG_ENABLED)
            {
                DialogSetFocus(Parent, Parent->Children[ ChildIndex ]);
                return;
            }
        }
    }

    // get the index of the Control With the Focus
    for (i = 0; i < Parent->ChildCount; i++)
    {
        if (Parent->Focus == Parent->Children[ i ])
            break;
    }

    // get the Next control after the Focus Control that isn't a label
    for (ChildIndex = i + 1; ChildIndex < Parent->ChildCount; ChildIndex++)
    {
        if (Parent->Children[ ChildIndex ]->Flags & DIALOG_CANGETFOCUS &&
                Parent->Children[ ChildIndex ]->Flags & DIALOG_ENABLED)
        {
            DialogSetFocus(Parent, Parent->Children[ ChildIndex ]);
            return;
        }
    }

    // reached the eol, so choose the first non-label in the list
    for (ChildIndex = 0; ChildIndex < Parent->ChildCount; ChildIndex++)
    {
        if (Parent->Children[ ChildIndex ]->Flags & DIALOG_CANGETFOCUS &&
                Parent->Children[ ChildIndex ]->Flags & DIALOG_ENABLED)
        {
            DialogSetFocus(Parent, Parent->Children[ ChildIndex ]);
            return;
        }
    }
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogFocusPrev(Dialog_t* Parent)
{
    size_t ChildIndex;
    size_t i;

    if (!Parent->Focus)
    {
        // no Focus is Currently set, choose the last non-label in the list
        for (ChildIndex = Parent->ChildCount - 1; ; ChildIndex--)
        {
            if (Parent->Children[ ChildIndex ]->Flags & DIALOG_CANGETFOCUS &&
                    Parent->Children[ ChildIndex ]->Flags & DIALOG_ENABLED)
            {
                DialogSetFocus(Parent, Parent->Children[ ChildIndex ]);
                return;
            }
            if (ChildIndex == 0)
                break;
        }
    }

    // get the index of the Control With the Focus
    for (i = Parent->ChildCount - 1; ; i--)
    {
        if (Parent->Focus == Parent->Children[ i ])
            break;
        if (i == 0)
            break;
    }

    // get the Previous control after the Focus Control that isn't a label
    for (ChildIndex = i - 1; ; ChildIndex--)
    {
        if (Parent->Children[ ChildIndex ]->Flags & DIALOG_CANGETFOCUS &&
                Parent->Children[ ChildIndex ]->Flags & DIALOG_ENABLED)
        {
            DialogSetFocus(Parent, Parent->Children[ ChildIndex ]);
            return;
        }
        if (ChildIndex == 0)
            break;
    }

    // reached the eol, so choose the last non-label in the list
    for (ChildIndex = Parent->ChildCount - 1; ; ChildIndex--)
    {
        if (Parent->Children[ ChildIndex ]->Flags & DIALOG_CANGETFOCUS &&
                Parent->Children[ ChildIndex ]->Flags & DIALOG_ENABLED)
        {
            DialogSetFocus(Parent, Parent->Children[ ChildIndex ]);
            return;
        }
        if (ChildIndex == 0)
            break;
    }
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

size_t DispatchDialogMessages(void)
{
    size_t i, ButtonDown, ButtonUp;
    size_t KeyHandled, MouseHandled;
    int MouseX, MouseY;
    Dialog_t* Dialog;

    for (i = 0; i < DIALOG_MAXTIMERS; i++)
    {
        if (Timers[ i ].Dialog)
        {
            if (++Timers[ i ].Counter >= Timers[ i ].Threshold)
            {
                Timers[ i ].Counter = 0;
                DialogSend(Timers[ i ].Dialog, WM_TIMER, 0, 0);
            }
        }
    }

    if (!DialogHead)
        return DIALOG_MSGNOTHANDLED;

    GetMouseMovement(&MouseX, &MouseY);
    if (MouseX || MouseY)
        DialogSend(DialogCapture, WM_MOUSEMOVE, 0, 0);

    ButtonDown = ButtonUp = 0;

    if (DialogHead->Flags & DIALOG_MODAL)
    {
        Dialog = DialogAtCoords(CursorX, CursorY);
        if (Dialog == DialogHead)
        {
            if ((MouseX || MouseY) && !DialogCapture)
                DialogSend(Dialog, WM_MOUSEMOVE, 0, 0);

            for (i = 0; i < 3; i += 2)
            {
                if (IsMButtonPressed(MouseButtons[ i + 0 ].ButtonIndex))
                {
                    ButtonDown = 1;
                    DialogSend(Dialog, MouseButtons[ i + 0 ].WindowMessage, 0, 0);
                }
                else if (IsMButtonReleased(MouseButtons[ i + 1 ].ButtonIndex))
                {
                    ButtonUp = 1;
                    DialogSend(Dialog, MouseButtons[ i + 1 ].WindowMessage, 0, 0);
                }
            }

            if (ButtonDown && Dialog != DialogHead)
                DialogShow(Dialog, 0);
        }
        else if (DialogCapture)
        {
            for (i = 0; i < 3; i += 2)
            {
                if (IsMButtonReleased(MouseButtons[ i + 1 ].ButtonIndex))
                    ButtonUp = 1;
            }
        }

        if (DialogCapture && ButtonUp)
            DialogCapture = nullptr;

        for (i = 0; i < 256; i++)
        {
            if (IsKeyPressed(i))
                DialogSend(DialogHead, WM_KEYDOWN, (void*)i, 0);

            else if (IsKeyReleased(i))
                DialogSend(DialogHead, WM_KEYUP, (void*)i, 0);
        }

        if (ButtonDown && !(Dialog && Dialog->Flags & DIALOG_POPUPMENU))
            DialogCloseAllPopups(nullptr);

        return DIALOG_MSGHANDLED;
    }

    KeyHandled = MouseHandled = 0;
    ButtonDown = ButtonUp = 0;

    Dialog = DialogAtCoords(CursorX, CursorY);
    if (Dialog)
    {
        if ((MouseX || MouseY) && !DialogCapture)
            DialogSend(Dialog, WM_MOUSEMOVE, 0, 0);

        for (i = 0; i < 3; i += 2)
        {
            if (IsMButtonPressed(MouseButtons[ i + 0 ].ButtonIndex))
            {
                ButtonDown = 1;
                DialogSend(Dialog, MouseButtons[ i + 0 ].WindowMessage, 0, 0);
            }
            else if (IsMButtonReleased(MouseButtons[ i + 1 ].ButtonIndex))
            {
                ButtonUp = 1;
                DialogSend(Dialog, MouseButtons[ i + 1 ].WindowMessage, 0, 0);
            }
        }

        if (ButtonDown && !(Dialog && Dialog->Flags & DIALOG_POPUPMENU))
            DialogCloseAllPopups(Dialog);

        if (ButtonDown && Dialog != DialogHead)
            DialogShow(Dialog, 0);

        MouseHandled = 1;
    }
    else if (DialogCapture)
    {
        for (i = 0; i < 3; i += 2)
        {
            if (IsMButtonPressed(MouseButtons[ i + 1 ].ButtonIndex))
                ButtonDown = 1;

            if (IsMButtonReleased(MouseButtons[ i + 1 ].ButtonIndex))
                ButtonUp = 1;
        }

        if (ButtonDown && !(Dialog && Dialog->Flags & DIALOG_POPUPMENU))
            DialogCloseAllPopups(Dialog);
    }
    else
    {
        for (i = 0; i < 3; i += 2)
            if (IsMButtonPressed(MouseButtons[ i + 1 ].ButtonIndex))
                ButtonDown = 1;

        if (ButtonDown && !(Dialog && Dialog->Flags & DIALOG_POPUPMENU))
            DialogCloseAllPopups(Dialog);
    }

    if (DialogCapture && ButtonUp)
        DialogCapture = nullptr;

    for (i = 0; i < 256; i++)
    {
        if (IsKeyPressed(i))
            KeyHandled |= DialogSend(DialogHead, WM_KEYDOWN, (void*)i, 0);

        else if (IsKeyReleased(i))
            KeyHandled |= DialogSend(DialogHead, WM_KEYUP, (void*)i, 0);
    }

    if (KeyHandled || MouseHandled)
        return DIALOG_MSGHANDLED;

    return DIALOG_MSGNOTHANDLED;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

Dialog_t* DialogControlAtCoords(Dialog_t* Parent, float X, float Y)
{
    Dialog_t* Child;
    Dialog_t* Current;
    size_t ControlIndex;
    float TotalX, TotalY;

    if (!Parent)
        return nullptr;

    for (ControlIndex = 0; ControlIndex < Parent->ChildCount; ControlIndex++)
    {
        Child = Parent->Children[ ControlIndex ];

        TotalX = TotalY = 0;
        Current = Child;

        while (Current->Parent)
        {
            TotalX += Current->X;
            TotalY += Current->Y;
            Current = Current->Parent;
        }

        if (X >=   Current->X + TotalX &&
                X <= (Current->X + TotalX + Child->W) &&
                Y >=   Current->Y + TotalY &&
                Y <= (Current->Y + TotalY + Child->H) &&
                Child->Flags & DIALOG_VISIBLE && Child->DialogProc != DialogProcStatic)
            return Child;
    }

    return nullptr;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void MoveMouseCursor(void)
{
    int MouseX, MouseY;

    if (!CursorVisible)
        return;

    GetMouseMovement(&MouseX, &MouseY);

    CursorX += MouseX;
    if (CursorX < 0.0f)
        CursorX = 0.0f;
    else if (CursorX > DialogViewportX2)
        CursorX = DialogViewportX2;

    CursorY += MouseY;
    if (CursorY < 0.0f)
        CursorY = 0.0f;
    else if (CursorY > DialogViewportY2)
        CursorY = DialogViewportY2;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DrawMouseCursor(void)
{
    float StartX, StartY;
    float FinishX, FinishY;

    if (!CursorVisible)
        return;

    StartX = (float)(CursorX - CURSOR_WIDTHHALF);
    if (StartX < 0.0f)
        StartX = 0.0f;

    FinishX = (float)(CursorX + CURSOR_WIDTHHALF);
    if (FinishX > DialogViewportX2)
        FinishX = DialogViewportX2;

    StartY = (float)(CursorY - CURSOR_HEIGHTHALF);
    if (StartY < 0.0f)
        StartY = 0.0f;

    FinishY = (float)(CursorY + CURSOR_HEIGHTHALF);
    if (FinishY > DialogViewportY2)
        FinishY = DialogViewportY2;

    glDepthFunc(GL_ALWAYS);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINES);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(StartX, CursorY); glVertex2f(FinishX, CursorY);
    glVertex2f(CursorX, StartY); glVertex2f(CursorX, FinishY);
    glEnd();
    glEnable(GL_TEXTURE_2D);
    glDepthFunc(GL_LESS);
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

size_t DialogRetValue(void)
{
    size_t Ret;
    Ret = RetValue;
    RetValue = -1;
    return Ret;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogSetRetValue(size_t Value)
{
    RetValue = Value;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

Dialog_t* DialogJustEnded(void)
{
    Dialog_t* Dialog;
    Dialog = JustEnded;
    JustEnded = nullptr;
    return Dialog;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

size_t DialogMessageBox(const std::string& Title, const std::string& Text, size_t Type, void(*Callback)(size_t RetValue))
{
    float messageBoxWidth;

    messageBoxWidth = (float)FontPixelWidth(DialogScheme->Fonts.ControlNormal, Text, -1);
    if (messageBoxWidth < 250)
        messageBoxWidth = 250;
    else
        messageBoxWidth += 64;

    switch (Type)
    {
    case MB_OKCANCEL:
    case MB_RETRYCANCEL:
    case MB_YESNO:
        messageBoxWidth = max(messageBoxWidth, 3 * MESSAGEBOX_BUTTONWIDTH);
        break;
    case MB_ABORTRETRYIGNORE:
    case MB_YESNOCANCEL:
        messageBoxWidth = max(messageBoxWidth, 2 * MESSAGEBOX_BUTTONWIDTH);
        break;
    default:
        messageBoxWidth = max(messageBoxWidth, MESSAGEBOX_BUTTONWIDTH);
        break;
    }

    const float messageBoxH = 150.0f;

    DialogMsgBox = DialogCreate(Title, DIALOG_CENTER | DIALOG_TITLE | DIALOG_MOVEABLE | DIALOG_MODAL,
                                0.0f, 0.0f, messageBoxWidth, messageBoxH, nullptr, DialogProcMessageBox, 0, 0);

    DialogCreate(Text, DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER,
                 0.0f, 45.0f, messageBoxWidth, 40.0f, DialogMsgBox, DialogProcLabel, 0, 0);

    DialogMsgBox->Hint = "Click a Button to dismiss this dialog window.";
    DialogMsgBox->Callback = Callback;

    float buttonY = messageBoxH - MESSAGEBOX_BUTTONHEIGHT - 8.0f;

    switch (Type)
    {
    case MB_ABORTRETRYIGNORE :
        DialogMsgBox->Data = (void*)MB_ABORTRETRYIGNORE;
        DialogCreate("Abort",  DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_OK | DIALOG_CANGETFOCUS,
                     (messageBoxWidth / 2) - (MESSAGEBOX_BUTTONWIDTH + 8 + MESSAGEBOX_BUTTONWIDTHHALF), buttonY, MESSAGEBOX_BUTTONWIDTH, MESSAGEBOX_BUTTONHEIGHT, DialogMsgBox, DialogProcButton, 0, 0);
        DialogCreate("Retry",  DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_CANGETFOCUS,
                     (messageBoxWidth / 2) - MESSAGEBOX_BUTTONWIDTHHALF, buttonY, MESSAGEBOX_BUTTONWIDTH, MESSAGEBOX_BUTTONHEIGHT, DialogMsgBox, DialogProcButton, 0, 0);
        DialogCreate("Ignore", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_CANGETFOCUS,
                     (messageBoxWidth / 2) + (MESSAGEBOX_BUTTONWIDTH + 8 + MESSAGEBOX_BUTTONWIDTHHALF), buttonY, MESSAGEBOX_BUTTONWIDTH, MESSAGEBOX_BUTTONHEIGHT, DialogMsgBox, DialogProcButton, 0, 0);
        break;
    case MB_OKCANCEL :
        DialogMsgBox->Data = (void*)MB_OKCANCEL;
        DialogCreate("OK",     DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_OK | DIALOG_CANGETFOCUS,
                     (messageBoxWidth / 2) - (MESSAGEBOX_BUTTONWIDTH + 4), buttonY, MESSAGEBOX_BUTTONWIDTH, MESSAGEBOX_BUTTONHEIGHT, DialogMsgBox, DialogProcButton, 0, 0);
        DialogCreate("Cancel", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_CANGETFOCUS,
                     (messageBoxWidth / 2) + 4, buttonY, MESSAGEBOX_BUTTONWIDTH, MESSAGEBOX_BUTTONHEIGHT, DialogMsgBox, DialogProcButton, 0, 0);
        break;
    case MB_RETRYCANCEL :
        DialogMsgBox->Data = (void*)MB_RETRYCANCEL;
        DialogCreate("Retry",  DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_OK | DIALOG_CANGETFOCUS,
                     (messageBoxWidth / 2) - (MESSAGEBOX_BUTTONWIDTH + 4), buttonY, MESSAGEBOX_BUTTONWIDTH, MESSAGEBOX_BUTTONHEIGHT, DialogMsgBox, DialogProcButton, 0, 0);
        DialogCreate("Cancel", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_CANGETFOCUS,
                     (messageBoxWidth / 2) + 4, buttonY, MESSAGEBOX_BUTTONWIDTH, MESSAGEBOX_BUTTONHEIGHT, DialogMsgBox, DialogProcButton, 0, 0);
        break;
    case MB_YESNO :
        DialogMsgBox->Data = (void*)MB_YESNO;
        DialogCreate("Yes", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_OK | DIALOG_CANGETFOCUS,
                     (messageBoxWidth / 2) - (MESSAGEBOX_BUTTONWIDTH + 4), buttonY, MESSAGEBOX_BUTTONWIDTH, MESSAGEBOX_BUTTONHEIGHT, DialogMsgBox, DialogProcButton, 0, 0);
        DialogCreate("No",  DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_CANGETFOCUS,
                     (messageBoxWidth / 2) + 4, buttonY, MESSAGEBOX_BUTTONWIDTH, MESSAGEBOX_BUTTONHEIGHT, DialogMsgBox, DialogProcButton, 0, 0);
        break;
    case MB_YESNOCANCEL :
        DialogMsgBox->Data = (void*)MB_YESNOCANCEL;
        DialogCreate("Yes",    DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_OK | DIALOG_CANGETFOCUS,
                     (messageBoxWidth / 2) - (MESSAGEBOX_BUTTONWIDTH + 8 + MESSAGEBOX_BUTTONWIDTHHALF), buttonY, MESSAGEBOX_BUTTONWIDTH, MESSAGEBOX_BUTTONHEIGHT, DialogMsgBox, DialogProcButton, 0, 0);
        DialogCreate("No",     DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_CANGETFOCUS,
                     (messageBoxWidth / 2) - MESSAGEBOX_BUTTONWIDTHHALF, buttonY, MESSAGEBOX_BUTTONWIDTH, MESSAGEBOX_BUTTONHEIGHT, DialogMsgBox, DialogProcButton, 0, 0);
        DialogCreate("Cancel", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_CANGETFOCUS,
                     (messageBoxWidth / 2) + (MESSAGEBOX_BUTTONWIDTH + 8 + MESSAGEBOX_BUTTONWIDTHHALF), buttonY, MESSAGEBOX_BUTTONWIDTH, MESSAGEBOX_BUTTONHEIGHT, DialogMsgBox, DialogProcButton, 0, 0);
        break;
    default :
        DialogMsgBox->Data = (void*)MB_OK;
        DialogCreate("OK", DIALOG_CHILD | DIALOG_VISIBLE | DIALOG_CENTER | DIALOG_OK,
                     (messageBoxWidth / 2) - MESSAGEBOX_BUTTONWIDTHHALF, buttonY, MESSAGEBOX_BUTTONWIDTH, MESSAGEBOX_BUTTONHEIGHT, DialogMsgBox, DialogProcButton, 0, 0);
        break;
    }

    DialogShow(DialogMsgBox, 0);
    return 1;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcMessageBox(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2)
{
    switch (Msg)
    {
    case WM_DESTROY :
        return DIALOG_MSGHANDLED;

    case WM_COMMAND :
        if (Param2 == Dialog->Children[ 1 ])
        {
            switch ((size_t)Dialog->Data)
            {
            case MB_ABORTRETRYIGNORE :  DialogSetRetValue(IDABORT); break;
            case MB_OKCANCEL :          DialogSetRetValue(IDOK); break;
            case MB_RETRYCANCEL :       DialogSetRetValue(IDRETRY); break;
            case MB_YESNO :             DialogSetRetValue(IDYES); break;
            case MB_YESNOCANCEL :       DialogSetRetValue(IDYES); break;
            case MB_OK :                DialogSetRetValue(IDOK); break;
            }

            if (Dialog->Callback)
                Dialog->Callback(RetValue);

            DialogHide(DialogMsgBox);
            DialogDelete(DialogMsgBox);

            return DIALOG_MSGHANDLED;
        }
        if (Param2 == Dialog->Children[ 2 ])
        {
            switch ((size_t)Dialog->Data)
            {
            case MB_ABORTRETRYIGNORE :  DialogSetRetValue(IDRETRY); break;
            case MB_OKCANCEL :          DialogSetRetValue(IDCANCEL); break;
            case MB_RETRYCANCEL :       DialogSetRetValue(IDCANCEL); break;
            case MB_YESNO :             DialogSetRetValue(IDNO); break;
            case MB_YESNOCANCEL :       DialogSetRetValue(IDNO); break;
            }

            if (Dialog->Callback)
                Dialog->Callback(RetValue);

            DialogHide(DialogMsgBox);
            DialogDelete(DialogMsgBox);

            return DIALOG_MSGHANDLED;
        }
        if (Param2 == Dialog->Children[ 3 ])
        {
            switch ((size_t)Dialog->Data)
            {
            case MB_ABORTRETRYIGNORE :  DialogSetRetValue(IDIGNORE); break;
            case MB_YESNOCANCEL :       DialogSetRetValue(IDCANCEL); break;
            }

            if (Dialog->Callback)
                Dialog->Callback(RetValue);

            DialogHide(DialogMsgBox);
            DialogDelete(DialogMsgBox);

            return DIALOG_MSGHANDLED;
        }
        break;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogResize(void)
{
    size_t i;

    for (i = 0; i < DialogCount; i++)
        DialogSend(&Dialogs[ i ], WM_SIZE, 0, 0);
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogEnable(Dialog_t* Dialog, size_t Enable)
{
    if (Enable)
        Dialog->Flags |= DIALOG_ENABLED;
    else
    {
        Dialog->Flags &= ~DIALOG_ENABLED;
        Dialog->Focus = nullptr;
    }
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogCloseAllPopups(Dialog_t* ExceptThisOne)
{
    size_t i;

    for (i = 0; i < DialogCount; i++)
    {
        if (Dialogs[ i ].Flags & DIALOG_POPUPMENU && &Dialogs[ i ] != ExceptThisOne)
            DialogHide(&Dialogs[ i ]);
    }
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogCenter(Dialog_t* Dialog)
{
    Dialog->X = (DialogViewportX2 / 2) - (Dialog->W / 2);
    Dialog->Y = (DialogViewportY2 / 2) - (Dialog->H / 2);
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

size_t DialogCreateTimer(Dialog_t* Dialog, size_t Interval)
{
    size_t i;

    for (i = 0; i < DIALOG_MAXTIMERS; i++)
    {
        if (nullptr == Timers[ i ].Dialog)
            break;
    }

    if (i >= DIALOG_MAXTIMERS)
        return -1;

    Timers[ i ].Counter     = 0;
    Timers[ i ].Threshold   = Interval;
    Timers[ i ].Dialog      = Dialog;

    return i;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

void DialogDeleteTimer(size_t ID)
{
    if (ID >= 0 && ID < DIALOG_MAXTIMERS)
        memset(&Timers[ ID ], 0, sizeof(Timer_t));
}


//-----------------------------------------------------------------------------
// Name: DialogSetViewport()
// Desc:
//-----------------------------------------------------------------------------

void DialogSetViewport(float X1, float Y1, float X2, float Y2)
{
    DialogViewportX1 = X1;
    DialogViewportY1 = Y1;
    DialogViewportX2 = X2;
    DialogViewportY2 = Y2;
}


//-----------------------------------------------------------------------------
// Name: IsDialogActive()
// Desc:
//-----------------------------------------------------------------------------

size_t IsDialogActive(Dialog_t* Dialog)
{
    return (Dialog->Flags & DIALOG_VISIBLE);
}


//-----------------------------------------------------------------------------
// Name: DialogUpdateFlags()
// Desc:
//-----------------------------------------------------------------------------

void DialogUpdateFlags(Dialog_t* Dialog, size_t addFlags, size_t removeFlags)
{
    Dialog->Flags &= ~removeFlags;
    Dialog->Flags |= addFlags;
}
