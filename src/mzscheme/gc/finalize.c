/*
 * Copyright 1988, 1989 Hans-J. Boehm, Alan J. Demers
 * Copyright (c) 1991-1996 by Xerox Corporation.  All rights reserved.

 * THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
 * OR IMPLIED.  ANY USE IS AT YOUR OWN RISK.
 *
 * Permission is hereby granted to use or copy this program
 * for any purpose,  provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 */
/* Boehm, February 1, 1996 1:19 pm PST */
# define I_HIDE_POINTERS
# include "gc_priv.h"
# include "gc_mark.h"

/* Type of mark procedure used for marking from finalizable object.	*/
/* This procedure normally does not mark the object, only its		*/
/* descendents.								*/
typedef void finalization_mark_proc(/* ptr_t finalizable_obj_ptr */); 

# define HASH3(addr,size,log_size) \
    ((((word)(addr) >> 3) ^ ((word)(addr) >> (3+(log_size)))) \
    & ((size) - 1))
#define HASH2(addr,log_size) HASH3(addr, 1 << log_size, log_size)

struct hash_chain_entry {
    word hidden_key;
    struct hash_chain_entry * next;
};

unsigned GC_finalization_failures = 0;
	/* Number of finalization requests that failed for lack of memory. */

/* MATTHEW: */
void (*GC_custom_finalize)(void);
void (*GC_push_last_roots_again)(void);

static struct disappearing_link {
    struct hash_chain_entry prolog;
#   define dl_hidden_link prolog.hidden_key
				/* Field to be cleared.		*/
#   define dl_next(x) (struct disappearing_link *)((x) -> prolog.next)
#   define dl_set_next(x,y) (x) -> prolog.next = (struct hash_chain_entry *)(y)

    word dl_hidden_obj;		/* Pointer to object base	*/

    /* MATTHEW: for restoring: */
    union {
      short kind;
#           define NORMAL_DL  0
#           define RESTORE_DL 1
#           define LATE_DL    2
      word value; /* old value when zeroed */
    } dl_special;
    struct disappearing_link *restore_next;
} **dl_head = 0;

static signed_word log_dl_table_size = -1;
			/* Binary log of				*/
			/* current size of array pointed to by dl_head.	*/
			/* -1 ==> size is 0.				*/

word GC_dl_entries = 0;	/* Number of entries currently in disappearing	*/
			/* link table.					*/

static struct finalizable_object {
    struct hash_chain_entry prolog;
#   define fo_hidden_base prolog.hidden_key
				/* Pointer to object base.	*/
				/* No longer hidden once object */
				/* is on finalize_now queue.	*/
#   define fo_next(x) (struct finalizable_object *)((x) -> prolog.next)
#   define fo_set_next(x,y) (x) -> prolog.next = (struct hash_chain_entry *)(y)
    GC_finalization_proc fo_fn;	/* Finalizer.			*/
    ptr_t fo_client_data;
    word fo_object_size;	/* In bytes.			*/
    finalization_mark_proc * fo_mark_proc;	/* Mark-through procedure */
    int eager_level; /* MATTHEW: eager finalizers don't care about cycles */
} **fo_head = 0;

struct finalizable_object * GC_finalize_now = 0;
	/* LIst of objects that should be finalized now.	*/

static signed_word log_fo_table_size = -1;

word GC_fo_entries = 0;

# ifdef SRC_M3
void GC_push_finalizer_structures()
{
    GC_push_all((ptr_t)(&dl_head), (ptr_t)(&dl_head) + sizeof(word));
    GC_push_all((ptr_t)(&fo_head), (ptr_t)(&fo_head) + sizeof(word));
}
# endif

/* Double the size of a hash table. *size_ptr is the log of its current	*/
/* size.  May be a noop.						*/
/* *table is a pointer to an array of hash headers.  If we succeed, we	*/
/* update both *table and *log_size_ptr.				*/
/* Lock is held.  Signals are disabled.					*/
void GC_grow_table(table, log_size_ptr)
struct hash_chain_entry ***table;
signed_word * log_size_ptr;
{
    register word i;
    register struct hash_chain_entry *p;
    int log_old_size = *log_size_ptr;
    register int log_new_size = log_old_size + 1;
    word old_size = ((log_old_size == -1)? 0: (1 << log_old_size));
    register word new_size = 1 << log_new_size;
    struct hash_chain_entry **new_table = (struct hash_chain_entry **)
    	GC_generic_malloc_inner_ignore_off_page(
    		(size_t)new_size * sizeof(struct hash_chain_entry *), NORMAL);
    
    if (new_table == 0) {
    	if (table == 0) {
    	    ABORT("Insufficient space for initial table allocation");
    	} else {
    	    return;
    	}
    }
    for (i = 0; i < old_size; i++) {
      p = (*table)[i];
      while (p != 0) {
        register ptr_t real_key = (ptr_t)REVEAL_POINTER(p -> hidden_key);
        register struct hash_chain_entry *next = p -> next;
        register int new_hash = HASH3(real_key, new_size, log_new_size);
        
        p -> next = new_table[new_hash];
        new_table[new_hash] = p;
        p = next;
      }
    }
    *log_size_ptr = log_new_size;
    *table = new_table;
}

