/*
  MzScheme
  Copyright (c) 2004-2005 PLT Scheme, Inc.
  Copyright (c) 1995-2001 Matthew Flatt

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  libscheme
  Copyright (c) 1994 Brent Benson
  All rights reserved.
*/

/* This file contains the MzScheme reader, including the normal reader
   and the one for .zo files. The normal reader is a recursive-descent
   parser. The really messy part is number parsing, which is in a
   different file, numstr.c. */

/* Rule on using scheme_ungetc(): the reader is generally allowed to
   use scheme_ungetc() only when it will definitely re-read the
   character as it continues. If the character will not be re-read
   (e.g., because an exception will be raised), then the reader must
   peek, instead. However, read-symbol uses ungetc() if the port does
   not have a specific peek handler, and in that case, read-symbol
   only ungetc()s a single character (that had been read by itself). */

#include "schpriv.h"
#include "schmach.h"
#include "schminc.h"
#include "schcpt.h"
#include <stdlib.h>
#include <ctype.h>
#ifdef USE_STACKAVAIL
# include <malloc.h>
#endif

#define MAX_QUICK_SYMBOL_SIZE 64

/* Init options for embedding: */
int scheme_square_brackets_are_parens = 1;
int scheme_curly_braces_are_parens = 1;

/* local function prototypes */

static Scheme_Object *read_case_sensitive(int, Scheme_Object *[]);
static Scheme_Object *read_bracket_as_paren(int, Scheme_Object *[]);
static Scheme_Object *read_brace_as_paren(int, Scheme_Object *[]);
static Scheme_Object *read_accept_graph(int, Scheme_Object *[]);
static Scheme_Object *read_accept_compiled(int, Scheme_Object *[]);
static Scheme_Object *read_accept_box(int, Scheme_Object *[]);
static Scheme_Object *read_accept_pipe_quote(int, Scheme_Object *[]);
static Scheme_Object *read_decimal_as_inexact(int, Scheme_Object *[]);
static Scheme_Object *read_accept_dot(int, Scheme_Object *[]);
static Scheme_Object *read_accept_quasi(int, Scheme_Object *[]);
static Scheme_Object *read_accept_reader(int, Scheme_Object *[]);
static Scheme_Object *print_graph(int, Scheme_Object *[]);
static Scheme_Object *print_struct(int, Scheme_Object *[]);
static Scheme_Object *print_box(int, Scheme_Object *[]);
static Scheme_Object *print_vec_shorthand(int, Scheme_Object *[]);
static Scheme_Object *print_hash_table(int, Scheme_Object *[]);
static Scheme_Object *print_unreadable(int, Scheme_Object *[]);
static Scheme_Object *print_honu(int, Scheme_Object *[]);

#define NOT_EOF_OR_SPECIAL(x) ((x) >= 0)

#define mzSPAN(port, pos)  ()

#define isdigit_ascii(n) ((n >= '0') && (n <= '9'))

#define RETURN_FOR_SPECIAL_COMMENT  0x1
#define RETURN_FOR_HASH_COMMENT     0x2
#define RETURN_FOR_DELIM            0x4
#define RETURN_FOR_COMMENT          0x8

static
#ifndef NO_INLINE_KEYWORD
MSC_IZE(inline)
#endif
long SPAN(Scheme_Object *port, long pos) {
  long cpos;
  scheme_tell_all(port, NULL, NULL, &cpos);
  return cpos - pos + 1;
}

#define SRCLOC_TMPL " in %q[%L%ld]"

#define mz_shape_cons 0
#define mz_shape_vec 1
#define mz_shape_hash_list 2
#define mz_shape_hash_elem 3

typedef struct Readtable {
  Scheme_Object so;
  Scheme_Hash_Table *mapping; /* pos int -> (cons int proc-or-char); neg int -> proc */
  char *fast_mapping;
} Readtable;

typedef struct ReadParams {
  MZTAG_IF_REQUIRED
  int can_read_compiled;
  int can_read_pipe_quote;
  int can_read_box;
  int can_read_graph;
  int can_read_reader;
  int case_sensitive;
  int square_brackets_are_parens;
  int curly_braces_are_parens;
  int read_decimal_inexact;
  int can_read_dot;
  int can_read_quasi;
  int honu_mode;
  Readtable *table;
} ReadParams;

#define THREAD_FOR_LOCALS scheme_current_thread
#define local_list_stack (THREAD_FOR_LOCALS->list_stack)
#define local_list_stack_pos (THREAD_FOR_LOCALS->list_stack_pos)
#define local_rename_memory (THREAD_FOR_LOCALS->rn_memory)


static Scheme_Object *read_list(Scheme_Object *port, Scheme_Object *stxsrc,
				long line, long col, long pos,
				int closer,
				int shape, int use_stack,
				Scheme_Hash_Table **ht,
				Scheme_Object *indentation,
				ReadParams *params);
static Scheme_Object *read_string(int is_byte, int is_honu_char,
				  Scheme_Object *port, Scheme_Object *stxsrc,
				  long line, long col, long pos,
				  Scheme_Hash_Table **ht,
				  Scheme_Object *indentation,
				  ReadParams *params);
static Scheme_Object *read_here_string(Scheme_Object *port, Scheme_Object *stxsrc,
				       long line, long col, long pos,
				       Scheme_Object *indentation,
				       ReadParams *params);
static Scheme_Object *read_quote(char *who, Scheme_Object *quote_symbol, int len,
				 Scheme_Object *port, Scheme_Object *stxsrc,
				  long line, long col, long pos,
				 Scheme_Hash_Table **ht,
				 Scheme_Object *indentation,
				 ReadParams *params);
static Scheme_Object *read_vector(Scheme_Object *port, Scheme_Object *stxsrc,
				  long line, long col, long pos,
				  char closer,
				  long reqLen, const mzchar *reqBuffer,
				  Scheme_Hash_Table **ht,
				  Scheme_Object *indentation,
				  ReadParams *params);
static Scheme_Object *read_number(int init_ch,
				  Scheme_Object *port, Scheme_Object *stxsrc,
				  long line, long col, long pos,
				  int, int, int, int,
				  Scheme_Hash_Table **ht,
				  Scheme_Object *indentation,
				  ReadParams *params,
				  Readtable *table);
static Scheme_Object *read_symbol(int init_ch,
				  Scheme_Object *port, Scheme_Object *stxsrc,
				  long line, long col, long pos,
				  Scheme_Hash_Table **ht,
				  Scheme_Object *indentation,
				  ReadParams *params,
				  Readtable *table);
static Scheme_Object *read_character(Scheme_Object *port, Scheme_Object *stcsrc,
				     long line, long col, long pos,
				     Scheme_Hash_Table **ht,
				     Scheme_Object *indentation,
				     ReadParams *params);
static Scheme_Object *read_box(Scheme_Object *port, Scheme_Object *stxsrc,
			       long line, long col, long pos,
			       Scheme_Hash_Table **ht,
			       Scheme_Object *indentation,
			       ReadParams *params);
static Scheme_Object *read_hash(Scheme_Object *port, Scheme_Object *stxsrc,
				long line, long col, long pos,
				char closer, int eq,
				Scheme_Hash_Table **ht,
				Scheme_Object *indentation,
				ReadParams *params);
static Scheme_Object *read_reader(Scheme_Object *port, Scheme_Object *stxsrc,
				  long line, long col, long pos,
				  Scheme_Hash_Table **ht,
				  Scheme_Object *indentation,
				  ReadParams *params);
static Scheme_Object *read_compiled(Scheme_Object *port,
				    Scheme_Hash_Table **ht);
static void unexpected_closer(int ch,
			      Scheme_Object *port, Scheme_Object *stxsrc,
			      long line, long col, long pos,
			      Scheme_Object *indentation);
static void pop_indentation(Scheme_Object *indentation);

static int skip_whitespace_comments(Scheme_Object *port, Scheme_Object *stxsrc,
				    Scheme_Hash_Table **ht,
				    Scheme_Object *indentation,
				    ReadParams *params);

static Scheme_Object *copy_to_protect_placeholders(Scheme_Object *v, Scheme_Object *src, Scheme_Hash_Table **ht);

#define READTABLE_WHITESPACE 0x1
#define READTABLE_CONTINUING 0x2
#define READTABLE_TERMINATING 0x4
#define READTABLE_SINGLE_ESCAPE 0x8
#define READTABLE_MULTIPLE_ESCAPE 0x10
#define READTABLE_MAPPED 0x20
static int readtable_kind(Readtable *t, int ch, ReadParams *params);
static Scheme_Object *readtable_handle(Readtable *t, int *_ch, int *_use_default, ReadParams *params,
				       Scheme_Object *port, Scheme_Object *src, long line, long col, long pos,
				       Scheme_Hash_Table **ht);
static Scheme_Object *readtable_handle_hash(Readtable *t, int ch, int *_use_default, ReadParams *params,
					    Scheme_Object *port, Scheme_Object *src, long line, long col, long pos,
					    Scheme_Hash_Table **ht);
static Scheme_Object *make_readtable(int argc, Scheme_Object **argv);
static Scheme_Object *readtable_p(int argc, Scheme_Object **argv);
static Scheme_Object *readtable_mapping(int argc, Scheme_Object **argv);
static Scheme_Object *current_readtable(int argc, Scheme_Object **argv);
static Scheme_Object *current_reader_guard(int argc, Scheme_Object **argv);

/* A list stack is used to speed up the creation of intermediate lists
   during .zo reading. */

#define NUM_CELLS_PER_STACK 500

typedef struct {
  int pos;
  Scheme_Simple_Object *stack;
} ListStackRec;

#define STACK_START(r) (r.pos = local_list_stack_pos, r.stack = local_list_stack)
#define STACK_END(r) (local_list_stack_pos = r.pos, local_list_stack = r.stack)

#ifdef MZ_PRECISE_GC
# define USE_LISTSTACK(x) 0
#else
# define USE_LISTSTACK(x) x
#endif

#ifdef MZ_PRECISE_GC
static void register_traversers(void);
#endif

typedef struct {
  Scheme_Type type;
  char closer;      /* expected close parent, bracket, etc. */
  char suspicious_closer; /* expected closer when suspicious line found */
  char multiline;   /* set to 1 if the match attempt spans a line */
  char quote_for_char; /* 1 => suspicious_quote refers to Honu char */
  long start_line;  /* opener's line */
  long last_line;   /* current line, already checked the identation */
  long suspicious_line; /* non-0 => first suspicious line since opener */
  long max_indent;  /* max indentation encountered so far since opener,
		       not counting indentation brackets by a more neseted
		       opener */
  long suspicious_quote; /* non-0 => first suspicious quote whose closer
			    is on a different line */
} Scheme_Indent;

static Scheme_Object *quote_symbol;
static Scheme_Object *quasiquote_symbol;
static Scheme_Object *unquote_symbol;
static Scheme_Object *unquote_splicing_symbol;
static Scheme_Object *syntax_symbol;
static Scheme_Object *unsyntax_symbol;
static Scheme_Object *unsyntax_splicing_symbol;
static Scheme_Object *quasisyntax_symbol;

static Scheme_Object *honu_comma, *honu_semicolon;
static Scheme_Object *honu_parens, *honu_braces, *honu_brackets;

static Scheme_Object *terminating_macro_symbol, *non_terminating_macro_symbol, *dispatch_macro_symbol;
static char *builtin_fast;

/* For recoginizing unresolved hash tables and commented-out graph introductions: */
static Scheme_Object *an_uninterned_symbol;

/* Table of built-in variable refs for .zo loading: */
static Scheme_Object **variable_references;

static unsigned char delim[128];
#define SCHEME_OK          0x1
#define HONU_OK            0x2
#define HONU_SYM_OK        0x4
#define HONU_NUM_OK        0x8
#define HONU_INUM_OK       0x10
#define HONU_INUM_SIGN_OK  0x20

/*========================================================================*/
/*                             initialization                             */
/*========================================================================*/

