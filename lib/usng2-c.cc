/* -*-c++-*- */
/* 
 * File: usng2-c.cc: USNG Coordinate Library C-API
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

#include "usng2-c.h"
#include "usng2.h"

extern "C" {

#include <string.h>
  
  char* usng2fromll84(double lon, double lat, int precision)
  {
    char* usng = NULL;
    
    try {
      std::string s = USNG2::USNG::fromLL84(USNG2::LL84(lat, lon), precision).toString();
      const char *sc = s.c_str();
      int length = s.length() + 1;
      
      usng = (char*)malloc(length);
      if (usng != NULL)
	strncpy(usng, sc, length);
    } catch (...) {
      ;
    }

    return usng;
  }

  
  int usng2toll84(const char* usng, double *lon, double *lat)
  {
    int err = 1;
    if (lon != NULL && lat != NULL) {
      try {
	auto u = USNG2::USNG::fromString(usng, USNG2::LL84(*lat, *lon));
	auto ll = u.toLL84();
	*lon = ll.lon;
	*lat = ll.lat;
	err = 0;
      } catch (...) {
	;
      }
    }
    return err;
  }


  char* usng2_ll84tolldm(double lon, double lat)
  {
    char* dm = NULL;
    try {
      auto ll = USNG2::LL84(lat, lon);
      auto s = ll.toDM();
      auto cs = s.c_str();
      auto l = s.length() + 1;
      dm = (char*) malloc(l);
      if (dm != NULL)
	strncpy(dm, cs, l);
    } catch (...) {
      ;
    }
    return dm;
  }

  
}