# if defined(__STDC__) || defined(__cplusplus)
    int GC_register_disappearing_link(GC_PTR * link)
# else
    int GC_register_disappearing_link(link)
    GC_PTR * link;
# endif
{
    ptr_t base;
    
    base = (ptr_t)GC_base((GC_PTR)link);
    if (base == 0)
    	ABORT("Bad arg to GC_register_disappearing_link");
    return(GC_general_register_disappearing_link(link, base));
}

/* MATTHEW: GC_register_late_disappearing_link */
static int late_dl; /* a stupid way to pass arguments (to minimize my changes). */
void GC_register_late_disappearing_link(void **link, void *obj)
{
  late_dl= 1;
  GC_general_register_disappearing_link((GC_PTR *)link, (GC_PTR)obj);
  late_dl = 0;
}


# if defined(__STDC__) || defined(__cplusplus)
    int GC_general_register_disappearing_link(GC_PTR * link,
    					      GC_PTR obj)
# else
    int GC_general_register_disappearing_link(link, obj)
    GC_PTR * link;
    GC_PTR obj;
# endif

{
    struct disappearing_link *curr_dl;
    int index;
    struct disappearing_link * new_dl;
    DCL_LOCK_STATE;
    
#if 1
    /* MATTHEW: If wxObjects are sometimes stack-allocated, 
       MrEd needs this. Keeping it for now just-in-case, though
       it should be eliminated in the future. */
    if (!GC_base(link))
      return 1;
#endif

    if ((word)link & (ALIGNMENT-1))
    	ABORT("Bad arg to GC_general_register_disappearing_link");
#   ifdef THREADS
    	DISABLE_SIGNALS();
    	LOCK();
#   endif
    if (log_dl_table_size == -1
        || GC_dl_entries > ((word)1 << log_dl_table_size)) {
#	ifndef THREADS
	    DISABLE_SIGNALS();
#	endif
    	GC_grow_table((struct hash_chain_entry ***)(&dl_head),
    		      &log_dl_table_size);
#	ifdef PRINTSTATS
	    GC_printf1("Grew dl table to %lu entries\n",
	    		(unsigned long)(1 << log_dl_table_size));
#	endif
#	ifndef THREADS
	    ENABLE_SIGNALS();
#	endif
    }
    index = HASH2(link, log_dl_table_size);
    curr_dl = dl_head[index];
    for (curr_dl = dl_head[index]; curr_dl != 0; curr_dl = dl_next(curr_dl)) {
        if (curr_dl -> dl_hidden_link == HIDE_POINTER(link)) {
            curr_dl -> dl_hidden_obj = HIDE_POINTER(obj);
#	    ifdef THREADS
                UNLOCK();
    	        ENABLE_SIGNALS();
#	    endif
            return(1);
        }
    }
#   ifdef THREADS
      new_dl = (struct disappearing_link *)
    	GC_generic_malloc_inner(sizeof(struct disappearing_link),NORMAL);
#   else
      new_dl = (struct disappearing_link *)
	GC_malloc(sizeof(struct disappearing_link));
#   endif
    if (new_dl != 0) {
        new_dl -> dl_hidden_obj = HIDE_POINTER(obj);
        new_dl -> dl_hidden_link = HIDE_POINTER(link);
	new_dl -> dl_special.kind = late_dl ? LATE_DL : (obj ? NORMAL_DL : RESTORE_DL); /* MATTHEW: Set flag */
        dl_set_next(new_dl, dl_head[index]);
        dl_head[index] = new_dl;
        GC_dl_entries++;
    } else {
        GC_finalization_failures++;
    }
#   ifdef THREADS
        UNLOCK();
        ENABLE_SIGNALS();
#   endif
    return(0);
}

# if defined(__STDC__) || defined(__cplusplus)
    int GC_unregister_disappearing_link(GC_PTR * link)
# else
    int GC_unregister_disappearing_link(link)
    GC_PTR * link;
