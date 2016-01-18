/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2016 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

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

#ifndef XCSOAR_SCREEN_POINT_HPP
#define XCSOAR_SCREEN_POINT_HPP

#include "Math/Point2D.hpp"

struct PixelPoint : IntPoint2D {
  PixelPoint() = default;

  template<typename... Args>
  constexpr PixelPoint(Args&&... args)
    :IntPoint2D(args...) {}
};

struct PixelSize {
  int cx, cy;

  PixelSize() = default;

  constexpr PixelSize(int _width, int _height)
    :cx(_width), cy(_height) {}

  constexpr PixelSize(unsigned _width, unsigned _height)
    :cx(_width), cy(_height) {}

  constexpr PixelSize(long _width, long _height)
    :cx(_width), cy(_height) {}

  bool operator==(const PixelSize &other) const {
    return cx == other.cx && cy == other.cy;
  }

  bool operator!=(const PixelSize &other) const {
    return !(*this == other);
  }
};

inline constexpr PixelPoint
operator+(PixelPoint p, PixelSize size)
{
  return { p.x + size.cx, p.y + size.cy };
}

#ifdef ENABLE_OPENGL
#include "Screen/OpenGL/Point.hpp"
#elif defined(USE_MEMORY_CANVAS)
#include "Screen/Memory/Point.hpp"
#elif defined(USE_GDI)
#include "Screen/GDI/Point.hpp"
#else
#error No Point implementation
#endif

#include "Compiler.h"

gcc_pure
static inline bool
OverlapsRect(const PixelRect &a, const PixelRect &b)
{
  return a.left < b.right && b.left <= a.right &&
    a.top <= b.bottom && b.top <= a.bottom;
}

#endif
