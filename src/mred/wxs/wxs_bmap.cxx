/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_bmap.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_gdi.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_bmap.h"



#ifndef wx_mac
# define wxBITMAP_TYPE_PICT 101
#endif

#define wxBITMAP_TYPE_UNKNOWN 0

static Scheme_Object *bitmapType_wxBITMAP_TYPE_BMP_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_GIF_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_XBM_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_XPM_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_PICT_sym = NULL;
static Scheme_Object *bitmapType_wxBITMAP_TYPE_UNKNOWN_sym = NULL;

static void init_symset_bitmapType(void) {
  REMEMBER_VAR_STACK();
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_BMP_sym);
  bitmapType_wxBITMAP_TYPE_BMP_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("bmp"));
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_GIF_sym);
  bitmapType_wxBITMAP_TYPE_GIF_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("gif"));
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_XBM_sym);
  bitmapType_wxBITMAP_TYPE_XBM_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("xbm"));
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_XPM_sym);
  bitmapType_wxBITMAP_TYPE_XPM_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("xpm"));
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_PICT_sym);
  bitmapType_wxBITMAP_TYPE_PICT_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("pict"));
  wxREGGLOB(bitmapType_wxBITMAP_TYPE_UNKNOWN_sym);
  bitmapType_wxBITMAP_TYPE_UNKNOWN_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("unknown"));
}

static int unbundle_symset_bitmapType(Scheme_Object *v, const char *where) {
  REMEMBER_VAR_STACK();
  if (!bitmapType_wxBITMAP_TYPE_UNKNOWN_sym) init_symset_bitmapType();
  if (0) { }
  else if (v == bitmapType_wxBITMAP_TYPE_BMP_sym) { return wxBITMAP_TYPE_BMP; }
  else if (v == bitmapType_wxBITMAP_TYPE_GIF_sym) { return wxBITMAP_TYPE_GIF; }
  else if (v == bitmapType_wxBITMAP_TYPE_XBM_sym) { return wxBITMAP_TYPE_XBM; }
  else if (v == bitmapType_wxBITMAP_TYPE_XPM_sym) { return wxBITMAP_TYPE_XPM; }
  else if (v == bitmapType_wxBITMAP_TYPE_PICT_sym) { return wxBITMAP_TYPE_PICT; }
  else if (v == bitmapType_wxBITMAP_TYPE_UNKNOWN_sym) { return wxBITMAP_TYPE_UNKNOWN; }
  if (where) WITH_REMEMBERED_STACK(scheme_wrong_type(where, "bitmapType symbol", -1, 0, &v));
  return 0;
}

static Scheme_Object *bundle_symset_bitmapType(int v) {
  if (!bitmapType_wxBITMAP_TYPE_UNKNOWN_sym) init_symset_bitmapType();
  switch (v) {
  case wxBITMAP_TYPE_BMP: return bitmapType_wxBITMAP_TYPE_BMP_sym;
  case wxBITMAP_TYPE_GIF: return bitmapType_wxBITMAP_TYPE_GIF_sym;
  case wxBITMAP_TYPE_XBM: return bitmapType_wxBITMAP_TYPE_XBM_sym;
  case wxBITMAP_TYPE_XPM: return bitmapType_wxBITMAP_TYPE_XPM_sym;
  case wxBITMAP_TYPE_PICT: return bitmapType_wxBITMAP_TYPE_PICT_sym;
  case wxBITMAP_TYPE_UNKNOWN: return bitmapType_wxBITMAP_TYPE_UNKNOWN_sym;
  default: return NULL;
  }
}




static Scheme_Object *saveBitmapType_wxBITMAP_TYPE_BMP_sym = NULL;
static Scheme_Object *saveBitmapType_wxBITMAP_TYPE_XBM_sym = NULL;
static Scheme_Object *saveBitmapType_wxBITMAP_TYPE_XPM_sym = NULL;
static Scheme_Object *saveBitmapType_wxBITMAP_TYPE_PICT_sym = NULL;

static void init_symset_saveBitmapType(void) {
  REMEMBER_VAR_STACK();
  wxREGGLOB(saveBitmapType_wxBITMAP_TYPE_BMP_sym);
  saveBitmapType_wxBITMAP_TYPE_BMP_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("bmp"));
  wxREGGLOB(saveBitmapType_wxBITMAP_TYPE_XBM_sym);
  saveBitmapType_wxBITMAP_TYPE_XBM_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("xbm"));
  wxREGGLOB(saveBitmapType_wxBITMAP_TYPE_XPM_sym);
  saveBitmapType_wxBITMAP_TYPE_XPM_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("xpm"));
  wxREGGLOB(saveBitmapType_wxBITMAP_TYPE_PICT_sym);
  saveBitmapType_wxBITMAP_TYPE_PICT_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("pict"));
}

