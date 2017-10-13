/* -*-c++-*- */
/* 
 * File: utm.h: UTM coordinate system projection library.
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

#include <string>
#include "proj4.h"
#include "ll84.h"

#ifdef SWIG
#define __attribute_pure__
#endif

namespace USNG2 {
  class UTM {
  private:
    static const char GridZones[];
    static const Proj4 ProjLL84;
    
  public:
    int zone;
    char grid_zone;
    double easting;
    double northing;

    UTM(int zone, double easting, double northing) :
      zone(zone),
      grid_zone('?'),
      easting(easting),
      northing(northing)
    {
			grid_zone = calc_ll_grid_zone( LL84(*this) );
		};
    explicit UTM(const LL84 &ll);
    explicit operator LL84() const;

    static unsigned int calc_ll_utm_zone(const LL84 &ll);
    static unsigned int calc_ll_grid_zone_idx(const LL84 &ll);
    static char calc_ll_grid_zone(const LL84 &ll);
    
    std::string toString() const;
    LL84 toLL84() const { return LL84(*this); };
  };
}
