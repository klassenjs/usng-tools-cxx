/* 
 * File: proj4.cc: C++ wrapper around the Proj4 projection library.
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

#include "proj4.h"
#include <stdexcept>

namespace USNG2 {
  std::string
  Proj4::get_proj_error_string() const
  {
    int *errnum = pj_get_errno_ref();
    if (errnum != NULL) {
      char *errstr = pj_strerrno(*errnum);
      if (errstr != NULL) {
	return std::string(errstr);
      }
    }
    return std::string("Unknown Proj4 error");
  }

  
  Proj4::Proj4(const std::string &init_string) : m_proj_pj(nullptr)
  {
    m_proj_pj = pj_init_plus(init_string.c_str());
    if (m_proj_pj == nullptr)
      throw(std::runtime_error(get_proj_error_string()));

    m_is_latlon = pj_is_latlong(m_proj_pj);
  }
  

  Proj4::~Proj4()
  {
    if (m_proj_pj != nullptr) {
      pj_free( m_proj_pj );
      m_proj_pj = nullptr;
    }
  }

  
  Point
  Proj4::transform(const Proj4 &to_proj, const Point &pt) const
  {
    double x = pt.x;
    double y = pt.y;
    if (m_is_latlon) {
      x = x * DEG_TO_RAD;
      y = y * DEG_TO_RAD;
    }
    int err = pj_transform(m_proj_pj, to_proj.m_proj_pj, 1, 1, &x, &y, NULL);
    if (err != 0) {
      throw(std::runtime_error(get_proj_error_string()));
    }
    if (to_proj.m_is_latlon) {
      x = x * RAD_TO_DEG;
      y = y * RAD_TO_DEG;
    }
    return Point { x, y };
  }
}