# endif
{
    struct disappearing_link *curr_dl, *prev_dl;
    int index;
    DCL_LOCK_STATE;
    
    DISABLE_SIGNALS();
    LOCK();
    index = HASH2(link, log_dl_table_size);
    if (((unsigned long)link & (ALIGNMENT-1))) goto out;
    prev_dl = 0; curr_dl = dl_head[index];
    while (curr_dl != 0) {
        if (curr_dl -> dl_hidden_link == HIDE_POINTER(link)) {
            if (prev_dl == 0) {
                dl_head[index] = dl_next(curr_dl);
            } else {
                dl_set_next(prev_dl, dl_next(curr_dl));
            }
            GC_dl_entries--;
            UNLOCK();
    	    ENABLE_SIGNALS();
            GC_free((GC_PTR)curr_dl);
            return(1);
        }
        prev_dl = curr_dl;
        curr_dl = dl_next(curr_dl);
    }
out:
    UNLOCK();
    ENABLE_SIGNALS();
    return(0);
}

/* Possible finalization_marker procedures.  Note that mark stack	*/
/* overflow is handled by the caller, and is not a disaster.		*/
void GC_normal_finalize_mark_proc(p)
ptr_t p;
{
    hdr * hhdr = HDR(p);
    
    PUSH_OBJ((word *)p, hhdr, GC_mark_stack_top,
	     &(GC_mark_stack[GC_mark_stack_size]));
}

/* This only pays very partial attention to the mark descriptor.	*/
/* It does the right thing for normal and atomic objects, and treats	*/
/* most others as normal.						*/
void GC_ignore_self_finalize_mark_proc(p)
ptr_t p;
{
    hdr * hhdr = HDR(p);
    word descr = hhdr -> hb_descr;
    ptr_t q, r;
    ptr_t scan_limit;
    ptr_t target_limit = p + WORDS_TO_BYTES(hhdr -> hb_sz) - 1;
    
    if ((descr & DS_TAGS) == DS_LENGTH) {
       scan_limit = p + descr - sizeof(word);
    } else {
       scan_limit = target_limit + 1 - sizeof(word);
    }
    for (q = p; q <= scan_limit; q += ALIGNMENT) {
    	r = *(ptr_t *)q;
    	if (r < p || r > target_limit) {
    	    GC_PUSH_ONE_HEAP((word)r);
    	}
    }
}

/*ARGSUSED*/
void GC_null_finalize_mark_proc(p)
ptr_t p;
{
}