static int unbundle_symset_saveBitmapType(Scheme_Object *v, const char *where) {
  REMEMBER_VAR_STACK();
  if (!saveBitmapType_wxBITMAP_TYPE_PICT_sym) init_symset_saveBitmapType();
  if (0) { }
  else if (v == saveBitmapType_wxBITMAP_TYPE_BMP_sym) { return wxBITMAP_TYPE_BMP; }
  else if (v == saveBitmapType_wxBITMAP_TYPE_XBM_sym) { return wxBITMAP_TYPE_XBM; }
  else if (v == saveBitmapType_wxBITMAP_TYPE_XPM_sym) { return wxBITMAP_TYPE_XPM; }
  else if (v == saveBitmapType_wxBITMAP_TYPE_PICT_sym) { return wxBITMAP_TYPE_PICT; }
  if (where) WITH_REMEMBERED_STACK(scheme_wrong_type(where, "saveBitmapType symbol", -1, 0, &v));
  return 0;
}


static Bool IsColor(wxBitmap *bm)
{
  return (bm->GetDepth() == 1);
}







class os_wxBitmap : public wxBitmap {
 public:

  os_wxBitmap(Scheme_Object * obj, string x0, int x1, int x2);
  os_wxBitmap(Scheme_Object * obj, int x0, int x1, Bool x2 = 0);
  os_wxBitmap(Scheme_Object * obj, pathname x0, int x1 = 0);
  ~os_wxBitmap();
};

Scheme_Object *os_wxBitmap_class;

os_wxBitmap::os_wxBitmap(Scheme_Object *, string x0, int x1, int x2)
: wxBitmap(x0, x1, x2)
{
}

os_wxBitmap::os_wxBitmap(Scheme_Object *, int x0, int x1, Bool x2)
: wxBitmap(x0, x1, x2)
{
}

os_wxBitmap::os_wxBitmap(Scheme_Object *, pathname x0, int x1)
: wxBitmap(x0, x1)
{
}

os_wxBitmap::~os_wxBitmap()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

