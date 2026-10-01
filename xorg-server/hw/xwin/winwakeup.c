/*
 *Copyright (C) 1994-2000 The XFree86 Project, Inc. All Rights Reserved.
 *
 *Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 *"Software"), to deal in the Software without restriction, including
 *without limitation the rights to use, copy, modify, merge, publish,
 *distribute, sublicense, and/or sell copies of the Software, and to
 *permit persons to whom the Software is furnished to do so, subject to
 *the following conditions:
 *
 *The above copyright notice and this permission notice shall be
 *included in all copies or substantial portions of the Software.
 *
 *THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *NONINFRINGEMENT. IN NO EVENT SHALL THE XFREE86 PROJECT BE LIABLE FOR
 *ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 *CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *Except as contained in this notice, the name of the XFree86 Project
 *shall not be used in advertising or otherwise to promote the sale, use
 *or other dealings in this Software without prior written authorization
 *from the XFree86 Project.
 *
 * Authors:	Dakshinamurthy Karra
 *		Suhaib M Siddiqi
 *		Peter Busch
 *		Harold L Hunt II
 */

#ifdef HAVE_XWIN_CONFIG_H
#include <xwin-config.h>
#endif
#include "win.h"

#define WIN_MAX_MESSAGES_PER_WAKEUP 128

/* See Porting Layer Definition - p. 7 */
void
winWakeupHandler(ScreenPtr pScreen, int iResult)
{
    MSG msg;
    int i;

    /*
     * Process the queued messages in order, at most
     * WIN_MAX_MESSAGES_PER_WAKEUP per call.
     *
     * Without /dev/windows the server cannot sleep on the message queue, so
     * calls are at least one Windows timer tick apart, and handling a single
     * message per call makes bursts of input arrive slowly.  The bound keeps
     * a flood of key messages from overflowing the mi event queue.
     *
     * WM_PAINT and WM_TIMER are only returned when nothing else is queued,
     * and WM_PAINT keeps being returned while a window's update region stays
     * invalid, so stop after one of them rather than spin.
     */
    for (i = 0; i < WIN_MAX_MESSAGES_PER_WAKEUP
         && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE); ++i) {
        if ((g_hDlgDepthChange == 0
             || !IsDialogMessage(g_hDlgDepthChange, &msg))
            && (g_hDlgExit == 0 || !IsDialogMessage(g_hDlgExit, &msg))
            && (g_hDlgAbout == 0 || !IsDialogMessage(g_hDlgAbout, &msg))) {
            DispatchMessage(&msg);
        }
        if (msg.message == WM_PAINT || msg.message == WM_TIMER)
            break;
    }
}