/* Register a finalization function.  See gc.h for details.	*/
/* in the nonthreads case, we try to avoid disabling signals,	*/
/* since it can be expensive.  Threads packages typically	*/
/* make it cheaper.						*/
void GC_register_finalizer_inner(obj, fn, cd, ofn, ocd, mp, eager_level) /* MATTHEW: eager_level */
GC_PTR obj;
GC_finalization_proc fn;
GC_PTR cd;
GC_finalization_proc * ofn;
GC_PTR * ocd;
finalization_mark_proc * mp;
int eager_level; /* MATTHEW */
{
    ptr_t base;
    struct finalizable_object * curr_fo, * prev_fo;
    int index;
    struct finalizable_object *new_fo;
    DCL_LOCK_STATE;

#   ifdef THREADS
	DISABLE_SIGNALS();
	LOCK();
#   endif
    if (log_fo_table_size == -1
        || GC_fo_entries > ((word)1 << log_fo_table_size)) {
#	ifndef THREADS
    	    DISABLE_SIGNALS();
#	endif
    	GC_grow_table((struct hash_chain_entry ***)(&fo_head),
    		      &log_fo_table_size);
#	ifdef PRINTSTATS
	    GC_printf1("Grew fo table to %lu entries\n",
	    		(unsigned long)(1 << log_fo_table_size));
#	endif
#	ifndef THREADS
	    ENABLE_SIGNALS();
#	endif
    }
    /* in the THREADS case signals are disabled and we hold allocation	*/
    /* lock; otherwise neither is true.  Proceed carefully.		*/
    base = (ptr_t)obj;
    index = HASH2(base, log_fo_table_size);
    prev_fo = 0; curr_fo = fo_head[index];
    while (curr_fo != 0) {
        if (curr_fo -> fo_hidden_base == HIDE_POINTER(base)) {
            /* Interruption by a signal in the middle of this	*/
            /* should be safe.  The client may see only *ocd	*/
            /* updated, but we'll declare that to be his	*/
            /* problem.						*/
            if (ocd) *ocd = (GC_PTR) curr_fo -> fo_client_data;
            if (ofn) *ofn = curr_fo -> fo_fn;
            /* Delete the structure for base. */
                if (prev_fo == 0) {
                  fo_head[index] = fo_next(curr_fo);
                } else {
                  fo_set_next(prev_fo, fo_next(curr_fo));
                }
            if (fn == 0) {
                GC_fo_entries--;
                  /* May not happen if we get a signal.  But a high	*/
                  /* estimate will only make the table larger than	*/
                  /* necessary.						*/
#		ifndef THREADS
                  GC_free((GC_PTR)curr_fo);
#		endif
            } else {
                curr_fo -> fo_fn = fn;
                curr_fo -> fo_client_data = (ptr_t)cd;
                curr_fo -> fo_mark_proc = mp;
		curr_fo -> eager_level = eager_level; /* MATTHEW */
		/* Reinsert it.  We deleted it first to maintain	*/
		/* consistency in the event of a signal.		*/
		if (prev_fo == 0) {
                  fo_head[index] = curr_fo;
                } else {
                  fo_set_next(prev_fo, curr_fo);
                }
            }
#	    ifdef THREADS
                UNLOCK();
    	    	ENABLE_SIGNALS();
#	    endif
            return;
        }
        prev_fo = curr_fo;
        curr_fo = fo_next(curr_fo);
    }
    if (ofn) *ofn = 0;
    if (ocd) *ocd = 0;
    if (fn == 0) {

      /* MATTHEW: */
      /* If this item is already queued, de-queue it. */
#if 1
      if (GC_finalize_now) {
	ptr_t real_ptr;
	register struct finalizable_object * curr_fo, *prev_fo;
	
	prev_fo = NULL;
	curr_fo = GC_finalize_now;
	while (curr_fo != 0) {
	  real_ptr = (ptr_t)(curr_fo -> fo_hidden_base);
	  if (real_ptr == obj) {
	    if (prev_fo)
	      fo_set_next(prev_fo, fo_next(curr_fo));
	    else
	      GC_finalize_now = fo_next(curr_fo);
	    GC_free((GC_PTR)curr_fo);
	    break;
	  }
	  prev_fo = curr_fo;
	  curr_fo = fo_next(curr_fo);
	}
      }
#endif

#	ifdef THREADS
            UNLOCK();
    	    ENABLE_SIGNALS();
#	endif
        return;
    }
#   ifdef THREADS
      new_fo = (struct finalizable_object *)
    	GC_generic_malloc_inner(sizeof(struct finalizable_object),NORMAL);
#   else
      new_fo = (struct finalizable_object *)
	GC_malloc(sizeof(struct finalizable_object));
#   endif
    if (new_fo != 0) {
        new_fo -> fo_hidden_base = (word)HIDE_POINTER(base);
	new_fo -> fo_fn = fn;
	new_fo -> fo_client_data = (ptr_t)cd;
	new_fo -> fo_object_size = GC_size(base);
	new_fo -> fo_mark_proc = mp;
	new_fo -> eager_level = eager_level; /* MATTHEW */
	fo_set_next(new_fo, fo_head[index]);
	GC_fo_entries++;
	fo_head[index] = new_fo;
    } else {
     	GC_finalization_failures++;
    }
#   ifdef THREADS
        UNLOCK();
    	ENABLE_SIGNALS();
#   endif
}

# if defined(__STDC__)
    void GC_register_finalizer(void * obj,
			       GC_finalization_proc fn, void * cd,
			       GC_finalization_proc *ofn, void ** ocd)
# else
    void GC_register_finalizer(obj, fn, cd, ofn, ocd)
    GC_PTR obj;
    GC_finalization_proc fn;
    GC_PTR cd;
    GC_finalization_proc * ofn;
    GC_PTR * ocd;
# endif
{
    GC_register_finalizer_inner(obj, fn, cd, ofn,
    				ocd, GC_normal_finalize_mark_proc, 
				0); /* MATTHEW */
}

/* MATTHEW: eager finalizers */
# if defined(__STDC__)
    void GC_register_eager_finalizer(void * obj, int eager_level,
			            GC_finalization_proc fn, void * cd,
			            GC_finalization_proc *ofn, void ** ocd)
# else
    void GC_register_eager_finalizer(obj, fn, cd, ofn, ocd)
    GC_PTR obj;
    GC_finalization_proc fn;
    GC_PTR cd;
    GC_finalization_proc * ofn;
    GC_PTR * ocd;
# endif
{
    GC_register_finalizer_inner(obj, fn, cd, ofn,
    				ocd, GC_normal_finalize_mark_proc, 
				eager_level);
}

