/* 
 * File: utm.cc: UTM coordinate system projection library.
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


  const double UTM::MajorAxis = 6378137.0;
  const double UTM::MinorAxis = 6356752.3;
  const double UTM::Ecc = (MajorAxis * MajorAxis - MinorAxis * MinorAxis) / (MajorAxis * MajorAxis);
  const double UTM::Ecc2 = Ecc / (1.0 - Ecc);
  const double UTM::K0 = 0.9996;
  const double UTM::E4 = Ecc * Ecc;
  const double UTM::E6 = Ecc * E4;
  const double UTM::Degrees2Radians = M_PI / 180.0;
  const double UTM::Radians2Degrees = 180.0 / M_PI;
  
  
  // Computes the meridian distance for the GRS-80 Spheroid.
  // See equation 3-22, USGS Professional Paper 1395.
  double
  UTM::meridianDist(double lat)
  {
    static const double c1 = UTM::MajorAxis * (1 - UTM::Ecc / 4 - 3 * UTM::E4 / 64 - 5 * UTM::E6 / 256);
    static const double c2 = -UTM::MajorAxis * (3 * UTM::Ecc / 8 + 3 * UTM::E4 / 32 + 45 * UTM::E6 / 1024);
    static const double c3 = UTM::MajorAxis * (15 * UTM::E4 / 256 + 45 * UTM::E6 / 1024);
    static const double c4 = -UTM::MajorAxis * 35 * UTM::E6 / 3072;
    
    return(c1 * lat + c2 * sin(lat * 2) + c3 * sin(lat * 4) + c4 * sin(lat * 6));
  }


  UTM::UTM(const LL84 &ll84)
  {
    const double in_lon = ll84.lon;
    const double in_lat = ll84.lat;

    // Calculate UTM Zone number from Longitude
    // -180 = 180W is grid 1... increment every 6 degrees going east
    // Note [-180, -174) is in grid 1, [-174,-168) is 2, [174, 180) is 60 
    this->zone = UTM::calc_ll_utm_zone(ll84);
    this->grid_zone = UTM::calc_ll_grid_zone(ll84);
    
    const double centeralMeridian = -((30 - this->zone) * 6 + 3) * UTM::Degrees2Radians;
			
    const double lat = in_lat * UTM::Degrees2Radians;
    const double lon = in_lon * UTM::Degrees2Radians;
			
    const double latSin = sin(lat);
    const double latCos = cos(lat);
    const double latTan = latSin / latCos;
    const double latTan2 = latTan * latTan;
    const double latTan4 = latTan2 * latTan2;
			
    const double n = UTM::MajorAxis / sqrt(1 - UTM::Ecc * (latSin*latSin));
    const double c = UTM::Ecc2 * latCos*latCos;
    const double a = latCos * (lon - centeralMeridian);
    const double m = meridianDist(lat);
			
    const double temp5 = 1.0 - latTan2 + c;
    const double temp6 = 5.0 - 18.0 * latTan2 + latTan4 + 72.0 * c - 58.0 * UTM::Ecc2;

    const double a2 = a*a;
    const double a3 = a2*a;
    const double a4 = a3*a;
    const double a5 = a4*a;
		
    const double x = K0 * n * (a + (temp5 * a3) / 6.0 + temp6 * a5 / 120.0) + 500000;
			
    const double temp7 = (5.0 - latTan2 + 9.0 * c + 4.0 * (c*c)) * a4 / 24.0;
    const double temp8 = 61.0 - 58.0 * latTan2 + latTan4 + 600.0 * c - 330.0 * Ecc2;
    const double temp9 = a5 * a / 720.0;
			
    const double y = K0 * (m + n * latTan * (a2 / 2.0 + temp7 + temp8 * temp9));

    this->easting = x;
    this->northing = y;
  }


  UTM::operator LL84() const
  {
    const double centeralMeridian = -((30 - this->zone) * 6 + 3) * UTM::Degrees2Radians;
			
    const double temp = sqrt(1.0 - UTM::Ecc);
    const double ecc1 = (1.0 - temp) / (1.0 + temp);
    const double ecc12 = ecc1 * ecc1;
    const double ecc13 = ecc1 * ecc12;
    const double ecc14 = ecc12 * ecc12;
			
    const double easting = this->easting - 500000.0;
			
    const double m = this->northing / K0;
    const double um = m / (UTM::MajorAxis * (1.0 - (UTM::Ecc / 4.0) - 3.0 * (UTM::E4 / 64.0) - 5.0 * (UTM::E6 / 256.0)));
			
    const double temp8 = (1.5 * ecc1) - (27.0 / 32.0) * ecc13;
    const double temp9 = ((21.0 / 16.0) * ecc12) - ((55.0 / 32.0) * ecc14);

    const double latrad1 = um + temp8 * sin(2 * um) + temp9 * sin(4 * um) + (151.0 * ecc13 / 96.0) * sin(6.0 * um);
			
    const double latsin1 = sin(latrad1);
    const double latcos1 = cos(latrad1);
    const double lattan1 = latsin1 / latcos1;
    const double n1 = UTM::MajorAxis / sqrt(1.0 - UTM::Ecc * latsin1 * latsin1);
    const double t2 = lattan1 * lattan1;
    const double c1 = UTM::Ecc2 * latcos1 * latcos1;
			
    const double temp20 = (1.0 - UTM::Ecc * latsin1 * latsin1);
    const double r1 = UTM::MajorAxis * (1.0 - UTM::Ecc) / sqrt(temp20 * temp20 * temp20);
			
    const double d1 = easting / (n1 * UTM::K0);
    const double d2 = d1 * d1;
    const double d3 = d1 * d2;
    const double d4 = d2 * d2;
    const double d5 = d1 * d4;
    const double d6 = d3 * d3;
			
    const double t12 = t2 * t2;
    const double c12 = c1 * c1;
			
    const double temp1 = n1 * lattan1 / r1;
    const double temp2 = 5.0 + 3.0 * t2 + 10.0 * c1 - 4.0 * c12 - 9.0 * UTM::Ecc2;
    const double temp4 = 61.0 + 90.0 * t2 + 298.0 * c1 + 45.0 * t12 - 252.0 * UTM::Ecc2 - 3.0 * c12;
    const double temp5 = (1.0 + 2.0 * t2 + c1) * d3 / 6.0;
    const double temp6 = 5.0 - 2.0 * c1 + 28.0 * t2 - 3.0 * c12 + 8.0 * UTM::Ecc2 + 24.0 * t12;

    const double lat = (latrad1 - temp1 * (d2 / 2.0 - temp2 * (d4 / 24.0) + temp4 * d6 / 720.0)) * UTM::Radians2Degrees;
    const double lon = (centeralMeridian + (d1 - temp5 + temp6 * d5 / 120.0) / latcos1) * UTM::Radians2Degrees;

    return LL84(lat, lon);
  }

  
  std::string
  UTM::toString() const
  {
    std::ostringstream oss;
    oss << std::fixed << "UTM zone: " << zone << ", easting: " << easting << ", northing: " << northing;
    return oss.str();
  }
}
