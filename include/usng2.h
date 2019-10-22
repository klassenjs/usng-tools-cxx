/* -*-c++-*- */
/* 
 * File: usng2.h: USNG Coordinate Library
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
#include "ll84.h"
#include "utm.h"
#include "ups.h"

namespace USNG2 {
  class USNG {
    // Example 15TVK12345678
    unsigned int utm_zone;        // 15
    char grid_zone;               // T
    char grid_square[2];          // VK
    double grid_easting;    // 12340.0
    double grid_northing;    // 56780.0
    int precision = 4; // 4 (number of significant digits in easting/northing)

    typedef struct {
      const char *ns_grid;
      const char *ew_grid;
    } utm_gss_t;
    static utm_gss_t getGridSquareSet(unsigned int utm_zone);
    
    static USNG parseString(std::string usng);
    bool is_valid(bool strict) const;
    bool is_complete() const;
  public:
    USNG();
    USNG(const unsigned int utm_zone, const char grid_zone,
	 const char grid_square[2],
	 const double grid_easting, const double grid_northing,
	 const int precision);

    static USNG fromString(std::string usng, bool strict = true);
    static USNG fromString(std::string usng, const LL84 &initial_ll);
    
    static USNG fromUTM(const UTM &utm, int precision);
    static USNG fromUTM(const UTM &utm);
    static USNG fromUPS(const UPS &ups, int precision);
    static USNG fromUPS(const UPS &ups);
    static USNG fromLL84(const LL84 &ll, int precision);
    static USNG fromLL84(const LL84 &ll);

    UTM toUTM(bool strict = false) const;
    UPS toUPS() const;
    LL84 toLL84() const;
    
    explicit operator UTM() const;
    explicit operator UPS() const;
    explicit operator LL84() const;
    
    std::string toString() const;
  };
}
