/* -*-c++-*- */
/* 
 * File: proj4.h: C++ wrapper around the Proj4 projection library.
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
#include <proj_api.h>

namespace USNG2 {

  struct Point {
    double x;
    double y;
  };
  
  class Proj4 {
    projPJ m_proj_pj; // Note: this is really a pointer so
                      //       need to disable copy or reference count
    bool m_is_latlon;
    
    std::string get_proj_error_string() const;
  public:
    Proj4(const std::string &init_string);
    ~Proj4();

    // Disable copy and move for now.
    Proj4(const Proj4&) =delete;
    Proj4 operator=(const Proj4&) =delete;
    Proj4(const Proj4&&) =delete;
    Proj4 operator=(const Proj4&&) =delete;
    
    Point transform(const Proj4 &to_proj, const Point &pt) const;
  };
  
}
