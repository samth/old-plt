/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_glob.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_utils.h"
#include "wx_dialg.h"
#include "wx_cmdlg.h"
#include "wx_timer.h"
#include "wx_dcps.h"
#include "wx_main.h"

#if USE_METAFILE
#include "wx_mf.h"
#endif




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_glob.h"

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

static void wxsFillPrivateColor(wxDC *dc, wxColour *c)
{
#ifdef wx_x
 ((wxWindowDC *)dc)->FillPrivateColor(c);
#endif
}

static Scheme_Object *fileSelMode_wxOPEN_sym = NULL;
static Scheme_Object *fileSelMode_wxSAVE_sym = NULL;
static Scheme_Object *fileSelMode_wxMULTIOPEN_sym = NULL;
static Scheme_Object *fileSelMode_wxOVERWRITE_PROMPT_sym = NULL;
static Scheme_Object *fileSelMode_wxHIDE_READONLY_sym = NULL;

static void init_symset_fileSelMode(void) {
  REMEMBER_VAR_STACK();
  wxREGGLOB(fileSelMode_wxOPEN_sym);
  fileSelMode_wxOPEN_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("get"));
  wxREGGLOB(fileSelMode_wxSAVE_sym);
  fileSelMode_wxSAVE_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("put"));
  wxREGGLOB(fileSelMode_wxMULTIOPEN_sym);
  fileSelMode_wxMULTIOPEN_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("multi"));
  wxREGGLOB(fileSelMode_wxOVERWRITE_PROMPT_sym);
  fileSelMode_wxOVERWRITE_PROMPT_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("overwrite-prompt"));
  wxREGGLOB(fileSelMode_wxHIDE_READONLY_sym);
  fileSelMode_wxHIDE_READONLY_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("hide-readonly"));
}

static int unbundle_symset_fileSelMode(Scheme_Object *v, const char *where) {
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, v);
  if (!fileSelMode_wxHIDE_READONLY_sym) WITH_VAR_STACK(init_symset_fileSelMode());
  if (0) { }
  else if (v == fileSelMode_wxOPEN_sym) { return wxOPEN; }
  else if (v == fileSelMode_wxSAVE_sym) { return wxSAVE; }
  else if (v == fileSelMode_wxMULTIOPEN_sym) { return wxMULTIOPEN; }
  else if (v == fileSelMode_wxOVERWRITE_PROMPT_sym) { return wxOVERWRITE_PROMPT; }
  else if (v == fileSelMode_wxHIDE_READONLY_sym) { return wxHIDE_READONLY; }
  if (where) WITH_VAR_STACK(scheme_wrong_type(where, "fileSelMode symbol", -1, 0, &v));
  return 0;
}


#define USE_PRINTER 1

extern Bool wxSchemeYield(void *sema);

extern void wxFlushDisplay(void);

#ifdef wx_x
# define FILE_SEL_DEF_PATTERN "*"
#else
# define FILE_SEL_DEF_PATTERN "*.*"
#endif

static char *wxStripMenuCodes_Scheme(char *in)
{
  static char *buffer = NULL;
  static long buflen = 0;
  long len;
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, in);

  len = strlen(in);
  if (buflen <= len) {
    if (!buffer)
      wxREGGLOB(buffer);
    buflen = 2 * len + 1;
    buffer = (char *)WITH_VAR_STACK(GC_malloc_atomic(buflen));
  }

  WITH_VAR_STACK(wxStripMenuCodes(in, buffer));
  return buffer;
}

#ifdef wx_xt
extern void wxBell(void);
#endif


extern int objscheme_istype_wxFrame(Scheme_Object *obj, const char *stop, int nullOK);
extern class wxFrame *objscheme_unbundle_wxFrame(Scheme_Object *obj, const char *where, int nullOK);
extern int objscheme_istype_wxDialogBox(Scheme_Object *obj, const char *stop, int nullOK);
extern class wxDialogBox *objscheme_unbundle_wxDialogBox(Scheme_Object *obj, const char *where, int nullOK);




#if !USE_METAFILE
#define wxMakeMetaFilePlaceable(a,b,c,d,e,f) TRUE
#endif