# if defined(__STDC__)
    void GC_register_finalizer_ignore_self(void * obj,
			       GC_finalization_proc fn, void * cd,
			       GC_finalization_proc *ofn, void ** ocd)
# else
    void GC_register_finalizer_ignore_self(obj, fn, cd, ofn, ocd)
    GC_PTR obj;
    GC_finalization_proc fn;
    GC_PTR cd;
    GC_finalization_proc * ofn;
    GC_PTR * ocd;
# endif
{
    GC_register_finalizer_inner(obj, fn, cd, ofn,
    				ocd, GC_ignore_self_finalize_mark_proc, 
				0); /* MATTHEW */
}

# if defined(__STDC__)
    void GC_register_finalizer_no_order(void * obj,
			       GC_finalization_proc fn, void * cd,
			       GC_finalization_proc *ofn, void ** ocd)
# else
    void GC_register_finalizer_no_order(obj, fn, cd, ofn, ocd)
    GC_PTR obj;
    GC_finalization_proc fn;
    GC_PTR cd;
    GC_finalization_proc * ofn;
    GC_PTR * ocd;
# endif
{
    GC_register_finalizer_inner(obj, fn, cd, ofn,
    				ocd, GC_null_finalize_mark_proc, 
				0); /* MATTHEW */
}

/* MATTHEW: eager finalization: */
static void finalize_eagers(int eager_level)
{
  struct finalizable_object * curr_fo, * prev_fo, * next_fo;
  struct finalizable_object * end_eager_mark;
  ptr_t real_ptr;
  register int i;
  int fo_size = (log_fo_table_size == -1 ) ? 0 : (1 << log_fo_table_size);

  end_eager_mark = GC_finalize_now; /* MATTHEW */
  for (i = 0; i < fo_size; i++) {
    curr_fo = fo_head[i];
    prev_fo = 0;
    while (curr_fo != 0) {
      if (curr_fo -> eager_level == eager_level) {
	real_ptr = (ptr_t)REVEAL_POINTER(curr_fo -> fo_hidden_base);
	if (!GC_is_marked(real_ptr)) {
	  /* We assume that (non-eager) finalization orders do not
	     need to take into account connections through memory
	     with eager finalizations. Otherwise, this mark bit
	     could break the chain from one (non-eager) finalizeable
	     to another. */
	  GC_set_mark_bit(real_ptr);
	  
	  /* Delete from hash table */
	  next_fo = fo_next(curr_fo);
	  if (prev_fo == 0) {
	    fo_head[i] = next_fo;
	  } else {
	    fo_set_next(prev_fo, next_fo);
	  }
	  GC_fo_entries--;
	  /* Add to list of objects awaiting finalization.	*/
	  fo_set_next(curr_fo, GC_finalize_now);
	  GC_finalize_now = curr_fo;
	  /* unhide object pointer so any future collections will	*/
	  /* see it.						*/
	  curr_fo -> fo_hidden_base = 
	    (word) REVEAL_POINTER(curr_fo -> fo_hidden_base);
	  GC_words_finalized +=
	    ALIGNED_WORDS(curr_fo -> fo_object_size)
	      + ALIGNED_WORDS(sizeof(struct finalizable_object));
#	    ifdef PRINTSTATS
	  if (!GC_is_marked((ptr_t)curr_fo)) {
	    ABORT("GC_finalize: found accessible unmarked object\n");
	  }
#	    endif
	  curr_fo = next_fo;
	} else {
	  prev_fo = curr_fo;
	  curr_fo = fo_next(curr_fo);
	}
      } else {
	prev_fo = curr_fo;
	curr_fo = fo_next(curr_fo);
      }
    }
  }
  
  /* MATTHEW: Mark from queued eagers: */
  for (curr_fo = GC_finalize_now; curr_fo != end_eager_mark; curr_fo = fo_next(curr_fo)) {
    /* MATTHEW: if this is an eager finalization, then objects
       accessible from real_ptr need to be marked */
    if (curr_fo -> eager_level == eager_level) {
      (*(curr_fo -> fo_mark_proc))(curr_fo -> fo_hidden_base);
      while (!GC_mark_stack_empty()) GC_mark_from_mark_stack();
      if (GC_mark_state != MS_NONE) {
	/* Mark stack overflowed. Very unlikely. 
	   Everything's ok, though. Just mark from scratch. */
	while (!GC_mark_some());
      }
    }
  }
}

