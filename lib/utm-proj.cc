/* 
 * File: utm-proc.cc: Alternate UTM coordinate system projection 
 *                    implementation based on proj4.
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

#include "utm.h"
#include "proj4.h"
#include <math.h>
#include <sstream>

namespace USNG2 {
  //                             -80  -72  -64  -56  -48  -40  -32  -24  -16  -8    0    8   16   24   32   40   48   56   64   72   (*Latitude) 
  //                                                                                                                Handle oddball zone 80-84
  const char UTM::GridZones[] = {'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'X'};
  
  unsigned int
  UTM::calc_ll_utm_zone(const LL84 &ll)
  {
    // assert lon in -180, 180
    return (int)floor((ll.lon - (-180.0)) / 6.0) + 1;
  }


  unsigned int
  UTM::calc_ll_grid_zone_idx(const LL84 &ll)
  {
    int idx = floor((ll.lat - (-80.0)) / 8);
    idx = std::min(idx, 20);
    idx = std::max(idx, 0);
    return idx;
  }
  
  char
  UTM::calc_ll_grid_zone(const LL84 &ll)
  {
    return GridZones[UTM::calc_ll_grid_zone_idx(ll)];
  }


  const Proj4 UTM::ProjLL84("+proj=latlong +ellps=GRS80 +datum=NAD83 +no_defs");
  

  UTM::UTM(const LL84 &ll84)
  {

    this->zone = UTM::calc_ll_utm_zone(ll84);
    this->grid_zone = UTM::calc_ll_grid_zone(ll84);

    std::ostringstream proj_str;
    proj_str << "+proj=utm +datum=NAD83 +ellps=GRS80 +zone=" << this->zone;
    const Proj4 proj_utm(proj_str.str().c_str());

    const Point out = UTM::ProjLL84.transform(proj_utm, Point{ ll84.lon, ll84.lat });

    this->easting = out.x;
    this->northing = out.y;
  }


  UTM::operator LL84() const
  {
    std::ostringstream proj_str;
    proj_str << "+proj=utm +datum=NAD83 +ellps=GRS80 +zone=" << this->zone;
    const Proj4 proj_utm(proj_str.str().c_str());
    const Point out = proj_utm.transform(UTM::ProjLL84, Point{ this->easting, this->northing });
    
    return LL84(out.y, out.x);
  }

  
  std::string
  UTM::toString() const
  {
    std::ostringstream oss;
    oss << std::fixed << "UTM zone: " << zone << ", easting: " << easting << ", northing: " << northing;
    return oss.str();
  }
}
