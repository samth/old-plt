/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_butn.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_buttn.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_butn.h"
#include "wxscomon.h"


static Scheme_Object *buttonStyle_1_sym = NULL;

static void init_symset_buttonStyle(void) {
  wxREGGLOB(buttonStyle_1_sym);
  buttonStyle_1_sym = scheme_intern_symbol("border");
}

static int unbundle_symset_buttonStyle(Scheme_Object *v, const char *where) {
  if (!buttonStyle_1_sym) init_symset_buttonStyle();
  Scheme_Object *i, *l = v;
  long result = 0;
  while (SCHEME_PAIRP(l)) {
  i = SCHEME_CAR(l);
  if (0) { }
  else if (i == buttonStyle_1_sym) { result = result | 1; }
  else { break; } 
  l = SCHEME_CDR(l);
  }
  if (SCHEME_NULLP(l)) return result;
  if (where) scheme_wrong_type(where, "buttonStyle symbol list", -1, 0, &v);
  return 0;
}





#define CB_FUNCTYPE wxFunction 


#undef CALLBACKCLASS
#undef CB_REALCLASS
#undef CB_UNBUNDLE
#undef CB_USER

#define CALLBACKCLASS os_wxButton
#define CB_REALCLASS wxButton
#define CB_UNBUNDLE objscheme_unbundle_wxButton
#define CB_USER METHODNAME("button%","initialization")

#undef CB_TOSCHEME
#undef CB_TOC
#define CB_TOSCHEME wxButtonCallbackToScheme
#define CB_TOC wxButtonCallbackToC


class CALLBACKCLASS;





extern wxCommandEvent *objscheme_unbundle_wxCommandEvent(Scheme_Object *,const char *,int);
extern Scheme_Object *objscheme_bundle_wxCommandEvent(wxCommandEvent *);

static void CB_TOSCHEME(CB_REALCLASS *obj, wxCommandEvent &event);










class os_wxButton : public wxButton {
 public:
  Scheme_Object *callback_closure;

  os_wxButton(Scheme_Object * obj, class wxPanel* x0, wxFunction x1, string x2, int x3 = -1, int x4 = -1, int x5 = -1, int x6 = -1, int x7 = 0, string x8 = "button");
  os_wxButton(Scheme_Object * obj, class wxPanel* x0, wxFunction x1, class wxBitmap* x2, int x3 = -1, int x4 = -1, int x5 = -1, int x6 = -1, int x7 = 0, string x8 = "button");
  ~os_wxButton();
  void OnDropFile(pathname x0);
  Bool PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1);
  Bool PreOnChar(class wxWindow* x0, class wxKeyEvent* x1);
  void OnSize(int x0, int x1);
  void OnSetFocus();
  void OnKillFocus();
};

Scheme_Object *os_wxButton_class;

os_wxButton::os_wxButton(Scheme_Object * o, class wxPanel* x0, wxFunction x1, string x2, int x3, int x4, int x5, int x6, int x7, string x8)
: wxButton(x0, x1, x2, x3, x4, x5, x6, x7, x8)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxButton::os_wxButton(Scheme_Object * o, class wxPanel* x0, wxFunction x1, class wxBitmap* x2, int x3, int x4, int x5, int x6, int x7, string x8)
: wxButton(x0, x1, x2, x3, x4, x5, x6, x7, x8)
{
  __gc_external = (void *)o;
  objscheme_backpointer(&__gc_external);
  objscheme_note_creation(o);
}

os_wxButton::~os_wxButton()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

void os_wxButton::OnDropFile(pathname x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxButton_class, "on-drop-file", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxButton::OnDropFile(x0);
  } else {
  mz_jmp_buf savebuf;
  p[0] = objscheme_bundle_pathname((char *)x0);
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = scheme_apply(method, 1, p);
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

Bool os_wxButton::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxButton_class, "pre-on-event", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[0] = objscheme_bundle_wxWindow(x0);
  p[1] = objscheme_bundle_wxMouseEvent(x1);
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 0; }

  v = scheme_apply(method, 2, p);
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return objscheme_unbundle_bool(v, "pre-on-event in button%"", extracting return value");
  }
}

