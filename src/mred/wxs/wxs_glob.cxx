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




#include "wxscheme.h"
#include "wxs_glob.h"
#include "wxscomon.h"


static Scheme_Object *fileSelMode_wxOPEN_sym = NULL;
static Scheme_Object *fileSelMode_wxSAVE_sym = NULL;
static Scheme_Object *fileSelMode_wxOVERWRITE_PROMPT_sym = NULL;
static Scheme_Object *fileSelMode_wxHIDE_READONLY_sym = NULL;

static void init_symset_fileSelMode(void) {
  fileSelMode_wxOPEN_sym = scheme_intern_symbol("get");
  fileSelMode_wxSAVE_sym = scheme_intern_symbol("put");
  fileSelMode_wxOVERWRITE_PROMPT_sym = scheme_intern_symbol("overwrite-prompt");
  fileSelMode_wxHIDE_READONLY_sym = scheme_intern_symbol("hide-readonly");
}

static int unbundle_symset_fileSelMode(Scheme_Object *v, const char *where) {
  if (!fileSelMode_wxHIDE_READONLY_sym) init_symset_fileSelMode();
  if (0) { }
  else if (v == fileSelMode_wxOPEN_sym) { return wxOPEN; }
  else if (v == fileSelMode_wxSAVE_sym) { return wxSAVE; }
  else if (v == fileSelMode_wxOVERWRITE_PROMPT_sym) { return wxOVERWRITE_PROMPT; }
  else if (v == fileSelMode_wxHIDE_READONLY_sym) { return wxHIDE_READONLY; }
  if (where) scheme_wrong_type(where, "fileSelMode symbol", -1, 0, &v);
  return 0;
}

static int istype_symset_fileSelMode(Scheme_Object *v, const char *where) {
  if (!fileSelMode_wxHIDE_READONLY_sym) init_symset_fileSelMode();
  if (0) { }
  else if (v == fileSelMode_wxOPEN_sym) { return 1; }
  else if (v == fileSelMode_wxSAVE_sym) { return 1; }
  else if (v == fileSelMode_wxOVERWRITE_PROMPT_sym) { return 1; }
  else if (v == fileSelMode_wxHIDE_READONLY_sym) { return 1; }
  if (where) scheme_wrong_type(where, "fileSelMode symbol", -1, 0, &v);
  return 0;
}

static Scheme_Object *bundle_symset_fileSelMode(int v) {
  if (!fileSelMode_wxHIDE_READONLY_sym) init_symset_fileSelMode();
  switch (v) {
  case wxOPEN: return fileSelMode_wxOPEN_sym;
  case wxSAVE: return fileSelMode_wxSAVE_sym;
  case wxOVERWRITE_PROMPT: return fileSelMode_wxOVERWRITE_PROMPT_sym;
  case wxHIDE_READONLY: return fileSelMode_wxHIDE_READONLY_sym;
  default: return NULL;
  }
}


#define USE_PRINTER 1

extern Bool wxSchemeYield(void *sema);

extern void wxFlushDisplay(void);

#ifdef wx_x
#define FILE_SEL_DEF_PATTERN "*"
#else
#define FILE_SEL_DEF_PATTERN "*.*"
#endif

