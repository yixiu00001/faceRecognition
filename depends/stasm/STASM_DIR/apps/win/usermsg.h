// usermsg.h: Routines for displaying messages in a Windows environment
//
// Copyright (C) 2005-2013, Stephen Milborrow

#ifndef STASM_USERMSG_H
#define STASM_USERMSG_H

namespace stasm
{
void CloseUserMsg(void); // remove popup message

void TimedUserMsg(HWND hwnd, const char* format, ...); // popup msg for 3 seconds

void UserMsg(HWND hwnd, const char* format, ...); // permanent popup msg (until CloseUserMsg)

} // namespace stasm
#endif // STASM_USERMSG_H