static Scheme_Object *wxsGlobalwxsFillPrivateColor(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  class wxDC* x0;
  class wxColour* x1;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);
  VAR_STACK_PUSH(2, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxDC(p[0], "fill-private-color", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxColour(p[1], "fill-private-color", 0));

  
  WITH_VAR_STACK(wxsFillPrivateColor(x0, x1));

  
  
  return scheme_void;
}

static Scheme_Object *wxsGlobalwxFlushDisplay(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  WITH_VAR_STACK(wxFlushDisplay());

  
  
  return scheme_void;
}

static Scheme_Object *wxsGlobalwxSchemeYield(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  void* x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);

  
  if (n > 0) {
    x0 = (void *)p[0];
  } else
    x0 = NULL;

  
  WITH_VAR_STACK(wxSchemeYield(x0));

  
  
  return scheme_void;
}

static Scheme_Object *wxsGlobalwxWriteResource(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  SETUP_PRE_VAR_STACK(1);
  PRE_VAR_STACK_PUSH(0, p);
  REMEMBER_VAR_STACK();
  Bool r;
  if ((n >= 3) && WITH_REMEMBERED_STACK(objscheme_istype_string(p[0], NULL)) && WITH_REMEMBERED_STACK(objscheme_istype_string(p[1], NULL)) && WITH_REMEMBERED_STACK(objscheme_istype_string(p[2], NULL))) {
    string x0;
    string x1;
    string x2;
    npathname x3;

    SETUP_VAR_STACK_PRE_REMEMBERED(5);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, x0);
    VAR_STACK_PUSH(2, x1);
    VAR_STACK_PUSH(3, x2);
    VAR_STACK_PUSH(4, x3);

    
    if ((n < 3) ||(n > 4)) 
      WITH_VAR_STACK(scheme_wrong_count("write-resource (string case)", 3, 4, n, p));
    x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "write-resource (string case)"));
    x1 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[1], "write-resource (string case)"));
    x2 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[2], "write-resource (string case)"));
    if (n > 3) {
      x3 = (npathname)WITH_VAR_STACK(objscheme_unbundle_nullable_pathname(p[3], "write-resource (string case)"));
    } else
      x3 = NULL;

    
    r = WITH_VAR_STACK(wxWriteResource(x0, x1, x2, x3));

    
    
  } else  {
    string x0;
    string x1;
    ExactLong x2;
    npathname x3;

    SETUP_VAR_STACK_PRE_REMEMBERED(4);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, x0);
    VAR_STACK_PUSH(2, x1);
    VAR_STACK_PUSH(3, x3);

    
    if ((n < 3) ||(n > 4)) 
      WITH_VAR_STACK(scheme_wrong_count("write-resource (number case)", 3, 4, n, p));
    x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "write-resource (number case)"));
    x1 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[1], "write-resource (number case)"));
    x2 = WITH_VAR_STACK(objscheme_unbundle_ExactLong(p[2], "write-resource (number case)"));
    if (n > 3) {
      x3 = (npathname)WITH_VAR_STACK(objscheme_unbundle_nullable_pathname(p[3], "write-resource (number case)"));
    } else
      x3 = NULL;

    
    r = WITH_VAR_STACK(wxWriteResource(x0, x1, x2, x3));

    
    
  }

  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *wxsGlobalwxGetResource(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  SETUP_PRE_VAR_STACK(1);
  PRE_VAR_STACK_PUSH(0, p);
  REMEMBER_VAR_STACK();
  Bool r;
  if ((n >= 3) && WITH_REMEMBERED_STACK(objscheme_istype_string(p[0], NULL)) && WITH_REMEMBERED_STACK(objscheme_istype_string(p[1], NULL)) && (WITH_REMEMBERED_STACK(objscheme_istype_box(p[2], NULL)) && WITH_REMEMBERED_STACK(objscheme_istype_string(objscheme_unbox(p[2], NULL), NULL)))) {
    string x0;
    string x1;
    string _x2;
    string* x2 = &_x2;
    npathname x3;
  Scheme_Object *sbox_tmp;

    SETUP_VAR_STACK_PRE_REMEMBERED(4);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, x0);
    VAR_STACK_PUSH(2, x1);
    VAR_STACK_PUSH(3, x3);

    
    if ((n < 3) ||(n > 4)) 
      WITH_VAR_STACK(scheme_wrong_count("get-resource (string case)", 3, 4, n, p));
    x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "get-resource (string case)"));
    x1 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[1], "get-resource (string case)"));
          *x2 = (sbox_tmp = WITH_VAR_STACK(objscheme_unbox(p[2], "get-resource (string case)")), (string)WITH_VAR_STACK(objscheme_unbundle_string(sbox_tmp, "get-resource (string case)"", extracting boxed argument")));
    if (n > 3) {
      x3 = (npathname)WITH_VAR_STACK(objscheme_unbundle_nullable_pathname(p[3], "get-resource (string case)"));
    } else
      x3 = NULL;

    
    r = WITH_VAR_STACK(wxGetResource(x0, x1, x2, x3));

    
    if (n > 2)
      WITH_VAR_STACK(objscheme_set_box(p[2], WITH_VAR_STACK(objscheme_bundle_string((char *)_x2))));
    
  } else  {
    string x0;
    string x1;
    long _x2;
    long* x2 = &_x2;
    npathname x3;
  Scheme_Object *sbox_tmp;

    SETUP_VAR_STACK_PRE_REMEMBERED(4);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, x0);
    VAR_STACK_PUSH(2, x1);
    VAR_STACK_PUSH(3, x3);

    
    if ((n < 3) ||(n > 4)) 
      WITH_VAR_STACK(scheme_wrong_count("get-resource (number case)", 3, 4, n, p));
    x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "get-resource (number case)"));
    x1 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[1], "get-resource (number case)"));
          *x2 = (sbox_tmp = WITH_VAR_STACK(objscheme_unbox(p[2], "get-resource (number case)")), WITH_VAR_STACK(objscheme_unbundle_integer(sbox_tmp, "get-resource (number case)"", extracting boxed argument")));
    if (n > 3) {
      x3 = (npathname)WITH_VAR_STACK(objscheme_unbundle_nullable_pathname(p[3], "get-resource (number case)"));
    } else
      x3 = NULL;

    
    r = WITH_VAR_STACK(wxGetResource(x0, x1, x2, x3));

    
    if (n > 2)
      WITH_VAR_STACK(objscheme_set_box(p[2], scheme_make_integer(_x2)));
    
  }

  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *wxsGlobalwxStripMenuCodes_Scheme(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  string r;
  string x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);

  
  x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "label->plain-label"));

  
  r = WITH_VAR_STACK(wxStripMenuCodes_Scheme(x0));

  
  
  return WITH_REMEMBERED_STACK(objscheme_bundle_string((char *)r));
}