static char *wxStripMenuCodes_Scheme(char *in)
{
  static char *buffer = NULL;
  static long buflen = 0;
  long len;

  len = strlen(in);
  if (buflen <= len) {
    if (buffer)
      delete[] buffer;
    buflen = 2 * len + 1;
    buffer = new char[buflen];
  }

  wxStripMenuCodes(in, buffer);
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







#pragma argsused
static Scheme_Object *wxsGlobalwxFlushDisplay(int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)

  

  
  wxFlushDisplay();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *wxsGlobalwxSchemeYield(int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  void* x0;

  
  if (n > 0) {
    x0 = (void *)p[0];
  } else
    x0 = NULL;

  
  wxSchemeYield(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *wxsGlobalwxWriteResource(int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  if ((n >= 3) && objscheme_istype_string(p[0], NULL) && objscheme_istype_string(p[1], NULL) && objscheme_istype_string(p[2], NULL)) {
    string x0;
    string x1;
    string x2;
    nstring x3;

    
    if ((n < 3) ||(n > 4)) 
      scheme_wrong_count("write-resource (string case)", 3, 4, n, p);
    x0 = (string)objscheme_unbundle_string(p[0], "write-resource (string case)");
    x1 = (string)objscheme_unbundle_string(p[1], "write-resource (string case)");
    x2 = (string)objscheme_unbundle_string(p[2], "write-resource (string case)");
    if (n > 3) {
      x3 = (nstring)objscheme_unbundle_nullable_string(p[3], "write-resource (string case)");
    } else
      x3 = NULL;

    
    r = wxWriteResource(x0, x1, x2, x3);

    
    
  } else  {
    string x0;
    string x1;
    long x2;
    nstring x3;

    
    if ((n < 3) ||(n > 4)) 
      scheme_wrong_count("write-resource (number case)", 3, 4, n, p);
    x0 = (string)objscheme_unbundle_string(p[0], "write-resource (number case)");
    x1 = (string)objscheme_unbundle_string(p[1], "write-resource (number case)");
    x2 = objscheme_unbundle_integer(p[2], "write-resource (number case)");
    if (n > 3) {
      x3 = (nstring)objscheme_unbundle_nullable_string(p[3], "write-resource (number case)");
    } else
      x3 = NULL;

    
    r = wxWriteResource(x0, x1, x2, x3);

    
    
  }

  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *wxsGlobalwxGetResource(int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  if ((n >= 3) && objscheme_istype_string(p[0], NULL) && objscheme_istype_string(p[1], NULL) && (objscheme_istype_box(p[2], NULL) && objscheme_istype_string(objscheme_unbox(p[2], NULL), NULL))) {
    string x0;
    string x1;
    string _x2;
    string* x2 = &_x2;
    nstring x3;

    
    if ((n < 3) ||(n > 4)) 
      scheme_wrong_count("get-resource (string case)", 3, 4, n, p);
    x0 = (string)objscheme_unbundle_string(p[0], "get-resource (string case)");
    x1 = (string)objscheme_unbundle_string(p[1], "get-resource (string case)");
          *x2 = (string)objscheme_unbundle_string(objscheme_unbox(p[2], "get-resource (string case)"), "get-resource (string case)"", extracting boxed argument");
    if (n > 3) {
      x3 = (nstring)objscheme_unbundle_nullable_string(p[3], "get-resource (string case)");
    } else
      x3 = NULL;

    
    r = wxGetResource(x0, x1, x2, x3);

    
    if (n > 2)
      objscheme_set_box(p[2], objscheme_bundle_string((char *)_x2));
    
  } else  {
    string x0;
    string x1;
    long _x2;
    long* x2 = &_x2;
    nstring x3;

    
    if ((n < 3) ||(n > 4)) 
      scheme_wrong_count("get-resource (number case)", 3, 4, n, p);
    x0 = (string)objscheme_unbundle_string(p[0], "get-resource (number case)");
    x1 = (string)objscheme_unbundle_string(p[1], "get-resource (number case)");
          *x2 = objscheme_unbundle_integer(objscheme_unbox(p[2], "get-resource (number case)"), "get-resource (number case)"", extracting boxed argument");
    if (n > 3) {
      x3 = (nstring)objscheme_unbundle_nullable_string(p[3], "get-resource (number case)");
    } else
      x3 = NULL;

    
    r = wxGetResource(x0, x1, x2, x3);

    
    if (n > 2)
      objscheme_set_box(p[2], scheme_make_integer(_x2));
    
  }

  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *wxsGlobalwxStripMenuCodes_Scheme(int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  string r;
  string x0;

  
  x0 = (string)objscheme_unbundle_string(p[0], "label->plain-label");

  
  r = wxStripMenuCodes_Scheme(x0);

  
  
  return objscheme_bundle_string((char *)r);
}

#pragma argsused
static Scheme_Object *wxsGlobalwxDisplaySize(int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int _x0;
  int* x0 = &_x0;
  int _x1;
  int* x1 = &_x1;

  
      *x0 = objscheme_unbundle_integer(objscheme_unbox(p[0], "display-size"), "display-size"", extracting boxed argument");
      *x1 = objscheme_unbundle_integer(objscheme_unbox(p[1], "display-size"), "display-size"", extracting boxed argument");

  
  wxDisplaySize(x0, x1);

  
  if (n > 0)
    objscheme_set_box(p[0], scheme_make_integer(_x0));
  if (n > 1)
    objscheme_set_box(p[1], scheme_make_integer(_x1));
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *wxsGlobalwxBell(int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)

  

  
  wxBell();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *wxsGlobalwxEndBusyCursor(int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)

  

  
  wxEndBusyCursor();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *wxsGlobalwxIsBusy(int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;

  

  
  r = wxIsBusy();

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *wxsGlobalwxBeginBusyCursor(int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)

  

  
  wxBeginBusyCursor();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *wxsGlobalwxMakeMetaFilePlaceable(int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  string x0;
  float x1;
  float x2;
  float x3;
  float x4;
  float x5;

  
  x0 = (string)objscheme_unbundle_string(p[0], "make-meta-file-placeable");
  x1 = objscheme_unbundle_float(p[1], "make-meta-file-placeable");
  x2 = objscheme_unbundle_float(p[2], "make-meta-file-placeable");
  x3 = objscheme_unbundle_float(p[3], "make-meta-file-placeable");
  x4 = objscheme_unbundle_float(p[4], "make-meta-file-placeable");
  x5 = objscheme_unbundle_float(p[5], "make-meta-file-placeable");

  
  r = wxMakeMetaFilePlaceable(x0, x1, x2, x3, x4, x5);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *wxsGlobalwxDisplayDepth(int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  int r;

  

  
  r = wxDisplayDepth();

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *wxsGlobalwxColourDisplay(int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;

  

  
  r = wxColourDisplay();

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *wxsGlobalwxFileSelector(int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  nstring r;
  nstring x0;
  nstring x1;
  nstring x2;
  nstring x3;
  nstring x4;
  int x5;
  class wxWindow* x6;
  int x7;
  int x8;

  
  x0 = (nstring)objscheme_unbundle_nullable_string(p[0], "file-selector");
  if (n > 1) {
    x1 = (nstring)objscheme_unbundle_nullable_string(p[1], "file-selector");
  } else
    x1 = NULL;
  if (n > 2) {
    x2 = (nstring)objscheme_unbundle_nullable_string(p[2], "file-selector");
  } else
    x2 = NULL;
  if (n > 3) {
    x3 = (nstring)objscheme_unbundle_nullable_string(p[3], "file-selector");
  } else
    x3 = NULL;
  if (n > 4) {
    x4 = (nstring)objscheme_unbundle_nullable_string(p[4], "file-selector");
  } else
    x4 = FILE_SEL_DEF_PATTERN;
  if (n > 5) {
    x5 = unbundle_symset_fileSelMode(p[5], "file-selector");
  } else
    x5 = wxOPEN;
  x6 = (((n <= 6) || XC_SCHEME_NULLP(p[6])) ? (wxWindow *)NULL : (objscheme_istype_wxFrame(p[6], NULL, 1) ? (wxWindow *)objscheme_unbundle_wxFrame(p[6], NULL, 0) : (objscheme_istype_wxDialogBox(p[6], NULL, 1) ? (wxWindow *)objscheme_unbundle_wxDialogBox(p[6], NULL, 0) : (scheme_wrong_type("file-selector", "frame% or dialog%", -1, 0, &p[6]), (wxWindow *)NULL))));
  if (n > 7) {
    x7 = objscheme_unbundle_integer(p[7], "file-selector");
  } else
    x7 = -1;
  if (n > 8) {
    x8 = objscheme_unbundle_integer(p[8], "file-selector");
  } else
    x8 = -1;

  
  r = wxFileSelector(x0, x1, x2, x3, x4, x5, x6, x7, x8);

  
  
  return objscheme_bundle_string((char *)r);
}

void objscheme_setup_wxsGlobal(void *env)
{
  scheme_install_xc_global("flush-display", scheme_make_prim_w_arity(wxsGlobalwxFlushDisplay, "flush-display", 0, 0), env);
  scheme_install_xc_global("yield", scheme_make_prim_w_arity(wxsGlobalwxSchemeYield, "yield", 0, 1), env);
  scheme_install_xc_global("write-resource", scheme_make_prim_w_arity(wxsGlobalwxWriteResource, "write-resource", 3, 4), env);
  scheme_install_xc_global("get-resource", scheme_make_prim_w_arity(wxsGlobalwxGetResource, "get-resource", 3, 4), env);
  scheme_install_xc_global("label->plain-label", scheme_make_prim_w_arity(wxsGlobalwxStripMenuCodes_Scheme, "label->plain-label", 1, 1), env);
  scheme_install_xc_global("display-size", scheme_make_prim_w_arity(wxsGlobalwxDisplaySize, "display-size", 2, 2), env);
  scheme_install_xc_global("bell", scheme_make_prim_w_arity(wxsGlobalwxBell, "bell", 0, 0), env);
  scheme_install_xc_global("end-busy-cursor", scheme_make_prim_w_arity(wxsGlobalwxEndBusyCursor, "end-busy-cursor", 0, 0), env);
  scheme_install_xc_global("is-busy?", scheme_make_prim_w_arity(wxsGlobalwxIsBusy, "is-busy?", 0, 0), env);
  scheme_install_xc_global("begin-busy-cursor", scheme_make_prim_w_arity(wxsGlobalwxBeginBusyCursor, "begin-busy-cursor", 0, 0), env);
  scheme_install_xc_global("make-meta-file-placeable", scheme_make_prim_w_arity(wxsGlobalwxMakeMetaFilePlaceable, "make-meta-file-placeable", 6, 6), env);
  scheme_install_xc_global("get-display-depth", scheme_make_prim_w_arity(wxsGlobalwxDisplayDepth, "get-display-depth", 0, 0), env);
  scheme_install_xc_global("is-color-display?", scheme_make_prim_w_arity(wxsGlobalwxColourDisplay, "is-color-display?", 0, 0), env);
  scheme_install_xc_global("file-selector", scheme_make_prim_w_arity(wxsGlobalwxFileSelector, "file-selector", 1, 9), env);
}