void scheme_init_read(Scheme_Env *env)
{
  REGISTER_SO(variable_references);

  REGISTER_SO(quote_symbol);
  REGISTER_SO(quasiquote_symbol);
  REGISTER_SO(unquote_symbol);
  REGISTER_SO(unquote_splicing_symbol);
  REGISTER_SO(syntax_symbol);
  REGISTER_SO(unsyntax_symbol);
  REGISTER_SO(unsyntax_splicing_symbol);
  REGISTER_SO(quasisyntax_symbol);
  REGISTER_SO(an_uninterned_symbol);

  quote_symbol = scheme_intern_symbol("quote");
  quasiquote_symbol = scheme_intern_symbol("quasiquote");
  unquote_symbol = scheme_intern_symbol("unquote");
  unquote_splicing_symbol = scheme_intern_symbol("unquote-splicing");
  syntax_symbol = scheme_intern_symbol("syntax");
  unsyntax_symbol = scheme_intern_symbol("unsyntax");
  unsyntax_splicing_symbol = scheme_intern_symbol("unsyntax-splicing");
  quasisyntax_symbol = scheme_intern_symbol("quasisyntax");

  an_uninterned_symbol = scheme_make_symbol("unresolved");

  REGISTER_SO(honu_comma);
  REGISTER_SO(honu_semicolon);
  REGISTER_SO(honu_parens);
  REGISTER_SO(honu_braces);
  REGISTER_SO(honu_brackets);

  honu_comma = scheme_intern_symbol(",");
  honu_semicolon = scheme_intern_symbol(";");
  honu_parens = scheme_intern_symbol("#%parens");
  honu_braces = scheme_intern_symbol("#%braces");
  honu_brackets = scheme_intern_symbol("#%brackets");

  {
    int i;
    for (i = 0; i < 128; i++) {
      delim[i] = SCHEME_OK;
    }
    for (i = 'A'; i <= 'Z'; i++) {
      delim[i] |= HONU_OK;
      delim[i + ('a'-'A')] |= HONU_OK;
    }
    for (i = '0'; i <= '9'; i++) {
      delim[i] |= (HONU_OK | HONU_NUM_OK);
    }
    delim['('] -= SCHEME_OK;
    delim[')'] -= SCHEME_OK;
    delim['['] -= SCHEME_OK;
    delim[']'] -= SCHEME_OK;
    delim['{'] -= SCHEME_OK;
    delim['}'] -= SCHEME_OK;
    delim['"'] -= SCHEME_OK;
    delim['\''] -= SCHEME_OK;
    delim[','] -= SCHEME_OK;
    delim[';'] -= SCHEME_OK;
    delim['`'] -= SCHEME_OK;
    delim['_'] |= HONU_OK;
    {
      GC_CAN_IGNORE const char *syms = "+-_=?:<>.!%^&*/~|";
      for (i = 0; syms[i]; i++) {
	delim[(int)syms[i]] |= HONU_SYM_OK;
      }
    }
    delim['.'] |= HONU_NUM_OK;
    delim['e'] |= HONU_INUM_OK;
    delim['E'] |= HONU_INUM_OK;
    delim['d'] |= HONU_INUM_OK;
    delim['D'] |= HONU_INUM_OK;
    delim['f'] |= HONU_INUM_OK;
    delim['F'] |= HONU_INUM_OK;
    delim['+'] |= HONU_INUM_SIGN_OK;
    delim['-'] |= HONU_INUM_SIGN_OK;
  }

#ifdef MZ_PRECISE_GC
  register_traversers();
#endif

  scheme_add_global_constant("current-readtable",
			     scheme_register_parameter(current_readtable,
						       "current-readtable",
						       MZCONFIG_READTABLE),
			     env);
  scheme_add_global_constant("current-reader-guard",
			     scheme_register_parameter(current_reader_guard,
						       "current-reader-guard",
						       MZCONFIG_READER_GUARD),
			     env);

  scheme_add_global_constant("read-case-sensitive",
			     scheme_register_parameter(read_case_sensitive,
						       "read-case-sensitive",
						       MZCONFIG_CASE_SENS),
			     env);
  scheme_add_global_constant("read-square-bracket-as-paren",
			     scheme_register_parameter(read_bracket_as_paren,
						       "read-square-bracket-as-paren",
						       MZCONFIG_SQUARE_BRACKETS_ARE_PARENS),
			     env);
  scheme_add_global_constant("read-curly-brace-as-paren",
			     scheme_register_parameter(read_brace_as_paren,
						       "read-curly-brace-as-paren",
						       MZCONFIG_CURLY_BRACES_ARE_PARENS),
			     env);
  scheme_add_global_constant("read-accept-graph",
			     scheme_register_parameter(read_accept_graph,
						       "read-accept-graph",
						       MZCONFIG_CAN_READ_GRAPH),
			     env);
  scheme_add_global_constant("read-accept-compiled",
			     scheme_register_parameter(read_accept_compiled,
						       "read-accept-compiled",
						       MZCONFIG_CAN_READ_COMPILED),
			     env);
  scheme_add_global_constant("read-accept-box",
			     scheme_register_parameter(read_accept_box,
						       "read-accept-box",
						       MZCONFIG_CAN_READ_BOX),
			     env);
  scheme_add_global_constant("read-accept-bar-quote",
			     scheme_register_parameter(read_accept_pipe_quote,
						       "read-accept-bar-quote",
						       MZCONFIG_CAN_READ_PIPE_QUOTE),
			     env);
  scheme_add_global_constant("read-decimal-as-inexact",
			     scheme_register_parameter(read_decimal_as_inexact,
						       "read-decimal-as-inexact",
						       MZCONFIG_READ_DECIMAL_INEXACT),
			     env);
  scheme_add_global_constant("read-accept-dot",
			     scheme_register_parameter(read_accept_dot,
						       "read-accept-dot",
						       MZCONFIG_CAN_READ_DOT),
			     env);
  scheme_add_global_constant("read-accept-quasiquote",
			     scheme_register_parameter(read_accept_quasi,
						       "read-accept-quasiquote",
						       MZCONFIG_CAN_READ_QUASI),
			     env);
  scheme_add_global_constant("read-accept-reader",
			     scheme_register_parameter(read_accept_reader,
						       "read-accept-reader",
						       MZCONFIG_CAN_READ_READER),
			     env);
  scheme_add_global_constant("print-graph",
			     scheme_register_parameter(print_graph,
						       "print-graph",
						       MZCONFIG_PRINT_GRAPH),
			     env);
  scheme_add_global_constant("print-struct",
			     scheme_register_parameter(print_struct,
						       "print-struct",
						       MZCONFIG_PRINT_STRUCT),
			     env);
  scheme_add_global_constant("print-box",
			     scheme_register_parameter(print_box,
						       "print-box",
						       MZCONFIG_PRINT_BOX),
			     env);
  scheme_add_global_constant("print-vector-length",
			     scheme_register_parameter(print_vec_shorthand,
						       "print-vector-length",
						       MZCONFIG_PRINT_VEC_SHORTHAND),
			     env);
  scheme_add_global_constant("print-hash-table",
			     scheme_register_parameter(print_hash_table,
						       "print-hash-table",
						       MZCONFIG_PRINT_HASH_TABLE),
			     env);
  scheme_add_global_constant("print-unreadable",
			     scheme_register_parameter(print_unreadable,
						       "print-unreadable",
						       MZCONFIG_PRINT_UNREADABLE),
			     env);

  scheme_add_global_constant("print-honu",
			     scheme_register_parameter(print_honu,
						       "print-honu",
						       MZCONFIG_HONU_MODE),
			     env);

  scheme_add_global_constant("make-readtable",
			     scheme_make_prim_w_arity(make_readtable,
						      "make-readtable",
						      1, -1),
			     env);
  scheme_add_global_constant("readtable?",
			     scheme_make_folding_prim(readtable_p,
						      "readtable?",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("readtable-mapping",
			     scheme_make_prim_w_arity2(readtable_mapping,
						       "readtable-mapping",
						       2, 2,
						       3, 3),
			     env);
}

void scheme_alloc_list_stack(Scheme_Thread *p)
{
  Scheme_Simple_Object *sa;
  p->list_stack_pos = 0;
  sa = MALLOC_N_RT(Scheme_Simple_Object, NUM_CELLS_PER_STACK);
  p->list_stack = sa;
#ifdef MZ_PRECISE_GC
  /* Must set the tag on the first element: */
  p->list_stack[0].iso.so.type = scheme_pair_type;
#endif
}

void scheme_clean_list_stack(Scheme_Thread *p)
{
  if (p->list_stack) {
    memset(p->list_stack + p->list_stack_pos, 0,
	   (NUM_CELLS_PER_STACK - p->list_stack_pos) * sizeof(Scheme_Simple_Object));
  }
}

static void track_indentation(Scheme_Object *indentation, int line, int col)
{
  if (!SCHEME_NULLP(indentation)) {
    Scheme_Indent *indt = (Scheme_Indent *)SCHEME_CAR(indentation);
    /* Already checked this line? */
    if (line > indt->last_line) {
      indt->last_line = line;
      indt->multiline = 1;
      /* At least as indented as before? */
      if (col >= indt->max_indent)
	indt->max_indent = col;
      else if (!indt->suspicious_line) {
	/* Not as indented, and no suspicious line found
	   already. Suspect that the closer should have
	   appeared earlier. */
	indt->suspicious_closer = indt->closer;
	indt->suspicious_line = line;
      }
    }
  }
}

/*========================================================================*/
/*                             parameters                                 */
/*========================================================================*/

#define DO_CHAR_PARAM(name, pos) \
  return scheme_param_config(name, scheme_make_integer(pos), argc, argv, -1, NULL, NULL, 1)

static Scheme_Object *
read_case_sensitive(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-case-sensitive", MZCONFIG_CASE_SENS);
}

static Scheme_Object *
read_bracket_as_paren(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-square-bracket-as-paren", MZCONFIG_SQUARE_BRACKETS_ARE_PARENS);
}

static Scheme_Object *
read_brace_as_paren(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-curly-brace-as-paren", MZCONFIG_CURLY_BRACES_ARE_PARENS);
}

static Scheme_Object *
read_accept_graph(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-accept-graph", MZCONFIG_CAN_READ_GRAPH);
}

static Scheme_Object *
read_accept_compiled(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-accept-compiled", MZCONFIG_CAN_READ_COMPILED);
}

static Scheme_Object *
read_accept_box(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-accept-box", MZCONFIG_CAN_READ_BOX);
}

static Scheme_Object *
read_accept_pipe_quote(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-accept-pipe-quote", MZCONFIG_CAN_READ_PIPE_QUOTE);
}

static Scheme_Object *
read_decimal_as_inexact(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-decimal-as-inexact", MZCONFIG_READ_DECIMAL_INEXACT);
}

static Scheme_Object *
read_accept_dot(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-accept-dot", MZCONFIG_CAN_READ_DOT);
}

static Scheme_Object *
read_accept_quasi(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-accept-quasiquote", MZCONFIG_CAN_READ_QUASI);
}

static Scheme_Object *
read_accept_reader(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-accept-reader", MZCONFIG_CAN_READ_READER);
}

static Scheme_Object *
print_graph(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("print-graph", MZCONFIG_PRINT_GRAPH);
}

static Scheme_Object *
print_struct(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("print-struct", MZCONFIG_PRINT_STRUCT);
}

static Scheme_Object *
print_box(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("print-box", MZCONFIG_PRINT_BOX);
}

static Scheme_Object *
print_vec_shorthand(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("print-vector-length", MZCONFIG_PRINT_VEC_SHORTHAND);
}

static Scheme_Object *
print_hash_table(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("print-vector-length", MZCONFIG_PRINT_HASH_TABLE);
}

static Scheme_Object *
print_unreadable(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("print-unreadable", MZCONFIG_PRINT_UNREADABLE);
}

static Scheme_Object *
print_honu(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("print-honu", MZCONFIG_HONU_MODE);
}

/*========================================================================*/
/*                             main read loop                             */
/*========================================================================*/

#ifdef DO_STACK_CHECK

static Scheme_Object *read_inner_inner(Scheme_Object *port,
				       Scheme_Object *stxsrc,
				       Scheme_Hash_Table **ht,
				       Scheme_Object *indentation,
				       ReadParams *params,
				       int comment_mode,
				       int pre_char,
				       Readtable *init_readtable);
static Scheme_Object *read_inner(Scheme_Object *port, 
				 Scheme_Object *stxsrc, 
				 Scheme_Hash_Table **ht,
				 Scheme_Object *indentation, 
				 ReadParams *params,
				 int comment_mode);

static Scheme_Object *read_inner_inner_k(void)
{
  Scheme_Thread *p = scheme_current_thread;
  Scheme_Object *o = (Scheme_Object *)p->ku.k.p1;
  Scheme_Hash_Table **ht = (Scheme_Hash_Table **)p->ku.k.p2;
  Scheme_Object *stxsrc = (Scheme_Object *)p->ku.k.p3;
  Scheme_Object *indentation = SCHEME_CAR((Scheme_Object *)p->ku.k.p4);
  ReadParams *params = (ReadParams *)SCHEME_CDR((Scheme_Object *)p->ku.k.p4);
  Readtable *table = (Readtable *)p->ku.k.p5;

  p->ku.k.p1 = NULL;
  p->ku.k.p2 = NULL;
  p->ku.k.p3 = NULL;
  p->ku.k.p4 = NULL;
  p->ku.k.p5 = NULL;

  return read_inner_inner(o, stxsrc, ht, indentation, params, p->ku.k.i1, p->ku.k.i2, table);
}
#endif

#define MAX_GRAPH_ID_DIGITS 8

static Scheme_Object *
read_inner_inner(Scheme_Object *port, Scheme_Object *stxsrc, Scheme_Hash_Table **ht,
		 Scheme_Object *indentation, ReadParams *params,
		 int comment_mode, int pre_char, Readtable *table)
{
  int ch, ch2, depth, dispatch_ch;
  long line = 0, col = 0, pos = 0;
  Scheme_Object *special_value;

#ifdef DO_STACK_CHECK
  {
# include "mzstkchk.h"
    {
      Scheme_Thread *p = scheme_current_thread;
      Scheme_Object *pr;
      ReadParams *params2;

      /* params may be on the stack, so move it to the heap: */
      params2 = MALLOC_ONE_RT(ReadParams);
      memcpy(params2, params, sizeof(ReadParams));
#ifdef MZ_PRECISE_GC
      params2->type = scheme_rt_read_params;
#endif

      p->ku.k.p1 = (void *)port;
      p->ku.k.p2 = (void *)ht;
      p->ku.k.p3 = (void *)stxsrc;

      pr = scheme_make_pair(indentation, (Scheme_Object *)params2);
      p->ku.k.p4 = (void *)pr;

      p->ku.k.p5 = (void *)table;

      p->ku.k.i1 = comment_mode;
      p->ku.k.i2 = pre_char;
      return scheme_handle_stack_overflow(read_inner_inner_k);
    }
  }
#endif

 start_over:

  SCHEME_USE_FUEL(1);

  while (1) {
    if (pre_char >= 0) {
      ch = pre_char;
      pre_char = -1;
    } else
      ch = scheme_getc_special_ok(port);
    if (NOT_EOF_OR_SPECIAL(ch)) {
      if (table) {
	if (!(readtable_kind(table, ch, params) & READTABLE_WHITESPACE))
	  break;
      } else if (!scheme_isspace(ch))
	break;
    } else
      break;
  }

 start_over_with_ch:

  scheme_tell_all(port, &line, &col, &pos);

  /* Found non-whitespace. Track indentation: */
  if (col >= 0) {
    if (SCHEME_PAIRP(indentation)) {
      /* Ignore if it's a comment start or spurious closer: */
      if ((ch != ';')
	  && !((ch == '#') && (scheme_peekc_special_ok(port) == '|'))
	  && (ch != ')')
	  && ((ch != '}') || !params->curly_braces_are_parens)
	  && ((ch != ']') || !params->square_brackets_are_parens)) {
	track_indentation(indentation, line, col);
      }
    }
  }

  special_value = NULL;
  if (table && NOT_EOF_OR_SPECIAL(ch)) {
    Scheme_Object *v;
    int use_default, ch2 = ch;
    v = readtable_handle(table, &ch2, &use_default, params,
			 port, stxsrc, line, col, pos, ht);
    if (!use_default) {
      dispatch_ch = SCHEME_SPECIAL;
      special_value = v;
    } else
      dispatch_ch = ch2;
  } else
    dispatch_ch = ch;

  switch ( dispatch_ch )
    {
    case EOF: 
      return scheme_eof;
    case SCHEME_SPECIAL:
      {
	Scheme_Object *v;
	if (special_value)
	  v = special_value;
	else
	  v = scheme_get_special(port, stxsrc, line, col, pos, 0, ht);
	if (scheme_special_comment_value(v)) {
	  /* a "comment" */
	  if (comment_mode & RETURN_FOR_SPECIAL_COMMENT)
	    return NULL;
	  else
	    goto start_over;
	} else if (SCHEME_STXP(v)) {
	  if (!stxsrc)
	    v = scheme_syntax_to_datum(v, 0, NULL);
	} else if (stxsrc) {
	  Scheme_Object *s;
	  s = scheme_make_stx_w_offset(scheme_false, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG);
	  v = scheme_datum_to_syntax(v, s, scheme_false, 1, 0);
	}
	if (!special_value)
	  v = copy_to_protect_placeholders(v, stxsrc, ht);
	return v;
      }
    case ']':
      if (!params->square_brackets_are_parens) {
	scheme_read_err(port, stxsrc, line, col, pos, 1, 0, indentation, "read: illegal use of close square bracket");
	return NULL;
      } else {
	unexpected_closer(ch, port, stxsrc, line, col, pos, indentation);
	return NULL;
      }
    case '}':
      if (!params->curly_braces_are_parens) {
	scheme_read_err(port, stxsrc, line, col, pos, 1, 0, indentation, "read: illegal use of close curly brace");
	return NULL;
      } else {
	unexpected_closer(ch, port, stxsrc, line, col, pos, indentation);
	return NULL;
      }
    case ')':
      unexpected_closer(ch, port, stxsrc, line, col, pos, indentation);
      return NULL;
    case '(':
      return read_list(port, stxsrc, line, col, pos, ')', mz_shape_cons, 0, ht, indentation, params);
    case '[':
      if (!params->square_brackets_are_parens) {
	scheme_read_err(port, stxsrc, line, col, pos, 1, 0, indentation, "read: illegal use of open square bracket");
	return NULL;
      } else
	return read_list(port, stxsrc, line, col, pos, ']', mz_shape_cons, 0, ht, indentation, params);
    case '{':
      if (!params->curly_braces_are_parens) {
	scheme_read_err(port, stxsrc, line, col, pos, 1, 0, indentation, "read: illegal use of open curly brace");
	return NULL;
      } else
	return read_list(port, stxsrc, line, col, pos, '}', mz_shape_cons, 0, ht, indentation, params);
    case '|':
      return read_symbol(ch, port, stxsrc, line, col, pos, ht, indentation, params, table);
    case '"':
      return read_string(0, 0, port, stxsrc, line, col, pos, ht, indentation, params);
    case '\'':
      if (params->honu_mode) {
	return read_string(0, 1, port, stxsrc, line, col, pos, ht, indentation, params);
      } else {
	return read_quote("quoting '", quote_symbol, 1, port, stxsrc, line, col, pos, ht, indentation, params);
      }
    case '`':
      if (params->honu_mode) {
	/* Raises illegal-char error: */
	return read_symbol(ch, port, stxsrc, line, col, pos, ht, indentation, params, table);
      } else if (!params->can_read_quasi) {
	scheme_read_err(port, stxsrc, line, col, pos, 1, 0, indentation, "read: illegal use of backquote");
	return NULL;
      } else
	return read_quote("quasiquoting `", quasiquote_symbol, 1, port, stxsrc, line, col, pos, ht, indentation, params);
    case ',':
      if (params->honu_mode) {
	if (stxsrc)
	  return scheme_make_stx_w_offset(honu_comma, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG);
	else
	  return honu_comma;
      } else if (!params->can_read_quasi) {
	scheme_read_err(port, stxsrc, line, col, pos, 1, 0, indentation, "read: illegal use of comma");
	return NULL;
      } else {
	if (scheme_peekc_special_ok(port) == '@') {
	  ch = scheme_getc(port); /* must be '@' */
	  return read_quote("unquoting ,@", unquote_splicing_symbol, 2, port, stxsrc, line, col, pos, ht, indentation, params);
	} else
	  return read_quote("unquoting ,", unquote_symbol, 1, port, stxsrc, line, col, pos, ht, indentation, params);
      }
    case ';':
      if (params->honu_mode) {
	if (stxsrc)
	  return scheme_make_stx_w_offset(honu_semicolon, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG);
	else
	  return honu_semicolon;
      } else {
	while (((ch = scheme_getc_special_ok(port)) != '\n') && (ch != '\r')) {
	  if (ch == EOF)
	    return scheme_eof;
	  if (ch == SCHEME_SPECIAL)
	    scheme_get_ready_read_special(port, stxsrc, ht);
	}
	if ((table && (comment_mode & RETURN_FOR_SPECIAL_COMMENT))
	    || (comment_mode & RETURN_FOR_COMMENT))
	  return NULL;
	goto start_over;
      }
    case '+':
    case '-':
      if (params->honu_mode) {
	return read_symbol(ch, port, stxsrc, line, col, pos, ht, indentation, params, table);
      }
    case '.': /* ^^^ fallthrough ^^^ */
      ch2 = scheme_peekc_special_ok(port);
      if ((NOT_EOF_OR_SPECIAL(ch2) && isdigit_ascii(ch2)) || (ch2 == '.')
	  || (!params->honu_mode
	      && ((ch2 == 'i') || (ch2 == 'I') /* Maybe inf */
		  || (ch2 == 'n') || (ch2 == 'N') /* Maybe nan*/ ))) {
	return read_number(ch, port, stxsrc, line, col, pos, 0, 0, 10, 0, ht, indentation, params, table);
      } else {
	return read_symbol(ch, port, stxsrc, line, col, pos, ht, indentation, params, table);
      }
    case '#':
      ch = scheme_getc_special_ok(port);

      if (table) {
	Scheme_Object *v;
	int use_default;
	v = readtable_handle_hash(table, ch, &use_default, params,
				  port, stxsrc, line, col, pos, ht);
	if (!use_default) {
	  if (v)
	    return v;
	  if (comment_mode & RETURN_FOR_SPECIAL_COMMENT)
	    return NULL;
	  goto start_over;
	}
      }

      switch ( ch )
	{
	case EOF:
	case SCHEME_SPECIAL:
	  scheme_read_err(port, stxsrc, line, col, pos, 1, ch, indentation, "read: bad syntax `#'");
	  break;
	case ';':
	  {
	    Scheme_Object *skipped;
	    skipped = read_inner(port, stxsrc, ht, indentation, params, 0);
	    if (SCHEME_EOFP(skipped))
	      scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), EOF, indentation,
			      "read: expected a commented-out element for `#;' (found end-of-file)");
	    /* For resolving graphs introduced in #; : */
	    if (*ht) {
	      Scheme_Object *v;
	      v = scheme_hash_get(*ht, an_uninterned_symbol);
	      if (!v)
		v = scheme_null;
	      v = scheme_make_pair(skipped, v);
	      scheme_hash_set(*ht, an_uninterned_symbol, v);
	    }

	    if ((comment_mode & RETURN_FOR_HASH_COMMENT)
		|| (table && (comment_mode & RETURN_FOR_SPECIAL_COMMENT))
		|| (comment_mode & RETURN_FOR_COMMENT))
	      return NULL;

	    goto start_over;
	  }
	  break;
	case '%':
	  if (!params->honu_mode) {
	    scheme_ungetc('%', port);
	    return read_symbol('#', port, stxsrc, line, col, pos, ht, indentation, params, table);
	  }
	  break;
	case '(':
	  if (!params->honu_mode) {
	    return read_vector(port, stxsrc, line, col, pos, ')', -1, NULL, ht, indentation, params);
	  }
	  break;
	case '[':
	  if (!params->honu_mode) {
	    if (!params->square_brackets_are_parens) {
	      scheme_read_err(port, stxsrc, line, col, pos, 2, 0, indentation, "read: bad syntax `#['");
	      return NULL;
	    } else
	      return read_vector(port, stxsrc, line, col, pos, ']', -1, NULL, ht, indentation, params);
	  }
	  break;
	case '{':
	  if (!params->honu_mode) {
	    if (!params->curly_braces_are_parens) {
	      scheme_read_err(port, stxsrc, line, col, pos, 2, 0, indentation, "read: bad syntax `#{'");
	      return NULL;
	    } else
	      return read_vector(port, stxsrc, line, col, pos, '}', -1, NULL, ht, indentation, params);
	  }
	  break;
	case '\\':
	  if (!params->honu_mode) {
	    Scheme_Object *chr;
	    chr = read_character(port, stxsrc, line, col, pos, ht, indentation, params);
	    if (stxsrc)
	      chr = scheme_make_stx_w_offset(chr, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG);
	    return chr;
	  }
	  break;
	case 'T':
	case 't': 
	  if (!params->honu_mode) {
	    return (stxsrc
		    ? scheme_make_stx_w_offset(scheme_true, line, col, pos, 2, stxsrc, STX_SRCTAG)
		    : scheme_true);
	  }
	case 'F':
	case 'f': 
	  if (!params->honu_mode) {
	    return (stxsrc
		    ? scheme_make_stx_w_offset(scheme_false, line, col, pos, 2, stxsrc, STX_SRCTAG)
		    : scheme_false);
	  }
	case 'c':
	case 'C':
	  if (!params->honu_mode) {
	    Scheme_Object *v;
	    int sens = 0;
	    int save_sens;

	    ch = scheme_getc_special_ok(port);
	    switch ( ch ) {
	    case 'i':
	    case 'I':
	      sens = 0;
	      break;
	    case 's':
	    case 'S':
	      sens = 1;
	      break;
	    default:
	      scheme_read_err(port, stxsrc, line, col, pos, 2, ch, indentation,
			      "read: expected `s' or `i' after #c");
	      return NULL;
	    }


	    save_sens = params->case_sensitive;
	    params->case_sensitive = sens;
	    
	    v = read_inner(port, stxsrc, ht, indentation, params, 0);
	    
	    params->case_sensitive = save_sens;
	    
	    if (SCHEME_EOFP(v)) {
	      scheme_read_err(port, stxsrc, line, col, pos, 2, EOF, indentation,
			      "read: end-of-file after #c%c",
			      sens ? 's' : 'i');
	      return NULL;
	    }

	    return v;
	  }
	  break;
	case 's':
	case 'S':
	  ch = scheme_getc_special_ok(port);
	  if ((ch == 'x') || (ch == 'X')) {
	    ReadParams params_copy;
	    Scheme_Object *v;

	    memcpy(&params_copy, params, sizeof(ReadParams));
	    params_copy.honu_mode = 0;

	    v = read_inner(port, stxsrc, ht, indentation, &params_copy, 0);

	    if (SCHEME_EOFP(v)) {
	      scheme_read_err(port, stxsrc, line, col, pos, 2, EOF, indentation,
			      "read: end-of-file after #sx");
	      return NULL;
	    }

	    return v;
	  } else {
	    scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), ch, indentation,
			    "read: expected `x' after `#s'");
	    return NULL;
	  }
	case 'X':
	case 'x': 
	  if (!params->honu_mode) {
	    return read_number(-1, port, stxsrc, line, col, pos, 0, 0, 16, 1, ht, indentation, params, table);
	  }
	  break;
	case 'B':
	case 'b': 
	  if (!params->honu_mode) {
	    return read_number(-1, port, stxsrc, line, col, pos, 0, 0, 2, 1, ht, indentation, params, table);
	  }
	  break;
	case 'O':
	case 'o': 
	  if (!params->honu_mode) {
	    return read_number(-1, port, stxsrc, line, col, pos, 0, 0, 8, 1, ht, indentation, params, table);
	  }
	  break;
	case 'D':
	case 'd': 
	  if (!params->honu_mode) {
	    return read_number(-1, port, stxsrc, line, col, pos, 0, 0, 10, 1, ht, indentation, params, table);
	  }
	  break;
	case 'E':
	case 'e': 
	  if (!params->honu_mode) {
	    return read_number(-1, port, stxsrc, line, col, pos, 0, 1, 10, 0, ht, indentation, params, table);
	  }
	  break;
	case 'I':
	case 'i': 
	  if (!params->honu_mode) {
	    return read_number(-1, port, stxsrc, line, col, pos, 1, 0, 10, 0, ht, indentation, params, table);
	  }
	  break;
	case '\'':
	  if (!params->honu_mode) {
	    return read_quote("quoting #'", syntax_symbol, 2, port, stxsrc, line, col, pos, ht, indentation, params);
	  }
	  break;
	case '`':
	  if (!params->honu_mode) {
	    return read_quote("quasiquoting #`", quasisyntax_symbol, 2, port, stxsrc, line, col, pos, ht, indentation, params);
	  }
	  break;
	case ',':
	  if (!params->honu_mode) {
	    if (scheme_peekc_special_ok(port) == '@') {
	      ch = scheme_getc(port); /* must be '@' */
	      return read_quote("unquoting #`@", unsyntax_splicing_symbol, 3, port, stxsrc, line, col, pos, ht, indentation, params);
	    } else
	      return read_quote("unquoting #`", unsyntax_symbol, 2, port, stxsrc, line, col, pos, ht, indentation, params);
	  }
	  break;
	case '~':
	  if (!params->honu_mode) {
	    if (params->can_read_compiled) {
	      Scheme_Object *cpld;
	      cpld = read_compiled(port, ht);
	      if (stxsrc)
		cpld = scheme_make_stx_w_offset(cpld, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG);
	      return cpld;
	    } else {
	      scheme_read_err(port, stxsrc, line, col, pos, 2, 0, indentation,
			      "read: #~ compiled expressions not currently enabled");
	      return NULL;
	    }
	  }
	  break;
	case '|':
	  if (!params->honu_mode) {
	    /* FIXME: integer overflow possible */
	    depth = 0;
	    ch2 = 0;
	    do {
	      ch = scheme_getc_special_ok(port);

	      if (ch == EOF)
		scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), EOF, indentation,
				"read: end of file in #| comment");
	      else if (ch == SCHEME_SPECIAL)
		scheme_get_ready_read_special(port, stxsrc, ht);

	      if ((ch2 == '|') && (ch == '#')) {
		if (!(depth--)) {
		  if ((table && (comment_mode & RETURN_FOR_SPECIAL_COMMENT))
		      || (comment_mode & RETURN_FOR_COMMENT))
		    return NULL;
		  goto start_over;
		}
	      } else if ((ch2 == '#') && (ch == '|')) {
		depth++;
		ch = 0; /* So we don't count '|' toward a closing "|#" */
	      }
	      ch2 = ch;
	    } while (1);
	  }
	  break;
	case '&':
	  if (!params->honu_mode) {
	    if (params->can_read_box)
	      return read_box(port, stxsrc, line, col, pos, ht, indentation, params);
	    else {
	      scheme_read_err(port, stxsrc, line, col, pos, 2, 0, indentation,
			      "read: #& expressions not currently enabled");
	      return NULL;
	    }
	  }
	  break;
	case 'r':
	  if (!params->honu_mode) {
	    int cnt = 0, is_byte = 0;
	    char *expect;

	    ch = scheme_getc_special_ok(port);
	    if (ch == 'x') {
	      expect = "x#";
	      ch = scheme_getc_special_ok(port);
	      cnt++;
	      if (ch == '#') {
		is_byte = 1;
		cnt++;
		ch = scheme_getc_special_ok(port);
	      }
	      if (ch == '"') {
		Scheme_Object *str;
		int is_err;

		/* Skip #rx[#]: */
		scheme_tell_all(port, &line, &col, &pos);

		str = read_string(is_byte, 0, port, stxsrc, line, col, pos, ht, indentation, params);

		if (stxsrc)
		  str = SCHEME_STX_VAL(str);

		str = scheme_make_regexp(str, is_byte, &is_err);

		if (is_err) {
		  scheme_read_err(port, stxsrc, line, col, pos, 2, 0, indentation,
				  "read: bad regexp string: %s", (char *)str);
		  return NULL;
		}

		if (stxsrc)
		  str = scheme_make_stx_w_offset(str, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG);

		return str;
	      }
	    } else if (ch == 'e') {
	      expect = "eader";
	      cnt++;
	      while (expect[cnt]) {
		ch = scheme_getc_special_ok(port);
		if (ch != expect[cnt])
		  break;
		cnt++;
	      }
	      if (!expect[cnt]) {
		/* Found #reader. Read an S-exp. */
		Scheme_Object *v;

		if (!params->can_read_reader) {
		  scheme_read_err(port, stxsrc, line, col, pos, 7, 0, indentation,
				  "read: #reader expressions not currently enabled");
		  return NULL;
		}

		v = read_reader(port, stxsrc, line, col, pos, ht, indentation, params);
		if (!v) {
		  if (comment_mode & RETURN_FOR_SPECIAL_COMMENT)
		    return NULL;
		  goto start_over;
		}
		return v;
	      }
	    } else
	      expect = "";

	    {
	      mzchar a[6];
	      int i;

	      for (i = 0; i < cnt; i++) {
		a[i] = expect[i];
	      }
	      if (NOT_EOF_OR_SPECIAL(ch)) {
		a[cnt++] = ch;
	      }

	      scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos),
			      ch, indentation,
			      "read: bad syntax `#r%u'",
			      a, cnt);
	      return NULL;
	    }
	  }
	  break;
	case 'h':
	  {
	    int honu = 0;

	    ch = scheme_getc_special_ok(port);
	    switch ( ch ) {
	    case 'a':
	      honu = 0;
	      break;
	    case 'o':
	      honu = -1;
	      break;
	    case 'x':
	      honu = 1;
	      break;
	    default:
	      if (!params->honu_mode) {
		scheme_read_err(port, stxsrc, line, col, pos, 2, ch, indentation,
				"read: expected `a', `u', or `o' after #h");
		return NULL;
	      }
	      honu = 0;
	      break;
	    }

	    if (params->honu_mode && (honu != 1)) {
	      scheme_read_err(port, stxsrc, line, col, pos, 2, ch, indentation,
			      "read: expected `x' after #h");
	      return NULL;
	    }

	    if (honu) {
	      ReadParams params_copy;
	      Scheme_Object *v;

	      if (honu == -1) {
		/* Check for "nu", still */
		ch = scheme_getc_special_ok(port);
		if (ch == 'n') {
		  ch = scheme_getc_special_ok(port);
		  if (ch == 'u') {
		    /* Done */
		  } else
		    scheme_read_err(port, stxsrc, line, col, pos, 4, ch, indentation,
				    "read: expected `u' after #hon");
		} else
		  scheme_read_err(port, stxsrc, line, col, pos, 3, ch, indentation,
			      "read: expected `nu' after #ho");
	      }

	      memcpy(&params_copy, params, sizeof(ReadParams));
	      params_copy.honu_mode = 1;

	      if (honu == 1) {
		v = read_inner(port, stxsrc, ht, indentation, params, 0);
		if (SCHEME_EOFP(v)) {
		  scheme_read_err(port, stxsrc, line, col, pos, 2, EOF, indentation,
				  "read: end-of-file after #hx");
		  return NULL;
		}
	      } else
		v = read_list(port, stxsrc, line, col, pos, EOF, mz_shape_cons, 0, ht, indentation, params);

	      return v;
	    } else {
	      GC_CAN_IGNORE const mzchar str[] = { 's', 'h', 'e', 'q', 0 };
	      int scanpos = 0, failed = 0;

	      do {
		ch = scheme_getc_special_ok(port);
		if ((mzchar)ch == str[scanpos]) {
		  scanpos++;
		} else {
		  if (scanpos == 2) {
		    if (!(ch == '(')
			&& ! (ch == '[' && params->square_brackets_are_parens)
			&& !(ch == '{' && params->curly_braces_are_parens))
		      failed = 1;
		  } else
		    failed = 1;
		  break;
		}
	      } while (str[scanpos]);

	      if (!failed) {
		/* Found recognized tag. Look for open paren... */
		if (scanpos > 2)
		  ch = scheme_getc_special_ok(port);

		if (ch == '(')
		  return read_hash(port, stxsrc, line, col, pos, ')', (scanpos == 4), ht, indentation, params);
		if (ch == '[' && params->square_brackets_are_parens)
		  return read_hash(port, stxsrc, line, col, pos, ']', (scanpos == 4), ht, indentation, params);
		if (ch == '{' && params->curly_braces_are_parens)
		  return read_hash(port, stxsrc, line, col, pos, '}', (scanpos == 4), ht, indentation, params);
	      }

	      /* Report an error. So far, we read 'ha', then scanpos chars of str, then ch. */
	      {
		mzchar str_part[7], one_more[2];

		memcpy(str_part, str, scanpos * sizeof(mzchar));
		str_part[scanpos] = 0;
		if (NOT_EOF_OR_SPECIAL(ch)) {
		  one_more[0] = ch;
		  one_more[1] = 0;
		} else
		  one_more[0] = 0;

		scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos),
				ch, indentation,
				"read: bad syntax `#ha%5%u'",
				str_part,
				one_more, NOT_EOF_OR_SPECIAL(ch) ? 1 : 0);
		return NULL;
	      }
	    }
	  }
	  break;
	case '"':
	  if (!params->honu_mode) {
	    return read_string(1, 0, port, stxsrc, line, col, pos, ht, indentation, params);
	  }
	  break;
	case '<':
	  if (!params->honu_mode) {
	    if (scheme_peekc_special_ok(port) == '<') {
	      /* Here-string */
	      ch = scheme_getc_special_ok(port);
	      return read_here_string(port, stxsrc, line, col, pos,indentation, params);
	    } else {
	      scheme_read_err(port, stxsrc, line, col, pos, 2, 0, indentation, "read: bad syntax `#<'");
	      return NULL;
	    }
	  }
	  break;
	default:
	  if (!params->honu_mode) {
	    int vector_length = -1;
	    int i = 0, j = 0, overflow = 0, digits = 0;
	    mzchar tagbuf[64], vecbuf[64]; /* just for errors */

	    while (NOT_EOF_OR_SPECIAL(ch) && isdigit_ascii(ch)) {
	      if (digits <= MAX_GRAPH_ID_DIGITS)
		digits++;

	      /* For vector error msgs, want to drop leading zeros: */
	      if (j || (ch != '0')) {
		if (j < 60) {
		  vecbuf[j++] = ch;
		} else if (j == 60) {
		  vecbuf[j++] = '.';
		  vecbuf[j++] = '.';
		  vecbuf[j++] = '.';
		  vecbuf[j] = 0;
		}
	      }

	      /* For tag error msgs, want to keep zeros: */
	      if (i < 60) {
		tagbuf[i++] = ch;
	      } else if (i == 60) {
		tagbuf[i++] = '.';
		tagbuf[i++] = '.';
		tagbuf[i++] = '.';
		tagbuf[i] = 0;
	      }

	      if (!overflow) {
		long old_len;

		if (vector_length < 0)
		  vector_length = 0;

		old_len = vector_length;
		vector_length = (vector_length * 10) + (ch - 48);
		if ((vector_length < 0)|| ((vector_length / 10) != old_len)) {
		  overflow = 1;
		}
	      }
	      ch = scheme_getc_special_ok(port);
	    }

	    if (overflow)
	      vector_length = -2;
	    vecbuf[j] = 0;
	    tagbuf[i] = 0;

	    if (ch == '(')
	      return read_vector(port, stxsrc, line, col, pos, ')', vector_length, vecbuf, ht, indentation, params);
	    if (ch == '[' && params->square_brackets_are_parens)
	      return read_vector(port, stxsrc, line, col, pos, ']', vector_length, vecbuf, ht, indentation, params);
	    if (ch == '{' && params->curly_braces_are_parens)
	      return read_vector(port, stxsrc, line, col, pos, '}', vector_length, vecbuf, ht, indentation, params);

	    if (ch == '#' && (vector_length != -1)) {
	      /* Not a vector after all: a graph reference */
	      Scheme_Object *ph;

	      if (!params->can_read_graph)
		scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), 0, indentation,
				"read: #..# expressions not currently enabled");

	      if (digits > MAX_GRAPH_ID_DIGITS)
		scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), 0, indentation,
				"read: graph id too long in #%5#",
				tagbuf);

	      if (*ht)
		ph = (Scheme_Object *)scheme_hash_get(*ht, scheme_make_integer(vector_length));
	      else
		ph = NULL;

	      if (!ph) {
		scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), 0, indentation,
				"read: no #%ld= preceding #%ld#",
				vector_length, vector_length);
		return scheme_void;
	      }
	      return ph;
	    }
	    if (ch == '=' && (vector_length != -1)) {
	      /* Not a vector after all: a graph definition */
	      Scheme_Object *v, *ph;

	      if (!params->can_read_graph)
		scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), 0, indentation,
				 "read: #..= expressions not currently enabled");

	      if (digits > MAX_GRAPH_ID_DIGITS)
		scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), 0, indentation,
				 "read: graph id too long in #%s=",
				 tagbuf);

	      if (*ht) {
		if (scheme_hash_get(*ht, scheme_make_integer(vector_length))) {
		  scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), 0, indentation,
				  "read: multiple #%ld= tags",
				  vector_length);
		  return NULL;
		}
	      } else {
		Scheme_Hash_Table *tht;
		tht = scheme_make_hash_table(SCHEME_hash_ptr);
		*ht = tht;
	      }
	      ph = scheme_alloc_small_object();
	      ph->type = scheme_placeholder_type;

	      scheme_hash_set(*ht, scheme_make_integer(vector_length), (void *)ph);

	      v = read_inner(port, stxsrc, ht, indentation, params, 0);
	      if (SCHEME_EOFP(v))
		scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), EOF, indentation,
				"read: expected an element for graph (found end-of-file)");
	      if (stxsrc && SCHEME_STXP(v)) /* might be a placeholder! */
		v = scheme_make_graph_stx(v, line, col, pos);
	      SCHEME_PTR_VAL(ph) = v;

	      return v;
	    }

	    {
	      char *lbuffer;
	      int pch = ch, ulen, blen;

	      if ((pch == EOF) || (pch == SCHEME_SPECIAL))
		pch = 0;

	      ulen = scheme_char_strlen(tagbuf);
	      blen = scheme_utf8_encode_all(tagbuf, ulen, NULL);
	      lbuffer = (char *)scheme_malloc_atomic(blen + MAX_UTF8_CHAR_BYTES + 1);
	      scheme_utf8_encode_all(tagbuf, ulen, lbuffer);
	      blen += scheme_utf8_encode(&pch, 0, 1,
					 lbuffer, blen,
					 0);
	      lbuffer[blen] = 0;

	      scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), ch, indentation,
			      "read: bad syntax `#%s'",
			      lbuffer);

	      return NULL;
	    }
	  }
	  break;
	}
      /* We get here only in honu mode */
      scheme_read_err(port, stxsrc, line, col, pos, 2, ch, indentation,
		      "read: bad syntax `#%c'",
		      ch);
      return NULL;
      break;
    case '/':
      if (params->honu_mode) {
	int ch2;
	ch2 = scheme_peekc_special_ok(port);
	if ((ch2 == '/') || (ch2 == '*')) {
	  /* Comment */
	  scheme_ungetc('/', port);
	  ch = skip_whitespace_comments(port, stxsrc, ht, indentation, params);
	  goto start_over_with_ch;
	}
      }
      return read_symbol(ch, port, stxsrc, line, col, pos, ht, indentation, params, table);
      break;
    default:
      if (isdigit_ascii(ch))
	return read_number(ch, port, stxsrc, line, col, pos, 0, 0, 10, 0, ht, indentation, params, table);
      else
	return read_symbol(ch, port, stxsrc, line, col, pos, ht, indentation, params, table);
    }
}