/* Called with world stopped.  Cause disappearing links to disappear,	*/
/* and invoke finalizers.						*/
void GC_finalize()
{
    struct disappearing_link * curr_dl, * prev_dl, * next_dl;
    struct finalizable_object * curr_fo, * prev_fo, * next_fo;
    ptr_t real_ptr, real_link;
    register int i;
    int dl_size = (log_dl_table_size == -1 ) ? 0 : (1 << log_dl_table_size);
    int fo_size = (log_fo_table_size == -1 ) ? 0 : (1 << log_fo_table_size);
    /* MATTHEW: for resetting the disapearing link */
    struct disappearing_link *done_dl = NULL, *last_done_dl = NULL;

    /* Make disappearing links disappear */
    /* MATTHEW: handle NULL real_link and remember old values */
    for (i = 0; i < dl_size; i++) {
      curr_dl = dl_head[i];
      prev_dl = 0;
      while (curr_dl != 0) {
	/* MATTHEW: skip late dls: */
	if (curr_dl->dl_special.kind == LATE_DL) {
	  prev_dl = curr_dl;
	  curr_dl = dl_next(curr_dl);
	  continue;
	}
	/* MATTHEW: reorder and set real_ptr based on real_link: */
        real_link = (ptr_t)REVEAL_POINTER(curr_dl -> dl_hidden_link);
        real_ptr = (ptr_t)REVEAL_POINTER(curr_dl -> dl_hidden_obj);
	if (!real_ptr)
	  real_ptr = (ptr_t)GC_base(*(GC_PTR *)real_link);
	/* MATTHEW: keep the dl entry if dl_special.kind = 1: */
        if (real_ptr && !GC_is_marked(real_ptr)) {
	  int needs_restore = (curr_dl->dl_special.kind == RESTORE_DL);
	  if (needs_restore)
  	    curr_dl->dl_special.value = *(word *)real_link;
	  *(word *)real_link = 0;

	  next_dl = dl_next(curr_dl);

          if (needs_restore && curr_dl->dl_special.value) {
	    if (!last_done_dl)
	      done_dl = curr_dl;
	    else
	      last_done_dl->restore_next = curr_dl;
	    last_done_dl = curr_dl;
	  } else {
	    if (prev_dl == 0)
	      dl_head[i] = next_dl;
	    else
	      dl_set_next(prev_dl, next_dl);

	    GC_clear_mark_bit((ptr_t)curr_dl);
 	    GC_dl_entries--;
	  }
	  curr_dl = next_dl;
	} else {
            prev_dl = curr_dl;
            curr_dl = dl_next(curr_dl);
        }
      }
    }

    /* MATTHEW: set NULL terminator: */
    if (last_done_dl)
      last_done_dl->restore_next = NULL;

  /* MATTHEW: All eagers first */
  /* Enqueue for finalization all EAGER objects that are still		*/
  /* unreachable.							*/
    GC_words_finalized = 0;
    finalize_eagers(1);
    if (GC_push_last_roots_again) GC_push_last_roots_again();
    finalize_eagers(2);
    if (GC_push_last_roots_again) GC_push_last_roots_again();

  /* Mark all objects reachable via chains of 1 or more pointers	*/
  /* from finalizable objects.						*/
  /* MATTHEW: non-eager finalizations only (eagers already marked) */
#   ifdef PRINTSTATS
        if (GC_mark_state != MS_NONE) ABORT("Bad mark state");
#   endif
    for (i = 0; i < fo_size; i++) {
      for (curr_fo = fo_head[i]; curr_fo != 0; curr_fo = fo_next(curr_fo)) {
	if (!(curr_fo -> eager_level)) { /* MATTHEW */
	  real_ptr = (ptr_t)REVEAL_POINTER(curr_fo -> fo_hidden_base);
	  if (!GC_is_marked(real_ptr)) {
            (*(curr_fo -> fo_mark_proc))(real_ptr);
            while (!GC_mark_stack_empty()) GC_mark_from_mark_stack();
            if (GC_mark_state != MS_NONE) {
	      /* Mark stack overflowed. Very unlikely. */
#		ifdef PRINTSTATS
	      if (GC_mark_state != MS_INVALID) ABORT("Bad mark state");
	      GC_printf0("Mark stack overflowed in finalization!!\n");
#		endif
	      /* Make mark bits consistent again.  Forget about	*/
	      /* finalizing this object for now.			*/
	      GC_set_mark_bit(real_ptr);
	      while (!GC_mark_some());
            }
#if 0
            if (GC_is_marked(real_ptr)) {
	      /* MATTHEW: we have some ok cycles (below a parent) */
	      printf("Finalization cycle involving %lx\n", real_ptr);
            }
#endif
	  }
	}
      }
    }
  /* Enqueue for finalization all objects that are still		*/
  /* unreachable.							*/
    /* GC_words_finalized = 0; */ /* MATTHEW: done above */
    for (i = 0; i < fo_size; i++) {
      curr_fo = fo_head[i];
      prev_fo = 0;
      while (curr_fo != 0) {
        real_ptr = (ptr_t)REVEAL_POINTER(curr_fo -> fo_hidden_base);
        if (!GC_is_marked(real_ptr)) {
            GC_set_mark_bit(real_ptr);

            /* Delete from hash table */
              next_fo = fo_next(curr_fo);
              if (prev_fo == 0) {
                fo_head[i] = next_fo;
              } else {
                fo_set_next(prev_fo, next_fo);
              }
              GC_fo_entries--;
            /* Add to list of objects awaiting finalization.	*/
              fo_set_next(curr_fo, GC_finalize_now);
              GC_finalize_now = curr_fo;
              /* unhide object pointer so any future collections will	*/
              /* see it.						*/
              curr_fo -> fo_hidden_base = 
              		(word) REVEAL_POINTER(curr_fo -> fo_hidden_base);
              GC_words_finalized +=
                 	ALIGNED_WORDS(curr_fo -> fo_object_size)
              		+ ALIGNED_WORDS(sizeof(struct finalizable_object));
#	    ifdef PRINTSTATS
              if (!GC_is_marked((ptr_t)curr_fo)) {
                ABORT("GC_finalize: found accessible unmarked object\n");
              }
#	    endif
            curr_fo = next_fo;
        } else {
            prev_fo = curr_fo;
            curr_fo = fo_next(curr_fo);
        }
      }
    }

    /* MATTHEW: Restore disappeared links. */
    curr_dl = done_dl;
    while (curr_dl != 0) {
      real_link = (ptr_t)REVEAL_POINTER(curr_dl -> dl_hidden_link);
      *(word *)real_link = curr_dl->dl_special.value;
      curr_dl->dl_special.kind = RESTORE_DL;
      prev_dl = curr_dl;
      curr_dl = curr_dl->restore_next;
      prev_dl->restore_next = NULL;
    }

    /* Remove dangling disappearing links. */
    for (i = 0; i < dl_size; i++) {
      curr_dl = dl_head[i];
      prev_dl = 0;
      while (curr_dl != 0) {
        real_link = GC_base((ptr_t)REVEAL_POINTER(curr_dl -> dl_hidden_link));
        if (real_link != 0 && !GC_is_marked(real_link)) {
            next_dl = dl_next(curr_dl);
            if (prev_dl == 0) {
                dl_head[i] = next_dl;
            } else {
                dl_set_next(prev_dl, next_dl);
            }
            GC_clear_mark_bit((ptr_t)curr_dl);
            GC_dl_entries--;
            curr_dl = next_dl;
        } else {
            prev_dl = curr_dl;
            curr_dl = dl_next(curr_dl);
        }
      }
    }

    /* MATTHEW: late disappearing links */
    for (i = 0; i < dl_size; i++) {
      curr_dl = dl_head[i];
      prev_dl = 0;
      while (curr_dl != 0) {
	if (curr_dl -> dl_special.kind == LATE_DL) {
	  /* MATTHEW: reorder and set real_ptr based on real_link: */
	  real_link = (ptr_t)REVEAL_POINTER(curr_dl -> dl_hidden_link);
	  real_ptr = (ptr_t)REVEAL_POINTER(curr_dl -> dl_hidden_obj);
	  if (!real_ptr)
	    real_ptr = (ptr_t)GC_base(*(GC_PTR *)real_link);
	  if (real_ptr && !GC_is_marked(real_ptr)) {
	    *(word *)real_link = 0;

	    next_dl = dl_next(curr_dl);

	    if (prev_dl == 0)
	      dl_head[i] = next_dl;
	    else
	      dl_set_next(prev_dl, next_dl);

	    GC_clear_mark_bit((ptr_t)curr_dl);
 	    GC_dl_entries--;

	    curr_dl = next_dl;
	  } else {
	    prev_dl = curr_dl;
	    curr_dl = dl_next(curr_dl);
	  }
	} else {
	  prev_dl = curr_dl;
	  curr_dl = dl_next(curr_dl);
        }
      }
    }

    /* MATTHEW: */
    if (GC_custom_finalize)
      GC_custom_finalize();
}

