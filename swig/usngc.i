/* usng2-cc.i */
%module USNGC
%{
#include "usng2.h"
using namespace USNG2;


std::string fromll84(double lon, double lat)
{
  return USNG::fromLL84(LL84(lat, lon),4).toString();
}
%}

%include "exception.i"

%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}

%include std_string.i

%include "ll84.h"
%include "utm.h"
%include "ups.h"
%include "usng2.h"

std::string fromll84(double lon, double lat);


