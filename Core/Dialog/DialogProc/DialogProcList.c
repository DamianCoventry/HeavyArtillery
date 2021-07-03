//-----------------------------------------------------------------------------
// Heavy Artillery
// Copyright (c) 2002, Damian Coventry
// All Rights Reserved
// Saturday, 6 April 2002
//-----------------------------------------------------------------------------


#include "stdafx.h"
#include "DialogProcList.h"
#include "DialogProcScrollBar.h"
#include "DialogProcDefault.h"
#include "Dialog.h"
#include "DialogScheme.h"
#include "DialogDraw.h"
#include "Input.h"
#include "Font.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define DIRECTINPUT_VERSION         0x0800
#include <dinput.h>
#include <stdlib.h>


//-----------------------------------------------------------------------------
// private functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: ShowHideScrollBar()
// Desc:
//-----------------------------------------------------------------------------

static void ShowHideScrollBar(Dialog_t* Dialog, List_t* List)
{
    if (List->Strings.size() > List->MaxVertStrings)
        Dialog->Children[0]->Flags |= DIALOG_VISIBLE;
    else
        Dialog->Children[0]->Flags &= ~DIALOG_VISIBLE;
}


//-----------------------------------------------------------------------------
// Name: EnsureVisible()
// Desc:
//-----------------------------------------------------------------------------

static void EnsureVisible(Dialog_t* Dialog, List_t* List, size_t Item)
{
    if (List->Strings.empty())
        return;

    while (Item < List->FirstIndex)
        DialogSend(Dialog, WM_VSCROLL, (void*)SB_LINEUP, 0);

    while (Item >= List->FirstIndex + List->MaxVertStrings)
        DialogSend(Dialog, WM_VSCROLL, (void*)SB_LINEDOWN, 0);
}


//-----------------------------------------------------------------------------
// Name: DialogProcList()
// Desc:
//-----------------------------------------------------------------------------

