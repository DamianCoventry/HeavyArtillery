//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Friday, 5 April 2002
//-----------------------------------------------------------------------------


#ifndef _DIALOG_H_
#define _DIALOG_H_

#include <string>

#define DIALOG_MAXDIALOGS           1024
#define PROPSHEET_MAXPAGES          16

#define DIALOG_VISIBLE              0x00000001
#define DIALOG_CHILD                0x00000002
#define DIALOG_MODAL                0x00000004
#define DIALOG_MOVEABLE             0x00000008
#define DIALOG_PROPPAGE             0x00000010
#define DIALOG_CENTER               0x00000020
#define DIALOG_OK                   0x00000040
#define DIALOG_CANCEL               0x00000080
#define DIALOG_ENABLED              0x00000100
#define DIALOG_HIGHLIGHT            0x00000200
#define DIALOG_TITLE                0x00000400
#define DIALOG_POPUPMENU            0x00000800
#define DIALOG_MENUITEM             0x00001000
#define DIALOG_RAISED               0x00002000
#define DIALOG_HASSUBMENU           0x00004000
#define DIALOG_CANGETFOCUS          0x00008000
#define DIALOG_TABSELECTED          0x00010000
#define DIALOG_TABUNSELECTED        0x00020000
#define DIALOG_RIGHTALIGN           0x00040000
#define DIALOG_BORDER               0x00080000
#define DIALOG_TOOLBAR              0x00100000

#define DIALOG_MAXCHILDREN          128

#define DIALOG_MSGNOTHANDLED        0
#define DIALOG_MSGHANDLED           1

#define DIALOG_BROADCAST            ((Dialog_t *)1)

#define WA_ONLYCLOSE                1

#define DIALOG_PADDING              8.0f
#define DIALOG_PADDINGx2            16.0f
#define BUTTON_PADDING              12.0f

#define MESSAGEBOX_BUTTONWIDTH      100
#define MESSAGEBOX_BUTTONWIDTHHALF  50
#define MESSAGEBOX_BUTTONHEIGHT     35

#define MENUITEM_BUTTONWIDTH        125
#define MENUITEM_BUTTONHEIGHT       25

#define TOOLDIALOG_TITLEBARHEIGHT   22
#define DIALOG_TITLEBARHEIGHT       32


//-----------------------------------------------------------------------------
// public structures
//-----------------------------------------------------------------------------

struct Dialog_s;

typedef size_t (*DialogProc_t)(struct Dialog_s* Dialog, size_t Msg, void* Param1, void* Param2);

typedef struct Dialog_s
{
    Dialog_s() { Reset(); }

    void Reset()
    {
        X = Y = W = H = 0.0f;
        Flags = ChildCount = DontDraw = OnClose = 0;

        Name.clear();
        Hint.clear();

        DialogProc = nullptr;
        Data = nullptr;
        Callback = nullptr;
        Focus = nullptr;
        Hover = nullptr;
        Parent = nullptr;
        Next = nullptr;
        Prev = nullptr;

        memset(Children, 0, sizeof(Dialog_s*) * DIALOG_MAXCHILDREN);
    }

    float X, Y;
    float W, H;
    size_t Flags;
    size_t ChildCount;
    size_t DontDraw;
    DialogProc_t DialogProc;
    std::string Name;
    void* Data;
    std::string Hint;
    size_t OnClose;

    void (*Callback)(size_t ButtonID);

    struct Dialog_s* Focus;
    struct Dialog_s* Hover;
    struct Dialog_s* Parent;
    struct Dialog_s* Next;
    struct Dialog_s* Prev;
    struct Dialog_s* Children[ DIALOG_MAXCHILDREN ];
}
Dialog_t;

typedef struct PropertySheet_s
{
    size_t CurrentPage;
    size_t PageCount;
    Dialog_t* Pages[ PROPSHEET_MAXPAGES ];
}
PropertySheet_t;


//-----------------------------------------------------------------------------
// public data
//-----------------------------------------------------------------------------

extern Dialog_t* DialogCapture;

extern Dialog_t* DialogHead;
extern Dialog_t* DialogTail;
extern Dialog_t* DialogMsgBox;

extern float CursorX;
extern float CursorY;

extern float DialogViewportX1;
extern float DialogViewportY1;
extern float DialogViewportX2;
extern float DialogViewportY2;


//-----------------------------------------------------------------------------
// public functions
//-----------------------------------------------------------------------------

Dialog_t* DialogCreate(const std::string& Name, size_t Flags,
                       float X, float Y, float W, float H,
                       Dialog_t* Parent, DialogProc_t DialogProc, void* Param1, void* Param2);

void DialogDelete(Dialog_t* Dialog);

void DialogTrackPopup(Dialog_t* Dialog, float X, float Y, void* Param);
void DialogShow(Dialog_t* Dialog, void* Param);
void DialogHide(Dialog_t* Dialog);
void DialogHideAll(Dialog_t* ExceptThisOne);

void DialogBringToTop(Dialog_t* Dialog);

void DrawActiveDialogs(void);
void DialogToggle(Dialog_t* Dialog);

void DialogScreenToClient(Dialog_t* Dialog, float* X, float* Y);
void DialogClientToScreen(Dialog_t* Dialog, float* X, float* Y);

Dialog_t* DialogAtCoords(float X, float Y);
Dialog_t* DialogControlAtCoords(Dialog_t* Parent, float X, float Y);

size_t DialogSend(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2);

size_t DispatchDialogMessages(void);

size_t DialogSetFocus(Dialog_t* Parent, Dialog_t* Child);
void DialogFocusNext(Dialog_t* Parent);
void DialogFocusPrev(Dialog_t* Parent);

void MoveMouseCursor(void);
void DrawMouseCursor(void);
void DialogShowCursor(size_t Visible);

size_t DialogRetValue(void);
void DialogSetRetValue(size_t Value);
Dialog_t* DialogJustEnded(void);

size_t DialogMessageBox(const std::string& Title, const std::string& Text, size_t Type, void(*Callback)(size_t RetValue));
size_t DialogProcMessageBox(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2);

void DialogResize(void);
void DialogEnable(Dialog_t* Dialog, size_t Enable);

void DialogCloseAllPopups(Dialog_t* ExceptThisOne);
void DialogCenter(Dialog_t* Dialog);

size_t DialogCreateTimer(Dialog_t* Dialog, size_t Interval);
void DialogDeleteTimer(size_t ID);

void DialogSetViewport(float X1, float Y1, float X2, float Y2);

size_t IsDialogActive(Dialog_t* Dialog);

void DialogUpdateFlags(Dialog_t* Dialog, size_t addFlags, size_t removeFlags);


#endif