static Scheme_Object *
read_inner(Scheme_Object *port, Scheme_Object *stxsrc, Scheme_Hash_Table **ht,
	   Scheme_Object *indentation, ReadParams *params,
	   int comment_mode)
{
  return read_inner_inner(port, stxsrc, ht, indentation, params, comment_mode, -1, params->table);
}

#ifdef DO_STACK_CHECK
static Scheme_Object *resolve_references(Scheme_Object *obj,
					 Scheme_Object *port,
					 int mkstx);

static Scheme_Object *resolve_k(void)
{
  Scheme_Thread *p = scheme_current_thread;
  Scheme_Object *o = (Scheme_Object *)p->ku.k.p1;
  Scheme_Object *port = (Scheme_Object *)p->ku.k.p2;

  p->ku.k.p1 = NULL;
  p->ku.k.p2 = NULL;

  return resolve_references(o, port, p->ku.k.i1);
}
#endif

static Scheme_Object *resolve_references(Scheme_Object *obj,
					 Scheme_Object *port,
					 int mkstx)
{
  Scheme_Object *start = obj, *result;

#ifdef DO_STACK_CHECK
  {
# include "mzstkchk.h"
    {
      Scheme_Thread *p = scheme_current_thread;
      p->ku.k.p1 = (void *)obj;
      p->ku.k.p2 = (void *)port;
      p->ku.k.i1 = mkstx;
      return scheme_handle_stack_overflow(resolve_k);
    }
  }
#endif

  SCHEME_USE_FUEL(1);

  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_placeholder_type)) {
    /* For placeholders generated by recursive read: */
    if (SCHEME_IMMUTABLEP(obj)) {
      obj = (Scheme_Object *)SCHEME_PTR_VAL(obj); 
    } else {
      obj = (Scheme_Object *)SCHEME_PTR_VAL(obj);
      while (SAME_TYPE(SCHEME_TYPE(obj), scheme_placeholder_type)) {
	if (SAME_OBJ(start, obj)) {
	  scheme_read_err(port, NULL, -1, -1, -1, -1, 0, NULL,
			  "read: illegal cycle");
	  return NULL;
	}
	obj = (Scheme_Object *)SCHEME_PTR_VAL(obj);
      }
      return obj;
    }
  }

  result = obj;
  if (mkstx && SCHEME_STXP(obj)) {
    obj = SCHEME_STX_VAL(obj);

    /* This check is needed because recursive read-syntax produces
       a placeholder value, and it might be embedded into a larger
       syntax object. */
    if (SAME_TYPE(SCHEME_TYPE(obj), scheme_placeholder_type)) {
      /* Assert: SCHEME_IMMUTABLEP(ph) */
      if (mkstx && !SCHEME_STXP(SCHEME_PTR_VAL(obj))) {
	/* A placeholder from read/recur used in read-syntax/recur.
	   Treat it as opaque. */
	return result;
      }
      return resolve_references(obj, port, mkstx);
    }
  }

  if (SCHEME_PAIRP(obj)) {
    Scheme_Object *rr;
    rr = resolve_references(SCHEME_CAR(obj), port, mkstx);
    SCHEME_CAR(obj) = rr;
    rr = resolve_references(SCHEME_CDR(obj), port, mkstx);
    SCHEME_CDR(obj) = rr;
  } else if (SCHEME_BOXP(obj)) {
    Scheme_Object *rr;
    rr = resolve_references(SCHEME_BOX_VAL(obj), port, mkstx);
    SCHEME_BOX_VAL(obj) = rr;
  } else if (SCHEME_VECTORP(obj)) {
    int i, len;
    Scheme_Object *prev_rr, *prev_v;

    prev_v = prev_rr = NULL;
    len = SCHEME_VEC_SIZE(obj);
    for (i = 0; i < len; i++) {
      Scheme_Object *rr;
      if (SCHEME_VEC_ELS(obj)[i] == prev_v) {
	rr = prev_rr;
      } else {
	prev_v = SCHEME_VEC_ELS(obj)[i];
	rr = resolve_references(prev_v, port, mkstx);
	prev_rr = rr;
      }
      SCHEME_VEC_ELS(obj)[i] = rr;
    }
  } else if (SCHEME_HASHTP(obj)) {
    Scheme_Object *l;
    Scheme_Hash_Table *t = (Scheme_Hash_Table *)obj;

    /* Use an_uninterned_symbol to recognize tables
       that come from #hash(...). */
    l = scheme_hash_get(t, an_uninterned_symbol);
    if (l) {
      /* l is a list of 2-element lists.
	 Resolve references inside l.
	 Then hash. */
      Scheme_Object *a, *key, *val;

      /* Make it immutable before we might hash on it */
      SCHEME_SET_IMMUTABLE(obj);

      l = resolve_references(l, port, mkstx);

      if (mkstx)
	l = scheme_syntax_to_datum(l, 0, NULL);

      scheme_hash_set(t, an_uninterned_symbol, NULL);
      for (; SCHEME_PAIRP(l); l = SCHEME_CDR(l)) {
	a = SCHEME_CAR(l);
	key = SCHEME_CAR(a);
	val = SCHEME_CDR(a);

	scheme_hash_set(t, key, val);
      }
    }
  }

  return result;
}

