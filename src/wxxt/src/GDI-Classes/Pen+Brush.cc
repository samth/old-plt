/*								-*- C++ -*-
 *
 * Purpose: pen and brush classes needed for drawing
 *
 * Authors: Markus Holzem and Julian Smart
 *
 * Copyright: (C) 1995, AIAI, University of Edinburgh (Julian)
 * Copyright: (C) 1995, GNU (Markus)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef __GNUG__
#pragma implementation "Pen+Brush.h"
#pragma implementation "Region.h"
#endif

#define  Uses_XLib
#define  Uses_wxPenBrush
#define  Uses_wxBitmap
#include "wx.h"
#include <string.h>

//-----------------------------------------------------------------------------
// wxPen create and destroy
//-----------------------------------------------------------------------------

wxPen::wxPen(void)
{
    wxColour *c;

    __type = wxTYPE_PEN;

    stipple = NULL;
    c = new wxColour(wxBLACK);
    c->Lock(1);
    colour  = c;
    style   = wxSOLID;
    join    = wxJOIN_ROUND;
    cap     = wxCAP_ROUND;
    nb_dash = 0;
    dash    = NULL;
    width   = 1;
    locked = 0;
}

wxPen::wxPen(wxColour *col, float Width, int Style)
{
    wxColour *c;

    __type = wxTYPE_PEN;

    c = new wxColour(col);
    c->Lock(1);
    colour  = c;
    width   = Width;
    style   = Style;
    stipple = NULL;
    join    = wxJOIN_ROUND;
    cap     = wxCAP_ROUND;
    nb_dash = 0;
    dash    = NULL;
    locked = 0;
}

wxPen::wxPen(const char *col, float Width, int Style)
{
    wxColour *c;

    __type = wxTYPE_PEN;

    c = new wxColour(col);
    c->Lock(1);
    colour  = c;
    width   = Width;
    style   = Style;
    stipple = NULL;
    join    = wxJOIN_ROUND;
    cap     = wxCAP_ROUND;
    nb_dash = 0;
    dash    = NULL;
    locked = 0;
}

wxPen::~wxPen(void)
{
  if (stipple)
    --stipple->selectedIntoDC;
}

void wxPen::SetStipple(wxBitmap *s)
{
  if (s && (!s->Ok() || (s->selectedIntoDC < 0)))
    return;

  if (s)
    s->selectedIntoDC++;
  if (stipple)
    --stipple->selectedIntoDC;

  stipple = s;
}

//-----------------------------------------------------------------------------
// wxBrush
//-----------------------------------------------------------------------------

wxBrush::wxBrush(void)
{
    wxColour *c;

    __type = wxTYPE_BRUSH;

    c = new wxColour(wxWHITE);
    c->Lock(1);
    colour  = c;
    stipple = NULL;
    style   = wxSOLID;
    locked  = 0;
}

wxBrush::wxBrush(wxColour *col, int Style)
{
    wxColour *c;

    __type = wxTYPE_BRUSH;

    c = new wxColour(col);
    c->Lock(1);
    colour  = c;
    style   = Style;
    stipple = NULL;
    locked  = 0;
}

wxBrush::wxBrush(const char *col, int Style)
{
    wxColour *c;

    __type = wxTYPE_BRUSH;

    c = new wxColour(col);
    c->Lock(1);
    colour  = c;
    style   = Style;
    stipple = NULL;
    locked  = 0;
}

wxBrush::~wxBrush(void)
{
  if (stipple)
    --stipple->selectedIntoDC;
}

void wxBrush::SetStipple(wxBitmap *s)
{
  if (s && (!s->Ok() || (s->selectedIntoDC < 0)))
    return;

  if (s)
    s->selectedIntoDC++;
  if (stipple)
    --stipple->selectedIntoDC;

  stipple = s;
}

//-----------------------------------------------------------------------------
// wxPenList
//-----------------------------------------------------------------------------

wxPenList::wxPenList(void)
: wxObject(WXGC_NO_CLEANUP)
{
  list = new wxChildList;
}

wxPenList::~wxPenList(void)
{
}

void wxPenList::AddPen(wxPen *Pen) 
{ 
  list->Append(Pen); 
  list->Show(Pen, -1); /* so it can be collected */
} 

wxPen *wxPenList::FindOrCreatePen(wxColour *colour, float w, int style)
{
  wxPen *pen;
  wxChildNode *node;
  int i = 0;
  
  if (!colour)
    return NULL;
  
  while ((node = list->NextNode(i))) {
    wxPen *each_pen;
    wxColour *pc;
    each_pen = (wxPen*)node->Data();
    pc = each_pen->GetColour();
    if (each_pen &&
	each_pen->GetWidthF() == w &&
	each_pen->GetStyle() == style &&
	pc->Red() == colour->Red() &&
	pc->Green() == colour->Green() &&
	pc->Blue() == colour->Blue())
      return each_pen;
  }
  
  pen = new wxPen(colour, w, style);
  pen->Lock(1);
  AddPen(pen);
  
  return pen;
}

wxPen *wxPenList::FindOrCreatePen(char *colour, float width, int style)
{
  wxColour *the_colour;
  the_colour = wxTheColourDatabase->FindColour(colour);
  if (the_colour)
    return FindOrCreatePen(the_colour, width, style);
  return NULL;
}

//-----------------------------------------------------------------------------
// wxBrushList
//-----------------------------------------------------------------------------

wxBrushList::wxBrushList(void)
: wxObject(WXGC_NO_CLEANUP)
{
  list = new wxChildList;
}

wxBrushList::~wxBrushList(void)
{
}

void wxBrushList::AddBrush(wxBrush *Brush) 
{ 
  list->Append(Brush); 
  list->Show(Brush, -1); /* so it can be collected */
} 

wxBrush *wxBrushList::FindOrCreateBrush(wxColour *colour, int style)
{
  wxBrush *brush;
  wxChildNode *node;
  int i = 0;

  if (!colour)
    return NULL;

  while ((node = list->NextNode(i))) {
    wxBrush *each_brush;
    wxColour *bc;
    each_brush = (wxBrush*)node->Data();
    bc = each_brush->GetColour();
    if (each_brush &&
	each_brush->GetStyle() == style &&
	bc->Red() == colour->Red() &&
	bc->Green() == colour->Green() &&
	bc->Blue() == colour->Blue())
      return each_brush;
  }

  brush = new wxBrush(colour, style);
  brush->Lock(1);
  AddBrush(brush);
  
  return brush;
}

wxBrush *wxBrushList::FindOrCreateBrush(char *colour, int style)
{
  wxColour *the_colour;
  the_colour = wxTheColourDatabase->FindColour(colour);
  if (the_colour)
    return FindOrCreateBrush(the_colour, style);
  return NULL;
}

#define UseXtRegions
#include "../../../wxcommon/Region.h"
#include "wx_types.h"

#include "../../../wxcommon/Region.cxx"