#ifdef JAVA_FINALIZATION

/* Enqueue all remaining finalizers to be run - Assumes lock is
 * held, and signals are disabled */
void GC_enqueue_all_finalizers()
{
    struct finalizable_object * curr_fo, * prev_fo, * next_fo;
    ptr_t real_ptr, real_link;
    register int i;
    int fo_size;
    
    fo_size = (log_fo_table_size == -1 ) ? 0 : (1 << log_fo_table_size);
    GC_words_finalized = 0;
    for (i = 0; i < fo_size; i++) {
        curr_fo = fo_head[i];
        prev_fo = 0;
      while (curr_fo != 0) {
          real_ptr = (ptr_t)REVEAL_POINTER(curr_fo -> fo_hidden_base);
          GC_MARK_FO(real_ptr, GC_normal_finalize_mark_proc);
          GC_set_mark_bit(real_ptr);
 
          /* Delete from hash table */
          next_fo = fo_next(curr_fo);
          if (prev_fo == 0) {
              fo_head[i] = next_fo;
          } else {
              fo_set_next(prev_fo, next_fo);
          }
          GC_fo_entries--;

          /* Add to list of objects awaiting finalization.	*/
          fo_set_next(curr_fo, GC_finalize_now);
          GC_finalize_now = curr_fo;

          /* unhide object pointer so any future collections will	*/
          /* see it.						*/
          curr_fo -> fo_hidden_base = 
        		(word) REVEAL_POINTER(curr_fo -> fo_hidden_base);

          GC_words_finalized +=
           	ALIGNED_WORDS(curr_fo -> fo_object_size)
        		+ ALIGNED_WORDS(sizeof(struct finalizable_object));
          curr_fo = next_fo;
        }
    }

    return;
}

