/* Generated by wbuild
 * (generator version 3.2)
 */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <string.h>
#include <stdio.h>
#include <X11/Xmu/Converters.h>
#include <X11/Xmu/CharSet.h>
#include <X11/bitmaps/gray>
#include <./xwFrameP.h>
static void set_shadow(
#if NeedFunctionPrototypes
Widget,XEvent*,String*,Cardinal*
#endif
);

static XtActionsRec actionsList[] = {
{"set_shadow", set_shadow},
};
static void _resolve_inheritance(
#if NeedFunctionPrototypes
WidgetClass
#endif
);
static void class_initialize(
#if NeedFunctionPrototypes
void
#endif
);
static void initialize(
#if NeedFunctionPrototypes
Widget ,Widget,ArgList ,Cardinal *
#endif
);
static void realize(
#if NeedFunctionPrototypes
Widget,XtValueMask *,XSetWindowAttributes *
#endif
);
static void destroy(
#if NeedFunctionPrototypes
Widget
#endif
);
static Boolean  set_values(
#if NeedFunctionPrototypes
Widget ,Widget ,Widget,ArgList ,Cardinal *
#endif
);
static void _expose(
#if NeedFunctionPrototypes
Widget,XEvent *,Region 
#endif
);
static void compute_inside(
#if NeedFunctionPrototypes
Widget,Position *,Position *,int *,int *
#endif
);
static Dimension  total_frame_width(
#if NeedFunctionPrototypes
Widget
#endif
);
static XtGeometryResult  query_geometry(
#if NeedFunctionPrototypes
Widget,XtWidgetGeometry *,XtWidgetGeometry *
#endif
);
static XtGeometryResult  geometry_manager(
#if NeedFunctionPrototypes
Widget ,XtWidgetGeometry *,XtWidgetGeometry *
#endif
);
static void resize(
#if NeedFunctionPrototypes
Widget
#endif
);
static void change_managed(
#if NeedFunctionPrototypes
Widget
#endif
);
#define done(type, value) do {\
      if (to->addr != NULL) {\
          if (to->size < sizeof(type)) {\
              to->size = sizeof(type);\
              return False;\
          }\
          *(type*)(to->addr) = (value);\
      } else {\
          static type static_val;\
          static_val = (value);\
          to->addr = (XtPointer)&static_val;\
      }\
      to->size = sizeof(type);\
      return True;\
  }while (0 )