static Scheme_Object *wxsGlobalwxDisplaySize(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;
  Scheme_Object *sbox_tmp;

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  
      *x0 = (sbox_tmp = WITH_VAR_STACK(objscheme_unbox(p[0], "display-size")), WITH_VAR_STACK(objscheme_unbundle_integer(sbox_tmp, "display-size"", extracting boxed argument")));
      *x1 = (sbox_tmp = WITH_VAR_STACK(objscheme_unbox(p[1], "display-size")), WITH_VAR_STACK(objscheme_unbundle_integer(sbox_tmp, "display-size"", extracting boxed argument")));

  
  WITH_VAR_STACK(wxDisplaySize(x0, x1));

  
  if (n > 0)
    WITH_VAR_STACK(objscheme_set_box(p[0], scheme_make_integer(_x0)));
  if (n > 1)
    WITH_VAR_STACK(objscheme_set_box(p[1], scheme_make_integer(_x1)));
  
  return scheme_void;
}

static Scheme_Object *wxsGlobalwxBell(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  WITH_VAR_STACK(wxBell());

  
  
  return scheme_void;
}

static Scheme_Object *wxsGlobalwxEndBusyCursor(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  WITH_VAR_STACK(wxEndBusyCursor());

  
  
  return scheme_void;
}

static Scheme_Object *wxsGlobalwxIsBusy(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  r = WITH_VAR_STACK(wxIsBusy());

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *wxsGlobalwxBeginBusyCursor(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  WITH_VAR_STACK(wxBeginBusyCursor());

  
  
  return scheme_void;
}

static Scheme_Object *wxsGlobalwxMakeMetaFilePlaceable(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  string x0;
  float x1;
  float x2;
  float x3;
  float x4;
  float x5;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);

  
  x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "make-meta-file-placeable"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_float(p[1], "make-meta-file-placeable"));
  x2 = WITH_VAR_STACK(objscheme_unbundle_float(p[2], "make-meta-file-placeable"));
  x3 = WITH_VAR_STACK(objscheme_unbundle_float(p[3], "make-meta-file-placeable"));
  x4 = WITH_VAR_STACK(objscheme_unbundle_float(p[4], "make-meta-file-placeable"));
  x5 = WITH_VAR_STACK(objscheme_unbundle_float(p[5], "make-meta-file-placeable"));

  
  r = WITH_VAR_STACK(wxMakeMetaFilePlaceable(x0, x1, x2, x3, x4, x5));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *wxsGlobalwxDisplayDepth(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  r = WITH_VAR_STACK(wxDisplayDepth());

  
  
  return scheme_make_integer(r);
}

