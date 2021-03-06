/* -*-c++-*- */
/* 
 * File: ll84.h: Simple class representing Latitude and Longitude.
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

namespace USNG2 {
  class LL84 {
  public:
    /*const*/ double lat;
    /*const*/ double lon;
    
    LL84(double plat, double plon);
    LL84();
    
    double geodesic_distance( const LL84 &other) const;

    std::string toString() const;
    std::string toD(int precision = 5) const;
    std::string toDM(int precision = 2) const;
    std::string toDMS(int precision = 0) const;

  private:
    static double normalize_lon(double plon);
  };
}
