/* Generated by wbuild
 * (generator version 3.2)
 */
#ifndef ___XWSCROLLWINP_H
#define ___XWSCROLLWINP_H
#include <./xwBoardP.h>
#include <./xwScrollWin.h>
_XFUNCPROTOBEGIN
typedef void (*scroll_response_Proc)(
#if NeedFunctionPrototypes
Widget ,XtPointer ,XtPointer 
#endif
);
#define XtInherit_scroll_response ((scroll_response_Proc) _XtInherit)

typedef struct {
/* methods */
scroll_response_Proc scroll_response;
/* class variables */
} XfwfScrolledWindowClassPart;

typedef struct _XfwfScrolledWindowClassRec {
CoreClassPart core_class;
CompositeClassPart composite_class;
XfwfCommonClassPart xfwfCommon_class;
XfwfFrameClassPart xfwfFrame_class;
XfwfBoardClassPart xfwfBoard_class;
XfwfScrolledWindowClassPart xfwfScrolledWindow_class;
} XfwfScrolledWindowClassRec;

typedef struct {
/* resources */
Boolean  autoAdjustScrollbars;
Boolean  traverseToChild;
Dimension  spacing;
Boolean  edgeBars;
Dimension  scrollbarWidth;
Dimension  shadowWidth;
Boolean  hideHScrollbar;
Boolean  hideVScrollbar;
int  vScrollAmount;
int  hScrollAmount;
Position  initialX;
Position  initialY;
Boolean  drawgrayScrollWin;
Boolean  doScroll;
XtCallbackList  scrollCallback;
XtCallbackProc  scrollResponse;
/* private state */
Widget  vscroll;
Widget  hscroll;
Widget  frame;
Widget  board;
Widget  CW;
Boolean  initializing;
XtCallbackProc  vscroll_resp;
XtCallbackProc  hscroll_resp;
} XfwfScrolledWindowPart;

typedef struct _XfwfScrolledWindowRec {
CorePart core;
CompositePart composite;
XfwfCommonPart xfwfCommon;
XfwfFramePart xfwfFrame;
XfwfBoardPart xfwfBoard;
XfwfScrolledWindowPart xfwfScrolledWindow;
} XfwfScrolledWindowRec;

externalref XfwfScrolledWindowClassRec xfwfScrolledWindowClassRec;

_XFUNCPROTOEND
#endif /* ___XWSCROLLWINP_H */