Scheme_Object *
_scheme_internal_read(Scheme_Object *port, Scheme_Object *stxsrc, int crc, int honu_mode, 
		      int recur, int extra_char, Scheme_Object *init_readtable)
{
  Scheme_Object *v, *v2;
  Scheme_Config *config;
  Scheme_Hash_Table **ht = NULL;
  ReadParams params;

  config = scheme_current_config();

  v = scheme_get_param(config, MZCONFIG_READTABLE);
  if (SCHEME_TRUEP(v))
    params.table = (Readtable *)v;
  else
    params.table = NULL;
  params.can_read_compiled = crc;
  v = scheme_get_param(config, MZCONFIG_CAN_READ_PIPE_QUOTE);
  params.can_read_pipe_quote = SCHEME_TRUEP(v);
  v = scheme_get_param(config, MZCONFIG_CAN_READ_BOX);
  params.can_read_box = SCHEME_TRUEP(v);
  v = scheme_get_param(config, MZCONFIG_CAN_READ_GRAPH);
  params.can_read_graph = SCHEME_TRUEP(v);
  v = scheme_get_param(config, MZCONFIG_CAN_READ_READER);
  params.can_read_reader = SCHEME_TRUEP(v);
  v = scheme_get_param(config, MZCONFIG_CASE_SENS);
  params.case_sensitive = SCHEME_TRUEP(v);
  v = scheme_get_param(config, MZCONFIG_SQUARE_BRACKETS_ARE_PARENS);
  params.square_brackets_are_parens = SCHEME_TRUEP(v);
  v = scheme_get_param(config, MZCONFIG_CURLY_BRACES_ARE_PARENS);
  params.curly_braces_are_parens = SCHEME_TRUEP(v);
  v = scheme_get_param(config, MZCONFIG_READ_DECIMAL_INEXACT);
  params.read_decimal_inexact = SCHEME_TRUEP(v);
  v = scheme_get_param(config, MZCONFIG_CAN_READ_QUASI);
  params.can_read_quasi = SCHEME_TRUEP(v);
  v = scheme_get_param(config, MZCONFIG_CAN_READ_DOT);
  params.can_read_dot = SCHEME_TRUEP(v);
  params.honu_mode = honu_mode;
  if (honu_mode)
    params.table = NULL;

  ht = NULL;
  if (recur) {
    v = scheme_extract_one_cc_mark(NULL, an_uninterned_symbol);
    if (v && SCHEME_PAIRP(v)) {
      if (SCHEME_FALSEP(SCHEME_CDR(v)) == !stxsrc)
	ht = (Scheme_Hash_Table **)SCHEME_CAR(v);
    }
  }
  if (!ht) {
    ht = MALLOC_N(Scheme_Hash_Table *, 1);
    recur = 0;
  }

  do {
    v = read_inner_inner(port, stxsrc, ht, scheme_null, &params, 
			 (RETURN_FOR_HASH_COMMENT 
			  | (recur ? (RETURN_FOR_COMMENT | RETURN_FOR_SPECIAL_COMMENT) : 0)),
			 extra_char, 
			 (init_readtable 
			  ? (SCHEME_FALSEP(init_readtable)
			     ? NULL
			     : (Readtable *)init_readtable)
			  : params.table));

    extra_char = -1;

    if (*ht && !recur) {
      /* Resolve placeholders: */
      if (v)
	v = resolve_references(v, port, !!stxsrc);

      /* In case some placeholders were introduced by #;: */
      v2 = scheme_hash_get(*ht, an_uninterned_symbol);
      if (v2)
	resolve_references(v2, port, !!stxsrc);

      if (!v)
	*ht = NULL;
    }

    if (!v && recur) {
      /* Return to indicate comment: */
      v = scheme_alloc_small_object();
      v->type = scheme_special_comment_type;
      SCHEME_PTR_VAL(v) = scheme_false;
      return v;
    }
  } while (!v);

  if (recur) {
    /* For a recursive read, make sure that the result is a placeholder. */
    Scheme_Object *ph;

    if (!SCHEME_EOFP(v)
	&& !SAME_TYPE(SCHEME_TYPE(v), scheme_placeholder_type)) {
      if (!*ht) {
	/* Create hash table to indicate that this placeholder
	   will need to be resolved, maybe: */
	Scheme_Hash_Table *tht;
	tht = scheme_make_hash_table(SCHEME_hash_ptr);
	*ht = tht;
      }

      ph = scheme_alloc_small_object();
      ph->type = scheme_placeholder_type;
      SCHEME_PTR_VAL(ph) = v;

      SCHEME_SET_IMMUTABLE(ph); /* Indicates that it's not a real reference */
      
      v = ph;
    }
  }

  return v;
}

static void *scheme_internal_read_k(void)
{
  Scheme_Thread *p = scheme_current_thread;
  Scheme_Object *port = (Scheme_Object *)p->ku.k.p1;
  Scheme_Object *stxsrc = (Scheme_Object *)p->ku.k.p2;
  Scheme_Object *init_readtable = (Scheme_Object *)p->ku.k.p3;

  p->ku.k.p1 = NULL;
  p->ku.k.p2 = NULL;
  p->ku.k.p3 = NULL;

  return (void *)_scheme_internal_read(port, stxsrc, p->ku.k.i1, p->ku.k.i2, 
				       p->ku.k.i3, p->ku.k.i4, init_readtable);
}

Scheme_Object *
scheme_internal_read(Scheme_Object *port, Scheme_Object *stxsrc, int crc, int cantfail, int honu_mode, 
		     int recur, int pre_char, Scheme_Object *init_readtable)
{
  Scheme_Thread *p = scheme_current_thread;

  if (crc < 0)
    crc = SCHEME_TRUEP(scheme_get_param(scheme_current_config(), MZCONFIG_CAN_READ_COMPILED));

  /* Need this before top_level_do: */
  if (USE_LISTSTACK(!p->list_stack))
    scheme_alloc_list_stack(p);

  if (cantfail) {
    return _scheme_internal_read(port, stxsrc, crc, honu_mode, recur, -1, NULL);
  } else {
    p->ku.k.p1 = (void *)port;
    p->ku.k.p2 = (void *)stxsrc;
    p->ku.k.i1 = crc;
    p->ku.k.i2 = honu_mode;
    p->ku.k.i3 = recur;
    p->ku.k.i4 = pre_char;
    p->ku.k.p3 = (void *)init_readtable;

    return (Scheme_Object *)scheme_top_level_do(scheme_internal_read_k, 0);
  }
}

Scheme_Object *scheme_read(Scheme_Object *port)
{
  return scheme_internal_read(port, NULL, -1, 0, 0, 0, -1, NULL);
}

Scheme_Object *scheme_read_syntax(Scheme_Object *port, Scheme_Object *stxsrc)
{
  return scheme_internal_read(port, stxsrc, -1, 0, 0, 0, -1, NULL);
}

Scheme_Object *scheme_resolve_placeholders(Scheme_Object *obj, int mkstx)
{
  return resolve_references(obj, NULL, mkstx);
}

/*========================================================================*/
/*                             list reader                                */
/*========================================================================*/

static Scheme_Object *honu_add_module_wrapper(Scheme_Object *list,
					      Scheme_Object *stxsrc,
					      Scheme_Object *port);

/* "(" (or other opener) has already been read */
static Scheme_Object *
read_list(Scheme_Object *port,
	  Scheme_Object *stxsrc, long line, long col, long pos,
	  int closer, int shape, int use_stack,
	  Scheme_Hash_Table **ht,
	  Scheme_Object *indentation,
	  ReadParams *params)
{
  Scheme_Object *list = NULL, *last = NULL, *car, *cdr, *pair, *infixed = NULL, *prefetched = NULL;
  int ch = 0, next, got_ch_already = 0;
  int brackets = params->square_brackets_are_parens;
  int braces = params->curly_braces_are_parens;
  long start, startcol, startline, dotpos, dotcol, dotline, dot2pos, dot2line, dot2col;

  scheme_tell_all(port, &startline, &startcol, &start);

  if (stxsrc) {
    /* Push onto the indentation stack: */
    Scheme_Indent *indt;
    indt = (Scheme_Indent *)scheme_malloc_atomic_tagged(sizeof(Scheme_Indent));
    indt->type = scheme_indent_type;

    indt->closer = closer;
    indt->max_indent = startcol + 1;
    indt->multiline = 0;
    indt->suspicious_line = 0;
    indt->suspicious_quote = 0;
    indt->start_line = startline;
    indt->last_line = startline;

    indentation = scheme_make_pair((Scheme_Object *)indt, indentation);
  }

  while (1) {
    if (prefetched)
      ch = 0;
    else if (got_ch_already)
      got_ch_already = 0;
    else
      ch = skip_whitespace_comments(port, stxsrc, ht, indentation, params);

    if ((ch == EOF) && (closer != EOF)) {
      char *suggestion = "";
      if (SCHEME_PAIRP(indentation)) {
	Scheme_Indent *indt;

	indt = (Scheme_Indent *)SCHEME_CAR(indentation);
	if (indt->suspicious_line) {
	  suggestion = scheme_malloc_atomic(100);
	  sprintf(suggestion,
		  "; indentation suggests a missing '%c' before line %ld",
		  indt->suspicious_closer,
		  indt->suspicious_line);
	}
      }

      scheme_read_err(port, stxsrc, startline, startcol, start, SPAN(port, start), EOF, indentation,
		      "read: expected a '%c'%s", closer, suggestion);
      return NULL;
    }

    if (ch == closer) {
      if (shape == mz_shape_hash_elem) {
	scheme_read_err(port, stxsrc, startline, startcol, start, SPAN(port, start), ch, indentation,
			"read: expected dotted hash pair before '%c'",
			closer);
	return NULL;
      }

      if (params->honu_mode) {
	/* Finish up the list */
	if (!list)
	  list = scheme_null;
	if (closer == ')')
	  car = honu_parens;
	else if (closer == ']')
	  car = honu_brackets;
	else if (closer == '}')
	  car = honu_braces;
	else
	  car = NULL;
	if (car) {
	  if (stxsrc)
	    car = scheme_make_stx_w_offset(car, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG);
	  list = scheme_make_pair(car, list);
	  if (stxsrc)
	    SCHEME_SET_PAIR_IMMUTABLE(list);
	}
      } else {
	if (!list) {
	  list = scheme_null;
	}
      }
      pop_indentation(indentation);
      list = (stxsrc
	      ? scheme_make_stx_w_offset(list, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG)
	      : list);
      if (params->honu_mode && (closer == EOF)) {
	list = honu_add_module_wrapper(list, stxsrc, port);
      }
      return list;
    }

    if (shape == mz_shape_hash_list) {
      /* Make sure we found a parenthesized something. */
      if (!(ch == '(')
	  && ! (ch == '[' && params->square_brackets_are_parens)
	  && !(ch == '{' && params->curly_braces_are_parens)) {
	long xl, xc, xp;
	
	/* If it's a special or we have a readtable, we need to read ahead
	   to make sure that it's not a comment. For consistency, always
	   read ahead. */
	scheme_ungetc(ch, port);
	prefetched = read_inner(port, stxsrc, ht, indentation, params, RETURN_FOR_SPECIAL_COMMENT);
	if (!prefetched)
	  continue; /* It was a comment; try again. */

	scheme_tell_all(port, &xl, &xc, &xp);
	scheme_read_err(port, stxsrc, xl, xc, xp, 1,
			ch, indentation,
			"read: expected '('%s%s to start a hash pair",
			params->square_brackets_are_parens ? " or '['" : "",
			params->curly_braces_are_parens ? " or '{'" : "");
	return NULL;
      } else {
	/* Found paren. Use read_list directly so we can specify mz_shape_hash_elem. */
	long xl, xc, xp;
	scheme_tell_all(port, &xl, &xc, &xp);
	car = read_list(port, stxsrc, xl, xc, xp,
			((ch == '(') ? ')' : ((ch == '[') ? ']' : '}')),
			mz_shape_hash_elem, use_stack, ht, indentation, params);
	/* car is guaranteed to have an appropriate shape */
      }
    } else {
      if (prefetched) {
	car = prefetched;
	prefetched = NULL;
      } else {
	scheme_ungetc(ch, port);
	car = read_inner(port, stxsrc, ht, indentation, params, RETURN_FOR_SPECIAL_COMMENT);
	if (!car) continue; /* special was a comment */
      }
      /* can't be eof, due to check above */
    }

    if (USE_LISTSTACK(use_stack)) {
      if (local_list_stack_pos >= NUM_CELLS_PER_STACK) {
	/* Overflow */
	Scheme_Simple_Object *sa;
	sa = MALLOC_N_RT(Scheme_Simple_Object, NUM_CELLS_PER_STACK);
	local_list_stack = sa;
	local_list_stack_pos = 0;
      }

      pair = (Scheme_Object *)(local_list_stack + (local_list_stack_pos++));
      pair->type = scheme_pair_type;
      SCHEME_CAR(pair) = car;
      SCHEME_CDR(pair) = scheme_null;
    } else {
      pair = scheme_make_pair(car, scheme_null);
      if (stxsrc)
	SCHEME_SET_PAIR_IMMUTABLE(pair);
    }

  retry_before_dot:

    ch = skip_whitespace_comments(port, stxsrc, ht, indentation, params);
    if ((ch == closer) && !params->honu_mode) {
      if (shape == mz_shape_hash_elem) {
	scheme_read_err(port, stxsrc, startline, startcol, start, SPAN(port, start), ch, indentation,
			"read: expected `.' and value for hash before '%c'",
			closer);
	return NULL;
      }

      cdr = pair;
      if (!list)
	list = cdr;
      else
	SCHEME_CDR(last) = cdr;

      if (infixed) {
	/* Assert: we're not using the list stack */
	list = scheme_make_pair(infixed, list);
	if (stxsrc)
	  SCHEME_SET_PAIR_IMMUTABLE(list);
      }

      pop_indentation(indentation);
      return (stxsrc
	      ? scheme_make_stx_w_offset(list, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG)
	      : list);
    } else if (!params->honu_mode
	       && params->can_read_dot
	       && (ch == '.')
	       && (next = scheme_peekc_special_ok(port),
		   ((next == EOF)
		    || (next == SCHEME_SPECIAL)
		    || (!params->table 
			&& (scheme_isspace(next)
			    || (next == '(')
			    || (next == ')')
			    || (next == '"')
			    || (next == ';')
			    || (next == '\'')
			    || (next == '`')
			    || (next == ',')
			    || ((next == '[') && brackets)
			    || ((next == '{') && braces)
			    || ((next == ']') && brackets)
			    || ((next == '}') && braces)))
		    || (params->table 
			&& (readtable_kind(params->table, next, params) 
			    & (READTABLE_WHITESPACE | READTABLE_TERMINATING)))))) {
      scheme_tell_all(port, &dotline, &dotcol, &dotpos);

      track_indentation(indentation, dotline, dotcol);

      if (((shape != mz_shape_cons) && (shape != mz_shape_hash_elem)) || infixed) {
	scheme_read_err(port, stxsrc, dotline, dotcol, dotpos, 1, 0, indentation,
			"read: illegal use of \".\"");
	return NULL;
      }
      /* can't be eof, due to check above: */
      cdr = read_inner(port, stxsrc, ht, indentation, params, 0);
      ch = skip_whitespace_comments(port, stxsrc, ht, indentation, params);
      if (ch != closer) {
	if (ch == '.') {
	  /* Parse as infix: */

	  if (shape == mz_shape_hash_elem) {
	    scheme_read_err(port, stxsrc, startline, startcol, start, SPAN(port, start), ch, indentation,
			    "read: expected '%c' after hash value",
			    closer);
	    return NULL;
	  }

	  {
	    scheme_tell_all(port, &dot2line, &dot2col, &dot2pos);
	    track_indentation(indentation, dot2line, dot2col);
	  }

	  infixed = cdr;

	  if (!list)
	    list = pair;
	  else
	    SCHEME_CDR(last) = pair;
	  last = pair;

	  /* Make sure there's not a closing paren immediately after the dot: */
	  ch = skip_whitespace_comments(port, stxsrc, ht, indentation, params);
	  if ((ch == closer) || (ch == EOF)) {
	    scheme_read_err(port, stxsrc, dotline, dotcol, dotpos, 1, (ch == EOF) ? EOF : 0, indentation,
			    "read: illegal use of \".\"");
	    return NULL;
	  }
	  got_ch_already = 1;
	} else {
	  scheme_read_err(port, stxsrc, dotline, dotcol, dotpos, 1, (ch == EOF) ? EOF : 0, indentation,
			  "read: illegal use of \".\"");
	  return NULL;
	}
      } else {
	SCHEME_CDR(pair) = cdr;
	cdr = pair;
	if (!list)
	  list = cdr;
	else
	  SCHEME_CDR(last) = cdr;

	/* Assert: infixed is NULL (otherwise we raised an exception above) */

	pop_indentation(indentation);

	return (stxsrc
		? scheme_make_stx_w_offset(list, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG)
		: list);
      }
    } else {
      if ((ch == SCHEME_SPECIAL) || (params->table && (ch != EOF))) {
	/* We have to try the read, because it might be a comment. */
	scheme_ungetc(ch, port);
	prefetched = read_inner(port, stxsrc, ht, indentation, params, RETURN_FOR_SPECIAL_COMMENT);
	if (!prefetched)
	  goto retry_before_dot;
      } else {
	got_ch_already = 1;
      }

      if (shape == mz_shape_hash_elem) {
	scheme_read_err(port, stxsrc, startline, startcol, start, SPAN(port, start), ch, indentation,
			"read: expected `.' and value for hash");
	return NULL;
      }

      cdr = pair;
      if (!list)
	list = cdr;
      else
	SCHEME_CDR(last) = cdr;
      last = cdr;
    }
  }
}

static Scheme_Object *
honu_add_module_wrapper(Scheme_Object *list, Scheme_Object *stxsrc, Scheme_Object *port)
{
# define cons scheme_make_immutable_pair
  Scheme_Object *v, *name;

  if (stxsrc)
    name = stxsrc;
  else
    name = ((Scheme_Input_Port *)port)->name;

  if (SCHEME_CHAR_STRINGP(name))
    name = scheme_char_string_to_byte_string_locale(name);

  if (SCHEME_PATHP(name)) {
    Scheme_Object *base;
    int isdir, i;
    name = scheme_split_path(SCHEME_BYTE_STR_VAL(name), SCHEME_BYTE_STRLEN_VAL(name), &base, &isdir);
    for (i = SCHEME_BYTE_STRLEN_VAL(name); i--; ) {
      if (SCHEME_BYTE_STR_VAL(name)[i] == '.')
	break;
    }
    if (i > 0)
      name = scheme_make_sized_path(SCHEME_BYTE_STR_VAL(name), i, 0);
    name = scheme_byte_string_to_char_string_locale(name);
    name = scheme_intern_exact_char_symbol(SCHEME_CHAR_STR_VAL(name), SCHEME_CHAR_STRLEN_VAL(name));
  } else if (!SCHEME_SYMBOLP(name)) {
    name = scheme_intern_symbol("unknown");
  }

  v = cons(scheme_intern_symbol("module"),
	   cons(name,
		cons(cons(scheme_intern_symbol("lib"),
			  cons(scheme_make_utf8_string("honu-module.ss"),
			       cons(scheme_make_utf8_string("honu-module"),
				    scheme_null))),
		     list)));
# undef cons
  if (stxsrc)
    v = scheme_datum_to_syntax(v, list, scheme_false, 0, 0);
  return v;
}

/*========================================================================*/
/*                            string reader                               */
/*========================================================================*/

