/* 
 * File: ups.cc: UPS projection library 
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

#include "ups.h"
#include <sstream>

namespace USNG2 {
  const Proj4 UPS::ProjLL84("+proj=latlong +ellps=GRS80 +datum=NAD83 +type=crs +no_defs");
  const Proj4 UPS::ProjNorth("+proj=stere +lat_0=90 +lat_ts=90 +lon_0=0 +k=0.994 +x_0=2000000 +y_0=2000000 +ellps=WGS84 +datum=WGS84 +units=m +type=crs +no_defs");
  const Proj4 UPS::ProjSouth("+proj=stere +lat_0=-90 +lat_ts=-90 +lon_0=0 +k=0.994 +x_0=2000000 +y_0=2000000 +ellps=WGS84 +datum=WGS84 +units=m +type=crs +no_defs");

  bool
  UPS::is_ll_north(const LL84 &ll)
  {
    return ll.lat > 0.0;
  }


  bool
  UPS::in_bounds() const
  {
    LL84 ll(*this);
    return ((this->is_north && ll.lat > 84.0 && ll.lat <= 90.0) ||
	    (ll.lat < -80.0 && ll.lat >= -90.0));
  }
  
  UPS::UPS(const LL84 &ll)
  {
    Point out;
    
    if (is_ll_north(ll)) {
      out = UPS::ProjLL84.transform(UPS::ProjNorth, Point {ll.lon, ll.lat});
    } else {
      out = UPS::ProjLL84.transform(UPS::ProjSouth, Point {ll.lon, ll.lat});
    }
    
    this->is_north = is_ll_north(ll);
    this->x = out.x;
    this->y = out.y;
  }

  UPS::operator LL84() const
  {
    Point out;
    
    if (this->is_north)
      out = UPS::ProjNorth.transform(UPS::ProjLL84, Point {this->x, this->y});
    else
      out = UPS::ProjSouth.transform(UPS::ProjLL84, Point {this->x, this->y});
    
    return LL84(out.y, out.x);
  }

  std::string
  UPS::toString() const
  {
    std::ostringstream oss;
    oss << std::fixed << "UPS ";
    if (this->is_north)
      oss << "North: ";
      else
	oss << "South: ";
    oss << "x: " << x << ", y: " << y;
    return oss.str();
  }
}

