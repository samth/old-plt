
/* String object interface */

#ifndef Py_STRINGOBJECT_H
#define Py_STRINGOBJECT_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

/*
Type PyStringObject represents a character string.  An extra zero byte is
reserved at the end to ensure it is zero-terminated, but a size is
present so strings with null bytes in them can be represented.  This
is an immutable object type.

There are functions to create new string objects, to test
an object for string-ness, and to get the
string value.  The latter function returns a null pointer
if the object is not of the proper type.
There is a variant that takes an explicit size as well as a
variant that assumes a zero-terminated string.  Note that none of the
functions should be applied to nil objects.
*/

/* Caching the hash (ob_shash) saves recalculation of a string's hash value.
   Interning strings (ob_sstate) tries to ensure that only one string
   object with a given value exists, so equality tests can be one pointer
   comparison.  This is generally restricted to strings that "look like"
   Python identifiers, although the intern() builtin can be used to force
   interning of any string.
   Together, these sped the interpreter by up to 20%. */

   #define FIVE_INTS(name) struct { int a; int b; int c; int d; int e; } name;

   #define DANIELS_BIG_GAP FIVE_INTS(a) FIVE_INTS(b) FIVE_INTS(c) FIVE_INTS(d) FIVE_INTS(e)

typedef struct {
    PyObject_VAR_HEAD
	/*DANIELS_BIG_GAP */
    long ob_shash;
    int ob_sstate;
    char ob_sval[1];
} PyStringObject;


#define SSTATE_NOT_INTERNED 0
#define SSTATE_INTERNED_MORTAL 1
#define SSTATE_INTERNED_IMMORTAL 2

PyAPI_DATA(PyTypeObject) PyBaseString_Type;
PyAPI_DATA(PyTypeObject) PyString_Type;

#define PyString_Check(op) PyObject_TypeCheck(op, &PyString_Type)

#define PyString_CheckExact(op) ((PyTypeObject *) PY_GET_TYPE(op) == &PyString_Type)
//#define PyString_CheckExact(op) ((op)->ob_type == &PyString_Type)

PyAPI_FUNC(PyObject *) PyString_FromStringAndSize(const char *, int);
PyAPI_FUNC(PyObject *) PyString_FromString(const char *);
PyAPI_FUNC(PyObject *) PyString_FromFormatV(const char*, va_list)
				Py_GCC_ATTRIBUTE((format(printf, 1, 0)));
PyAPI_FUNC(PyObject *) PyString_FromFormat(const char*, ...)
				Py_GCC_ATTRIBUTE((format(printf, 1, 2)));
PyAPI_FUNC(int) PyString_Size(PyObject *);
PyAPI_FUNC(char *) PyString_AsString(PyObject *);
PyAPI_FUNC(PyObject *) PyString_Repr(PyObject *, int);
PyAPI_FUNC(void) PyString_Concat(PyObject **, PyObject *);
PyAPI_FUNC(void) PyString_ConcatAndDel(PyObject **, PyObject *);
PyAPI_FUNC(int) _PyString_Resize(PyObject **, int);
PyAPI_FUNC(int) _PyString_Eq(PyObject *, PyObject*);
PyAPI_FUNC(PyObject *) PyString_Format(PyObject *, PyObject *);
PyAPI_FUNC(PyObject *) _PyString_FormatLong(PyObject*, int, int,
						  int, char**, int*);
extern DL_IMPORT(PyObject *) PyString_DecodeEscape(const char *, int,
						   const char *, int,
						   const char *);

PyAPI_FUNC(void) PyString_InternInPlace(PyObject **);
PyAPI_FUNC(void) PyString_InternImmortal(PyObject **);
PyAPI_FUNC(PyObject *) PyString_InternFromString(const char *);
PyAPI_FUNC(void) _Py_ReleaseInternedStrings(void);

/* Use only if you know it's a string */
#define PyString_CHECK_INTERNED(op) (((PyStringObject *)(op))->ob_sstate)

/* Macro, trading safety for speed */
#define PyString_AS_STRING(op) (((PyStringObject *)(op))->ob_sval)

//#define PyString_GET_SIZE(op)  (strlen(PyString_AsString(op)))
#define PyString_GET_SIZE(op)  (((PyStringObject *)(op))->ob_size)

/* _PyString_Join(sep, x) is like sep.join(x).  sep must be PyStringObject*,
   x must be an iterable object. */
PyAPI_FUNC(PyObject *) _PyString_Join(PyObject *sep, PyObject *x);

/* --- Generic Codecs ----------------------------------------------------- */

/* Create an object by decoding the encoded string s of the
   given size. */

PyAPI_FUNC(PyObject*) PyString_Decode(
    const char *s,              /* encoded string */
    int size,                   /* size of buffer */
    const char *encoding,       /* encoding */
    const char *errors          /* error handling */
    );

/* Encodes a char buffer of the given size and returns a
   Python object. */

PyAPI_FUNC(PyObject*) PyString_Encode(
    const char *s,              /* string char buffer */
    int size,                   /* number of chars to encode */
    const char *encoding,       /* encoding */
    const char *errors          /* error handling */
    );

/* Encodes a string object and returns the result as Python
   object. */

PyAPI_FUNC(PyObject*) PyString_AsEncodedObject(
    PyObject *str,	 	/* string object */
    const char *encoding,	/* encoding */
    const char *errors		/* error handling */
    );

/* Encodes a string object and returns the result as Python string
   object.

   If the codec returns an Unicode object, the object is converted
   back to a string using the default encoding.

   DEPRECATED - use PyString_AsEncodedObject() instead. */

PyAPI_FUNC(PyObject*) PyString_AsEncodedString(
    PyObject *str,	 	/* string object */
    const char *encoding,	/* encoding */
    const char *errors		/* error handling */
    );

/* Decodes a string object and returns the result as Python
   object. */

PyAPI_FUNC(PyObject*) PyString_AsDecodedObject(
    PyObject *str,	 	/* string object */
    const char *encoding,	/* encoding */
    const char *errors		/* error handling */
    );

/* Decodes a string object and returns the result as Python string
   object.

   If the codec returns an Unicode object, the object is converted
   back to a string using the default encoding.

   DEPRECATED - use PyString_AsDecodedObject() instead. */

PyAPI_FUNC(PyObject*) PyString_AsDecodedString(
    PyObject *str,	 	/* string object */
    const char *encoding,	/* encoding */
    const char *errors		/* error handling */
    );

/* Provides access to the internal data buffer and size of a string
   object or the default encoded version of an Unicode object. Passing
   NULL as *len parameter will force the string buffer to be
   0-terminated (passing a string with embedded NULL characters will
   cause an exception).  */

PyAPI_FUNC(int) PyString_AsStringAndSize(
    register PyObject *obj,	/* string or Unicode object */
    register char **s,		/* pointer to buffer variable */
    register int *len		/* pointer to length variable or NULL
				   (only possible for 0-terminated
				   strings) */
    );


#ifdef __cplusplus
}
#endif
#endif /* !Py_STRINGOBJECT_H */