/* Invoke all remaining finalizers that haven't yet been run. 
 * This is needed for strict compliance with the Java standard, 
 * which can make the runtime guarantee that all finalizers are run.
 * Unfortunately, the Java standard implies we have to keep running
 * finalizers until there are no more left, a potential infinite loop.
 * YUCK.  * This routine is externally callable, so is called without 
 * the allocation lock 
 */
void GC_finalize_all()
{
    DCL_LOCK_STATE;

    DISABLE_SIGNALS();
    LOCK();
    while (GC_fo_entries > 0) {
      GC_enqueue_all_finalizers();
      UNLOCK();
      ENABLE_SIGNALS();
      GC_INVOKE_FINALIZERS();
      DISABLE_SIGNALS();
      LOCK();
    }
    UNLOCK();
    ENABLE_SIGNALS();
}
#endif

/* Invoke finalizers for all objects that are ready to be finalized.	*/
/* Should be called without allocation lock.				*/
int GC_invoke_finalizers()
{
    static int doing = 0; /* MATTHEW */
    register struct finalizable_object * curr_fo;
    register int count = 0;
    DCL_LOCK_STATE;

    /* MATTHEW: don't allow nested finalizations */
    if (doing)
      return 0;
    doing++;
    
    while (GC_finalize_now != 0) {
#	ifdef THREADS
	    DISABLE_SIGNALS();
	    LOCK();
#	endif
    	curr_fo = GC_finalize_now;
#	ifdef THREADS
 	    if (curr_fo != 0) GC_finalize_now = fo_next(curr_fo);
	    UNLOCK();
	    ENABLE_SIGNALS();
	    if (curr_fo == 0) break;
#	else
	    GC_finalize_now = fo_next(curr_fo);
#	endif
 	fo_set_next(curr_fo, 0);
    	(*(curr_fo -> fo_fn))((ptr_t)(curr_fo -> fo_hidden_base),
    			      curr_fo -> fo_client_data);
    	curr_fo -> fo_client_data = 0;
	++count;
#	ifdef UNDEFINED
	    /* This is probably a bad idea.  It throws off accounting if */
	    /* nearly all objects are finalizable.  O.w. it shouldn't	 */
	    /* matter.							 */
    	    GC_free((GC_PTR)curr_fo);
#	endif
    }

    doing--; /* MATTHEW */

    return count;
}

# ifdef __STDC__
    GC_PTR GC_call_with_alloc_lock(GC_fn_type fn,
    					 GC_PTR client_data)
# else
    GC_PTR GC_call_with_alloc_lock(fn, client_data)
    GC_fn_type fn;
    GC_PTR client_data;
# endif
{
    GC_PTR result;
    DCL_LOCK_STATE;
    
#   ifdef THREADS
      DISABLE_SIGNALS();
      LOCK();
      SET_LOCK_HOLDER();
#   endif
    result = (*fn)(client_data);
#   ifdef THREADS
      UNSET_LOCK_HOLDER();
      UNLOCK();
      ENABLE_SIGNALS();
#   endif
    return(result);
}