#pragma argsused
static Scheme_Object *os_wxBitmapSaveFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);
  pathname x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);
  VAR_STACK_PUSH(2, x0);

  
  x0 = (pathname)WITH_VAR_STACK(objscheme_unbundle_pathname(p[0], "save-file in bitmap%"));
  x1 = WITH_VAR_STACK(unbundle_symset_saveBitmapType(p[1], "save-file in bitmap%"));

  
  r = WITH_VAR_STACK(((wxBitmap *)((Scheme_Class_Object *)obj)->primdata)->SaveFile(x0, x1));

  if (1) scheme_process_block(0.0);
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxBitmapLoadFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);
  pathname x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);
  VAR_STACK_PUSH(2, x0);

  
  x0 = (pathname)WITH_VAR_STACK(objscheme_unbundle_pathname(p[0], "load-file in bitmap%"));
  if (n > 1) {
    x1 = WITH_VAR_STACK(unbundle_symset_bitmapType(p[1], "load-file in bitmap%"));
  } else
    x1 = 0;

  
  r = WITH_VAR_STACK(((wxBitmap *)((Scheme_Class_Object *)obj)->primdata)->LoadFile(x0, x1));

  if (r) scheme_process_block(0.0);
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxBitmapIsColor(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  r = WITH_VAR_STACK(IsColor(((wxBitmap *)((Scheme_Class_Object *)obj)->primdata)));

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxBitmapOk(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  r = WITH_VAR_STACK(((wxBitmap *)((Scheme_Class_Object *)obj)->primdata)->Ok());

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxBitmapGetWidth(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  r = WITH_VAR_STACK(((wxBitmap *)((Scheme_Class_Object *)obj)->primdata)->GetWidth());

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxBitmapGetHeight(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  r = WITH_VAR_STACK(((wxBitmap *)((Scheme_Class_Object *)obj)->primdata)->GetHeight());

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxBitmapGetDepth(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, p);

  

  
  r = WITH_VAR_STACK(((wxBitmap *)((Scheme_Class_Object *)obj)->primdata)->GetDepth());

  
  
  return scheme_make_integer(r);
}

#pragma argsused
static Scheme_Object *os_wxBitmap_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  os_wxBitmap *realobj;
  if ((n >= 1) && objscheme_istype_number(p[0], NULL)) {
    int x0;
    int x1;
    Bool x2;

    SETUP_VAR_STACK_REMEMBERED(2);
    VAR_STACK_PUSH(0, obj);
    VAR_STACK_PUSH(1, p);

    
    if ((n < 2) ||(n > 3)) 
      WITH_VAR_STACK(scheme_wrong_count("initialization in bitmap% (width/height case)", 2, 3, n, p));
    x0 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[0], 1, 10000, "initialization in bitmap% (width/height case)"));
    x1 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[1], 1, 10000, "initialization in bitmap% (width/height case)"));
    if (n > 2) {
      x2 = WITH_VAR_STACK(objscheme_unbundle_bool(p[2], "initialization in bitmap% (width/height case)"));
    } else
      x2 = 0;

    
    realobj = NEW_OBJECT(os_wxBitmap, (obj, x0, x1, x2));
    realobj->__gc_external = (void *)obj;
    objscheme_note_creation(obj);
    
    
  } else if ((n >= 2) && objscheme_istype_string(p[0], NULL) && objscheme_istype_number(p[1], NULL)) {
    string x0;
    int x1;
    int x2;

    SETUP_VAR_STACK_REMEMBERED(3);
    VAR_STACK_PUSH(0, obj);
    VAR_STACK_PUSH(1, p);
    VAR_STACK_PUSH(2, x0);

    
    if (n != 3) 
      WITH_VAR_STACK(scheme_wrong_count("initialization in bitmap% (datastring case)", 3, 3, n, p));
    x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "initialization in bitmap% (datastring case)"));
    x1 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[1], 1, 10000, "initialization in bitmap% (datastring case)"));
    x2 = WITH_VAR_STACK(objscheme_unbundle_integer_in(p[2], 1, 10000, "initialization in bitmap% (datastring case)"));

    if (SCHEME_STRTAG_VAL(p[0]) < (((x1 * x2) + 7) >> 3)) scheme_arg_mismatch(METHODNAME("bitmap%","initialization"), "string too short: ", p[0]);
    realobj = NEW_OBJECT(os_wxBitmap, (obj, x0, x1, x2));
    realobj->__gc_external = (void *)obj;
    objscheme_note_creation(obj);
    
    
  } else  {
    pathname x0;
    int x1;

    SETUP_VAR_STACK_REMEMBERED(3);
    VAR_STACK_PUSH(0, obj);
    VAR_STACK_PUSH(1, p);
    VAR_STACK_PUSH(2, x0);

    
    if ((n < 1) ||(n > 2)) 
      WITH_VAR_STACK(scheme_wrong_count("initialization in bitmap% (pathname case)", 1, 2, n, p));
    x0 = (pathname)WITH_VAR_STACK(objscheme_unbundle_pathname(p[0], "initialization in bitmap% (pathname case)"));
    if (n > 1) {
      x1 = WITH_VAR_STACK(unbundle_symset_bitmapType(p[1], "initialization in bitmap% (pathname case)"));
    } else
      x1 = 0;

    
    realobj = NEW_OBJECT(os_wxBitmap, (obj, x0, x1));
    realobj->__gc_external = (void *)obj;
    objscheme_note_creation(obj);
    if (realobj->Ok()) scheme_process_block(0.0);
    
  }

  ((Scheme_Class_Object *)obj)->primdata = realobj;
  objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata);
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxBitmap(void *env)
{
  if (os_wxBitmap_class) {
    objscheme_add_global_class(os_wxBitmap_class, "bitmap%", env);
  } else {
    REMEMBER_VAR_STACK();
    os_wxBitmap_class = objscheme_def_prim_class(env, "bitmap%", "object%", os_wxBitmap_ConstructScheme, 7);

    wxREGGLOB("bitmap%");

    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "save-file", os_wxBitmapSaveFile, 2, 2));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "load-file", os_wxBitmapLoadFile, 1, 2));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "is-color?", os_wxBitmapIsColor, 0, 0));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "ok?", os_wxBitmapOk, 0, 0));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "get-width", os_wxBitmapGetWidth, 0, 0));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "get-height", os_wxBitmapGetHeight, 0, 0));
    WITH_REMEMBERED_STACK(scheme_add_method_w_arity(os_wxBitmap_class, "get-depth", os_wxBitmapGetDepth, 0, 0));


    WITH_REMEMBERED_STACK(scheme_made_class(os_wxBitmap_class));


  }
}

int objscheme_istype_wxBitmap(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxBitmap_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "bitmap% object or " XC_NULL_STR: "bitmap% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxBitmap(class wxBitmap *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;

  SETUP_VAR_STACK(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, realobj);

  if ((sobj = WITH_VAR_STACK(objscheme_bundle_by_type(realobj, realobj->__type))))
    return sobj;
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxBitmap_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(&obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxBitmap *objscheme_unbundle_wxBitmap(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxBitmap(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(obj));
  if (o->primflag)
    return (os_wxBitmap *)o->primdata;
  else
    return (wxBitmap *)o->primdata;
}