Bool os_wxButton::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxButton_class, "pre-on-char", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[0] = objscheme_bundle_wxWindow(x0);
  p[1] = objscheme_bundle_wxKeyEvent(x1);
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 0; }

  v = scheme_apply(method, 2, p);
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return objscheme_unbundle_bool(v, "pre-on-char in button%"", extracting return value");
  }
}

void os_wxButton::OnSize(int x0, int x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxButton_class, "on-size", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxButton::OnSize(x0, x1);
  } else {
  
  p[0] = scheme_make_integer(x0);
  p[1] = scheme_make_integer(x1);
  

  v = scheme_apply(method, 2, p);
  
  
  }
}

void os_wxButton::OnSetFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxButton_class, "on-set-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxButton::OnSetFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = scheme_apply(method, 0, p);
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxButton::OnKillFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
  static void *mcache = 0;

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxButton_class, "on-kill-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    wxButton::OnKillFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = scheme_apply(method, 0, p);
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

#pragma argsused
static Scheme_Object *os_wxButtonSetLabel(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  if ((n >= 1) && objscheme_istype_wxBitmap(p[0], NULL, 0)) {
    class wxBitmap* x0;

    
    if (n != 1) 
      scheme_wrong_count("set-label in button% (bitmap label case)", 1, 1, n, p);
    x0 = objscheme_unbundle_wxBitmap(p[0], "set-label in button% (bitmap label case)", 0);

    { if (x0 && !x0->Ok()) scheme_arg_mismatch(METHODNAME("button%","set-label"), "bad bitmap: ", p[0]); if (x0 && BM_SELECTED(x0)) scheme_arg_mismatch(METHODNAME("button%","set-label"), "bitmap is currently installed into a bitmap-dc%: ", p[0]); }
    ((wxButton *)((Scheme_Class_Object *)obj)->primdata)->SetLabel(x0);

    
    
  } else  {
    string x0;

    
    if (n != 1) 
      scheme_wrong_count("set-label in button% (string label case)", 1, 1, n, p);
    x0 = (string)objscheme_unbundle_string(p[0], "set-label in button% (string label case)");

    
    ((wxButton *)((Scheme_Class_Object *)obj)->primdata)->SetLabel(x0);

    
    
  }

  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxButtonOnDropFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  pathname x0;

  
  x0 = (pathname)objscheme_unbundle_pathname(p[0], "on-drop-file in button%");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxButton *)((Scheme_Class_Object *)obj)->primdata)->wxButton::OnDropFile(x0);
  else
    ((wxButton *)((Scheme_Class_Object *)obj)->primdata)->OnDropFile(x0);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxButtonPreOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxMouseEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "pre-on-event in button%", 0);
  x1 = objscheme_unbundle_wxMouseEvent(p[1], "pre-on-event in button%", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxButton *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnEvent(x0, x1);
  else
    r = ((wxButton *)((Scheme_Class_Object *)obj)->primdata)->PreOnEvent(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxButtonPreOnChar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxKeyEvent* x1;

  
  x0 = objscheme_unbundle_wxWindow(p[0], "pre-on-char in button%", 0);
  x1 = objscheme_unbundle_wxKeyEvent(p[1], "pre-on-char in button%", 0);

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = ((os_wxButton *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnChar(x0, x1);
  else
    r = ((wxButton *)((Scheme_Class_Object *)obj)->primdata)->PreOnChar(x0, x1);

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxButtonOnSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);
  int x0;
  int x1;

  
  x0 = objscheme_unbundle_integer(p[0], "on-size in button%");
  x1 = objscheme_unbundle_integer(p[1], "on-size in button%");

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxButton *)((Scheme_Class_Object *)obj)->primdata)->wxButton::OnSize(x0, x1);
  else
    ((wxButton *)((Scheme_Class_Object *)obj)->primdata)->OnSize(x0, x1);

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxButtonOnSetFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxButton *)((Scheme_Class_Object *)obj)->primdata)->wxButton::OnSetFocus();
  else
    ((wxButton *)((Scheme_Class_Object *)obj)->primdata)->OnSetFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxButtonOnKillFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
 WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  objscheme_check_valid(obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    ((os_wxButton *)((Scheme_Class_Object *)obj)->primdata)->wxButton::OnKillFocus();
  else
    ((wxButton *)((Scheme_Class_Object *)obj)->primdata)->OnKillFocus();

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxButton_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  os_wxButton *realobj;
  if ((n >= 3) && objscheme_istype_wxPanel(p[0], NULL, 0) && (SCHEME_NULLP(p[1]) || objscheme_istype_proc2(p[1], NULL)) && objscheme_istype_wxBitmap(p[2], NULL, 0)) {
    class wxPanel* x0;
    wxFunction x1;
    class wxBitmap* x2;
    int x3;
    int x4;
    int x5;
    int x6;
    int x7;
    string x8;

    Scheme_Object *tmp_callback = NULL;
    if ((n < 3) ||(n > 9)) 
      scheme_wrong_count("initialization in button% (bitmap label case)", 3, 9, n, p);
    x0 = objscheme_unbundle_wxPanel(p[0], "initialization in button% (bitmap label case)", 0);
    x1 = (SCHEME_NULLP(p[1]) ? NULL : (WXGC_IGNORE(tmp_callback), objscheme_istype_proc2(p[1], CB_USER), tmp_callback = p[1], (CB_FUNCTYPE)CB_TOSCHEME));
    x2 = objscheme_unbundle_wxBitmap(p[2], "initialization in button% (bitmap label case)", 0);
    if (n > 3) {
      x3 = objscheme_unbundle_integer(p[3], "initialization in button% (bitmap label case)");
    } else
      x3 = -1;
    if (n > 4) {
      x4 = objscheme_unbundle_integer(p[4], "initialization in button% (bitmap label case)");
    } else
      x4 = -1;
    if (n > 5) {
      x5 = objscheme_unbundle_integer(p[5], "initialization in button% (bitmap label case)");
    } else
      x5 = -1;
    if (n > 6) {
      x6 = objscheme_unbundle_integer(p[6], "initialization in button% (bitmap label case)");
    } else
      x6 = -1;
    if (n > 7) {
      x7 = unbundle_symset_buttonStyle(p[7], "initialization in button% (bitmap label case)");
    } else
      x7 = 0;
    if (n > 8) {
      x8 = (string)objscheme_unbundle_string(p[8], "initialization in button% (bitmap label case)");
    } else
      x8 = "button";

    { if (x2 && !x2->Ok()) scheme_arg_mismatch(METHODNAME("button%","initialization"), "bad bitmap: ", p[2]); if (x2 && BM_SELECTED(x2)) scheme_arg_mismatch(METHODNAME("button%","initialization"), "bitmap is currently installed into a bitmap-dc%: ", p[2]); }if (!x5) x5 = -1;if (!x6) x6 = -1;
    realobj = new os_wxButton(obj, x0, x1, x2, x3, x4, x5, x6, x7, x8);
    
    realobj->callback_closure = tmp_callback; objscheme_backpointer(&realobj->callback_closure);
  } else  {
    class wxPanel* x0;
    wxFunction x1;
    string x2;
    int x3;
    int x4;
    int x5;
    int x6;
    int x7;
    string x8;

    Scheme_Object *tmp_callback = NULL;
    if ((n < 3) ||(n > 9)) 
      scheme_wrong_count("initialization in button% (string label case)", 3, 9, n, p);
    x0 = objscheme_unbundle_wxPanel(p[0], "initialization in button% (string label case)", 0);
    x1 = (SCHEME_NULLP(p[1]) ? NULL : (WXGC_IGNORE(tmp_callback), objscheme_istype_proc2(p[1], CB_USER), tmp_callback = p[1], (CB_FUNCTYPE)CB_TOSCHEME));
    x2 = (string)objscheme_unbundle_string(p[2], "initialization in button% (string label case)");
    if (n > 3) {
      x3 = objscheme_unbundle_integer(p[3], "initialization in button% (string label case)");
    } else
      x3 = -1;
    if (n > 4) {
      x4 = objscheme_unbundle_integer(p[4], "initialization in button% (string label case)");
    } else
      x4 = -1;
    if (n > 5) {
      x5 = objscheme_unbundle_integer(p[5], "initialization in button% (string label case)");
    } else
      x5 = -1;
    if (n > 6) {
      x6 = objscheme_unbundle_integer(p[6], "initialization in button% (string label case)");
    } else
      x6 = -1;
    if (n > 7) {
      x7 = unbundle_symset_buttonStyle(p[7], "initialization in button% (string label case)");
    } else
      x7 = 0;
    if (n > 8) {
      x8 = (string)objscheme_unbundle_string(p[8], "initialization in button% (string label case)");
    } else
      x8 = "button";

    if (!x5) x5 = -1;if (!x6) x6 = -1;
    realobj = new os_wxButton(obj, x0, x1, x2, x3, x4, x5, x6, x7, x8);
    
    realobj->callback_closure = tmp_callback; objscheme_backpointer(&realobj->callback_closure);
  }

  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxButton(void *env)
{
if (os_wxButton_class) {
    objscheme_add_global_class(os_wxButton_class, "button%", env);
} else {
  os_wxButton_class = objscheme_def_prim_class(env, "button%", "item%", os_wxButton_ConstructScheme, 7);

 scheme_add_method_w_arity(os_wxButton_class, "set-label", os_wxButtonSetLabel, 1, 1);
 scheme_add_method_w_arity(os_wxButton_class, "on-drop-file", os_wxButtonOnDropFile, 1, 1);
 scheme_add_method_w_arity(os_wxButton_class, "pre-on-event", os_wxButtonPreOnEvent, 2, 2);
 scheme_add_method_w_arity(os_wxButton_class, "pre-on-char", os_wxButtonPreOnChar, 2, 2);
 scheme_add_method_w_arity(os_wxButton_class, "on-size", os_wxButtonOnSize, 2, 2);
 scheme_add_method_w_arity(os_wxButton_class, "on-set-focus", os_wxButtonOnSetFocus, 0, 0);
 scheme_add_method_w_arity(os_wxButton_class, "on-kill-focus", os_wxButtonOnKillFocus, 0, 0);


  scheme_made_class(os_wxButton_class);

  objscheme_install_bundler((Objscheme_Bundler)objscheme_bundle_wxButton, wxTYPE_BUTTON);

}
}

int objscheme_istype_wxButton(Scheme_Object *obj, const char *stop, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxButton_class))
    return 1;
  else {
    if (!stop)
       return 0;
    scheme_wrong_type(stop, nullOK ? "button% object or " XC_NULL_STR: "button% object", -1, 0, &obj);
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxButton(class wxButton *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;
  if ((realobj->__type != wxTYPE_BUTTON) && (sobj = objscheme_bundle_by_type(realobj, realobj->__type)))
    return sobj;
  obj = (Scheme_Class_Object *)scheme_make_uninited_object(os_wxButton_class);

  obj->primdata = realobj;
  objscheme_register_primpointer(&obj->primdata);
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  objscheme_backpointer(&realobj->__gc_external);
  return (Scheme_Object *)obj;
}

class wxButton *objscheme_unbundle_wxButton(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  (void)objscheme_istype_wxButton(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  objscheme_check_valid(obj);
  if (o->primflag)
    return (os_wxButton *)o->primdata;
  else
    return (wxButton *)o->primdata;
}



static void CB_TOSCHEME(CB_REALCLASS *realobj, wxCommandEvent &event)
{
  Scheme_Object *p[2];
  Scheme_Class_Object *obj;
  mz_jmp_buf savebuf;

  obj = (Scheme_Class_Object *)realobj->__gc_external;

  if (!obj) {
    // scheme_signal_error("bad callback object");
    return;
  }

  p[0] = (Scheme_Object *)obj;
  p[1] = objscheme_bundle_wxCommandEvent(&event);

  COPY_JMPBUF(savebuf, scheme_error_buf);

  if (!scheme_setjmp(scheme_error_buf))
    scheme_apply_multi(((CALLBACKCLASS *)obj->primdata)->callback_closure, 2, p);

  COPY_JMPBUF(scheme_error_buf, savebuf);
}
