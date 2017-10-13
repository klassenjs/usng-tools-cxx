/* 
 * File: ll84.cc: Simple class representing Latitude and Longitude.
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

#include "ll84.h"
#include <math.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace USNG2 {
  double
  LL84::normalize_lon(double lon)
  {   
    return fmod((lon + 180.0), 360.0) - 180.0;
  }
  

  LL84::LL84(double plat, double plon) : lat(plat), lon(normalize_lon(plon))
  {
    if(lat > 90.0 || lat < -90.0)
      throw(std::invalid_argument("Latitude out of range -90 to +90"));
  }


  LL84::LL84() : LL84(0.0, 0.0)
  {}
  
 
  double
  LL84::geodesic_distance( const LL84 &other ) const
  {  
    // http://en.wikipedia.org/wiki/Great-circle_distance
    // http://en.wikipedia.org/wiki/Vincenty%27s_formulae 
    
    // It is ok if this is approximate, just used to find
    // the closest matching grid for a partial coordinate
    double lat_s = lat * M_PI / 180.0;
    double lat_f = other.lat * M_PI / 180.0;
    double d_lon = (other.lon - lon) * M_PI / 180;
    return( atan2(
		  sqrt(pow(cos(lat_f) * sin(d_lon), 2) + pow(cos(lat_s) * sin(lat_f) - sin(lat_s) * cos(lat_f) * cos(d_lon), 2)) ,
		  ((sin(lat_s) * sin(lat_f)) + (cos(lat_s) * cos(lat_f) * cos(d_lon)))
		  )
	    );
  }

  
  std::string
  LL84::toString() const
  {
    std::ostringstream oss;
    oss << "LL84 latitude: " << lat << ", longitude: " << lon;
    return oss.str();
  }

  std::string
  LL84::toD(int precision) const
  {
    std::ostringstream oss;

    const char *lat_h = (this->lat > 0)? "N" : "S";      // Hemisphere
    double lat_d = fabs(this->lat);                      // Degrees

    const char *lon_h = (this->lon > 0)? "E" : "W";
    double lon_d = fabs(this->lon);

    oss << std::fixed
	<< std::setprecision(precision) << lat_d << "°"
	<< lat_h
	<< " "
	<< std::setprecision(precision) << lon_d << "°"
	<< lon_h;
    return oss.str();
  }
  
  std::string
  LL84::toDM(int precision) const
  {
    std::ostringstream oss;

    const char *lat_h = (this->lat > 0)? "N" : "S";      // Hemisphere
    double lat_d;                                        // Degrees
    double lat_m = modf(fabs(this->lat), &lat_d) * 60.0; // Decimal Minutes   

    const char *lon_h = (this->lon > 0)? "E" : "W";
    double lon_d;
    double lon_m = modf(fabs(this->lon), &lon_d) * 60.0;
    
    oss << std::fixed
	<< std::setprecision(0) << lat_d << "°"
	<< std::setprecision(precision) << lat_m << "'"
	<< lat_h
	<< " "
	<< std::setprecision(0) << lon_d << "°"
      	<< std::setprecision(precision) << lon_m << "'"
	<< lon_h;
    return oss.str();
  }

  std::string
  LL84::toDMS(int precision) const
  {
    std::ostringstream oss;

    const char *lat_h = (this->lat > 0)? "N" : "S";      // Hemisphere
    double lat_d;                                        // Degrees
    double lat_m = modf(fabs(this->lat), &lat_d) * 60.0; // Minutes   
    double lat_s = modf(lat_m, &lat_m) * 60.0;           // Seconds
    
    const char *lon_h = (this->lon > 0)? "E" : "W";
    double lon_d;
    double lon_m = modf(fabs(this->lon), &lon_d) * 60.0;
    double lon_s = modf(lon_m, &lon_m) * 60.0;
    
    oss << std::fixed
	<< std::setprecision(0) << lat_d << "°"
        << std::setprecision(0) << lat_m << "'"
        << std::setprecision(precision) << lat_s << "\""
	<< lat_h
	<< " "
	<< std::setprecision(0) << lon_d << "°"      
      	<< std::setprecision(0) << lon_m << "'"
      	<< std::setprecision(precision) << lon_s << "\""
	<< lon_h;
    return oss.str();
  }
  
}
