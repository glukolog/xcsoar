/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009

	M Roberts (original release)
	Robin Birch <robinb@ruffnready.co.uk>
	Samuel Gisiger <samuel.gisiger@triadis.ch>
	Jeff Goodenough <jeff@enborne.f2s.com>
	Alastair Harrison <aharrison@magic.force9.co.uk>
	Scott Penrose <scottp@dd.com.au>
	John Wharington <jwharington@gmail.com>
	Lars H <lars_hn@hotmail.com>
	Rob Dunning <rob@raspberryridgesheepfarm.com>
	Russell King <rmk@arm.linux.org.uk>
	Paolo Ventafridda <coolwind@email.it>
	Tobias Lohner <tobias@lohner-net.de>
	Mirek Jezek <mjezek@ipplc.cz>
	Max Kellermann <max@duempel.org>
	Tobias Bieniek <tobias.bieniek@gmx.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/
#if !defined(XCSOAR_RENDER_TASK_POINT_HPP)
#define XCSOAR_RENDER_TASK_POINT_HPP

#include "Task/Visitors/TaskPointVisitor.hpp"
#include "MapDrawHelper.hpp"

class RenderObservationZone;

class RenderTaskPoint:
  public TaskPointConstVisitor,
  public MapDrawHelper
{
public:
  RenderTaskPoint(MapDrawHelper &_helper, 
                  RenderObservationZone &_ozv,
                  const bool draw_bearing,
                  const GEOPOINT location);

  void Visit(const UnorderedTaskPoint& tp);
  void Visit(const StartPoint& tp);
  void Visit(const FinishPoint& tp);
  void Visit(const AATPoint& tp);
  void Visit(const ASTPoint& tp);
  void set_layer(unsigned set);
  void set_active_index(unsigned active_index);
protected:
  void draw_ordered(const OrderedTaskPoint& tp);
  bool leg_active();
  bool point_past();
  bool point_current();
  bool do_draw_samples(const TaskPoint& tp);
  bool do_draw_bearing(const TaskPoint &tp);
  bool do_draw_target(const TaskPoint &tp);
  bool do_draw_isoline(const TaskPoint &tp);
  void draw_bearing(const TaskPoint &tp);
  virtual void draw_target(const TaskPoint &tp);
  void draw_task_line(const GEOPOINT& start, const GEOPOINT& end);
  void draw_isoline(const AATPoint& tp);
  void draw_samples(const OrderedTaskPoint& tp);
  void draw_oz_background(const OrderedTaskPoint& tp);
  void draw_oz_foreground(const OrderedTaskPoint& tp);
  const bool m_draw_bearing;
  const Pen pen_leg_active;
  const Pen pen_leg_inactive;
  const Pen pen_leg_arrow;
  const Pen pen_isoline;
  GEOPOINT m_last_point;
  unsigned m_index;
  RenderObservationZone &ozv;
  unsigned m_active_index;
  unsigned m_layer;
  const GEOPOINT m_location;
};


#endif