static void create_darkgc(
#if NeedFunctionPrototypes
Widget
#endif
);
static void create_lightgc(
#if NeedFunctionPrototypes
Widget
#endif
);
static void compute_topcolor(
#if NeedFunctionPrototypes
Widget,int ,XrmValue *
#endif
);
static void compute_bottomcolor(
#if NeedFunctionPrototypes
Widget,int ,XrmValue *
#endif
);
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void create_darkgc(Widget self)
#else
static void create_darkgc(self)Widget self;
#endif
{
    XtGCMask mask=0;
    XGCValues values;

    if (((XfwfFrameWidget)self)->xfwfFrame.darkgc != NULL) XtReleaseGC(self, ((XfwfFrameWidget)self)->xfwfFrame.darkgc);
    switch (((XfwfFrameWidget)self)->xfwfFrame.shadowScheme) {
    case XfwfColor:
        mask = GCForeground;
        values.foreground = ((XfwfFrameWidget)self)->xfwfFrame.bottomShadowColor;
        break;
    case XfwfStipple:
        mask = GCFillStyle | GCStipple | GCForeground | GCBackground;
        values.fill_style = FillOpaqueStippled;
        values.stipple = ((XfwfFrameWidget)self)->xfwfFrame.bottomShadowStipple ? ((XfwfFrameWidget)self)->xfwfFrame.bottomShadowStipple : GetGray(self);
        values.foreground = BlackPixelOfScreen(XtScreen(self));
        values.background = ((XfwfFrameWidget)self)->core.background_pixel;
        break;
    case XfwfBlack:
	mask = GCForeground;
	values.foreground = BlackPixelOfScreen(XtScreen(self));
	break;
    case XfwfAuto:
        if (DefaultDepthOfScreen(XtScreen(self)) > 4
            && ((XfwfFrameWidgetClass)self->core.widget_class)->xfwfCommon_class.darker_color(self, ((XfwfFrameWidget)self)->core.background_pixel, &values.foreground)) {
            mask = GCForeground;
	    ((XfwfFrameWidget)self)->xfwfCommon.highlightColor = values.foreground;
        } else {
            mask = GCFillStyle | GCBackground | GCForeground | GCStipple;
            values.fill_style = FillOpaqueStippled;
            /* values.background = $background_pixel; */
            values.background = WhitePixelOfScreen(XtScreen(self));
            values.foreground = BlackPixelOfScreen(XtScreen(self));
            values.stipple = GetDarkGray(self);
        }
        break;
    }
    ((XfwfFrameWidget)self)->xfwfFrame.darkgc = XtGetGC(self, mask, &values);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void create_lightgc(Widget self)
#else
static void create_lightgc(self)Widget self;
#endif
{
    XtGCMask mask=0;
    XGCValues values;

    if (((XfwfFrameWidget)self)->xfwfFrame.lightgc != NULL) XtReleaseGC(self, ((XfwfFrameWidget)self)->xfwfFrame.lightgc);
    switch (((XfwfFrameWidget)self)->xfwfFrame.shadowScheme) {
    case XfwfColor:
        mask = GCForeground;
        values.foreground = ((XfwfFrameWidget)self)->xfwfFrame.topShadowColor;
        break;
    case XfwfStipple:
        mask = GCFillStyle | GCStipple | GCForeground | GCBackground;
        values.fill_style = FillOpaqueStippled;
        values.background = ((XfwfFrameWidget)self)->core.background_pixel;
        values.stipple = ((XfwfFrameWidget)self)->xfwfFrame.topShadowStipple ? ((XfwfFrameWidget)self)->xfwfFrame.topShadowStipple : GetGray(self);
        values.foreground = WhitePixelOfScreen(XtScreen(self));
        break;
    case XfwfBlack:
	mask = GCForeground;
	values.foreground = BlackPixelOfScreen(XtScreen(self));
	break;
    case XfwfAuto:
        if (DefaultDepthOfScreen(XtScreen(self)) > 4
            && ((XfwfFrameWidgetClass)self->core.widget_class)->xfwfCommon_class.lighter_color(self, ((XfwfFrameWidget)self)->core.background_pixel, &values.foreground)) {
            mask = GCForeground;
        } else {
            mask = GCFillStyle | GCBackground | GCForeground | GCStipple;
            values.fill_style = FillOpaqueStippled;
            /* values.background = $background_pixel; */
            values.background = WhitePixelOfScreen(XtScreen(self));
            values.foreground = BlackPixelOfScreen(XtScreen(self));
            values.stipple = GetLightGray(self);
        }
        break;
    }
    ((XfwfFrameWidget)self)->xfwfFrame.lightgc = XtGetGC(self, mask, &values);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void compute_topcolor(Widget self,int  offset,XrmValue * value)
#else
static void compute_topcolor(self,offset,value)Widget self;int  offset;XrmValue * value;
#endif
{
    static Pixel color;
#if 1
    ((XfwfFrameWidgetClass)self->core.widget_class)->xfwfCommon_class.lighter_color(self, ((XfwfFrameWidget)self)->core.background_pixel, &color);
#else
    (void) choose_color(self, 1.35, ((XfwfFrameWidget)self)->core.background_pixel, &color);
#endif
    value->addr = (XtPointer) &color;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void compute_bottomcolor(Widget self,int  offset,XrmValue * value)
#else
static void compute_bottomcolor(self,offset,value)Widget self;int  offset;XrmValue * value;
#endif
{
    static Pixel color;
#if 1
    ((XfwfFrameWidgetClass)self->core.widget_class)->xfwfCommon_class.darker_color(self, ((XfwfFrameWidget)self)->core.background_pixel, &color);
#else
    (void) choose_color(self, 0.6, ((XfwfFrameWidget)self)->core.background_pixel, &color);
#endif
    value->addr = (XtPointer) &color;
}

static XtResource resources[] = {
{XtNcursor,XtCCursor,XtRCursor,sizeof(((XfwfFrameRec*)NULL)->xfwfFrame.cursor),XtOffsetOf(XfwfFrameRec,xfwfFrame.cursor),XtRImmediate,(XtPointer)None },
{XtNframeType,XtCFrameType,XtRFrameType,sizeof(((XfwfFrameRec*)NULL)->xfwfFrame.frameType),XtOffsetOf(XfwfFrameRec,xfwfFrame.frameType),XtRImmediate,(XtPointer)XfwfRaised },
{XtNframeWidth,XtCFrameWidth,XtRDimension,sizeof(((XfwfFrameRec*)NULL)->xfwfFrame.frameWidth),XtOffsetOf(XfwfFrameRec,xfwfFrame.frameWidth),XtRImmediate,(XtPointer)0 },
{XtNouterOffset,XtCOuterOffset,XtRDimension,sizeof(((XfwfFrameRec*)NULL)->xfwfFrame.outerOffset),XtOffsetOf(XfwfFrameRec,xfwfFrame.outerOffset),XtRImmediate,(XtPointer)0 },
{XtNinnerOffset,XtCInnerOffset,XtRDimension,sizeof(((XfwfFrameRec*)NULL)->xfwfFrame.innerOffset),XtOffsetOf(XfwfFrameRec,xfwfFrame.innerOffset),XtRImmediate,(XtPointer)0 },
{XtNshadowScheme,XtCShadowScheme,XtRShadowScheme,sizeof(((XfwfFrameRec*)NULL)->xfwfFrame.shadowScheme),XtOffsetOf(XfwfFrameRec,xfwfFrame.shadowScheme),XtRImmediate,(XtPointer)XfwfAuto },
{XtNtopShadowColor,XtCTopShadowColor,XtRPixel,sizeof(((XfwfFrameRec*)NULL)->xfwfFrame.topShadowColor),XtOffsetOf(XfwfFrameRec,xfwfFrame.topShadowColor),XtRCallProc,(XtPointer)compute_topcolor },
{XtNbottomShadowColor,XtCBottomShadowColor,XtRPixel,sizeof(((XfwfFrameRec*)NULL)->xfwfFrame.bottomShadowColor),XtOffsetOf(XfwfFrameRec,xfwfFrame.bottomShadowColor),XtRCallProc,(XtPointer)compute_bottomcolor },
{XtNtopShadowStipple,XtCTopShadowStipple,XtRBitmap,sizeof(((XfwfFrameRec*)NULL)->xfwfFrame.topShadowStipple),XtOffsetOf(XfwfFrameRec,xfwfFrame.topShadowStipple),XtRImmediate,(XtPointer)NULL },
{XtNbottomShadowStipple,XtCBottomShadowStipple,XtRBitmap,sizeof(((XfwfFrameRec*)NULL)->xfwfFrame.bottomShadowStipple),XtOffsetOf(XfwfFrameRec,xfwfFrame.bottomShadowStipple),XtRImmediate,(XtPointer)NULL },
{XtNborderWidth,XtCBorderWidth,XtRDimension,sizeof(((XfwfFrameRec*)NULL)->core.border_width),XtOffsetOf(XfwfFrameRec,core.border_width),XtRImmediate,(XtPointer)0 },
};

XfwfFrameClassRec xfwfFrameClassRec = {
{ /* core_class part */
/* superclass   	*/  (WidgetClass) &xfwfCommonClassRec,
/* class_name   	*/  "XfwfFrame",
/* widget_size  	*/  sizeof(XfwfFrameRec),
/* class_initialize 	*/  class_initialize,
/* class_part_initialize*/  _resolve_inheritance,
/* class_inited 	*/  FALSE,
/* initialize   	*/  initialize,
/* initialize_hook 	*/  NULL,
/* realize      	*/  realize,
/* actions      	*/  actionsList,
/* num_actions  	*/  1,
/* resources    	*/  resources,
/* num_resources 	*/  11,
/* xrm_class    	*/  NULLQUARK,
/* compres_motion 	*/  True ,
/* compress_exposure 	*/  XtExposeCompressMultiple ,
/* compress_enterleave 	*/  True ,
/* visible_interest 	*/  False ,
/* destroy      	*/  destroy,
/* resize       	*/  resize,
/* expose       	*/  XtInheritExpose,
/* set_values   	*/  set_values,
/* set_values_hook 	*/  NULL,
/* set_values_almost 	*/  XtInheritSetValuesAlmost,
/* get_values+hook 	*/  NULL,
/* accept_focus 	*/  XtInheritAcceptFocus,
/* version      	*/  XtVersion,
/* callback_private 	*/  NULL,
/* tm_table      	*/  NULL,
/* query_geometry 	*/  query_geometry,
/* display_acceleator 	*/  XtInheritDisplayAccelerator,
/* extension    	*/  NULL 
},
{ /* composite_class part */
geometry_manager,
change_managed,
XtInheritInsertChild,
XtInheritDeleteChild,
NULL
},
{ /* XfwfCommon_class part */
compute_inside,
total_frame_width,
_expose,
XtInherit_highlight_border,
XtInherit_unhighlight_border,
XtInherit_hilite_callbacks,
XtInherit_would_accept_focus,
XtInherit_traverse,
XtInherit_lighter_color,
XtInherit_darker_color,
XtInherit_set_color,
/* traversal_trans */  NULL ,
/* traversal_trans_small */  NULL ,
/* travMode */  1 ,
},
{ /* XfwfFrame_class part */
 /* dummy */  0
},
};
WidgetClass xfwfFrameWidgetClass = (WidgetClass) &xfwfFrameClassRec;
/*ARGSUSED*/
static void set_shadow(self,event,params,num_params)Widget self;XEvent*event;String*params;Cardinal*num_params;
{
    Position x, y;
    int w, h;
    FrameType f = XfwfSunken;

    if (*num_params == 0) f = ((XfwfFrameWidget)self)->xfwfFrame.old_frame_type;  /* Reset to old style */
    else if (strcmp("raised", params[0]) == 0) f = XfwfRaised;
    else if (strcmp("sunken", params[0]) == 0) f = XfwfSunken;
    else if (strcmp("chiseled", params[0]) == 0) f = XfwfChiseled;
    else if (strcmp("ledged", params[0]) == 0) f = XfwfLedged;
    else XtWarning("Unknown frame type in set_shadow action");

    if (((XfwfFrameWidget)self)->xfwfFrame.frameType != f) {
        ((XfwfFrameWidget)self)->xfwfFrame.frameType = f;
        xfwfCommonClassRec.xfwfCommon_class.compute_inside(self, &x, &y, &w, &h);
	w -= 2*((XfwfFrameWidget)self)->xfwfFrame.outerOffset;
	h -= 2*((XfwfFrameWidget)self)->xfwfFrame.outerOffset;
        XfwfDrawFrame(self, x + ((XfwfFrameWidget)self)->xfwfFrame.outerOffset, y + ((XfwfFrameWidget)self)->xfwfFrame.outerOffset,
                      max(w, 0), max(h, 0),
                      ((XfwfFrameWidget)self)->xfwfFrame.frameType, ((XfwfFrameWidget)self)->xfwfFrame.frameWidth, ((XfwfFrameWidget)self)->xfwfFrame.lightgc, ((XfwfFrameWidget)self)->xfwfFrame.darkgc);
    }
}

static void _resolve_inheritance(class)
WidgetClass class;
{
  XfwfFrameWidgetClass c = (XfwfFrameWidgetClass) class;
  XfwfFrameWidgetClass super;
  static CompositeClassExtensionRec extension_rec = {
    NULL, NULLQUARK, XtCompositeExtensionVersion,
    sizeof(CompositeClassExtensionRec), True};
  CompositeClassExtensionRec *ext;
  ext = (XtPointer)XtMalloc(sizeof(*ext));
  *ext = extension_rec;
  ext->next_extension = c->composite_class.extension;
  c->composite_class.extension = ext;
  if (class == xfwfFrameWidgetClass) return;
  super = (XfwfFrameWidgetClass)class->core_class.superclass;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void class_initialize(void)
#else
static void class_initialize()
#endif
{
    static XtConvertArgRec screenArg[] = {
    {XtBaseOffset, (XtPointer)XtOffset(Widget, core.screen), sizeof(Screen*)}};

    XtSetTypeConverter(XtRString, XtRFrameType, cvtStringToFrameType,
                       NULL, 0, XtCacheNone, NULL);
    XtSetTypeConverter(XtRFrameType, XtRString, cvtFrameTypeToString,
                       NULL, 0, XtCacheNone, NULL);

    XtAddConverter(XtRString, XtRBitmap, XmuCvtStringToBitmap,
                       screenArg, XtNumber(screenArg));

    XtSetTypeConverter(XtRString, XtRShadowScheme, cvtStringToShadowScheme,
                       NULL, 0, XtCacheNone, NULL);
    XtSetTypeConverter(XtRShadowScheme, XtRString, cvtShadowSchemeToString,
                       NULL, 0, XtCacheNone, NULL);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void initialize(Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
static void initialize(request,self,args,num_args)Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
{
    Dimension frame;

    ((XfwfFrameWidget)self)->xfwfFrame.lightgc = NULL;
    ((XfwfFrameWidget)self)->xfwfFrame.darkgc = NULL;
    ((XfwfFrameWidget)self)->xfwfFrame.old_frame_type = ((XfwfFrameWidget)self)->xfwfFrame.frameType;
    /* Make sure the width and height are at least as large as the frame */
    frame = ((XfwfFrameWidgetClass)self->core.widget_class)->xfwfCommon_class.total_frame_width(self);
    if (((XfwfFrameWidget)self)->core.width < 2 * frame) ((XfwfFrameWidget)self)->core.width = 2 * frame;
    if (((XfwfFrameWidget)self)->core.height < 2 * frame) ((XfwfFrameWidget)self)->core.height = 2 * frame;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void realize(Widget self,XtValueMask * mask,XSetWindowAttributes * attributes)
#else
static void realize(self,mask,attributes)Widget self;XtValueMask * mask;XSetWindowAttributes * attributes;
#endif
{
    *mask |= CWCursor;
    attributes->cursor = ((XfwfFrameWidget)self)->xfwfFrame.cursor;
    xfwfCommonClassRec.core_class.realize(self, mask, attributes);

    ((XfwfFrameWidget)self)->xfwfFrame.gray = (Pixmap)NULL;
    ((XfwfFrameWidget)self)->xfwfFrame.lightGray = (Pixmap)NULL;
    ((XfwfFrameWidget)self)->xfwfFrame.darkGray = (Pixmap)NULL;

    create_lightgc(self);
    create_darkgc(self);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void destroy(Widget self)
#else
static void destroy(self)Widget self;
#endif
{
  if (((XfwfFrameWidget)self)->xfwfFrame.darkgc) XtReleaseGC(self, ((XfwfFrameWidget)self)->xfwfFrame.darkgc); ((XfwfFrameWidget)self)->xfwfFrame.darkgc = NULL;
  if (((XfwfFrameWidget)self)->xfwfFrame.lightgc) XtReleaseGC(self, ((XfwfFrameWidget)self)->xfwfFrame.lightgc); ((XfwfFrameWidget)self)->xfwfFrame.lightgc = NULL;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static Boolean  set_values(Widget  old,Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
static Boolean  set_values(old,request,self,args,num_args)Widget  old;Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
{
    Boolean need_redisplay = False;

    if (((XfwfFrameWidget)self)->xfwfFrame.cursor != ((XfwfFrameWidget)old)->xfwfFrame.cursor && XtIsRealized(self))
        XDefineCursor(XtDisplay(self), XtWindow(self), ((XfwfFrameWidget)self)->xfwfFrame.cursor);

    if (((XfwfFrameWidget)self)->xfwfFrame.frameType == XfwfChiseled || ((XfwfFrameWidget)self)->xfwfFrame.frameType == XfwfLedged)
        ((XfwfFrameWidget)self)->xfwfFrame.frameWidth = 2 * ((int) (((XfwfFrameWidget)self)->xfwfFrame.frameWidth / 2));

    if (((XfwfFrameWidget)self)->xfwfFrame.shadowScheme     != ((XfwfFrameWidget)old)->xfwfFrame.shadowScheme
    ||  ((XfwfFrameWidget)self)->core.background_pixel != ((XfwfFrameWidget)old)->core.background_pixel) {
        create_darkgc(self);
        create_lightgc(self);
        need_redisplay = True;
    } else if (((XfwfFrameWidget)self)->xfwfFrame.shadowScheme == XfwfColor) {
        if (((XfwfFrameWidget)self)->xfwfFrame.topShadowColor != ((XfwfFrameWidget)old)->xfwfFrame.topShadowColor) {
            create_lightgc(self);
            need_redisplay = True;
        }
        if (((XfwfFrameWidget)self)->xfwfFrame.bottomShadowColor != ((XfwfFrameWidget)old)->xfwfFrame.bottomShadowColor) {
            create_darkgc(self);
            need_redisplay = True;
        }
    } else if (((XfwfFrameWidget)self)->xfwfFrame.shadowScheme == XfwfStipple) {
        if (((XfwfFrameWidget)self)->xfwfFrame.topShadowStipple != ((XfwfFrameWidget)old)->xfwfFrame.topShadowStipple) {
            create_lightgc(self);
            need_redisplay = True;
        }
        if (((XfwfFrameWidget)self)->xfwfFrame.bottomShadowStipple != ((XfwfFrameWidget)old)->xfwfFrame.bottomShadowStipple) {
            create_darkgc(self);
            need_redisplay = True;
        }
    }

    if (((XfwfFrameWidget)self)->xfwfFrame.outerOffset != ((XfwfFrameWidget)old)->xfwfFrame.outerOffset)
        need_redisplay = True;

    if (((XfwfFrameWidget)self)->xfwfFrame.innerOffset != ((XfwfFrameWidget)old)->xfwfFrame.innerOffset)
        need_redisplay = True;

    if (((XfwfFrameWidget)self)->xfwfFrame.frameType != ((XfwfFrameWidget)old)->xfwfFrame.frameType) {
        ((XfwfFrameWidget)self)->xfwfFrame.old_frame_type = ((XfwfFrameWidget)self)->xfwfFrame.frameType;
        need_redisplay = True;
    }

    if (((XfwfFrameWidget)self)->xfwfFrame.frameWidth != ((XfwfFrameWidget)old)->xfwfFrame.frameWidth)
        need_redisplay = True;
    else if (((XfwfFrameWidget)self)->xfwfFrame.frameWidth == 0)
        need_redisplay = False;

    return need_redisplay;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void _expose(Widget self,XEvent * event,Region  region)
#else
static void _expose(self,event,region)Widget self;XEvent * event;Region  region;
#endif
{
    Position x, y;
    int w, h;

    if (! XtIsRealized(self)) return;
    if (region != NULL) {
        XSetRegion(XtDisplay(self), ((XfwfFrameWidget)self)->xfwfFrame.lightgc, region);
        XSetRegion(XtDisplay(self), ((XfwfFrameWidget)self)->xfwfFrame.darkgc, region);
    }
    ((XfwfFrameWidgetClass)self->core.widget_class)->xfwfCommon_class.compute_inside(self, &x, &y, &w, &h);
    w += 2*((XfwfFrameWidget)self)->xfwfFrame.innerOffset + 2*((XfwfFrameWidget)self)->xfwfFrame.frameWidth;
    h += 2*((XfwfFrameWidget)self)->xfwfFrame.innerOffset + 2*((XfwfFrameWidget)self)->xfwfFrame.frameWidth;
    XfwfDrawFrame(self, 
		  x - ((XfwfFrameWidget)self)->xfwfFrame.frameWidth - ((XfwfFrameWidget)self)->xfwfFrame.innerOffset,
		  y - ((XfwfFrameWidget)self)->xfwfFrame.frameWidth - ((XfwfFrameWidget)self)->xfwfFrame.innerOffset,
		  max(w, 0),
		  max(h, 0),
		  ((XfwfFrameWidget)self)->xfwfFrame.frameType, ((XfwfFrameWidget)self)->xfwfFrame.frameWidth, ((XfwfFrameWidget)self)->xfwfFrame.lightgc, ((XfwfFrameWidget)self)->xfwfFrame.darkgc);
    if (region != NULL) {
        XSetClipMask(XtDisplay(self), ((XfwfFrameWidget)self)->xfwfFrame.lightgc, None);
        XSetClipMask(XtDisplay(self), ((XfwfFrameWidget)self)->xfwfFrame.darkgc, None);
    }
    xfwfCommonClassRec.xfwfCommon_class._expose(self, event, region);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void compute_inside(Widget self,Position * x,Position * y,int * w,int * h)
#else
static void compute_inside(self,x,y,w,h)Widget self;Position * x;Position * y;int * w;int * h;
#endif
{
    xfwfCommonClassRec.xfwfCommon_class.compute_inside(self, x, y, w, h);
    *x += ((XfwfFrameWidget)self)->xfwfFrame.outerOffset + ((XfwfFrameWidget)self)->xfwfFrame.frameWidth + ((XfwfFrameWidget)self)->xfwfFrame.innerOffset;
    *y += ((XfwfFrameWidget)self)->xfwfFrame.outerOffset + ((XfwfFrameWidget)self)->xfwfFrame.frameWidth + ((XfwfFrameWidget)self)->xfwfFrame.innerOffset;
    *w -= 2 * (((XfwfFrameWidget)self)->xfwfFrame.outerOffset + ((XfwfFrameWidget)self)->xfwfFrame.frameWidth + ((XfwfFrameWidget)self)->xfwfFrame.innerOffset);
    *h -= 2 * (((XfwfFrameWidget)self)->xfwfFrame.outerOffset + ((XfwfFrameWidget)self)->xfwfFrame.frameWidth + ((XfwfFrameWidget)self)->xfwfFrame.innerOffset);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static Dimension  total_frame_width(Widget self)
#else
static Dimension  total_frame_width(self)Widget self;
#endif
{
    return xfwfCommonClassRec.xfwfCommon_class.total_frame_width(self) + ((XfwfFrameWidget)self)->xfwfFrame.outerOffset + ((XfwfFrameWidget)self)->xfwfFrame.frameWidth + ((XfwfFrameWidget)self)->xfwfFrame.innerOffset ;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static XtGeometryResult  query_geometry(Widget self,XtWidgetGeometry * request,XtWidgetGeometry * reply)
#else
static XtGeometryResult  query_geometry(self,request,reply)Widget self;XtWidgetGeometry * request;XtWidgetGeometry * reply;
#endif
{
    XtWidgetGeometry request2, reply2;
    XtGeometryResult result;
    Dimension h;

    if (((XfwfFrameWidget)self)->composite.num_children == 0) return XtGeometryYes;

    /* We're only interested in size and stacking order */
    reply->request_mode =
        (CWWidth | CWHeight | CWStackMode) & request->request_mode;

    /* If nothing of interest is left, we can return immediately */
    if (reply->request_mode == 0)
        return XtGeometryYes;

    /* Prepare a request to the child */
    h = 2 * (((XfwfFrameWidget)self)->xfwfFrame.outerOffset + ((XfwfFrameWidget)self)->xfwfFrame.frameWidth + ((XfwfFrameWidget)self)->xfwfFrame.innerOffset);
    request2.request_mode = reply->request_mode;
    request2.width = request->width - h;
    request2.height = request->height - h;
    request2.sibling = request->sibling;
    request2.stack_mode = request->stack_mode;

    result = XtQueryGeometry(((XfwfFrameWidget)self)->composite.children[0], &request2, &reply2);

    /* If the child accepted its proposal, we accept ours */
    if (result == XtGeometryYes) return XtGeometryYes;

    /* If the child doesn't want any change, we don't want any, either */
    if (result == XtGeometryNo) return XtGeometryNo;

    /* Otherwise, ignore everything but size and stacking order */
    reply->request_mode &= reply2.request_mode;
    if (reply->request_mode == 0) return XtGeometryYes;

    reply->width = reply2.width + h;
    reply->height = reply2.height + h;
    reply->sibling = reply2.sibling;
    reply->stack_mode = reply2.stack_mode;
    return XtGeometryAlmost;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static XtGeometryResult  geometry_manager(Widget  child,XtWidgetGeometry * request,XtWidgetGeometry * reply)
#else
static XtGeometryResult  geometry_manager(child,request,reply)Widget  child;XtWidgetGeometry * request;XtWidgetGeometry * reply;
#endif
{ Widget self = XtParent(child); {
    XtWidgetGeometry request2, reply2;
    XtGeometryResult result;
    Position x, y;
    int w, h, extraw, extrah;

    ((XfwfFrameWidgetClass)self->core.widget_class)->xfwfCommon_class.compute_inside(self, &x, &y, &w, &h);
    if (! (request->request_mode & (CWWidth|CWHeight))) return XtGeometryYes;
    extraw = ((XfwfFrameWidget)self)->core.width - w;
    extrah = ((XfwfFrameWidget)self)->core.height - h;
    request2.request_mode = request->request_mode & (CWWidth|CWHeight);
    request2.width = request->width + extraw;
    request2.height = request->height + extrah;
    result = XtMakeGeometryRequest(self, &request2, &reply2);
    if (result == XtGeometryNo) return XtGeometryNo;
    if (result == XtGeometryYes) return XtGeometryYes;
    reply->request_mode = reply2.request_mode & (CWWidth|CWHeight);
    reply->width = reply2.width - extraw;
    reply->height = reply2.height - extrah;
    return XtGeometryAlmost;
}
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void resize(Widget self)
#else
static void resize(self)Widget self;
#endif
{
    Position x, y;
    int w, h;
    Widget child;

    if (((XfwfFrameWidget)self)->composite.num_children == 0) return;
    ((XfwfFrameWidgetClass)self->core.widget_class)->xfwfCommon_class.compute_inside(self, &x, &y, &w, &h);
    child = ((XfwfFrameWidget)self)->composite.children[0];
    w -= 2 * ((XfwfFrameWidget)child)->core.border_width;
    h -= 2 * ((XfwfFrameWidget)child)->core.border_width;
    XtConfigureWidget(child, x, y, max(1, w), max(1, h), ((XfwfFrameWidget)child)->core.border_width);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
static void change_managed(Widget self)
#else
static void change_managed(self)Widget self;
#endif
{
    XtWidgetGeometry request2, reply2;
    XtGeometryResult result;
    Widget child;
    Position x, y;
    int w, h;

    if (((XfwfFrameWidget)self)->composite.num_children == 0) return;
    ((XfwfFrameWidgetClass)self->core.widget_class)->xfwfCommon_class.compute_inside(self, &x, &y, &w, &h);
    child = ((XfwfFrameWidget)self)->composite.children[0];
    request2.request_mode = CWWidth | CWHeight;
    request2.width = ((XfwfFrameWidget)child)->core.width + ((XfwfFrameWidget)self)->core.width - w;
    request2.height = ((XfwfFrameWidget)child)->core.height + ((XfwfFrameWidget)self)->core.height - h;
    result = XtMakeGeometryRequest(self, &request2, &reply2);
    ((XfwfFrameWidgetClass)self->core.widget_class)->xfwfCommon_class.compute_inside(self, &x, &y, &w, &h);
    w -= 2 * ((XfwfFrameWidget)child)->core.border_width;
    h -= 2 * ((XfwfFrameWidget)child)->core.border_width;
    XtConfigureWidget(child, x, y, max(1, w), max(1, h), ((XfwfFrameWidget)child)->core.border_width);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
void XfwfDrawFrame(Widget self,int  x,int  y,int  w,int  h,FrameType  tp,int  t,GC  lightgc,GC  darkgc)
#else
void XfwfDrawFrame(self,x,y,w,h,tp,t,lightgc,darkgc)Widget self;int  x;int  y;int  w;int  h;FrameType  tp;int  t;GC  lightgc;GC  darkgc;
#endif
{
    XPoint tlPoints[7], brPoints[7];

    if (t == 0) return;
    switch (tp) {
    case XfwfPlain:
	XDrawRectangle(XtDisplay(self), XtWindow(self), darkgc, x+1, y+1, w-1, h-1);
	break;
    case XfwfRaised:
    case XfwfSunken:
        tlPoints[0].x = x;              tlPoints[0].y = y;
        tlPoints[1].x = x + w;          tlPoints[1].y = y;
        tlPoints[2].x = x + w - t;      tlPoints[2].y = y + t;
        tlPoints[3].x = x + t;          tlPoints[3].y = y + t;
        tlPoints[4].x = x + t;          tlPoints[4].y = y + h - t;
        tlPoints[5].x = x;              tlPoints[5].y = y + h;
        tlPoints[6].x = x;              tlPoints[6].y = y;
        brPoints[0].x = x + w;          brPoints[0].y = y + h;
        brPoints[1].x = x;              brPoints[1].y = y + h;
        brPoints[2].x = x + t;          brPoints[2].y = y + h - t;
        brPoints[3].x = x + w - t;      brPoints[3].y = y + h - t;
        brPoints[4].x = x + w - t;      brPoints[4].y = y + t;
        brPoints[5].x = x + w;          brPoints[5].y = y;
        brPoints[6].x = x + w;          brPoints[6].y = y + h;
        if (tp == XfwfSunken) {
            XFillPolygon(XtDisplay(self), XtWindow(self),
                         darkgc, tlPoints, 7, Nonconvex, CoordModeOrigin);
            XFillPolygon(XtDisplay(self), XtWindow(self),
                         lightgc, brPoints, 7, Nonconvex, CoordModeOrigin);
        } else {
            XFillPolygon(XtDisplay(self), XtWindow(self),
                         lightgc, tlPoints, 7, Nonconvex, CoordModeOrigin);
            XFillPolygon(XtDisplay(self), XtWindow(self),
                         darkgc, brPoints, 7, Nonconvex, CoordModeOrigin);
        }
        break;
    case XfwfLedged:
        XfwfDrawFrame(self, x, y, w, h, XfwfRaised, t/2, lightgc, darkgc);
        XfwfDrawFrame(self, x+t/2, y+t/2, w-2*(int)(t/2), h-2*(int)(t/2),
                  XfwfSunken, t/2, lightgc, darkgc);
        break;
    case XfwfChiseled:
        XfwfDrawFrame(self, x, y, w, h, XfwfSunken, t/2, lightgc, darkgc);
        XfwfDrawFrame(self, x+t/2, y+t/2, w-2*(int)(t/2), h-2*(int)(t/2),
                  XfwfRaised, t/2, lightgc, darkgc);
        break;
    }

}
/*ARGSUSED*/
#if NeedFunctionPrototypes
Boolean  cvtStringToFrameType(Display * display,XrmValuePtr  args,Cardinal * num_args,XrmValuePtr  from,XrmValuePtr  to,XtPointer * converter_data)
#else
Boolean  cvtStringToFrameType(display,args,num_args,from,to,converter_data)Display * display;XrmValuePtr  args;Cardinal * num_args;XrmValuePtr  from;XrmValuePtr  to;XtPointer * converter_data;
#endif
{
    String s = (String) from->addr;

    if (*num_args != 0)
        XtAppErrorMsg(XtDisplayToApplicationContext(display),
                      "cvtStringToFrameType", "wrongParameters",
                      "XtToolkitError",
                      "String to frame type conversion needs no arguments",
                      (String*) NULL, (Cardinal*) NULL);

    if (XmuCompareISOLatin1(s, "raised") == 0) done(FrameType, XfwfRaised);
    if (XmuCompareISOLatin1(s, "sunken") == 0) done(FrameType, XfwfSunken);
    if (XmuCompareISOLatin1(s, "chiseled") == 0) done(FrameType, XfwfChiseled);
    if (XmuCompareISOLatin1(s, "ledged") == 0) done(FrameType, XfwfLedged);
    XtDisplayStringConversionWarning(display, s, XtRFrameType);
    done(FrameType, XfwfRaised);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
Boolean  cvtFrameTypeToString(Display * display,XrmValuePtr  args,Cardinal * num_args,XrmValuePtr  from,XrmValuePtr  to,XtPointer * converter_data)
#else
Boolean  cvtFrameTypeToString(display,args,num_args,from,to,converter_data)Display * display;XrmValuePtr  args;Cardinal * num_args;XrmValuePtr  from;XrmValuePtr  to;XtPointer * converter_data;
#endif
{
    if (*num_args != 0)
        XtAppErrorMsg(XtDisplayToApplicationContext(display),
                      "cvtFrameTypeToString", "wrongParameters",
                      "XtToolkitError",
                      "Fframe type to String conversion needs no arguments",
                      (String*) NULL, (Cardinal*) NULL);
    switch (*(FrameType*)from->addr) {
    case XfwfRaised: done(String, "raised");
    case XfwfSunken: done(String, "sunken");
    case XfwfChiseled: done(String, "chiseled");
    case XfwfLedged: done(String, "ledged");
    default: XtError("Illegal FrameType");
    }
    return FALSE;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
Boolean  cvtStringToShadowScheme(Display * display,XrmValuePtr  args,Cardinal * num_args,XrmValuePtr  from,XrmValuePtr  to,XtPointer * converter_data)
#else
Boolean  cvtStringToShadowScheme(display,args,num_args,from,to,converter_data)Display * display;XrmValuePtr  args;Cardinal * num_args;XrmValuePtr  from;XrmValuePtr  to;XtPointer * converter_data;
#endif
{
    String s = (String) from->addr;

    if (*num_args != 0)
        XtAppErrorMsg(XtDisplayToApplicationContext(display),
                      "cvtStringToShadowScheme", "wrongParameters",
                      "XtToolkitError",
                      "String to shadow scheme conversion needs no arguments",
                      (String*) NULL, (Cardinal*) NULL);

    if (XmuCompareISOLatin1(s, "auto")==0) done(ShadowScheme, XfwfAuto);
    if (XmuCompareISOLatin1(s, "color")==0) done(ShadowScheme, XfwfColor);
    if (XmuCompareISOLatin1(s, "stipple")==0) done(ShadowScheme, XfwfStipple);
    XtDisplayStringConversionWarning(display, s, XtRShadowScheme);
    done(ShadowScheme, XfwfAuto);
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
Boolean  cvtShadowSchemeToString(Display * display,XrmValuePtr  args,Cardinal * num_args,XrmValuePtr  from,XrmValuePtr  to,XtPointer * converter_data)
#else
Boolean  cvtShadowSchemeToString(display,args,num_args,from,to,converter_data)Display * display;XrmValuePtr  args;Cardinal * num_args;XrmValuePtr  from;XrmValuePtr  to;XtPointer * converter_data;
#endif
{
    if (*num_args != 0)
        XtAppErrorMsg(XtDisplayToApplicationContext(display),
                      "cvtShadowSchemeToString", "wrongParameters",
                      "XtToolkitError",
                      "Shadow scheme to String conversion needs no arguments",
                      (String*) NULL, (Cardinal*) NULL);

    switch (*(ShadowScheme*)from->addr) {
    case XfwfAuto: done(String, "auto");
    case XfwfColor: done(String, "color");
    case XfwfStipple: done(String, "stipple");
    case XfwfPlain: done(String, "plain");
    default: XtError("Illegal ShadowScheme");
    }
    return FALSE;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
Pixmap  GetGray(Widget self)
#else
Pixmap  GetGray(self)Widget self;
#endif
{
  if (!((XfwfFrameWidget)self)->xfwfFrame.gray) {
    ((XfwfFrameWidget)self)->xfwfFrame.gray = XCreateBitmapFromData(XtDisplay(self), XtWindow(self),
        gray_bits, gray_width, gray_height);
  }
  return ((XfwfFrameWidget)self)->xfwfFrame.gray;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
Pixmap  GetLightGray(Widget self)
#else
Pixmap  GetLightGray(self)Widget self;
#endif
{
  if (!((XfwfFrameWidget)self)->xfwfFrame.lightGray) {
    static char light_bits[] = { 0x02, 0x04, 0x01};

    ((XfwfFrameWidget)self)->xfwfFrame.lightGray = XCreateBitmapFromData(XtDisplay(self), XtWindow(self),
        light_bits, 3, 3);
  }
  return ((XfwfFrameWidget)self)->xfwfFrame.lightGray;
}
/*ARGSUSED*/
#if NeedFunctionPrototypes
Pixmap  GetDarkGray(Widget self)
#else
Pixmap  GetDarkGray(self)Widget self;
#endif
{
  if (!((XfwfFrameWidget)self)->xfwfFrame.darkGray) {
    static char dark_bits[] = { 0x05, 0x03, 0x06};

    ((XfwfFrameWidget)self)->xfwfFrame.darkGray = XCreateBitmapFromData(XtDisplay(self), XtWindow(self),
        dark_bits, 3, 3);
  }
  return ((XfwfFrameWidget)self)->xfwfFrame.darkGray;
}