/* '"' has already been read */
static Scheme_Object *
read_string(int is_byte, int is_honu_char, Scheme_Object *port,
	    Scheme_Object *stxsrc, long line, long col, long pos,
	    Scheme_Hash_Table **ht,
	    Scheme_Object *indentation, ReadParams *params)
{
  mzchar *buf, *oldbuf, onstack[32];
  int i, j, n, n1, ch, closer = (is_honu_char ? '\'' : '"');
  long size = 31, oldsize;
  Scheme_Object *result;

  i = 0;
  buf = onstack;
  while ((ch = scheme_getc_special_ok(port)) != closer) {
    if ((ch == EOF) || (is_honu_char && (i > 0))) {
      scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), ch, indentation,
		      "read: expected a closing %s%s",
		      is_honu_char ? "'" : "'\"'",
		      (ch == EOF) ? "" : " after one character");
      return NULL;
    } else if (ch == SCHEME_SPECIAL) {
      scheme_get_ready_read_special(port, stxsrc, ht);
      scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), SCHEME_SPECIAL, indentation,
		      "read: found non-character while reading a %s",
		      is_honu_char ? "character constant" : "string");
      return NULL;
    }
    /* Note: errors will tend to leave junk on the port, with an open \". */
    /* Escape-sequence handling by Eli Barzilay. */
    if (ch == '\\') {
      ch = scheme_getc_special_ok(port);
      if (ch == EOF) {
	scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), EOF, indentation,
			"read: expected a closing %s",
			is_honu_char ? "'" : "'\"'");
	return NULL;
      } else if (ch == SCHEME_SPECIAL) {
	scheme_get_ready_read_special(port, stxsrc, ht);
	scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), SCHEME_SPECIAL, indentation,
			"read: found non-character while reading a %s",
			is_honu_char ? "character constant" : "string");
	return NULL;
      }
      switch ( ch ) {
      case '\\': case '\"': case '\'': break;
      case 'a': ch = '\a'; break;
      case 'b': ch = '\b'; break;
      case 'e': ch = 27; break; /* escape */
      case 'f': ch = '\f'; break;
      case 'n': ch = '\n'; break;
      case 'r': ch = '\r'; break;
      case 't': ch = '\t'; break;
      case 'v': ch = '\v'; break;
      case '\r':
        if (scheme_peekc_special_ok(port) == '\n')
	  scheme_getc(port);
	continue; /* <---------- !!!! */
      case '\n':
        continue; /* <---------- !!!! */
      case 'x':
	ch = scheme_getc_special_ok(port);
	if (NOT_EOF_OR_SPECIAL(ch) && scheme_isxdigit(ch)) {
	  n = ch<='9' ? ch-'0' : (scheme_toupper(ch)-'A'+10);
	  ch = scheme_peekc_special_ok(port);
	  if (NOT_EOF_OR_SPECIAL(ch) && scheme_isxdigit(ch)) {
	    n = n*16 + (ch<='9' ? ch-'0' : (scheme_toupper(ch)-'A'+10));
	    scheme_getc(port); /* must be ch */
	  }
	  ch = n;
	} else {
	  if (ch == SCHEME_SPECIAL)
	    scheme_get_ready_read_special(port, stxsrc, ht);
	  scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), ch, indentation,
			  "read: no hex digit following \\x in %s",
			  is_honu_char ? "character constant" : "string");
	  return NULL;
	}
	break;
      case 'u':
      case 'U':
	if (!is_byte) {
	  int maxc = ((ch == 'u') ? 4 : 6);
	  ch = scheme_getc_special_ok(port);
	  if (NOT_EOF_OR_SPECIAL(ch) && scheme_isxdigit(ch)) {
	    int count = 1;
	    n = ch<='9' ? ch-'0' : (scheme_toupper(ch)-'A'+10);
	    while (count < maxc) {
	      ch = scheme_peekc_special_ok(port);
	      if (NOT_EOF_OR_SPECIAL(ch) && scheme_isxdigit(ch)) {
		n = n*16 + (ch<='9' ? ch-'0' : (scheme_toupper(ch)-'A'+10));
		scheme_getc(port); /* must be ch */
		count++;
	      } else
		break;
	    }
	    /* disallow surrogate points, etc */
	    if (((n >= 0xD800) && (n <= 0xDFFF))
		|| (n > 0x10FFFF)) {
	      ch = -1;
	    } else {
	      ch = n;
	    }
	  } else {
	    if (ch == SCHEME_SPECIAL)
	      scheme_get_ready_read_special(port, stxsrc, ht);
	    scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), ch, indentation,
			    "read: no hex digit following \\%c in %s",
			    ((maxc == 4) ? 'u' : 'U'),
			    is_honu_char ? "character constant" : "string");
	    return NULL;
	  }
	  break;
	} /* else FALLTHROUGH!!! */
      default:
	if ((ch >= '0') && (ch <= '7')) {
	  for (n = j = 0; j < 3; j++) {
	    n1 = 8*n + ch - '0';
	    if (n1 > 255) {
	      scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), 0, indentation,
			      "read: escape sequence \\%o out of range in %s", n1,
			      is_honu_char ? "character constant" : "string");
	      return NULL;
	    }
	    n = n1;
	    if (j < 2) {
	      ch = scheme_peekc_special_ok(port);
	      if (!((ch >= '0') && (ch <= '7'))) {
		break;
	      } else {
		scheme_getc(port); /* must be ch */
	      }
	    }
	  }
	  ch = n;
	} else {
	  scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), 0, indentation,
			  "read: unknown escape sequence \\%c in %s%s", ch,
			  is_byte ? "byte " : "",
			  is_honu_char ? "character constant" : "string");
	  return NULL;
	}
	break;
      }
    } else if ((ch == '\n') || (ch == '\r')) {
      /* Suspicious string... remember the line */
      if (line > 0) {
	if (SCHEME_PAIRP(indentation)) {
	  Scheme_Indent *indt;
	  indt = (Scheme_Indent *)SCHEME_CAR(indentation);
	  /* Only remember if there's no earlier suspcious string line: */
	  if (!indt->suspicious_quote) {
	    indt->suspicious_quote = line;
	    indt->quote_for_char = is_honu_char;
	  }
	}
      }
    }

    if (ch < 0) {
      scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), 0, indentation,
		      "read: out-of-range character in %s%s",
		      is_byte ? "byte " : "",
		      is_honu_char ? "character constant" : "string");
      return NULL;
    }

    if (i >= size) {
      oldsize = size;
      oldbuf = buf;

      size *= 2;
      buf = (mzchar *)scheme_malloc_atomic((size + 1) * sizeof(mzchar));
      memcpy(buf, oldbuf, oldsize * sizeof(mzchar));
    }
    buf[i++] = ch;
  }
  buf[i] = '\0';

  if (is_honu_char) {
    if (i)
      result = scheme_make_character(buf[0]);
    else {
      scheme_read_err(port, stxsrc, line, col, pos, 2, 0, indentation,
		      "read: expected one character before closing '");
      return NULL;
    }
  } else if (!is_byte)
    result = scheme_make_immutable_sized_char_string(buf, i, i <= 31);
  else {
    /* buf is not UTF-8 encoded; all of the chars are less than 256.
       We just need to change to bytes.. */
    char *s;
    s = (char *)scheme_malloc_atomic(i + 1);
    for (j = 0; j < i; j++) {
      ((unsigned char *)s)[j] = buf[j];
    }
    s[i] = 0;
    result = scheme_make_immutable_sized_byte_string(s, i, 0);
  }
  if (stxsrc)
    result =  scheme_make_stx_w_offset(result, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG);
  return result;
}

static Scheme_Object *
read_here_string(Scheme_Object *port, Scheme_Object *stxsrc,
		 long line, long col, long pos,
		 Scheme_Object *indentation,
		 ReadParams *params)
     /* #<< has been read already */
{
  int tlen = 0, len = 0, size = 12;
  mzchar *tag, *naya, *s, buf[12], c;
  Scheme_Object *str;

  tag = buf;
  while (1) {
    c = scheme_getc(port);
    if (c == '\n') {
      break;
    } else if (c == EOF) {
      scheme_read_err(port, stxsrc, line, col, pos, 3 + tlen, EOF, indentation,
		      "read: found end-of-file after #<< and before first and-of-line");
      return NULL;
    } else {
      if (tlen >= size) {
	size *= 2;
	naya = (mzchar *)scheme_malloc_atomic(size * sizeof(mzchar));
	memcpy(naya, tag, tlen * sizeof(mzchar));
	tag = naya;
      }
      tag[tlen++] = c;
    }
  }
  if (!tlen) {
    scheme_read_err(port, stxsrc, line, col, pos, 3, 0, indentation,
		    "read: no characters after #<< before and-of-line");
    return NULL;
  }

  size = 10 + tlen;
  s = (mzchar *)scheme_malloc_atomic(size * sizeof(mzchar));
  while (1) {
    c = scheme_getc(port);
    if (c == EOF) {
      scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), EOF, indentation,
		      "read: found end-of-file before terminating %u%s",
		      tag, 
		      (tlen > 50) ? 50 : tlen,
		      (tlen > 50) ? "..." : "");
      return NULL;
    }
    if (len >= size) {
      size *= 2;
      naya = (mzchar *)scheme_malloc_atomic(size * sizeof(mzchar));
      memcpy(naya, s, len * sizeof(mzchar));
      s = naya;
    }
    s[len++] = c;
    if ((len >= tlen)
	&& ((len == tlen)
	    || (s[len - tlen - 1] == '\n'))
	&& !memcmp(s XFORM_OK_PLUS (len - tlen), tag, sizeof(mzchar) * tlen)) {
      c = scheme_peekc(port);
      if ((c == '\r') || (c == '\n') || (c == EOF))
	break;
    }
  }

  len -= (tlen + 1);
  if (len < 0)
    len = 0;

  str = scheme_make_sized_char_string(s, len, 1);

  if (stxsrc)
    str = scheme_make_stx_w_offset(str, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG);
  
  return str;
}

char *scheme_extract_indentation_suggestions(Scheme_Object *indentation)
{
  long suspicious_quote = 0;
  int is_honu_char = 0;
  char *suspicions = "";

  /* search back through indentation records to find the
     first suspicious quote */
  while (SCHEME_PAIRP(indentation)) {
    Scheme_Indent *indt;
    indt = (Scheme_Indent *)SCHEME_CAR(indentation);
    indentation = SCHEME_CDR(indentation);
    if (indt->suspicious_quote) {
      suspicious_quote = indt->suspicious_quote;
      is_honu_char = indt->quote_for_char;
    }
  }

  if (suspicious_quote) {
    suspicions = (char *)scheme_malloc_atomic(64);
    sprintf(suspicions,
	    "; newline within %s suggests a missing %s on line %ld",
	    is_honu_char ? "character" : "string",
	    is_honu_char ? "'" : "'\"'",
	    suspicious_quote);
  }

  return suspicions;
}

/*========================================================================*/
/*                            vector reader                               */
/*========================================================================*/

/* "#(" has been read */
static Scheme_Object *
read_vector (Scheme_Object *port,
	     Scheme_Object *stxsrc, long line, long col, long pos,
	     char closer,
	     long requestLength, const mzchar *reqBuffer,
	     Scheme_Hash_Table **ht,
	     Scheme_Object *indentation, ReadParams *params)
/* requestLength == -1 => no request
   requestLength == -2 => overflow */
{
  Scheme_Object *lresult, *obj, *vec, **els;
  int len, i;
  ListStackRec r;

  STACK_START(r);
  lresult = read_list(port, stxsrc, line, col, pos, closer, mz_shape_vec, 1, ht, indentation, params);

  if (requestLength == -2) {
    STACK_END(r);
    scheme_raise_out_of_memory("read", "making vector of size %5", reqBuffer);
    return NULL;
  }

  if (stxsrc)
    obj = ((Scheme_Stx *)lresult)->val;
  else
    obj = lresult;

  len = scheme_list_length(obj);
  if (requestLength >= 0 && len > requestLength) {
    char buffer[20];
    STACK_END(r);
    sprintf(buffer, "%ld", requestLength);
    scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), 0, indentation,
		    "read: vector length %ld is too small, "
		    "%d values provided",
		    requestLength, len);
    return NULL;
  }
  if (requestLength < 0)
    requestLength = len;
  vec = scheme_make_vector(requestLength, NULL);
  els = SCHEME_VEC_ELS(vec);
  for (i = 0; i < len ; i++) {
    els[i] = SCHEME_CAR(obj);
    obj = SCHEME_CDR(obj);
  }
  els = NULL;
  STACK_END(r);
  if (i < requestLength) {
    if (len)
      obj = SCHEME_VEC_ELS(vec)[len - 1];
    else {
      obj = scheme_make_integer(0);
      if (stxsrc)
	obj = scheme_make_stx_w_offset(obj, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG);
    }

    if (stxsrc && (requestLength > 1)) {
      /* Set the graph flag if obj sharing is visible: */
      Scheme_Object *v;
      v = SCHEME_STX_VAL(obj);
      if (SCHEME_PAIRP(v) || SCHEME_VECTORP(v) || SCHEME_BOXP(v))
	obj = scheme_make_graph_stx(obj, -1, -1, -1);
    }

    els = SCHEME_VEC_ELS(vec);
    for (; i < requestLength; i++) {
      els[i] = obj;
    }
    els = NULL;
  }

  if (stxsrc) {
    if (SCHEME_VEC_SIZE(vec) > 0)
      SCHEME_SET_VECTOR_IMMUTABLE(vec);
    ((Scheme_Stx *)lresult)->val = vec;
    return lresult;
  } else
    return vec;
}

/*========================================================================*/
/*                            symbol reader                               */
/*========================================================================*/

/* Also dispatches to number reader, since things not-a-number are
   symbols. */

static int check_honu_num(mzchar *buf, int i);
typedef int (*Getc_Fun_r)(Scheme_Object *port);

/* nothing has been read, except maybe some flags */
static Scheme_Object  *
read_number_or_symbol(int init_ch, Scheme_Object *port,
		      Scheme_Object *stxsrc, long line, long col, long pos,
		      int is_float, int is_not_float,
		      int radix, int radix_set,
		      int is_symbol, int pipe_quote,
		      Scheme_Hash_Table **ht,
		      Scheme_Object *indentation, ReadParams *params, Readtable *table)
{
  mzchar *buf, *oldbuf, onstack[MAX_QUICK_SYMBOL_SIZE];
  int size, oldsize;
  int i, ch, quoted, quoted_ever = 0, running_quote = 0;
  int running_quote_ch = 0;
  long rq_pos = 0, rq_col = 0, rq_line = 0;
  int case_sens = params->case_sensitive;
  int decimal_inexact = params->read_decimal_inexact;
  Scheme_Object *o;
  int delim_ok;
  int ungetc_ok;
  int honu_mode, e_ok = 0;
  int far_char_ok;
  int single_escape, multiple_escape;
  Getc_Fun_r getc_special_ok_fun;

  ungetc_ok = scheme_peekc_is_ungetc(port);

  if (ungetc_ok) {
    getc_special_ok_fun = scheme_getc_special_ok;
  } else {
    getc_special_ok_fun = scheme_peekc_special_ok;
  }

  i = 0;
  size = MAX_QUICK_SYMBOL_SIZE - 1;
  buf = onstack;

  if (init_ch < 0)
    ch = getc_special_ok_fun(port);
  else {
    /* Assert: this one won't need to be ungotten */
    ch = init_ch;
  }

  if (is_float || is_not_float || radix_set)
    honu_mode = 0;
  else
    honu_mode = params->honu_mode;

  if (table) {
    far_char_ok = 0;
    delim_ok = 0;
  } else if (!honu_mode) {
    delim_ok = SCHEME_OK;
    far_char_ok = 1;
  } else {
    pipe_quote = 0;
    if (!is_symbol) {
      delim_ok = (HONU_NUM_OK | HONU_INUM_OK);
      e_ok = 1;
      far_char_ok = 0;
    } else if (delim[ch] & HONU_SYM_OK) {
      delim_ok = HONU_SYM_OK;
      far_char_ok = 0;
    } else {
      delim_ok = HONU_OK;
      far_char_ok = 1;
    }
  }

  while (NOT_EOF_OR_SPECIAL(ch)
	 && (running_quote
	     || (!table 
		 && !scheme_isspace(ch) 
		 && (((ch < 128) && (delim[ch] & delim_ok))
		     || ((ch >= 128) && far_char_ok)))
	     || table)) {
    if (table) {
      int v;
      v = readtable_kind(table, ch, params);
      if (!running_quote && (v & (READTABLE_TERMINATING | READTABLE_WHITESPACE)))
	break;
      single_escape = (v & READTABLE_SINGLE_ESCAPE);
      multiple_escape = (v & READTABLE_MULTIPLE_ESCAPE);
    } else {
      single_escape = (ch == '\\');
      multiple_escape = ((ch == '|') && pipe_quote);
    }
    if (!ungetc_ok) {
      if (init_ch < 0)
	scheme_getc(port); /* must be a character */
      else
	init_ch = -1;
    }
    if (single_escape && !running_quote) {
      int esc_ch = ch;
      ch = scheme_getc_special_ok(port);
      if (ch == EOF) {
	scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), EOF, indentation,
			"read: EOF following `%c' in symbol", esc_ch);
	return NULL;
      } else if (ch == SCHEME_SPECIAL) {
	scheme_get_ready_read_special(port, stxsrc, ht);
	scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), SCHEME_SPECIAL, indentation,
			"read: non-character following `%c' in symbol", esc_ch);
	return NULL;
      }
      quoted = 1;
      quoted_ever = 1;
    } else if (multiple_escape && (!running_quote || (ch == running_quote_ch))) {
      quoted_ever = 1;
      running_quote = !running_quote;
      running_quote_ch = ch;
      quoted = 0;

      scheme_tell_all(port, &rq_line, &rq_col, &rq_pos);

      ch = getc_special_ok_fun(port);
      continue; /* <-- !!! */
    } else
      quoted = 0;

    if (i >= size) {
      oldsize = size;
      oldbuf = buf;

      size *= 2;
      buf = (mzchar *)scheme_malloc_atomic((size + 1) * sizeof(mzchar));
      memcpy(buf, oldbuf, oldsize * sizeof(mzchar));
    }

    if (!case_sens && !quoted && !running_quote)
      ch = scheme_tolower(ch);

    buf[i++] = ch;

    if (delim_ok & HONU_INUM_OK) {
      if ((ch == 'e') || (ch == 'E')) {
	/* Allow a +/- next */
	delim_ok = (HONU_NUM_OK | HONU_INUM_OK | HONU_INUM_SIGN_OK);
      } else
	delim_ok = (HONU_NUM_OK | HONU_INUM_OK);
    }

    ch = getc_special_ok_fun(port);
  }

  if (running_quote && (ch == SCHEME_SPECIAL)) {
    scheme_get_ready_read_special(port, stxsrc, ht);
    scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), SCHEME_SPECIAL, indentation,
		    "read: non-character following `%c' in symbol", running_quote_ch);
  }

  if (ungetc_ok)
    scheme_ungetc(ch, port);

  if (running_quote) {
    scheme_read_err(port, stxsrc, rq_line, rq_col, rq_pos, SPAN(port, rq_pos), EOF, indentation,
		    "read: unbalanced `%c'", running_quote_ch);
    return NULL;
  }

  buf[i] = '\0';

  if (!quoted_ever && (i == 1) && (buf[0] == '.') && !honu_mode) {
    long xl, xc, xp;
    scheme_tell_all(port, &xl, &xc, &xp);
    scheme_read_err(port, stxsrc, xl, xc, xp,
		    1, 0, indentation,
		    "read: illegal use of \".\"");
    return NULL;
  }

  if (!i && honu_mode) {
    /* If we end up with an empty string, then the first character
       is simply illegal */
    scheme_read_err(port, stxsrc, line, col, pos, 1, 0, indentation,
		    "read: illegal character: %c", ch);
    return NULL;
  }

  if (honu_mode && !is_symbol) {
    /* Honu inexact syntax is not quite a subset of Scheme: it can end
       in an "f" or "d" to indicate the precision. We can easily check
       whether the string has the right shape, and then move the "f"
       or "d" in place of the "e" in that case. */
    int found_e;
    found_e = check_honu_num(buf, i);
    if (found_e < 0) {
      scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), 0, indentation,
		      "read: bad number: %5", buf);
      return NULL;
    }
    if (delim[buf[i - 1]] & HONU_INUM_OK) {
      /* We have a precision id to move */
      if (found_e) {
	/* Easy case: replace e: */
	buf[found_e] = buf[i - 1];
	i--;
      } else {
	/* Slightly harder: add a 0 at the end for the exponent */
	if (i >= size) {
	  oldsize = size;
	  oldbuf = buf;

	  size *= 2;
	  buf = (mzchar *)scheme_malloc_atomic((size + 1) * sizeof(mzchar));
	  memcpy(buf, oldbuf, oldsize * sizeof(mzchar));
	}
	buf[i++] = '0';
	buf[i] = 0;
      }
    }
  }

  if ((is_symbol || quoted_ever) && !is_float && !is_not_float && !radix_set)
    o = scheme_false;
  else {
    o = scheme_read_number(buf, i,
			   is_float, is_not_float, decimal_inexact,
			   radix, radix_set,
			   port, NULL, 0,
			   stxsrc, line, col, pos, SPAN(port, pos),
			   indentation);
  }

  if (SAME_OBJ(o, scheme_false)) {
    if (honu_mode && !is_symbol) {
      scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), 0, indentation,
		      "read: bad number: %5", buf);
      return NULL;
    }
    o = scheme_intern_exact_char_symbol(buf, i);
  }

  if (stxsrc)
    o = scheme_make_stx_w_offset(o, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG);

  return o;
}

static Scheme_Object  *
read_number(int init_ch,
	    Scheme_Object *port,
	    Scheme_Object *stxsrc, long line, long col, long pos,
	    int is_float, int is_not_float,
	    int radix, int radix_set,
	    Scheme_Hash_Table **ht,
	    Scheme_Object *indentation, ReadParams *params, Readtable *table)
{
  return read_number_or_symbol(init_ch,
			       port, stxsrc, line, col, pos,
			       is_float, is_not_float,
			       radix, radix_set, 0,
			       params->can_read_pipe_quote,
			       ht, indentation, params, table);
}

static Scheme_Object  *
read_symbol(int init_ch,
	    Scheme_Object *port,
	    Scheme_Object *stxsrc, long line, long col, long pos,
	    Scheme_Hash_Table **ht,
	    Scheme_Object *indentation, ReadParams *params, Readtable *table)
{
  return read_number_or_symbol(init_ch,
			       port, stxsrc, line, col, pos,
			       0, 0, 10, 0, 1,
			       params->can_read_pipe_quote,
			       ht, indentation, params, table);
}

static int check_honu_num(mzchar *buf, int i)
{
  int j, found_e = 0, found_dot = 0;
  for (j = 0; j < i; j++) {
    if (buf[j] == '.') {
      if (found_dot) {
	j = 0;
	break; /* bad number */
      }
      found_dot = 1;
    } else if ((buf[j] == 'e') || (buf[j] == 'E')) {
      if (!j)
	break; /* bad number */
      found_e = j;
      /* Allow a sign next: */
      j++;
      if ((buf[j] == '+') || (buf[j] == '-'))
	j++;
      /* At least one digit: */
      if (!isdigit_ascii(buf[j])) {
	j = 0;
	break;
      }
      /* All digits, up to end: */
      while (isdigit_ascii(buf[j])) {
	j++;
      }
      if (!buf[j])
	break; /* good number */
      if (buf[j + 1]) {
	j = 0;
	break; /* bad number */
      }
      switch (buf[j]) {
      case 'd':
      case 'D':
      case 'f':
      case 'F':
	break; /* good number */
      default:
	j = 0;
	break; /* bad number */
      }
      break;
    } else if (delim[buf[j]] & HONU_INUM_OK) {
      if (j + 1 == i) {
	/* Fine -- ends in d/f, even though there's no e */
      } else {
	j = 0;
	break; /* bad number */
      }
    }
  }
  if (!j) {
    return -1;
  }
  return found_e;
}

/*========================================================================*/
/*                              char reader                               */
/*========================================================================*/

static int u_strcmp(mzchar *s, const char *_t)
{
  int i;
  unsigned char *t = (unsigned char *)_t;

  for (i = 0; s[i] && (scheme_tolower(s[i]) == scheme_tolower(((unsigned char *)t)[i])); i++) {
  }
  if (s[i] || t[i])
    return 1;
  return 0;
}

/* "#\" has been read */
static Scheme_Object *
read_character(Scheme_Object *port,
	       Scheme_Object *stxsrc, long line, long col, long pos,
	       Scheme_Hash_Table **ht,
	       Scheme_Object *indentation, ReadParams *params)
{
  int ch, next;

  ch = scheme_getc_special_ok(port);

  if (ch == SCHEME_SPECIAL) {
    scheme_get_ready_read_special(port, stxsrc, ht);
    scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), SCHEME_SPECIAL, indentation,
		    "read: found non-character after #\\");
    return NULL;
  }

  next = scheme_peekc_special_ok(port);

  if ((ch >= '0' && ch <= '7') && (next >= '0' && next <= '7')) {
    /* a is the same as next */
    int last;

    last = (scheme_getc(port) /* is char */, scheme_peekc_special_ok(port));

    if (last != SCHEME_SPECIAL)
      scheme_getc(port); /* must be last */

    if (last < '0' || last > '7' || ch > '3') {
      scheme_read_err(port, stxsrc, line, col, pos, ((last == EOF) || (last == SCHEME_SPECIAL)) ? 3 : 4, last, indentation,
		      "read: bad character constant #\\%c%c%c",
		      ch, next, ((last == EOF) || (last == SCHEME_SPECIAL)) ? ' ' : last);
      return NULL;
    }

    ch = ((ch - '0') << 6) + ((next - '0') << 3) + (last - '0');

    return scheme_make_char(ch);
  }

  if (((ch == 'u') || (ch == 'U')) && NOT_EOF_OR_SPECIAL(next) && scheme_isxdigit(next)) {
    int count = 0, n = 0, nbuf[8], maxc = ((ch == 'u') ? 4 : 6);
    while (count < maxc) {
      ch = scheme_peekc_special_ok(port);
      if (NOT_EOF_OR_SPECIAL(ch) && scheme_isxdigit(ch)) {
	nbuf[count] = ch;
	n = n*16 + (ch<='9' ? ch-'0' : (scheme_toupper(ch)-'A'+10));
	scheme_getc(port); /* must be ch */
	count++;
      } else
	break;
    }
    /* disallow surrogate points, etc. */
    if ((n < 0)
	|| ((n >= 0xD800) && (n <= 0xDFFF))
	|| (n > 0x10FFFF)) {
      scheme_read_err(port, stxsrc, line, col, pos, count + 2, 0, indentation,
		      "read: bad character constant #\\%c%u",
		      (maxc == 6) ? 'U' : 'u',
		      nbuf, count);
      return NULL;
    } else {
      ch = n;
    }
  } else if ((ch != EOF) && scheme_isalpha(ch) && NOT_EOF_OR_SPECIAL(next) && scheme_isalpha(next)) {
    mzchar *buf, *oldbuf, onstack[32];
    int i;
    long size = 31, oldsize;

    i = 1;
    buf = onstack;
    buf[0] = ch;
    while ((ch = scheme_peekc_special_ok(port), NOT_EOF_OR_SPECIAL(ch) && scheme_isalpha(ch))) {
      scheme_getc(port); /* is alpha character */
      if (i >= size) {
	oldsize = size;
	oldbuf = buf;

	size *= 2;
	buf = (mzchar *)scheme_malloc_atomic((size + 1) * sizeof(mzchar));
	memcpy(buf, oldbuf, oldsize * sizeof(mzchar));
      }
      buf[i++] = ch;
    }
    buf[i] = '\0';

    switch (scheme_tolower(buf[0])) {
    case 'n': /* maybe `newline' or 'null' or 'nul' */
      if (!u_strcmp(buf, "newline"))
	return scheme_make_char('\n');
      if (!u_strcmp(buf, "null") || !u_strcmp(buf, "nul"))
	return scheme_make_char('\0');
      break;
    case 's': /* maybe `space' */
      if (!u_strcmp(buf, "space"))
	return scheme_make_char(' ');
      break;
    case 'r': /* maybe `rubout' or `return' */
      if (!u_strcmp(buf, "rubout"))
	return scheme_make_char(0x7f);
      if (!u_strcmp(buf, "return"))
	return scheme_make_char('\r');
      break;
    case 'p': /* maybe `page' */
      if (!u_strcmp(buf, "page"))
	return scheme_make_char('\f');
      break;
    case 't': /* maybe `tab' */
      if (!u_strcmp(buf, "tab"))
	return scheme_make_char('\t');
      break;
    case 'v': /* maybe `vtab' */
      if (!u_strcmp(buf, "vtab"))
	return scheme_make_char(0xb);
      break;
    case 'b': /* maybe `backspace' */
      if (!u_strcmp(buf, "backspace"))
	return scheme_make_char('\b');
      break;
    case 'l': /* maybe `linefeed' */
      if (!u_strcmp(buf, "linefeed"))
	return scheme_make_char('\n');
      break;
    default:
      break;
    }

    scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), 0, indentation,
		    "read: bad character constant: #\\%5",
		    buf);
  }

  if (ch == EOF) {
    scheme_read_err(port, stxsrc, line, col, pos, 2, EOF, indentation,
		    "read: expected a character after #\\");
  }

  return scheme_make_char(ch);
}

/*========================================================================*/
/*                            quote readers                               */
/*========================================================================*/

/* "'", etc. has been read */
static Scheme_Object *
read_quote(char *who, Scheme_Object *quote_symbol, int len,
	   Scheme_Object *port,
	   Scheme_Object *stxsrc, long line, long col, long pos,
	   Scheme_Hash_Table **ht,
	   Scheme_Object *indentation, ReadParams *params)
{
  Scheme_Object *obj, *ret;

  obj = read_inner(port, stxsrc, ht, indentation, params, 0);
  if (SCHEME_EOFP(obj))
    scheme_read_err(port, stxsrc, line, col, pos, len, EOF, indentation,
		    "read: expected an element for %s (found end-of-file)",
		    who);
  ret = (stxsrc
	 ? scheme_make_stx_w_offset(quote_symbol, line, col, pos, len, stxsrc, STX_SRCTAG)
	 : quote_symbol);
  ret = scheme_make_pair(ret, scheme_make_pair(obj, scheme_null));
  if (stxsrc) {
    SCHEME_SET_PAIR_IMMUTABLE(ret);
    SCHEME_SET_PAIR_IMMUTABLE(SCHEME_CDR(ret));
    ret = scheme_make_stx_w_offset(ret, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG);
  }
  return ret;
}

/* "#&" has been read */
static Scheme_Object *read_box(Scheme_Object *port,
			       Scheme_Object *stxsrc, long line, long col, long pos,
			       Scheme_Hash_Table **ht,
			       Scheme_Object *indentation, ReadParams *params)
{
  Scheme_Object *o, *bx;

  o = read_inner(port, stxsrc, ht, indentation, params, 0);

  if (SCHEME_EOFP(o))
    scheme_read_err(port, stxsrc, line, col, pos, 2, EOF, indentation,
		    "read: expected an element for #& box (found end-of-file)");

  bx = scheme_box(o);

  if (stxsrc) {
    SCHEME_SET_BOX_IMMUTABLE(bx);
    bx = scheme_make_stx_w_offset(bx, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG);
  }

  return bx;
}

/*========================================================================*/
/*                         hash table reader                              */
/*========================================================================*/

/* "(" has been read */
static Scheme_Object *read_hash(Scheme_Object *port, Scheme_Object *stxsrc,
				long line, long col, long pos,
				char closer,  int eq,
				Scheme_Hash_Table **ht,
				Scheme_Object *indentation, ReadParams *params)
{
  Scheme_Object *l, *result;
  Scheme_Hash_Table *t;

  /* using mz_shape_hash_list ensures that l is a list of pairs */
  l = read_list(port, stxsrc, line, col, pos, closer, mz_shape_hash_list, 0, ht, indentation, params);

  if (eq)
    t = scheme_make_hash_table(SCHEME_hash_ptr);
  else
    t = scheme_make_hash_table_equal();

  /* Wait for placeholders to be resolved before mapping keys to
     values, because a placeholder may be used in a key. */
  scheme_hash_set(t, an_uninterned_symbol, l);

  if (!*ht) {
    /* So that resolve_references is called to build the table: */
    Scheme_Hash_Table *tht;
    tht = scheme_make_hash_table(SCHEME_hash_ptr);
    *ht = tht;
  }

  result = (Scheme_Object *)t;

  if (stxsrc)
    result = scheme_make_stx_w_offset(result, line, col, pos, SPAN(port, pos), stxsrc, STX_SRCTAG);

  return result;
}

/*========================================================================*/
/*                               utilities                                */
/*========================================================================*/

static int
skip_whitespace_comments(Scheme_Object *port, Scheme_Object *stxsrc,
			 Scheme_Hash_Table **ht, Scheme_Object *indentation, ReadParams *params)
{
  int ch;
  int blockc_1, blockc_2;

  if (params->honu_mode) {
    blockc_1 = '/';
    blockc_2 = '*';
  } else {
    blockc_1 = '#';
    blockc_2 = '|';
  }

 start_over:

  if (params->table) {
    while ((ch = scheme_getc_special_ok(port), NOT_EOF_OR_SPECIAL(ch))) {
      if (!(readtable_kind(params->table, ch, params) & READTABLE_WHITESPACE))
	break;
    }
    return ch;
  } else {
    while ((ch = scheme_getc_special_ok(port), NOT_EOF_OR_SPECIAL(ch) && scheme_isspace(ch))) {}
  }

  if ((!params->honu_mode && (ch == ';'))
      || (params->honu_mode && (ch == '/')
	  && (scheme_peekc_special_ok(port) == '/'))) {
    do {
      ch = scheme_getc_special_ok(port);
      if (ch == SCHEME_SPECIAL)
	scheme_get_ready_read_special(port, stxsrc, ht);
    } while (ch != '\n' && ch != '\r' && ch != EOF);
    goto start_over;
  }

  if (ch == blockc_1 && (scheme_peekc_special_ok(port) == blockc_2)) {
    int depth = 0;
    int ch2 = 0;
    long col, pos, line;

    scheme_tell_all(port, &line, &col, &pos);

    (void)scheme_getc(port); /* re-read '|' */
    do {
      ch = scheme_getc_special_ok(port);

      if (ch == EOF)
	scheme_read_err(port, stxsrc, line, col, pos, SPAN(port, pos), EOF, indentation,
			"read: end of file in #| comment");
      else if (ch == SCHEME_SPECIAL)
	scheme_get_ready_read_special(port, stxsrc, ht);

      if ((ch2 == blockc_2) && (ch == blockc_1)) {
	if (!(depth--))
	  goto start_over;
      } else if ((ch2 == blockc_1) && (ch == blockc_2)) {
	depth++;
	ch = 0; /* So we don't count '|' toward a closing "|#" */
      }
      ch2 = ch;
    } while (1);

    goto start_over;
  }
  if (ch == '#' && (scheme_peekc_special_ok(port) == ';')) {
    Scheme_Object *skipped;
    long col, pos, line;

    scheme_tell_all(port, &line, &col, &pos);

    track_indentation(indentation, line, col);

    (void)scheme_getc(port); /* re-read ';' */

    skipped = read_inner(port, stxsrc, ht, indentation, params, 0);
    if (SCHEME_EOFP(skipped))
      scheme_read_err(port, stxsrc, line, col, pos,  SPAN(port, pos), EOF, indentation,
		      "read: expected a commented-out element for `#;' (found end-of-file)");

    /* For resolving graphs introduced in #; : */
    if (*ht) {
      Scheme_Object *v;
      v = scheme_hash_get(*ht, an_uninterned_symbol);
      if (!v)
	v = scheme_null;
      v = scheme_make_pair(skipped, v);
      scheme_hash_set(*ht, an_uninterned_symbol, v);
    }

    goto start_over;
  }

  return ch;
}

static void unexpected_closer(int ch,
			      Scheme_Object *port, Scheme_Object *stxsrc,
			      long line, long col, long pos,
			      Scheme_Object *indentation)
{
  char *suggestion = "", *found = "unexpected";

  if (SCHEME_PAIRP(indentation)) {
    Scheme_Indent *indt;
    int opener;
    char *missing;

    indt = (Scheme_Indent *)SCHEME_CAR(indentation);

    found = scheme_malloc_atomic(100);

    if (indt->closer == '}')
      opener = '{';
    else if (indt->closer == ']')
      opener = '[';
    else
      opener = '(';

    /* Missing intermediate closers, or just need something else entirely? */
    {
      Scheme_Object *l;
      Scheme_Indent *indt2;

      missing = "expected";
      for (l = SCHEME_CDR(indentation); SCHEME_PAIRP(l); l = SCHEME_CDR(l)) {
	indt2 = (Scheme_Indent *)SCHEME_CAR(l);
	if (indt2->closer == ch) {
	  missing = "missing";
	}
      }
    }

    if (ch == indt->closer) {
      sprintf(found, "unexpected");
    } else if (indt->multiline) {
      sprintf(found,
	      "%s '%c' to close '%c' on line %ld, found instead",
	      missing,
	      indt->closer,
	      opener,
	      indt->start_line);
    } else {
      sprintf(found,
	      "%s '%c' to close preceding '%c', found instead",
	      missing,
	      indt->closer,
	      opener);
    }

    if (indt->suspicious_line) {
      suggestion = scheme_malloc_atomic(100);
      sprintf(suggestion,
	      "; indentation suggests a missing '%c' before line %ld",
	      indt->suspicious_closer,
	      indt->suspicious_line);
    }
  }

  scheme_read_err(port, stxsrc, line, col, pos, 1, 0, indentation, "read: %s '%c'%s",
		  found, ch, suggestion);
}

static void pop_indentation(Scheme_Object *indentation)
{
  /* Pop off indentation stack, and propagate
     suspicions if none found earlier. */
  if (SCHEME_PAIRP(indentation)) {
    Scheme_Indent *indt;
    indt = (Scheme_Indent *)SCHEME_CAR(indentation);
    indentation = SCHEME_CDR(indentation);
    if (SCHEME_PAIRP(indentation)) {
      Scheme_Indent *old_indt;
      old_indt = (Scheme_Indent *)SCHEME_CAR(indentation);

      if (!old_indt->suspicious_line) {
	if (indt->suspicious_line) {
	  old_indt->suspicious_line = indt->suspicious_line;
	  old_indt->suspicious_closer = indt->suspicious_closer;
	}
      }
      if (!old_indt->suspicious_quote) {
	if (indt->suspicious_quote) {
	  old_indt->suspicious_quote = indt->suspicious_quote;
	  old_indt->quote_for_char = indt->quote_for_char;
	}
      }
    }
  }
}

/*========================================================================*/
/*                               .zo reader                               */
/*========================================================================*/

#define ZO_CHECK(x) if (!(x)) scheme_ill_formed_code(port);
#define RANGE_CHECK(x, y) ZO_CHECK (x y)
#define RANGE_CHECK_GETS(x) RANGE_CHECK(x, <= port->size - port->pos)

typedef struct CPort {
  MZTAG_IF_REQUIRED
  unsigned long pos, size;
  unsigned char *start;
  unsigned long symtab_size;
  long base;
  Scheme_Object *orig_port;
  Scheme_Hash_Table **ht;
  Scheme_Object **symtab;
  Scheme_Object *insp; /* inspector for module-variable access */
} CPort;
#define CP_GETC(cp) ((int)(cp->start[cp->pos++]))
#define CP_TELL(port) (port->pos + port->base)

static Scheme_Object *read_marshalled(int type, CPort *port);
static Scheme_Object *read_compact_list(int c, int proper, int use_stack, CPort *port);
static Scheme_Object *read_compact_quote(CPort *port, int embedded);

void scheme_ill_formed(struct CPort *port
#if TRACK_ILL_FORMED_CATCH_LINES
		       , const char *file, int line
#endif
		       )
{
  scheme_read_err(port->orig_port, NULL, -1, -1, CP_TELL(port), -1, 0, NULL,
		  "read (compiled): ill-formed code"
#if TRACK_ILL_FORMED_CATCH_LINES
		  " [%s:%d]", file, line
#endif
		  );
}

static long read_compact_number(CPort *port)
{
  /* >>> See also read_compact_number_from_port(), below. <<< */

  long flag, v, a, b, c, d;

  ZO_CHECK(port->pos < port->size);

  flag = CP_GETC(port);

  if (flag < 252)
    return flag;
  else if (flag == 252) {
    ZO_CHECK(port->pos + 1 < port->size);

    a = CP_GETC(port);
    b = CP_GETC(port);

    v = a
      + (b << 8);
    return v;
  } else if (flag == 254) {
    ZO_CHECK(port->pos < port->size);

    return -CP_GETC(port);
  }

  ZO_CHECK(port->pos + 3 < port->size);

  a = CP_GETC(port);
  b = CP_GETC(port);
  c = CP_GETC(port);
  d = CP_GETC(port);

  v = a
    + (b << 8)
    + (c << 16)
    + (d << 24);

  if (flag == 253)
    return v;
  else
    return -v;
}

static char *read_compact_chars(CPort *port,
				char *buffer,
				int bsize, int l)
{
  /* Range check is performed before the function is called. */
  char *s;

  if (l < bsize)
    s = buffer;
  else
    s = (char *)scheme_malloc_atomic(l + 1);

  memcpy(s, port->start + port->pos, l);
  port->pos += l;

  s[l] = 0;

  return s;
}

static Scheme_Object *read_compact_svector(CPort *port, int l)
{
  Scheme_Object *o;
  mzshort *v;

  o = scheme_alloc_object();
  o->type = scheme_svector_type;

  SCHEME_SVEC_LEN(o) = l;
  if (l)
    v = MALLOC_N_ATOMIC(mzshort, l);
  else
    v = NULL;
  SCHEME_SVEC_VEC(o) = v;

  while (l--) {
    mzshort cn;
    cn = read_compact_number(port);
    v[l] = cn;
  }

  return o;
}

static int cpt_branch[256];

static Scheme_Object *read_compact(CPort *port, int use_stack);

static Scheme_Object *read_compact_k(void)
{
  Scheme_Thread *p = scheme_current_thread;
  CPort *port = (CPort *)p->ku.k.p1;

  p->ku.k.p1 = NULL;

  return read_compact(port, p->ku.k.i1);
}

static Scheme_Object *read_compact(CPort *port, int use_stack)
{
#define BLK_BUF_SIZE 32
  unsigned int l;
  char *s, buffer[BLK_BUF_SIZE];
  int ch;
  int need_car = 0, proper = 0;
  Scheme_Object *v, *first = NULL, *last = NULL;

#ifdef DO_STACK_CHECK
  {
# include "mzstkchk.h"
    {
      Scheme_Thread *p = scheme_current_thread;
      p->ku.k.p1 = (void *)port;
      p->ku.k.i1 = use_stack;
      return scheme_handle_stack_overflow(read_compact_k);
    }
  }
#endif

  while (1) {
    ZO_CHECK(port->pos < port->size);
    ch = CP_GETC(port);

    switch(cpt_branch[ch]) {
    case CPT_ESCAPE:
      {
	int len;
	Scheme_Object *ep;
	char *s;
	ReadParams params;

	len = read_compact_number(port);

	RANGE_CHECK_GETS((unsigned)len);

#if defined(MZ_PRECISE_GC)
	s = read_compact_chars(port, buffer, BLK_BUF_SIZE, len);
	if (s != buffer)
	  len = -len; /* no alloc in sized_byte_string_input_port */
#else
	s = (char *)port->start + port->pos;
	port->pos += len;
	len = -len; /* no alloc in sized_byte_string_input_port */
#endif

	ep = scheme_make_sized_byte_string_input_port(s, len);

	params.can_read_compiled = 1;
	params.can_read_pipe_quote = 1;
	params.can_read_box = 1;
	params.can_read_graph = 1;
	/* Use startup value of case sensitivity so legacy code will work. */
	params.case_sensitive = scheme_case_sensitive;
	params.square_brackets_are_parens = 1;
	params.curly_braces_are_parens = 1;
	params.read_decimal_inexact = 1;
	params.can_read_dot = 1;
	params.can_read_quasi = 1;
	params.honu_mode = 0;
	params.table = NULL;

	v = read_inner(ep, NULL, port->ht, scheme_null, &params, 0);
      }
      break;
    case CPT_SYMBOL:
      l = read_compact_number(port);
      RANGE_CHECK_GETS(l);
      s = read_compact_chars(port, buffer, BLK_BUF_SIZE, l);
      v = scheme_intern_exact_symbol(s, l);

      l = read_compact_number(port);
      RANGE_CHECK(l, < port->symtab_size);
      port->symtab[l] = v;
      break;
    case CPT_SYMREF:
      l = read_compact_number(port);
      RANGE_CHECK(l, < port->symtab_size);
      v = port->symtab[l];
      break;
    case CPT_WEIRD_SYMBOL:
      {
	int uninterned;

	uninterned = read_compact_number(port);

	l = read_compact_number(port);
	RANGE_CHECK_GETS(l);
	s = read_compact_chars(port, buffer, BLK_BUF_SIZE, l);

	if (uninterned)
	  v = scheme_make_exact_symbol(s, l);
	else
	  v = scheme_intern_exact_parallel_symbol(s, l);

	l = read_compact_number(port);
	RANGE_CHECK(l, < port->symtab_size);
	port->symtab[l] = v;
	/* The fact that other uses of the symbol go through the table
	   means that uninterned symbols are consistently re-created for
	   a particular compiled expression. */
      }
      break;
    case CPT_BYTE_STRING:
      l = read_compact_number(port);
      RANGE_CHECK_GETS(l);
      s = read_compact_chars(port, buffer, BLK_BUF_SIZE, l);
      v = scheme_make_immutable_sized_byte_string(s, l, l < BLK_BUF_SIZE);
      break;
    case CPT_CHAR_STRING:
      {
	unsigned int el;
	mzchar *us;
	el = read_compact_number(port);
	l = read_compact_number(port);
	RANGE_CHECK_GETS(el);
	s = read_compact_chars(port, buffer, BLK_BUF_SIZE, el);
	us = (mzchar *)scheme_malloc_atomic((l + 1) * sizeof(mzchar));
	scheme_utf8_decode_all(s, el, us, 0);
	us[l] = 0;
	v = scheme_make_immutable_sized_char_string(us, l, 0);
      }
      break;
    case CPT_CHAR:
      l = read_compact_number(port);
      v = scheme_make_character(l);
      break;
    case CPT_INT:
      v = scheme_make_integer(read_compact_number(port));
      break;
    case CPT_NULL:
      v = scheme_null;
      break;
    case CPT_TRUE:
      v = scheme_true;
      break;
    case CPT_FALSE:
      v = scheme_false;
      break;
    case CPT_VOID:
      v = scheme_void;
      break;
    case CPT_BOX:
      v = scheme_box(read_compact(port, 0));
      break;
    case CPT_PAIR:
      if (need_car) {
	Scheme_Object *car, *cdr;
	car = read_compact(port, 0);
	cdr = read_compact(port, 0);
	v = scheme_make_pair(car, cdr);
      } else {
	need_car = 1;
	continue;
      }
      break;
    case CPT_LIST:
      l = read_compact_number(port);
      if (need_car) {
	if (l == 1) {
	  Scheme_Object *car, *cdr;
	  car = read_compact(port, 0);
	  cdr = read_compact(port, 0);
	  v = scheme_make_pair(car, cdr);
	} else
	  v = read_compact_list(l, 0, 0, port);
      } else {
	need_car = l;
	continue;
      }
      break;
    case CPT_VECTOR:
      {
	Scheme_Object *vec;
	unsigned int i;

	l = read_compact_number(port);
	vec = scheme_make_vector(l, NULL);

	for (i = 0; i < l; i++) {
	  Scheme_Object *cv;
	  cv = read_compact(port, 0);
	  SCHEME_VEC_ELS(vec)[i] = cv;
	}

	v = vec;
      }
      break;
    case CPT_HASH_TABLE:
      {
	Scheme_Hash_Table *t;
	Scheme_Object *l;
	int eq, len;

	eq = read_compact_number(port);
	if (eq)
	  t = scheme_make_hash_table_equal();
	else
	  t = scheme_make_hash_table(SCHEME_hash_ptr);
	len = read_compact_number(port);
	
	l = scheme_null;
	while (len--) {
	  Scheme_Object *k, *v;
	  k = read_compact(port, 0);
	  v = read_compact(port, 0);
	  /* We can't always hash directly, because a key or value
	     might have a graph reference inside it. */
	  l = scheme_make_pair(scheme_make_pair(k, v), l);
	}

	/* Map an unintenred sym to l so that resolve_references
	   completes the table construction. */
	scheme_hash_set(t, an_uninterned_symbol, l);
	if (!(*port->ht)) {
	  /* So that resolve_references is called to build the table: */
	  Scheme_Hash_Table *tht;
	  tht = scheme_make_hash_table(SCHEME_hash_ptr);
	  *(port->ht) = tht;
	}

	v = (Scheme_Object *)t;
      }
      break;
    case CPT_STX:
    case CPT_GSTX:
      {
	if (!local_rename_memory) {
	  Scheme_Hash_Table *rht;
	  rht = scheme_make_hash_table(SCHEME_hash_ptr);
	  local_rename_memory = rht;
	}

	v = read_compact(port, 1);
	v = scheme_datum_to_syntax(v, scheme_false, (Scheme_Object *)local_rename_memory,
				   ch == CPT_GSTX, 0);
	if (!v)
	  scheme_ill_formed_code(port);
      }
      break;
    case CPT_MARSHALLED:
      v = read_marshalled(read_compact_number(port), port);
      break;
    case CPT_QUOTE:
      v = read_compact_quote(port, 1);
      break;
    case CPT_REFERENCE:
      l = read_compact_number(port);
      RANGE_CHECK(l, < EXPECTED_PRIM_COUNT);
      v = variable_references[l];
      break;
    case CPT_LOCAL:
      {
	int p;
	p = read_compact_number(port);
	if (p < 0)
	  scheme_ill_formed_code(port);
	v = scheme_make_local(scheme_local_type, p);
      }
      break;
    case CPT_LOCAL_UNBOX:
      {
	int p;
	p = read_compact_number(port);
	if (p < 0)
	  scheme_ill_formed_code(port);
	v = scheme_make_local(scheme_local_unbox_type, p);
      }
      break;
    case CPT_SVECTOR:
      {
	int l;
	l = read_compact_number(port);
	v = read_compact_svector(port, l);
      }
      break;
    case CPT_APPLICATION:
      {
	int c, i;
	Scheme_App_Rec *a;

	c = read_compact_number(port) + 1;

	a = scheme_malloc_application(c);
	for (i = 0; i < c; i++) {
	  v = read_compact(port, 1);
	  a->args[i] = v;
	}

	scheme_finish_application(a);
	v = (Scheme_Object *)a;
      }
      break;
    case CPT_LET_ONE:
      {
	Scheme_Let_One *lo;
	int et;

	lo = (Scheme_Let_One *)scheme_malloc_tagged(sizeof(Scheme_Let_One));
	lo->iso.so.type = scheme_let_one_type;

	v = read_compact(port, 1);
	lo->value = v;
	v = read_compact(port, 1);
	lo->body = v;
	et = scheme_get_eval_type(lo->value);
	SCHEME_LET_EVAL_TYPE(lo) = et;

	v = (Scheme_Object *)lo;
      }
      break;
    case CPT_BRANCH:
      {
	Scheme_Object *test, *tbranch, *fbranch;
	test = read_compact(port, 1);
	tbranch = read_compact(port, 1);
	fbranch = read_compact(port, 1);
	v = scheme_make_branch(test, tbranch, fbranch);
      }
      break;
    case CPT_MODULE_INDEX:
	{
	  Scheme_Object *path, *base;

	  l = read_compact_number(port); /* symtab index */
	  path = read_compact(port, 0);
	  base = read_compact(port, 0);

	  v = scheme_make_modidx(path, base, scheme_false);

	  RANGE_CHECK(l, < port->symtab_size);
	  port->symtab[l] = v;
	}
	break;
    case CPT_MODULE_VAR:
      {
	Module_Variable *mv;
	Scheme_Object *mod, *var;
	int pos;

	l = read_compact_number(port); /* symtab index */
	mod = read_compact(port, 0);
	var = read_compact(port, 0);
	pos = read_compact_number(port);

	mv = MALLOC_ONE_TAGGED(Module_Variable);
	mv->so.type = scheme_module_variable_type;
	mv->modidx = mod;
	mv->insp = port->insp;
	mv->sym = var;
	mv->pos = pos;

	v = (Scheme_Object *)mv;

	RANGE_CHECK(l, < port->symtab_size);
	port->symtab[l] = v;
      }
      break;
    case CPT_SMALL_LOCAL_START:
    case CPT_SMALL_LOCAL_UNBOX_START:
      {
	Scheme_Type type;
	int k;

	if (CPT_BETWEEN(ch, SMALL_LOCAL_UNBOX)) {
	  k = 1;
	  type = scheme_local_unbox_type;
	  ch -= CPT_SMALL_LOCAL_UNBOX_START;
	} else {
	  k = 0;
	  type = scheme_local_type;
	  ch -= CPT_SMALL_LOCAL_START;
	}
	if (ch < MAX_CONST_LOCAL_POS)
	  v = scheme_local[ch][k];
	else
	  v = scheme_make_local(type, ch);
      }
      break;
    case CPT_SMALL_MARSHALLED_START:
      {
	l = ch - CPT_SMALL_MARSHALLED_START;
	v = read_marshalled(l, port);
      }
      break;
    case CPT_SMALL_SYMBOL_START:
      {
	l = ch - CPT_SMALL_SYMBOL_START;
	RANGE_CHECK_GETS(l);
	s = read_compact_chars(port, buffer, BLK_BUF_SIZE, l);
	v = scheme_intern_exact_symbol(s, l);

	l = read_compact_number(port);
	RANGE_CHECK(l, < port->symtab_size);
	port->symtab[l] = v;
      }
      break;
    case CPT_SMALL_NUMBER_START:
      {
	l = ch - CPT_SMALL_NUMBER_START;
	v = scheme_make_integer(l);
      }
      break;
    case CPT_SMALL_SVECTOR_START:
      {
	l = ch - CPT_SMALL_SVECTOR_START;
	v = read_compact_svector(port, l);
      }
      break;
    case CPT_SMALL_PROPER_LIST_START:
    case CPT_SMALL_LIST_START:
      {
	int ppr = CPT_BETWEEN(ch, SMALL_PROPER_LIST);
	l = ch - (ppr ? CPT_SMALL_PROPER_LIST_START : CPT_SMALL_LIST_START);
      	if (need_car) {
	  if (l == 1) {
	    Scheme_Object *car, *cdr;
	    car = read_compact(port, 0);
	    cdr = (ppr
		   ? scheme_null
		   : read_compact(port, 0));
	    v = scheme_make_pair(car, cdr);
	  } else
	    v = read_compact_list(l, ppr, /* use_stack */ 0, port);
	} else {
	  proper = ppr;
	  need_car = l;
	  continue;
	}
      }
      break;
    case CPT_SMALL_APPLICATION_START:
      {
	int c, i;
	Scheme_App_Rec *a;

	c = (ch - CPT_SMALL_APPLICATION_START) + 1;

	a = scheme_malloc_application(c);
	for (i = 0; i < c; i++) {
	  v = read_compact(port, 1);
	  a->args[i] = v;
	}

	scheme_finish_application(a);

	v = (Scheme_Object *)a;
      }
      break;
    case CPT_SMALL_APPLICATION2:
      {
	short et;
	Scheme_App2_Rec *app;

	app = MALLOC_ONE_TAGGED(Scheme_App2_Rec);
	app->iso.so.type = scheme_application2_type;

	v = read_compact(port, 1);
	app->rator = v;
	v = read_compact(port, 1);
	app->rand = v;

	et = scheme_get_eval_type(app->rand);
	et = et << 3;
	et += scheme_get_eval_type(app->rator);
	SCHEME_APPN_FLAGS(app) = et;

	v = (Scheme_Object *)app;
      }
      break;
    case CPT_SMALL_APPLICATION3:
      {
	short et;
	Scheme_App3_Rec *app;

	app = MALLOC_ONE_TAGGED(Scheme_App3_Rec);
	app->iso.so.type = scheme_application3_type;

	v = read_compact(port, 1);
	app->rator = v;
	v = read_compact(port, 1);
	app->rand1 = v;
	v = read_compact(port, 1);
	app->rand2 = v;

	et = scheme_get_eval_type(app->rand2);
	et = et << 3;
	et += scheme_get_eval_type(app->rand1);
	et = et << 3;
	et += scheme_get_eval_type(app->rator);
	SCHEME_APPN_FLAGS(app) = et;

	v = (Scheme_Object *)app;
      }
      break;
    default:
      v = NULL;
      break;
    }

    if (!v)
      scheme_ill_formed_code(port);

    if (need_car) {
      Scheme_Object *pair;

      if (USE_LISTSTACK(use_stack)) {
	if (local_list_stack_pos >= NUM_CELLS_PER_STACK) {
	  /* Overflow */
	  Scheme_Simple_Object *sa;
	  sa = MALLOC_N_RT(Scheme_Simple_Object, NUM_CELLS_PER_STACK);
	  local_list_stack = sa;
	  local_list_stack_pos = 0;
	}

	pair = (Scheme_Object *)(local_list_stack + (local_list_stack_pos++));
	pair->type = scheme_pair_type;
	SCHEME_CAR(pair) = v;
	SCHEME_CDR(pair) = scheme_null;
      } else
	pair = scheme_make_pair(v, scheme_null);

      if (last)
	SCHEME_CDR(last) = pair;
      else
	first = pair;
      last = pair;
      --need_car;
      if (!need_car && proper)
	break;
    } else {
      if (last)
	SCHEME_CDR(last) = v;
      break;
    }
  }

  return first ? first : v;
}

static Scheme_Object *read_compact_list(int c, int proper, int use_stack, CPort *port)
{
  Scheme_Object *v, *first, *last, *pair;

  v = read_compact(port, 0);
  if (USE_LISTSTACK(use_stack)) {
    if (local_list_stack_pos >= NUM_CELLS_PER_STACK) {
      /* Overflow */
      Scheme_Simple_Object *sa;
      sa = MALLOC_N_RT(Scheme_Simple_Object, NUM_CELLS_PER_STACK);
      local_list_stack = sa;
      local_list_stack_pos = 0;
    }

    last = (Scheme_Object *)(local_list_stack + (local_list_stack_pos++));
    last->type = scheme_pair_type;
    SCHEME_CAR(last) = v;
    SCHEME_CDR(last) = scheme_null;
  } else
    last = scheme_make_pair(v, scheme_null);

  first = last;

  while (--c) {
    v = read_compact(port, 0);

    if (USE_LISTSTACK(use_stack)) {
      if (local_list_stack_pos >= NUM_CELLS_PER_STACK) {
	/* Overflow */
	Scheme_Simple_Object *sa;
	sa = MALLOC_N_RT(Scheme_Simple_Object, NUM_CELLS_PER_STACK);
	local_list_stack = sa;
	local_list_stack_pos = 0;
      }

      pair = (Scheme_Object *)(local_list_stack + (local_list_stack_pos++));
      pair->type = scheme_pair_type;
      SCHEME_CAR(pair) = v;
      SCHEME_CDR(pair) = scheme_null;
    } else
      pair = scheme_make_pair(v, scheme_null);

    SCHEME_CDR(last) = pair;
    last = pair;
  }

  if (!proper) {
    v = read_compact(port, 0);
    SCHEME_CDR(last) = v;
  }

  return first;
}

static Scheme_Object *read_compact_quote(CPort *port, int embedded)
{
  Scheme_Hash_Table **q_ht, **old_ht;
  Scheme_Object *v;

  /* Use a new hash table. A compiled quoted form may have graph
     structure, but only local graph structure is allowed. */
  q_ht = MALLOC_N(Scheme_Hash_Table *, 1);
  *q_ht = NULL;

  old_ht = port->ht;
  port->ht = q_ht;

  v = read_compact(port, 0);

  port->ht = old_ht;

  if (*q_ht)
    resolve_references(v, NULL, 0);

  return v;
}

static Scheme_Object *read_marshalled(int type, CPort *port)
{
  Scheme_Object *l;
  ListStackRec r;
  Scheme_Type_Reader reader;

  STACK_START(r);
  l = read_compact(port, 1);

  if ((type < 0) || (type >= _scheme_last_type_)) {
    STACK_END(r);
    scheme_ill_formed_code(port);
  }

  reader = scheme_type_readers[type];

  if (!reader) {
    STACK_END(r);
    scheme_ill_formed_code(port);
  }

  l = reader(l);

  STACK_END(r);

  if (!l)
    scheme_ill_formed_code(port);

  return l;
}

static long read_compact_number_from_port(Scheme_Object *port)
{
  /* >>> See also read_compact_number_port(), above. <<< */

  long flag, v, a, b, c, d;

  flag = scheme_get_byte(port);

  if (flag < 252)
    return flag;
  if (flag == 254)
    return -scheme_get_byte(port);

  a = scheme_get_byte(port);
  b = scheme_get_byte(port);

  if (flag == 252) {
    v = a
      + (b << 8);
    return v;
  }

  c = scheme_get_byte(port);
  d = scheme_get_byte(port);

  v = a
    + (b << 8)
    + (c << 16)
    + (d << 24);

  if (flag == 253)
    return v;
  else
    return -v;
}

/* "#~" has been read */
static Scheme_Object *read_compiled(Scheme_Object *port,
				    Scheme_Hash_Table **ht)
{
  Scheme_Thread *p = scheme_current_thread;
  Scheme_Object *result, *insp;
  long size, got;
  CPort *rp;
  long symtabsize;
  Scheme_Object **symtab;

  if (USE_LISTSTACK(!p->list_stack))
    scheme_alloc_list_stack(p);

  if (!cpt_branch[1]) {
    int i;

    for (i = 0; i < 256; i++) {
      cpt_branch[i] = i;
    }

#define FILL_IN(v) \
    for (i = CPT_ ## v ## _START; i < CPT_ ## v ## _END; i++) { \
      cpt_branch[i] = CPT_ ## v ## _START; \
    }
    FILL_IN(SMALL_NUMBER);
    FILL_IN(SMALL_SYMBOL);
    FILL_IN(SMALL_MARSHALLED);
    FILL_IN(SMALL_LIST);
    FILL_IN(SMALL_PROPER_LIST);
    FILL_IN(SMALL_LOCAL);
    FILL_IN(SMALL_LOCAL_UNBOX);
    FILL_IN(SMALL_SVECTOR);
    FILL_IN(SMALL_APPLICATION);

    /* These two are handled specially: */
    cpt_branch[CPT_SMALL_APPLICATION2] = CPT_SMALL_APPLICATION2;
    cpt_branch[CPT_SMALL_APPLICATION3] = CPT_SMALL_APPLICATION3;
  }

  if (!variable_references)
    variable_references = scheme_make_builtin_references_table();

  /* Check version: */
  size = read_compact_number_from_port(port);
  {
    char buf[64];

    if (size < 0) size = 0;
    if (size > 63) size = 63;

    got = scheme_get_bytes(port, size, buf, 0);
    buf[got] = 0;

    if (strcmp(buf, MZSCHEME_VERSION))
      scheme_read_err(port, NULL, -1, -1, -1, -1, 0, NULL,
		      "read (compiled): code compiled for version %s, not %s",
		      (buf[0] ? buf : "???"), MZSCHEME_VERSION);
  }

  symtabsize = read_compact_number_from_port(port);

  size = read_compact_number_from_port(port);
  rp = MALLOC_ONE_RT(CPort);
#ifdef MZ_PRECISE_GC
  rp->type = scheme_rt_compact_port;
#endif
  {
    unsigned char *st;
    st = (unsigned char *)scheme_malloc_atomic(size);
    rp->start = st;
  }
  rp->pos = 0;
  {
    long base;
    scheme_tell_all(port, NULL, NULL, &base);
    rp->base = base;
  }
  rp->orig_port = port;
  rp->size = size;
  if ((got = scheme_get_bytes(port, size, (char *)rp->start, 0)) != size)
    scheme_read_err(port, NULL, -1, -1, -1, -1, 0, NULL,
		    "read (compiled): ill-formed code (bad count: %ld != %ld, started at %ld)",
		    got, size, rp->base);

  local_rename_memory = NULL;

  symtab = MALLOC_N(Scheme_Object *, symtabsize);
  rp->symtab_size = symtabsize;
  rp->ht = ht;
  rp->symtab = symtab;

  insp = scheme_get_param(scheme_current_config(), MZCONFIG_CODE_INSPECTOR);
  rp->insp = insp;

  result = read_marshalled(scheme_compilation_top_type, rp);

  local_rename_memory = NULL;

  if (SAME_TYPE(SCHEME_TYPE(result), scheme_compilation_top_type)) {
    Scheme_Compilation_Top *top = (Scheme_Compilation_Top *)result;

    scheme_validate_code(rp, top->code,
			 top->max_let_depth,
			 top->prefix->num_toplevels,
			 top->prefix->num_stxes);
    /* If no exception, the the resulting code is ok. */
  } else
    scheme_ill_formed_code(rp);

  return result;
}

/*========================================================================*/
/*                           readtable support                            */
/*========================================================================*/

Scheme_Object *scheme_make_default_readtable()
{
  return scheme_false;
}

static int readtable_kind(Readtable *t, int ch, ReadParams *params)
{
  int k;
  Scheme_Object *v;

  if (ch < 128)
    k = t->fast_mapping[ch];
  else {
    v = scheme_hash_get(t->mapping, scheme_make_integer(ch));
    if (!v) {
      if (scheme_isspace(ch))
	k = READTABLE_WHITESPACE;
      else
	k = READTABLE_CONTINUING;
    } else
      k = SCHEME_INT_VAL(SCHEME_CAR(v));
  }

  if (k == READTABLE_MAPPED) {
    /* ch is mapped to a default behavior: */
    v = scheme_hash_get(t->mapping, scheme_make_integer(ch));
    ch = SCHEME_INT_VAL(SCHEME_CDR(v));
    if (ch < 128)
      k = builtin_fast[ch];
    else if (scheme_isspace(ch))
      k = READTABLE_WHITESPACE;
    else
      k = READTABLE_CONTINUING;
  }

  if (k == READTABLE_MULTIPLE_ESCAPE) {
    /* This is the only one sensitive to params. */
    if (!params->can_read_pipe_quote)
      return READTABLE_CONTINUING;
  }

  return k;
}

static Scheme_Object *readtable_call(int w_char, int ch, Scheme_Object *proc, ReadParams *params,
				     Scheme_Object *port, Scheme_Object *src, long line, long col, long pos,
				     Scheme_Hash_Table **ht)
{
  int cnt;
  Scheme_Object *a[6], *v;
  Scheme_Cont_Frame_Data cframe;
  
  if (w_char) {
    a[0] = scheme_make_character(ch);
    a[1] = port;
    a[2] = proc;
    if (!src && scheme_check_proc_arity(NULL, 2, 2, 3, a)) {
      cnt = 2;
    } else {
      cnt = 6;
      a[2] = (src ? src : scheme_false);
      a[3] = (line > 0) ? scheme_make_integer(line) : scheme_false;
      a[4] = (col > 0) ? scheme_make_integer(col-1) : scheme_false;
      a[5] = (pos > 0) ? scheme_make_integer(pos) : scheme_false;
    }
  } else {
    if (src) {
      cnt = 2;
      a[0] = src;
      a[1] = port;
    } else {
      cnt = 1;
      a[0] = port;
    }
  }

  scheme_push_continuation_frame(&cframe);
  scheme_set_in_read_mark(src, ht);

  v = scheme_apply(proc, cnt, a);

  scheme_pop_continuation_frame(&cframe);

  if (!scheme_special_comment_value(v)
      && !SAME_TYPE(scheme_placeholder_type, SCHEME_TYPE(v))) {
    if (SCHEME_STXP(v)) {
      if (!src)
	v = scheme_syntax_to_datum(v, 0, NULL);
    } else if (src) {
      Scheme_Object *s;
      s = scheme_make_stx_w_offset(scheme_false, line, col, pos, SPAN(port, pos), src, STX_SRCTAG);
      v = scheme_datum_to_syntax(v, s, scheme_false, 1, 0);
    }

    v = copy_to_protect_placeholders(v, src, ht);
  }
  
  return v;
}

void scheme_set_in_read_mark(Scheme_Object *src, Scheme_Hash_Table **ht)
{
  Scheme_Object *v;

  if (ht)
    v = scheme_make_pair((Scheme_Object *)ht, 
			 (src ? scheme_true : scheme_false));
  else
    v = scheme_false;
  scheme_set_cont_mark(an_uninterned_symbol, v);
}

static Scheme_Object *readtable_handle(Readtable *t, int *_ch, int *_use_default, ReadParams *params,
				       Scheme_Object *port, Scheme_Object *src, long line, long col, long pos,
				       Scheme_Hash_Table **ht)
{
  int ch = *_ch;
  Scheme_Object *v;

  v = scheme_hash_get(t->mapping, scheme_make_integer(ch));

  if (!v) {
    *_use_default = 1;
    return NULL;
  }

  if (SCHEME_INT_VAL(SCHEME_CAR(v)) == READTABLE_MAPPED) {
    *_ch = SCHEME_INT_VAL(SCHEME_CDR(v));
    *_use_default = 1;
    return NULL;
  }

  *_use_default = 0;

  v = SCHEME_CDR(v);

  v = readtable_call(1, ch, v, params, port, src, line, col, pos, ht);
  
  return v;
}

static Scheme_Object *readtable_handle_hash(Readtable *t, int ch, int *_use_default, ReadParams *params,
					    Scheme_Object *port, Scheme_Object *src, long line, long col, long pos,
					    Scheme_Hash_Table **ht)
{
  Scheme_Object *v;

  v = scheme_hash_get(t->mapping, scheme_make_integer(-ch));

  if (!v) {
    *_use_default = 1;
    return NULL;
  }

  *_use_default = 0;

  v = readtable_call(1, ch, v, params, port, src, line, col, pos, ht);

  if (scheme_special_comment_value(v))
    return NULL;
  else
    return v;
}

static Scheme_Object *make_readtable(int argc, Scheme_Object **argv)
{
  Scheme_Object *sym, *val;
  Readtable *t, *orig_t;
  Scheme_Hash_Table *ht;
  char *fast;
  int i, ch;

  if (SCHEME_FALSEP(argv[0]))
    orig_t = NULL;
  else {
    if (!SAME_TYPE(scheme_readtable_type, SCHEME_TYPE(argv[0]))) {
      scheme_wrong_type("make-readtable", "readtable or #f", 0, argc, argv);
      return NULL;
    }
    orig_t = (Readtable *)argv[0];
  }

  if (!terminating_macro_symbol) {
    REGISTER_SO(terminating_macro_symbol);
    REGISTER_SO(non_terminating_macro_symbol);
    REGISTER_SO(dispatch_macro_symbol);
    REGISTER_SO(builtin_fast);
    terminating_macro_symbol = scheme_intern_symbol("terminating-macro");
    non_terminating_macro_symbol = scheme_intern_symbol("non-terminating-macro");
    dispatch_macro_symbol = scheme_intern_symbol("dispatch-macro");
    
    fast = scheme_malloc_atomic(128);
    memset(fast, READTABLE_CONTINUING, 128);
    for (i = 0; i < 128; i++) {
      if (scheme_isspace(i))
	fast[i] = READTABLE_WHITESPACE;
    }
    fast[';'] = READTABLE_TERMINATING;
    fast['\''] = READTABLE_TERMINATING;
    fast[','] = READTABLE_TERMINATING;
    fast['"'] = READTABLE_TERMINATING;
    fast['|'] = READTABLE_MULTIPLE_ESCAPE;
    fast['\\'] = READTABLE_SINGLE_ESCAPE;
    fast['('] = READTABLE_TERMINATING;
    fast['['] = READTABLE_TERMINATING;
    fast['{'] = READTABLE_TERMINATING;
    fast[')'] = READTABLE_TERMINATING;
    fast[']'] = READTABLE_TERMINATING;
    fast['}'] = READTABLE_TERMINATING;
    builtin_fast = fast;
  }

  t = MALLOC_ONE_TAGGED(Readtable);
  t->so.type = scheme_readtable_type;
  if (orig_t)
    ht = scheme_clone_hash_table(orig_t->mapping);
  else
    ht = scheme_make_hash_table(SCHEME_hash_ptr);
  t->mapping = ht;
  fast = scheme_malloc_atomic(128);
  memcpy(fast, (orig_t ? orig_t->fast_mapping : builtin_fast), 128);
  t->fast_mapping = fast;

  for (i = 1; i < argc; i += 3) {
    if (!SCHEME_CHARP(argv[i])) {
      scheme_wrong_type("make-readtable", "character", i, argc, argv);
      return NULL;
    }

    if (i + 1 >= argc) {
      scheme_arg_mismatch("make-readtable",
			  "expected 'terminating-macro, 'non-terminating-macro, 'dispatch-macro,"
			  " or character argument after character argument: ",
			  argv[i]);
    }

    sym = argv[i + 1];
    if (!SAME_OBJ(sym, terminating_macro_symbol)
	&& !SAME_OBJ(sym, non_terminating_macro_symbol)
	&& !SAME_OBJ(sym, dispatch_macro_symbol)
	&& !SCHEME_CHARP(sym)) {
      scheme_wrong_type("make-readtable", 
			"'terminating-macro, 'non-terminating-macro, 'dispatch-macro, or character", 
			i+1, argc, argv);
      return NULL;
    }

    if (i + 2 >= argc) {
      scheme_arg_mismatch("make-readtable",
			  (SCHEME_CHARP(sym) 
			   ? "expected readtable or #f argument after character argument: "
			   : "expected procedure argument after symbol argument: "),
			  argv[i+1]);
    }

    if (SAME_OBJ(sym, dispatch_macro_symbol)) {
      ch = SCHEME_CHAR_VAL(argv[i]);
      scheme_check_proc_arity("make-readtable", 6, i+2, argc, argv);
      scheme_hash_set(t->mapping, scheme_make_integer(-ch), argv[i+2]);
    } else {
      if (SCHEME_CHARP(sym)) {
	Readtable *src;
	int sch;

	if (SCHEME_FALSEP(argv[i+2])) {
	  src = NULL;
	} else {
	  if (!SAME_TYPE(scheme_readtable_type, SCHEME_TYPE(argv[i+2]))) {
	    scheme_wrong_type("make-readtable", "readtable or #f", i+2, argc, argv);
	    return NULL;
	  }
	  src = (Readtable *)(argv[i+2]);
	}
	sch = SCHEME_CHAR_VAL(argv[i+1]);
	if (!src)
	  val = NULL; /* use default */
	else
	  val = scheme_hash_get(src->mapping, scheme_make_integer(sch));
	if (!val)
	  val = scheme_make_pair(scheme_make_integer(READTABLE_MAPPED), scheme_make_integer(sch));
      } else {
	int kind;
	scheme_check_proc_arity("make-readtable", 6, i+2, argc, argv);
	kind = (SAME_OBJ(sym, non_terminating_macro_symbol)
		? READTABLE_CONTINUING
		: READTABLE_TERMINATING);
	val = scheme_make_pair(scheme_make_integer(kind), argv[i+2]);
      }

      ch = SCHEME_CHAR_VAL(argv[i]);
      if (!val) {
	scheme_hash_set(t->mapping, scheme_make_integer(ch), NULL);
	if (ch < 128)
	  t->fast_mapping[ch] = 0;
      } else {
	scheme_hash_set(t->mapping, scheme_make_integer(ch), val);
	if (ch < 128)
	  t->fast_mapping[ch] = SCHEME_INT_VAL(SCHEME_CAR(val));
      }
    }
  }

  return (Scheme_Object *)t;
}

static Scheme_Object *readtable_mapping(int argc, Scheme_Object **argv)
{
  Scheme_Object *v1, *v2, *a[3];
  Readtable *t;
  int ch;

  if (!SAME_TYPE(scheme_readtable_type, SCHEME_TYPE(argv[0]))) {
    scheme_wrong_type("readtable-mapping", "readtable", 0, argc, argv);
    return NULL;
  }
  if (!SCHEME_CHARP(argv[1])) {
    scheme_wrong_type("readtable-mapping", "character", 1, argc, argv);
    return NULL;
  }
  
  t = (Readtable *)argv[0];
  ch = SCHEME_CHAR_VAL(argv[1]);
  
  v1 = scheme_hash_get(t->mapping, scheme_make_integer(ch));
  v2 = scheme_hash_get(t->mapping, scheme_make_integer(-ch));

  a[0] = argv[1];
  a[1] = scheme_false;
  if (v1) {
    int v;
    v = SCHEME_INT_VAL(SCHEME_CAR(v1));
    if (v & READTABLE_MAPPED) {
      v = SCHEME_INT_VAL(SCHEME_CDR(v1));
      a[0] = scheme_make_character(v);
      a[1] = scheme_false;
    } else if (v & READTABLE_CONTINUING) {
      a[0] = non_terminating_macro_symbol;
      a[1] = SCHEME_CDR(v1);
    } else if (v & READTABLE_TERMINATING) {
      a[0] = terminating_macro_symbol;
      a[1] = SCHEME_CDR(v1);
    }
  }
  a[2] = scheme_false;
  if (v2) {
    a[2] = v2;
  }

  return scheme_values(3, a);
}

static Scheme_Object *readtable_p(int argc, Scheme_Object **argv)
{
  return (SAME_TYPE(scheme_readtable_type, SCHEME_TYPE(argv[0]))
	  ? scheme_true
	  : scheme_false);
}

static Scheme_Object *readtable_or_false_p(int argc, Scheme_Object **argv)
{
  if (SCHEME_FALSEP(argv[0]))
    return scheme_true;
  return readtable_p(argc, argv);
}

static Scheme_Object *current_readtable(int argc, Scheme_Object **argv)
{
  return scheme_param_config("current-readtable", 
			     scheme_make_integer(MZCONFIG_READTABLE),
			     argc, argv,
			     -1, readtable_or_false_p, "readtable", 0);
}

static Scheme_Object *current_reader_guard(int argc, Scheme_Object **argv)
{
  return scheme_param_config("current-reader-guard", 
			     scheme_make_integer(MZCONFIG_READER_GUARD),
			     argc, argv,
			     1, NULL, NULL, 0);
}

/* "#reader" has been read */
static Scheme_Object *read_reader(Scheme_Object *port,
				  Scheme_Object *stxsrc, long line, long col, long pos,
				  Scheme_Hash_Table **ht,
				  Scheme_Object *indentation, ReadParams *params)
{
  Scheme_Object *modpath, *name, *a[2], *proc, *v;

  modpath = scheme_read(port);

  if (SCHEME_EOFP(modpath)) {
    scheme_read_err(port, stxsrc, line, col, pos, 1, EOF, indentation, 
		    "read: expected a datum after #reader, found end-of-file");
    return NULL;
  }

  proc = scheme_get_param(scheme_current_config(), MZCONFIG_READER_GUARD);

  a[0] = modpath;
  modpath = scheme_apply(proc, 1, a);
  
  a[0] = modpath;
  if (stxsrc)
    name = scheme_intern_symbol("read-syntax");
  else
    name = scheme_intern_symbol("read");
  a[1] = name;

  proc = scheme_dynamic_require(2, a);

  a[0] = proc;
  if (!scheme_check_proc_arity(NULL, stxsrc ? 2 : 1, 0, 1, a)) {
    scheme_wrong_type("#reader",
		      (stxsrc ? "procedure (arity 2)" : "procedure (arity 1)"),
		      -1, -1, a);
    return NULL;
  }

  v = readtable_call(0, 0, proc, params,
		     port, stxsrc, line, col, pos,
		     ht);

  if (scheme_special_comment_value(v))
    return NULL;
  else
    return v;
}

static int is_placeholder(Scheme_Object *a, Scheme_Object *src)
{
  if (src && SCHEME_STXP(a))
    a = SCHEME_STX_VAL(a);
  return SAME_TYPE(scheme_placeholder_type, SCHEME_TYPE(a));
}

#ifdef DO_STACK_CHECK
static Scheme_Object *copy_to_protect(Scheme_Object *v, Scheme_Object *src, Scheme_Hash_Table *ht, Scheme_Hash_Table **oht);

static Scheme_Object *copy_to_protect_k(void)
{
  Scheme_Thread *p = scheme_current_thread;
  Scheme_Object *v = (Scheme_Object *)p->ku.k.p1;
  Scheme_Object *src = (Scheme_Object *)p->ku.k.p2;
  Scheme_Hash_Table *ht = (Scheme_Hash_Table *)p->ku.k.p3;
  Scheme_Hash_Table **oht = (Scheme_Hash_Table **)p->ku.k.p4;

  p->ku.k.p1 = NULL;
  p->ku.k.p2 = NULL;
  p->ku.k.p3 = NULL;
  p->ku.k.p4 = NULL;

  return copy_to_protect(v, src, ht, oht);
}
#endif

static Scheme_Object *copy_to_protect(Scheme_Object *v, Scheme_Object *src, Scheme_Hash_Table *ht, Scheme_Hash_Table **oht)
{
  Scheme_Object *o, *ph;
  int immutable;

#ifdef DO_STACK_CHECK
  {
# include "mzstkchk.h"
    {
      Scheme_Thread *p = scheme_current_thread;
      p->ku.k.p1 = (void *)v;
      p->ku.k.p2 = (void *)src;
      p->ku.k.p3 = (void *)ht;
      p->ku.k.p4 = (void *)oht;
      return scheme_handle_stack_overflow(copy_to_protect_k);
    }
  }
#endif
  SCHEME_USE_FUEL(1);

  if (src && SCHEME_STXP(v))
    o = SCHEME_STX_VAL(v);
  else
    o = v;

  if ((SCHEME_PAIRP(o)
       || SCHEME_BOXP(o)
       || SCHEME_VECTORP(o))
      && (!src || SCHEME_STXP(v))) {
    ph = scheme_hash_get(ht, o);
    if (ph) {
      if (src) {
	if (SCHEME_STXP(SCHEME_PTR_VAL(ph)))
	  scheme_make_graph_stx(SCHEME_PTR_VAL(ph), -1, -1, -1);
	else
	  SCHEME_PTR_VAL(ph) = scheme_true;
      }
      if (!*oht) {
	/* Let outer read know that it needs to resolve placeholders */
	ht = scheme_make_hash_table(SCHEME_hash_ptr);
	*oht = ht;
      }
      return ph;
    }

    ph = scheme_alloc_small_object();
    ph->type = scheme_placeholder_type;
    SCHEME_PTR_VAL(ph) = scheme_false;
    scheme_hash_set(ht, o, ph);
  } else
    ph = NULL;

  if (SCHEME_PAIRP(o)) {
    Scheme_Object *a, *d;
    a = copy_to_protect(SCHEME_CAR(o), src, ht, oht);
    d = copy_to_protect(SCHEME_CDR(o), src, ht, oht);
    immutable = SCHEME_IMMUTABLEP(o);
    if (immutable 
	&& SAME_OBJ(a, SCHEME_CAR(o))
	&& SAME_OBJ(d, SCHEME_CDR(o))
	&& !is_placeholder(a, src)
	&& !is_placeholder(d, src)) {
      /* No copy needed */
      if (ph)
	scheme_hash_set(ht, o, NULL);
      return v;
    }
    o = scheme_make_pair(a, d);
    if (src || immutable)
      SCHEME_SET_PAIR_IMMUTABLE(o);
  } else if (SCHEME_BOXP(o)) {
    Scheme_Object *x;
    x = copy_to_protect(SCHEME_BOX_VAL(o), src, ht, oht);
    immutable = SCHEME_IMMUTABLEP(o);
    if (immutable
	&& SAME_OBJ(x, SCHEME_BOX_VAL(o))
	&& !is_placeholder(x, src)) {
      /* No copy needed */
      if (ph)
	scheme_hash_set(ht, o, NULL);
      return v;
    }
    o = scheme_box(x);
    if (src || immutable)
      SCHEME_SET_BOX_IMMUTABLE(o);
  } else if (SCHEME_VECTORP(o)) {
    int immutable = SCHEME_IMMUTABLEP(o);
    int i, len, use_copy = !immutable;
    Scheme_Object *a, *o2;

    len = SCHEME_VEC_SIZE(o);
    o2 = scheme_make_vector(len, NULL);

    for (i = 0; i < len; i++) {
      a = copy_to_protect(SCHEME_VEC_ELS(o)[i], src, ht, oht);
      if (!SAME_OBJ(a, SCHEME_VEC_ELS(o)[i])
	  || is_placeholder(a, src))
	use_copy = 1;
      SCHEME_VEC_ELS(o2)[i] = a;
    }
    if (!use_copy) {
      /* No copy needed */
      if (ph)
	scheme_hash_set(ht, o, NULL);
      return v;
    }
    o = o2;
    if (src || immutable)
      SCHEME_SET_VECTOR_IMMUTABLE(o);
  } else {
    /* Assert: !ph */
    return v;
  }

  if (src && SCHEME_STXP(v)) {
    o = scheme_datum_to_syntax(o, v, v, 0, 2);
    if (ph && SCHEME_TRUEP(SCHEME_PTR_VAL(ph)))
      scheme_make_graph_stx(o, -1, -1, -1);
  }

  if (ph)
    SCHEME_PTR_VAL(ph) = o;

  return o;
}

static Scheme_Object *copy_to_protect_placeholders(Scheme_Object *v, Scheme_Object *src, Scheme_Hash_Table **oht)
{
  Scheme_Hash_Table *ht;
  ht = scheme_make_hash_table(SCHEME_hash_ptr);
  return copy_to_protect(v, src, ht, oht);
}

/*========================================================================*/
/*                         precise GC traversers                          */
/*========================================================================*/

#ifdef MZ_PRECISE_GC

START_XFORM_SKIP;

#define MARKS_FOR_READ_C
#include "mzmark.c"

static void register_traversers(void)
{
  GC_REG_TRAV(scheme_indent_type, mark_indent);
  GC_REG_TRAV(scheme_rt_compact_port, mark_cport);
  GC_REG_TRAV(scheme_readtable_type, mark_readtable);
  GC_REG_TRAV(scheme_rt_read_params, mark_read_params);
}

END_XFORM_SKIP;

#endif