static Scheme_Object *wxsGlobalwxColourDisplay(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  r = WITH_VAR_STACK(wxColourDisplay());

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *wxsGlobalwxFileSelector(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  nstring r;
  nstring x0;
  npathname x1;
  nstring x2;
  nstring x3;
  nstring x4;
  int x5;
  class wxWindow* x6;
  int x7;
  int x8;

  SETUP_VAR_STACK_REMEMBERED(7);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);
  VAR_STACK_PUSH(2, x1);
  VAR_STACK_PUSH(3, x2);
  VAR_STACK_PUSH(4, x3);
  VAR_STACK_PUSH(5, x4);
  VAR_STACK_PUSH(6, x6);

  
  x0 = (nstring)WITH_VAR_STACK(objscheme_unbundle_nullable_string(p[0], "file-selector"));
  if (n > 1) {
    x1 = (npathname)WITH_VAR_STACK(objscheme_unbundle_nullable_pathname(p[1], "file-selector"));
  } else
    x1 = NULL;
  if (n > 2) {
    x2 = (nstring)WITH_VAR_STACK(objscheme_unbundle_nullable_string(p[2], "file-selector"));
  } else
    x2 = NULL;
  if (n > 3) {
    x3 = (nstring)WITH_VAR_STACK(objscheme_unbundle_nullable_string(p[3], "file-selector"));
  } else
    x3 = NULL;
  if (n > 4) {
    x4 = (nstring)WITH_VAR_STACK(objscheme_unbundle_nullable_string(p[4], "file-selector"));
  } else
    x4 = FILE_SEL_DEF_PATTERN;
  if (n > 5) {
    x5 = WITH_VAR_STACK(unbundle_symset_fileSelMode(p[5], "file-selector"));
  } else
    x5 = wxOPEN;
  x6 = (((n <= 6) || XC_SCHEME_NULLP(p[6])) ? (wxWindow *)NULL : (WITH_VAR_STACK(objscheme_istype_wxFrame(p[6], NULL, 1)) ? (wxWindow *)WITH_VAR_STACK(objscheme_unbundle_wxFrame(p[6], NULL, 0)) : (WITH_VAR_STACK(objscheme_istype_wxDialogBox(p[6], NULL, 1)) ? (wxWindow *)WITH_VAR_STACK(objscheme_unbundle_wxDialogBox(p[6], NULL, 0)) : (WITH_VAR_STACK(scheme_wrong_type("file-selector", "frame% or dialog%", -1, 0, &p[6])), (wxWindow *)NULL))));
  if (n > 7) {
    x7 = WITH_VAR_STACK(objscheme_unbundle_integer(p[7], "file-selector"));
  } else
    x7 = -1;
  if (n > 8) {
    x8 = WITH_VAR_STACK(objscheme_unbundle_integer(p[8], "file-selector"));
  } else
    x8 = -1;

  
  r = WITH_VAR_STACK(wxFileSelector(x0, x1, x2, x3, x4, x5, x6, x7, x8));

  
  
  return WITH_REMEMBERED_STACK(objscheme_bundle_string((char *)r));
}

