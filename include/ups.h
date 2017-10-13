/* -*-c++-*- */
/* 
 * File: ups.h: UPS projection library
 *
 * Copyright (c) 2008-2016 James Klassen
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the “Software”), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies of this Software or works derived from this
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include "ll84.h"
#include "proj4.h"
#include <string>

namespace USNG2 {
  class UPS {
    static const Proj4 ProjLL84;
    static const Proj4 ProjNorth;
    static const Proj4 ProjSouth;

  public:
    bool is_north;
    double x;
    double y;
    UPS(bool north, double x, double y) :
      is_north(north),
      x(x),
      y(y)
    {};
    explicit UPS(const LL84 &ll);
    explicit operator LL84() const;

    static bool is_ll_north(const LL84 &ll);
    bool in_bounds() const;
    std::string toString() const;
  };
}