size_t DialogProcList(Dialog_t* Dialog, size_t Msg, void* Param1, void* Param2)
{
    float X, Y;
    size_t i, Index;
    float Percent;
    List_t* List;
    Dialog_t* Child;

    List = (List_t*)Dialog->Data;

    switch (Msg)
    {
    case WM_CREATE:
        List = new List_t();

        List->SelectedIndex = LB_ERR;
        List->MaxVertStrings = (size_t)(Dialog->H / DialogScheme->Fonts.ControlNormal->Height);

        Dialog->Data = (void*)List;

        // create a child vertical scrollbar, just don't make it visible yet
        Dialog->Children[0] = DialogCreate("Listscrollbar", DIALOG_CHILD,
                                           Dialog->W - SCROLLBAR_SIZE - 1, 1,
                                           SCROLLBAR_SIZE, Dialog->H - 2,
                                           Dialog, DialogProcScrollBar, 0, 0);

        return DIALOG_MSGHANDLED;

    case WM_SIZE:
        List->MaxVertStrings = (size_t)(Dialog->H / DialogScheme->Fonts.ControlNormal->Height);
        return DIALOG_MSGHANDLED;

    case WM_DESTROY:
        if (Dialog->Data)
        {
            for (const auto& value : List->Values)
                DialogSend(Dialog->Parent, WM_DELETEITEM, Dialog, (void*)&value);

            delete Dialog->Data;
            Dialog->Data = nullptr;
        }
        return DIALOG_MSGHANDLED;

    case WM_SETTEXT:
        if ((size_t)Param1 >= 0 && (size_t)Param1 < List->Strings.size())
            List->Strings[(size_t)Param1] = *(std::string*)Param2;
        return DIALOG_MSGHANDLED;

    case WM_GETTEXT:
        if ((size_t)Param1 >= 0 && (size_t)Param1 < List->Strings.size())
            *(std::string*)Param2 = List->Strings[(size_t)Param1];
        return DIALOG_MSGHANDLED;

    case LB_FINDSTRING:
    {
        std::string* OtherString = (std::string*)Param2;
        if ((size_t)Param1 >= 0 && (size_t)Param1 < List->Strings.size())
        {
            for (i = (size_t)Param1; i < List->Strings.size(); i++)
            {
                if (_stricmp(List->Strings[i].c_str(), OtherString->c_str()) == 0)
                {
                    DialogSetRetValue(i);
                    return DIALOG_MSGHANDLED;
                }
            }
        }
        DialogSetRetValue(-1);
        return DIALOG_MSGHANDLED;
    }
    case LB_ADDSTRING:
        List->Strings.push_back(*(std::string*)Param1);
        List->Values.push_back(0);

        ShowHideScrollBar(Dialog, List);

        DialogSend(Dialog, WM_VSCROLL, (void*)SB_BOTTOM, 0);
        return DIALOG_MSGHANDLED;

    case LB_DELETESTRING:
    {
        if ((size_t)Param1 >= 0 && (size_t)Param1 < List->Strings.size())
        {
            DialogSend(Dialog->Parent, WM_DELETEITEM, Dialog, (void*)List->Values[(size_t)Param1]);

            auto stringItor = List->Strings.begin();
            std::advance(stringItor, (size_t)Param1);
            List->Strings.erase(stringItor);

            auto valueItor = List->Values.begin();
            std::advance(valueItor, (size_t)Param1);
            List->Values.erase(valueItor);

            ShowHideScrollBar(Dialog, List);

            if (List->SelectedIndex == (size_t)Param1)
                List->SelectedIndex = LB_ERR;
        }
        return DIALOG_MSGHANDLED;
    }
    case LB_SETCURSEL:
        if ((size_t)Param1 >= 0 && (size_t)Param1 < List->Strings.size())
        {
            List->SelectedIndex = (size_t)Param1;
            EnsureVisible(Dialog, List, List->SelectedIndex);

            DialogSend(Dialog->Parent, WM_COMMAND, (void*)LBN_SELCHANGE, Dialog);
        }
        else    // assume caller is trying to clear the selection
            List->SelectedIndex = LB_ERR;
        return DIALOG_MSGHANDLED;

    case LB_GETCURSEL:
        DialogSetRetValue(List->SelectedIndex);
        return DIALOG_MSGHANDLED;

    case LB_GETCOUNT:
        DialogSetRetValue(List->Strings.size());
        return DIALOG_MSGHANDLED;

    case LB_SETITEMDATA:
        if ((size_t)Param1 >= 0 && (size_t)Param1 < List->Strings.size())
            List->Values[(size_t)Param1] = (size_t)Param2;
        return DIALOG_MSGHANDLED;

    case LB_GETITEMDATA:
        if ((size_t)Param1 >= 0 && (size_t)Param1 < List->Strings.size())
            DialogSetRetValue(List->Values[(size_t)Param1]);
        return DIALOG_MSGHANDLED;

    case LB_RESETCONTENT:
        for (i = 0; i < List->Strings.size(); i++)
            DialogSend(Dialog->Parent, WM_DELETEITEM, Dialog, (void*)List->Values[i]);

        Dialog->Children[0]->Flags &= ~DIALOG_VISIBLE;

        List->FirstIndex = 0;
        List->SelectedIndex = LB_ERR;
        List->Strings.clear();
        List->Values.clear();
        return DIALOG_MSGHANDLED;

    case WM_PAINT:
    {
        DialogClientToScreen(Dialog, &X, &Y);
        DialogDrawRectControl(X, Y, Dialog->W, Dialog->H);
        auto OrigX = X, OrigY = Y;

        Index = List->FirstIndex;
        for (i = 0; i < List->MaxVertStrings && Index < List->Strings.size(); ++i)
        {
            if (Index == List->SelectedIndex)
            {
                DialogDrawRect(X + 1, Y, Dialog->W - 1, (float)DialogScheme->Fonts.ControlNormal->Height,
                    DialogScheme->Colors.TitleFocus.R, DialogScheme->Colors.TitleFocus.G, DialogScheme->Colors.TitleFocus.B);

                FontWrite(DialogScheme->Fonts.ControlHighlight, X + 4, Y,
                    DialogScheme->Colors.FontControlHighlight, List->Strings[Index]);
            }
            else
                FontWrite(DialogScheme->Fonts.ControlNormal, X + 4, Y,
                    DialogScheme->Colors.FontControlNormal, List->Strings[Index]);

            Y += DialogScheme->Fonts.ControlNormal->Height;
            ++Index;
        }

        if (Dialog->Children[0]->Flags & DIALOG_VISIBLE)
            DialogSend(Dialog->Children[0], Msg, Param1, Param2);

        if (Dialog->Parent->Focus == Dialog)
            DialogDrawRectFocus(OrigX, OrigY, Dialog->W, Dialog->H);

        return DIALOG_MSGHANDLED;
    }
    case WM_VSCROLL:
        switch ((size_t)Param1)
        {
        case SB_LINEUP:
            if (List->FirstIndex > 0)
                --List->FirstIndex;

            if (Dialog->Children[0]->Flags & DIALOG_VISIBLE)
            {
                // synchronise scrollbox
                Percent = ((float)List->FirstIndex / ((float)List->Strings.size() - (float)List->MaxVertStrings)) * 100.0f;
                Dialog->Children[0]->Children[2]->Y = SCROLLBAR_SIZE + (size_t)((float)Percent * (float)(((float)Dialog->Children[0]->H - (3.0f * (float)SCROLLBAR_SIZE)) / 100.0f));
            }
            break;
        case SB_LINEDOWN:
            if (++List->FirstIndex > List->Strings.size() - List->MaxVertStrings)
                List->FirstIndex = List->Strings.size() - List->MaxVertStrings;

            if (Dialog->Children[0]->Flags & DIALOG_VISIBLE)
            {
                // synchronise scrollbox
                Percent = ((float)List->FirstIndex / ((float)List->Strings.size() - (float)List->MaxVertStrings)) * 100.0f;
                Dialog->Children[0]->Children[2]->Y = SCROLLBAR_SIZE + (size_t)((float)Percent * (float)(((float)Dialog->Children[0]->H - (3.0f * (float)SCROLLBAR_SIZE)) / 100.0f));
            }
            break;
        case SB_PAGEUP:
            if (List->MaxVertStrings >= List->FirstIndex)
                List->FirstIndex = 0;
            else
                List->FirstIndex -= List->MaxVertStrings;

            if (Dialog->Children[0]->Flags & DIALOG_VISIBLE)
            {
                // synchronise scrollbox
                Percent = ((float)List->FirstIndex / ((float)List->Strings.size() - (float)List->MaxVertStrings)) * 100.0f;
                Dialog->Children[0]->Children[2]->Y = SCROLLBAR_SIZE + (size_t)((float)Percent * (float)(((float)Dialog->Children[0]->H - (3.0f * (float)SCROLLBAR_SIZE)) / 100.0f));
            }
            break;
        case SB_PAGEDOWN:
            if (List->Strings.size() > List->MaxVertStrings)
            {
                List->FirstIndex += List->MaxVertStrings;
                if (List->FirstIndex > List->Strings.size() - List->MaxVertStrings)
                    List->FirstIndex = List->Strings.size() - List->MaxVertStrings;
            }

            if (Dialog->Children[0]->Flags & DIALOG_VISIBLE)
            {
                // synchronise scrollbox
                Percent = ((float)List->FirstIndex / ((float)List->Strings.size() - (float)List->MaxVertStrings)) * 100.0f;
                Dialog->Children[0]->Children[2]->Y = SCROLLBAR_SIZE + (size_t)((float)Percent * (float)(((float)Dialog->Children[0]->H - (3.0f * (float)SCROLLBAR_SIZE)) / 100.0f));
            }
            break;
        case SB_TOP:
            List->FirstIndex = 0;
            if (Dialog->Children[0]->Flags & DIALOG_VISIBLE)
            {
                // synchronise scrollbox
                Dialog->Children[0]->Children[2]->Y = SCROLLBAR_SIZE;
            }
            break;
        case SB_BOTTOM:
            if (List->Strings.size() > List->MaxVertStrings)
                List->FirstIndex = List->Strings.size() - List->MaxVertStrings;
            if (Dialog->Children[0]->Flags & DIALOG_VISIBLE)
            {
                // synchronise scrollbox
                Dialog->Children[0]->Children[2]->Y = Dialog->Children[0]->H - (2 * SCROLLBAR_SIZE);
            }
            break;
        case SB_THUMBTRACK:
            List->FirstIndex = (size_t)((((float)List->Strings.size() - (float)List->MaxVertStrings) / 100.0f) * (float)(size_t)Param2);
            break;
        }
        return DIALOG_MSGHANDLED;

    case WM_KEYDOWN:
        if (IsKeyPressed(DIK_UP) || IsKeyPressed(DIK_LEFT))
        {
            if (!List->Strings.empty())
            {
                if (List->SelectedIndex == LB_ERR)
                    List->SelectedIndex = 0;
                else if (List->SelectedIndex > 0)
                    --List->SelectedIndex;

                EnsureVisible(Dialog, List, List->SelectedIndex);

                DialogSend(Dialog->Parent, WM_COMMAND, (void*)LBN_SELCHANGE, Dialog);
            }
            return DIALOG_MSGHANDLED;
        }

        if (IsKeyPressed(DIK_DOWN) || IsKeyPressed(DIK_RIGHT))
        {
            if (!List->Strings.empty())
            {
                if (List->SelectedIndex == LB_ERR)
                    List->SelectedIndex = 0;
                else if (++List->SelectedIndex > List->Strings.size() - 1)
                    List->SelectedIndex = List->Strings.size() - 1;

                EnsureVisible(Dialog, List, List->SelectedIndex);

                DialogSend(Dialog->Parent, WM_COMMAND, (void*)LBN_SELCHANGE, Dialog);
            }
            return DIALOG_MSGHANDLED;
        }

        if (IsKeyPressed(DIK_PRIOR))
        {
            if (List->SelectedIndex == LB_ERR)
                List->SelectedIndex = 0;
            else if (List->MaxVertStrings > List->SelectedIndex)
                List->SelectedIndex = 0;
            else
                List->SelectedIndex -= List->MaxVertStrings;

            EnsureVisible(Dialog, List, List->SelectedIndex);

            DialogSend(Dialog->Parent, WM_COMMAND, (void*)LBN_SELCHANGE, Dialog);
            return DialogSend(Dialog, WM_VSCROLL, (void*)SB_PAGEUP, 0);
        }

        if (IsKeyPressed(DIK_NEXT))
        {
            if (List->SelectedIndex == LB_ERR)
                List->SelectedIndex = 0;
            else
            {
                List->SelectedIndex += List->MaxVertStrings;
                if (List->SelectedIndex > List->Strings.size() - 1)
                    List->SelectedIndex = List->Strings.size() - 1;
            }

            EnsureVisible(Dialog, List, List->SelectedIndex);

            DialogSend(Dialog->Parent, WM_COMMAND, (void*)LBN_SELCHANGE, Dialog);
            return DialogSend(Dialog, WM_VSCROLL, (void*)SB_PAGEDOWN, 0);
        }

        if (IsKeyPressed(DIK_HOME))
        {
            List->SelectedIndex = 0;
            EnsureVisible(Dialog, List, List->SelectedIndex);
            DialogSend(Dialog->Parent, WM_COMMAND, (void*)LBN_SELCHANGE, Dialog);
            return DialogSend(Dialog, WM_VSCROLL, (void*)SB_TOP, 0);
        }

        if (IsKeyPressed(DIK_END))
        {
            List->SelectedIndex = List->Strings.size() - 1;
            EnsureVisible(Dialog, List, List->SelectedIndex);
            DialogSend(Dialog->Parent, WM_COMMAND, (void*)LBN_SELCHANGE, Dialog);
            return DialogSend(Dialog, WM_VSCROLL, (void*)SB_BOTTOM, 0);
        }
        break; // Let tab and Shift+tab to be processed.

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
        Child = DialogControlAtCoords(Dialog, CursorX, CursorY);
        if (Child && (Child->Flags & DIALOG_VISIBLE) && (Child->Flags & DIALOG_ENABLED))
        {
            DialogSetFocus(Dialog, Child);
            return DialogSend(Child, Msg, Param1, Param2);
        }
        else if (!(Dialog->Flags & DIALOG_CHILD))
            DialogSetFocus(Dialog, nullptr);

        DialogCapture = Dialog;

        if (List->Strings.empty())
            List->SelectedIndex = LB_ERR;
        else
        {
            X = CursorX;
            Y = CursorY;
            DialogScreenToClient(Dialog, &X, &Y);
            List->SelectedIndex = (size_t)(List->FirstIndex + (Y / DialogScheme->Fonts.ControlNormal->Height));

            if (List->SelectedIndex > List->Strings.size() - 1)
                List->SelectedIndex = LB_ERR;
            else
                EnsureVisible(Dialog, List, List->SelectedIndex);
        }

        //snd_play( snd_click1, 0 );
        DialogSend(Dialog->Parent, WM_COMMAND, (void*)LBN_SELCHANGE, Dialog);
        return DIALOG_MSGHANDLED;
    }

    return DialogProcDefault(Dialog, Msg, Param1, Param2);
}