void objscheme_setup_wxsGlobal(void *env)
{
  Scheme_Object *functmp;
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);
  functmp = WITH_VAR_STACK(scheme_make_prim_w_arity(wxsGlobalwxsFillPrivateColor, "fill-private-color", 2, 2));
  WITH_VAR_STACK(scheme_install_xc_global("fill-private-color", functmp, env));
  functmp = WITH_VAR_STACK(scheme_make_prim_w_arity(wxsGlobalwxFlushDisplay, "flush-display", 0, 0));
  WITH_VAR_STACK(scheme_install_xc_global("flush-display", functmp, env));
  functmp = WITH_VAR_STACK(scheme_make_prim_w_arity(wxsGlobalwxSchemeYield, "yield", 0, 1));
  WITH_VAR_STACK(scheme_install_xc_global("yield", functmp, env));
  functmp = WITH_VAR_STACK(scheme_make_prim_w_arity(wxsGlobalwxWriteResource, "write-resource", 3, 4));
  WITH_VAR_STACK(scheme_install_xc_global("write-resource", functmp, env));
  functmp = WITH_VAR_STACK(scheme_make_prim_w_arity(wxsGlobalwxGetResource, "get-resource", 3, 4));
  WITH_VAR_STACK(scheme_install_xc_global("get-resource", functmp, env));
  functmp = WITH_VAR_STACK(scheme_make_prim_w_arity(wxsGlobalwxStripMenuCodes_Scheme, "label->plain-label", 1, 1));
  WITH_VAR_STACK(scheme_install_xc_global("label->plain-label", functmp, env));
  functmp = WITH_VAR_STACK(scheme_make_prim_w_arity(wxsGlobalwxDisplaySize, "display-size", 2, 2));
  WITH_VAR_STACK(scheme_install_xc_global("display-size", functmp, env));
  functmp = WITH_VAR_STACK(scheme_make_prim_w_arity(wxsGlobalwxBell, "bell", 0, 0));
  WITH_VAR_STACK(scheme_install_xc_global("bell", functmp, env));
  functmp = WITH_VAR_STACK(scheme_make_prim_w_arity(wxsGlobalwxEndBusyCursor, "end-busy-cursor", 0, 0));
  WITH_VAR_STACK(scheme_install_xc_global("end-busy-cursor", functmp, env));
  functmp = WITH_VAR_STACK(scheme_make_prim_w_arity(wxsGlobalwxIsBusy, "is-busy?", 0, 0));
  WITH_VAR_STACK(scheme_install_xc_global("is-busy?", functmp, env));
  functmp = WITH_VAR_STACK(scheme_make_prim_w_arity(wxsGlobalwxBeginBusyCursor, "begin-busy-cursor", 0, 0));
  WITH_VAR_STACK(scheme_install_xc_global("begin-busy-cursor", functmp, env));
  functmp = WITH_VAR_STACK(scheme_make_prim_w_arity(wxsGlobalwxMakeMetaFilePlaceable, "make-meta-file-placeable", 6, 6));
  WITH_VAR_STACK(scheme_install_xc_global("make-meta-file-placeable", functmp, env));
  functmp = WITH_VAR_STACK(scheme_make_prim_w_arity(wxsGlobalwxDisplayDepth, "get-display-depth", 0, 0));
  WITH_VAR_STACK(scheme_install_xc_global("get-display-depth", functmp, env));
  functmp = WITH_VAR_STACK(scheme_make_prim_w_arity(wxsGlobalwxColourDisplay, "is-color-display?", 0, 0));
  WITH_VAR_STACK(scheme_install_xc_global("is-color-display?", functmp, env));
  functmp = WITH_VAR_STACK(scheme_make_prim_w_arity(wxsGlobalwxFileSelector, "file-selector", 1, 9));
  WITH_VAR_STACK(scheme_install_xc_global("file-selector", functmp, env));
}

#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif
