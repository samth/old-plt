/*
  MzScheme
  Copyright (c) 1995-97 Matthew Flatt

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

#include "scheme.h"

/* #define STANDALONE_WITH_EMBEDDED_EXTENSION */
/*    STANDALONE_WITH_EMBEDDED_EXTENSION builds an executable with
      built-in extensions. The extension is initialized by calling
      scheme_initialize(env), where `env' is the initial environment.
      By default, command-line parsing, the REP, and initilization
      file loading are turned off. */

#ifdef STANDALONE_WITH_EMBEDDED_EXTENSION
# define DONT_PARSE_COMMAND_LINE
# define DONT_RUN_REP
# define DONT_LOAD_INIT_FILE
#endif

#ifdef FILES_HAVE_FDS
# include <sys/types.h>
# include <sys/time.h>
# ifdef SELECT_INCLUDE
#  include <sys/select.h>
# endif
#endif
#ifndef NO_USER_BREAK_HANDLER
# include <signal.h>
#endif
#ifdef UNISTD_INCLUDE
# include <unistd.h>
#endif
#ifdef USE_LOCALE
# include <locale.h>
#endif
#ifdef MACINTOSH_EVENTS
# include <Events.h>
#endif
#ifdef MACINTOSH_SIOUX
# include <console.h>
# pragma far_data on
# include <SIOUX.h>
# pragma far_data off
#endif
#ifdef MACINTOSH_SET_STACK
# include <Memory.h>
#endif
#ifdef MACINTOSH_EVENTS
# include "simpledrop.h"
#endif

/* #define NO_GCING */

#if defined(_IBMR2)
static void dangerdanger(int ignored)
{
  char *s = "mzscheme: danger - paging space low\n";
  write(2, s, strlen(s));
  scheme_collect_garbage();
}
#endif

#ifndef NO_USER_BREAK_HANDLER

#ifdef MACINTOSH_EVENTS
static int break_flag;
static Scheme_Object *orig_evaluator;
#endif

#ifndef MACINTOSH_EVENTS
extern Scheme_Process *scheme_main_process;
 
static void user_break_hit(int ignore)
{
  Scheme_Process *p = scheme_main_process;

  scheme_break_thread(p);

# ifdef SIGSET_NEEDS_REINSTALL
  MZ_SIGSET(SIGINT, user_break_hit);
# endif
}
#endif

#ifdef MACINTOSH_EVENTS

static int WeAreFront()
{
  static int inited;
  static ProcessSerialNumber us;
  ProcessSerialNumber front;
  Boolean r;
  
  if (!inited) {
    GetCurrentProcess(&us);
    inited = 1;
  }
  GetFrontProcess(&front);
  SameProcess(&us, &front, &r);
  
  return r;
}

static int check_break_flag()
{
#ifdef MACINTOSH_GIVE_TIME
  static long last_time;
  static int front = 1;

  if (TickCount() > last_time + (front ? 30 : 0)) {
      EventRecord e;
      front = WeAreFront();
      while (WaitNextEvent(everyEvent, &e, front ? 0 : 30, NULL)) {
        if ((e.what == keyDown)
            && ((((e.message & charCodeMask) == '.') 
                 && (e.modifiers & cmdKey))
                || (((e.message & charCodeMask) == 3)
	                && (e.modifiers & controlKey)))) {
	       return 1;
	     }
# ifdef MACINTOSH_SIOUX
        SIOUXHandleOneEvent(&e);
# endif
       }
      last_time = TickCount();
    }
#endif
    return 0;
}

static void handle_one(EventRecord *e)
{
# ifdef MACINTOSH_SIOUX
  SIOUXHandleOneEvent(e);
# endif
}

static Scheme_Object *adjust_break_flag_and_eval(int argc, Scheme_Object **argv)
{
  break_flag = 0;
  return _scheme_tail_apply(orig_evaluator, argc, argv);
}
#endif

#endif

#ifdef MZ_STACK_START_HACK
void *mzscheme_stack_start;
#endif

#ifdef MACINTOSH_EVENTS
void Drop_Runtime(char **argv, int argc)
{
  int i;
	
  for (i = 0; i < argc; i++)
    scheme_load(argv[i]);
}

void Drop_Quit()
{
  exit(0);
}
#endif

#ifndef DONT_PARSE_COMMAND_LINE

#ifdef MACINTOSH_SIOUX
# pragma far_data on
static void SetSIOUX(void)
{
  SIOUXSettings.initializeTB = 0;
}
# pragma far_data off
#endif

#endif /* DONT_PARSE_COMMAND_LINE */

#ifdef EXPAND_FILENAME_TILDE
# define INIT_FILENAME "~/.mzschemerc"
#else
# define INIT_FILENAME "mzscheme.rc"
#endif
#define PRINTF printf
#define PROGRAM "MzScheme"
#define PROGRAM_LC "mzscheme"
#define BANNER scheme_banner()
#define MZSCHEME_CMD_LINE

#include "cmdline.inc"

#ifdef STANDALONE_WITH_EMBEDDED_EXTENSION
extern Scheme_Object *scheme_initialize(Scheme_Env *env);
#endif

#ifdef NO_GCING
extern int GC_free_space_divisor;
#endif

static void do_scheme_rep(void)
{
# ifndef NO_USER_BREAK_HANDLER
#  ifdef MACINTOSH_EVENTS
  scheme_set_param(scheme_config, MZCONFIG_ENABLE_BREAK, scheme_true);
  
  orig_evaluator = scheme_get_param(scheme_config, MZCONFIG_EVAL_HANDLER);
  scheme_set_param(scheme_config, MZCONFIG_EVAL_HANDLER, scheme_make_prim(adjust_break_flag_and_eval));
#  endif
# endif
  
  /* enter read-eval-print loop */
  scheme_rep();
}

int cont_run(FinishArgs *f)
{
  return finish_cmd_line_run(f, do_scheme_rep);
}

int actual_main(int argc, char *argv[])
{
#ifdef NO_GCING
  GC_free_space_divisor = 1;
#endif

#ifdef MACINTOSH_SET_STACK
  long calcLimit;
  THz zone;
	
  zone = GetZone();
  calcLimit = ((long)LMGetCurStackBase()-(*(long *)zone)-sizeof(Zone))*3/4;
  if (calcLimit % 2)
    calcLimit++;
  SetApplLimit((Ptr)((*(long *)zone)+sizeof(Zone)+calcLimit));
#endif

#ifdef MACINTOSH_EVENTS
  MaxApplZone();
	
  InitGraf(&qd.thePort);		/* initialize Mac stuff */
  InitFonts();
  InitWindows();
  InitMenus();
  TEInit();
  InitDialogs(NULL);
  
  MoreMasters();
  MoreMasters();
	
#ifdef MACINTOSH_SIOUX
# pragma far_data on
  SIOUXSettings.initializeTB = 0;
# pragma far_data off
#endif

  scheme_handle_aewait_event = handle_one;

  Drop_GetArgs(&argc, &argv);
#endif

#ifdef MACINTOSH_SIOUX
  {
    int argc2;
    char **argv2;
    argc2 = ccommand(&argv2);
    if (argc2 > 1) {
      int i, j;
      char **both = (char **)malloc(sizeof(char *) * (argc + argc2 - 1));
      for (i = 0; i < argc; i++)
        both[i] = argv[i];
      for (j = 1; j < argc2; j++, i++)
        both[i] = argv2[j];
        
      argv = both;
      argc += argc2 - 1;
    }
  }
  
# pragma far_data on
  SIOUXSettings.autocloseonquit = 0;
  SIOUXSettings.asktosaveonclose = 0;
# pragma far_data off
#endif

#ifdef USE_LOCALE
  /* See note in string.c */
  setlocale( LC_ALL, "" );
#endif

#ifndef MACINTOSH_EVENTS
  MZ_SIGSET(SIGINT, user_break_hit);
#else
  scheme_check_for_break = check_break_flag;
#endif

  return run_from_cmd_line(argc, argv, scheme_basic_env, cont_run);
}

int main(int argc, char **argv)
{
#ifdef USE_SENORA_GC
  void *mzscheme_stack_start;
#endif
#if defined(MZ_STACK_START_HACK) || defined(USE_SENORA_GC)
  long start2;

  mzscheme_stack_start = (void *)&start2;
#endif

#ifdef USE_SENORA_GC
  GC_set_stack_base(mzscheme_stack_start);
#endif

  scheme_actual_main = actual_main;

  return scheme_image_main(argc, argv);
}

#if 0
/* For testing STANDALONE_WITH_EMBEDDED_EXTENSION */
Scheme_Object *scheme_initialize(Scheme_Env *env)
{
  return scheme_eval_string("(lambda (v) (and (eq? v #t) (lambda () (printf \"These were the args: ~a~n\" argv))))", env);
}
#endif
