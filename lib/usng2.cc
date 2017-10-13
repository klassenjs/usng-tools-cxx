/*
 * Library to convert between NAD83 Lat/Lon and US National Grid
 * Maintained at https://github.com/klassenjs/usng_tools
 *
 * License:
 * 
 * Copyright (c) 2008-2016 James Klassen
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the 'Software'), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies of this Software or works derived from this
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* TODO: Norway and others odd grid
 *       UTM as hash instead of function?
 *       More tolerant of extended zones in UPS zones?
 *       Return box instead of point?
 *       Return list of coordinates w/distances for truncated search as well as best.
 *       Internalize UPS projection (remove proj4js dependency).
 *       
 */ 

#include "usng2.h"
#include "proj4.h"
#include <stdexcept>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <math.h>

namespace USNG2 {
  // Note these can be std::vector<char> asdf = {'a', 'b', .. }  in C++11

  // Note: grid locations are the SW corner of the grid square (because easting and northing are always positive)
  //                            0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19   x 100,000m northing
  const char NSLetters135[] = {'A','B','C','D','E','F','G','H','J','K','L','M','N','P','Q','R','S','T','U','V'};
  const char NSLetters246[] = {'F','G','H','J','K','L','M','N','P','Q','R','S','T','U','V','A','B','C','D','E'};
  
  //                           1   2   3   4   5   6   7   8   x 100,000m easting
  const char EWLetters14[] = {'A','B','C','D','E','F','G','H'};
  const char EWLetters25[] = {'J','K','L','M','N','P','Q','R'};
  const char EWLetters36[] = {'S','T','U','V','W','X','Y','Z'};
  
  //                             -80  -72  -64  -56  -48  -40  -32  -24  -16  -8    0    8   16   24   32   40   48   56   64   72   (*Latitude) 
  //                                                                                                             Handle oddball zone 80-84
  const char   GridZones[]    = {'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'X'};
  const double GridZonesDeg[] = {-80, -72, -64, -56, -48, -40, -32, -24, -16, -8,   0,   8,  16,  24,  32,  40,  48,  58,  64,  72,  80};
  
  // TODO: This is approximate and actually depends on longitude too.
  // GridZonesNorthing[i] =  2 * PI * 6356752.3 * (GridZonesDeg[i] / 360.0)
  const double GridZonesNorthing[] = {-8875700.589498, -7988130.530548, -7100560.471598, -6212990.412648, -5325420.353699,
				      -4437850.294749, -3550280.235799, -2662710.176849, -1775140.117900, -887570.058950,
				      0.00000,   887570.058950,  1775140.117900,  2662710.176849,  3550280.235799,
				      4437850.294749, 5325420.353699,  6212990.412648,  7100560.471598,  7988130.530548,
				      8875700.589498 };
  
  // Grid Letters for UPS
  //                          0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16   17
  const char XLetters[]  = { 'A', 'B', 'C', 'F', 'G', 'H', 'J', 'K', 'L', 'P', 'Q', 'R', 'S', 'T', 'U', 'X', 'Y', 'Z' };
  const char YNLetters[] = { 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'A', 'B', 'C', 'D', 'E', 'F', 'G' };
  const char YSLetters[] = { 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
			     'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M' };

  /* Default to Lat = 0.0, Lon = 0.0 */
  USNG::USNG() :
    utm_zone{31},
    grid_zone{'N'},
    grid_square{'A', 'A'},
    grid_easting{66021.0},
    grid_northing{0.0},
    precision{0}
    {}

  USNG::USNG(const unsigned int utm_zone, const char grid_zone,
	     const char grid_square[2],
	     const double grid_easting, const double grid_northing,
	     const int precision) :
    utm_zone(utm_zone), grid_zone(grid_zone),
    grid_square{grid_square[0], grid_square[1]},
    grid_easting(grid_easting), grid_northing(grid_northing),
    precision(precision)
    {}


  USNG::utm_gss_t
  USNG::getGridSquareSet(unsigned int utm_zone)
  {
    const int grid_square_set = utm_zone % 6;
    const char* ns_grid;
    const char* ew_grid;
    switch (grid_square_set) {
    case 1:
      ns_grid = NSLetters135;
      ew_grid = EWLetters14;
      break;
    case 2:
      ns_grid = NSLetters246;
      ew_grid = EWLetters25;
      break;
    case 3:
      ns_grid = NSLetters135;
      ew_grid = EWLetters36;
      break;
    case 4:
      ns_grid = NSLetters246;
      ew_grid = EWLetters14;
      break;
    case 5:
      ns_grid = NSLetters135;
      ew_grid = EWLetters25;
      break;
    case 0: // grid_square_set will == 0, but it is technically group 6 
      ns_grid = NSLetters246;
      ew_grid = EWLetters36;
      break;
    default:
      throw(std::runtime_error("USNG: Can't get here"));
    }
    return USNG::utm_gss_t{ ns_grid, ew_grid };
  }

  
  
  /* Method to convert a USNG coordinate string into a NAD83/WGS84 LonLat Point 
   * First parameter: usng = A valid USNG coordinate ASCII string (possibly truncated)
   *	Possible cases:
   *		Full USNG: 14TPU3467
   *		Truncated:   TPU3467 allowed??? 
   *		Truncated:    PU3467
   *		Truncated:      3467
   *		Truncated: 14TPU
   *		Truncated: 14T       allowed???
   *		Truncated:    PU
   * Second parameter: optional LL84 point to use to disambiguate a truncated USNG point
   * Returns: The parsed USNG coordinate
   */ 
  USNG
  USNG::parseString(std::string usng_str)
  {
    // 1) Preprocssing:
    // 1a) Remove any whitespace from the string.
    usng_str.erase(std::remove_if(usng_str.begin(),
				  usng_str.end(),
				  [](char x){return std::isspace(x);}),
		   usng_str.end());
    
    // 1b) Uppercase string
    std::transform(usng_str.begin(), usng_str.end(),
		   usng_str.begin(),
    		   static_cast<int(*)(int)>(&std::toupper));
    
    // 2) Strip any digits off the end to get easting and northing
    double easting = 0.0;
    double northing = 0.0;
    int precision = 0;
    {
      auto first_number = std::find_if_not(usng_str.rbegin(), usng_str.rend(),
					   static_cast<int(*)(int)>(&std::isdigit)).base();
      
      int number_of_digits = usng_str.end() - first_number;
      precision = number_of_digits / 2;
      double scale_factor = pow(10, (5 - precision));
      
      if (number_of_digits != precision * 2) {
	throw(std::invalid_argument("USNG strings require an even number of digits"));
      }

      if (precision > 0) {
	std::string easting_s(first_number, first_number + precision);
	std::string northing_s(first_number + precision, usng_str.end());
      
	easting = std::stoi(easting_s) * scale_factor;
	northing = std::stoi(northing_s) * scale_factor;
      }
      
      usng_str.erase(first_number, usng_str.end());
    }
    
    // 3) Get the 100km Grid Designator (if any)
    std::string grid_square("??");
    char grid_zone = '?';
    unsigned int utm_zone = 99;
    if (usng_str.begin() <= usng_str.end() - 2) {	    
      grid_square = std::string(usng_str.end() - 2, usng_str.end());
      usng_str.erase(usng_str.end() - 2, usng_str.end());
      
      // Verify both are Uppercase ASCII.
      // Note: Can't use isupper because which letters are included is locale depenedent.
      if (!std::all_of(grid_square.begin(), grid_square.end(),
		       [](char x){return ('A' <= x && x <= 'Z');})) {
	throw(std::invalid_argument("USNG grid zone not two letters"));
      }
      
      // 4) Get the grid zone (if any)
      if (usng_str.length() > 0) {
	grid_zone = *(usng_str.end() - 1);
      }

      // 5) Get the UTM zone (if any)
      if (grid_zone != 'A' && grid_zone != 'B' && grid_zone != 'Y' && grid_zone != 'Z') {
	if (usng_str.length() > 3) {
	  throw(std::invalid_argument("USNG extra characters at beginning of string"));
	} else if (usng_str.length() > 1) {
	  std::string utm_zone_s(usng_str.begin(), usng_str.end() - 1);
	  utm_zone = std::stoi(utm_zone_s);
	}
      } else if (usng_str.length() > 1) {
	  throw(std::invalid_argument("USNG extra characters at beginning of string"));
      }
    }
    
    return USNG(utm_zone, grid_zone, grid_square.c_str(), easting, northing, precision);
  }


  // True if fully parsed, false if truncated coordinate
  bool
  USNG::is_complete() const
  {
    return (
	    (this->utm_zone <= 60 && this->grid_zone != '?' && grid_square[0] != '?' && grid_square[1] != '?')
	    ||
	    (
	     (this->grid_zone == 'A' || this->grid_zone == 'B' || this->grid_zone == 'Y' || this->grid_zone == 'Z')
	     &&
	     (this->grid_square[0] != '?' && this->grid_square[1] != '?')
	    )
	   );
  }

  
  // True if fully parsed and can generate a geographic coordinate
  bool
  USNG::is_valid(bool strict) const
  {
    bool valid = true;
    if (this->is_complete())
      try {
	if (this->grid_zone == 'A' || this->grid_zone == 'B' || this->grid_zone == 'Y' || this->grid_zone == 'Z')
	  this->toUPS();
	else
	  this->toUTM(true);
      } catch (std::runtime_error &e) {
	valid = false;
      } catch (std::invalid_argument &e) {
	valid = false;
      }
    else
      valid = false;
    
    return valid;
  }

  
  
  USNG
  USNG::fromString(std::string usng_str)
  {
    auto u = USNG::parseString(usng_str);

    if (!u.is_valid(false)) {
      throw(std::runtime_error("USNG: Not enough information to parse string"));
    }
    return u;
  }


  USNG
  USNG::fromString(std::string usng_str, const LL84 &initial_ll)
  {
    auto u = USNG::parseString(usng_str);

    if(u.is_complete()) { // nothing to do
      return u;
    }

    double min_arc_distance = 1000;
    unsigned int min_utm_zone = 99;
    char min_grid_zone = '?';
    char min_grid_square[2] = {'?', '?'};
    
    auto try_combo = [&min_arc_distance, &min_utm_zone, &min_grid_zone, &min_grid_square, initial_ll]
      (int utm_zone, char grid_zone, char grid_square[2], double easting, double northing, int precision)
      {
	try {
	  auto res = USNG(utm_zone, grid_zone, grid_square, easting, northing, precision).toLL84();
	  double arc_distance = initial_ll.geodesic_distance(res);
	  if (arc_distance < min_arc_distance) {
	    min_arc_distance = arc_distance;
	    min_utm_zone = utm_zone % 60;
	    min_grid_zone = grid_zone;
	    min_grid_square[0] = grid_square[0];
	    min_grid_square[1] = grid_square[1];
	  }
	} catch (std::runtime_error) {
	  ; // Ignore errors from invalid combinations
	} catch (std::invalid_argument) {
	  ; // Ignore errors from invalid combinations
	}
      };

    /* Prepare UPS grids */
    char ups_zones[2];
    const char* y_zones;
    int y_max = 0;
    if (initial_ll.lat > 0) {
      ups_zones[0] = 'Y';
      ups_zones[1] = 'Z';
      y_zones = YNLetters;
      y_max = 14;
    }	else {
      ups_zones[0] = 'A';
      ups_zones[1] = 'B';
      y_zones = YSLetters;
      y_max = 24;
    }
    
    if (u.grid_square[0] != '?' && u.grid_square[1] != '?') {
      // Only need to find closest matching UTM zone and grid zone (or UPS grid zone), have grid square.

      // Find closest UTM zone
      unsigned int ll_utm_zone = UTM::calc_ll_utm_zone(initial_ll);
      for (unsigned int utm_zone = ll_utm_zone - 1; utm_zone <= ll_utm_zone + 1; utm_zone++) {
	for (auto grid_zone : GridZones) { 
	  try_combo(utm_zone, grid_zone, u.grid_square, u.grid_easting, u.grid_northing, u.precision);
	}
      }

      // Find closest UPS zone
      for (auto ups_zone : ups_zones) {
	try_combo(99, ups_zone, u.grid_square, u.grid_easting, u.grid_northing, u.precision);
      }
    } else { // Need to find UTM and grid zones (or UPS grid zone), and grid square.
      
      // Find closest UTM zone/square
      unsigned int ll_utm_zone = UTM::calc_ll_utm_zone(initial_ll);
      for (unsigned int utm_zone = ll_utm_zone - 1; utm_zone <= ll_utm_zone + 1; utm_zone++) {
	auto gss = USNG::getGridSquareSet(utm_zone);
	
	for(auto &grid_zone : GridZones) {
	  for (int ns_grid_idx = 0; ns_grid_idx < 20; ns_grid_idx++) {
	    auto ns_grid = gss.ns_grid[ns_grid_idx];
	    for (int ew_grid_idx = 0; ew_grid_idx < 8; ew_grid_idx++) {
	      auto ew_grid =  gss.ew_grid[ew_grid_idx];
	      char grid_square[2] = { ew_grid, ns_grid };
	      try_combo(utm_zone, grid_zone, grid_square, u.grid_easting, u.grid_northing, u.precision);
	    }
	  }						
	}
      }

      // Find Closest UPS zone/square
      for (auto ups_zone: ups_zones) {
	for (int y_idx = 0; y_idx < y_max; y_idx++) {
	  for (int x_idx = 0; x_idx < 18; x_idx++) {
	    char grid_square[2] = { XLetters[x_idx], y_zones[y_idx] };
	    try_combo(99, ups_zone, grid_square, u.grid_easting, u.grid_northing, u.precision);
	  }
	}
      }
    }
    
    if (min_grid_zone == '?') {
      throw(std::runtime_error("USNG: Couldn't find a match"));
    }
    return USNG(min_utm_zone, min_grid_zone, min_grid_square, u.grid_easting, u.grid_northing, u.precision);
  }

  
  USNG
  USNG::fromUTM(const UTM &utm, int precision)
  {
    auto gss = USNG::getGridSquareSet(utm.zone);

    int ew_idx = (int)floor(utm.easting / 100000) - 1; // should be [100000, 900000]
    int ns_idx = (int)floor(fmod(utm.northing, 2000000) / 100000); // should [0, 10000000) => [0, 2000000)
    if(ns_idx < 0) { /* handle southern hemisphere */
      ns_idx += 20;
    }

    char grid_square[2] = { gss.ew_grid[ew_idx], gss.ns_grid[ns_idx] };
    
    double grid_easting  = fmod(utm.easting, 100000);
    double grid_northing = fmod(utm.northing, 100000);
    if(grid_northing < 0) {
      grid_northing += 100000;
    }

    return USNG(utm.zone, utm.grid_zone, grid_square, grid_easting, grid_northing, precision);
  }

  
  USNG
  USNG::fromUTM(const UTM &utm)
  {
    return USNG::fromUTM(utm, 4);
  }
  
  
  USNG
  USNG::fromUPS(const UPS &ups, int precision)
  {
    if (!ups.in_bounds())
      throw(std::invalid_argument("USNG: UPS only valid in zones A,B,Y & Z (North of 84N, South of 80S)"));

    char grid_zone;
    char grid_square[2];
    int grid_square_x_idx = floor((ups.x - 2000000.0) / 100000.0);
    int grid_square_y_idx = floor((ups.y - 2000000.0) / 100000.0);

    if(!ups.is_north) { // South grid_zones A & B
      if(grid_square_y_idx < 0)
	grid_square_y_idx += 24;

      if(grid_square_x_idx < 0) {
	grid_zone = 'A';
	grid_square_x_idx += 18;
      } else {
	grid_zone = 'B';
      }
      
      grid_square[0] = XLetters[grid_square_x_idx];
      grid_square[1] = YSLetters[grid_square_y_idx];
    } else { // North grid zones Y & Z
      if(grid_square_y_idx < 0)
	grid_square_y_idx += 14;

      if(grid_square_x_idx < 0) {
	grid_zone = 'Y';
	grid_square_x_idx += 18;
      } else {
	grid_zone = 'Z';
      }

      
      grid_square[0] = XLetters[grid_square_x_idx];
      grid_square[1] = YNLetters[grid_square_y_idx];
    }
    
    // Calc X and Y integer to 100,000s place
    double x = floor(fmod(ups.x, 100000.0));
    double y = floor(fmod(ups.y, 100000.0));

    return USNG(99, grid_zone, grid_square, x, y, precision);
  }

  USNG
  USNG::fromUPS(const UPS &ups)
  {
    return USNG::fromUPS(ups, 4);
  }

  USNG
  USNG::fromLL84(const LL84 &ll, int precision)
  {
    if ((ll.lat > 84.0) || (ll.lat < -80.0)) {
      return USNG::fromUPS(UPS(ll), precision);
    }
    return USNG::fromUTM(UTM(ll), precision);
  }

  USNG
  USNG::fromLL84(const LL84 &ll)
  {
    return USNG::fromLL84(ll, 4);
  }
  
  std::string
  USNG::toString() const
  {
    double factor = pow(10, precision - 5);
    int e = (int)floor(grid_easting * factor);
    int n = (int)floor(grid_northing * factor);

    std::ostringstream oss;

    if (utm_zone < 61) // Not in UPS grid_zones A,B,Y,Z
      oss << utm_zone;
    oss << grid_zone << " " << grid_square[0] << grid_square[1];
    if (precision > 0)
      oss << " "
	  << std::setw(precision) << std::setfill('0') << e << " "
    	  << std::setw(precision) << std::setfill('0') << n;

    return oss.str();
  }


  UTM
  USNG::toUTM(bool strict) const
  { 
    if(this->utm_zone > 60)
      throw(std::runtime_error("USNG: Invalid UTM zone"));

    auto gss = getGridSquareSet(this->utm_zone);
    const char *ns_grid = gss.ns_grid;
    const char *ew_grid = gss.ew_grid;
    const int ew_idx = std::find(ew_grid, ew_grid + 8, this->grid_square[0]) - ew_grid;
    const int ns_idx = std::find(ns_grid, ns_grid + 20, this->grid_square[1]) - ns_grid;
    
    if(ew_idx >= 8 || ns_idx >= 20) {
      std::ostringstream oss;
      oss << "USNG: Invalid USNG 100km grid designator for UTM zone " << this->utm_zone << ".";
      throw(std::runtime_error(oss.str()));
    }
    
    double utm_easting = ((ew_idx + 1) * 100000.0) + this->grid_easting; // Should be [100,000, 900,000]
    double utm_northing = ((ns_idx + 0) * 100000.0) + this->grid_northing; // Should be [0, 2,000,000)
    
    // TODO: this really depends on easting too...
    // At this point know UTM zone, Grid Zone (min latitude), and easting
    // Right now this is look up table returns a max number based on lon == utm zone center
    const int grid_zone_idx = std::find(GridZones, GridZones + 21, this->grid_zone) - GridZones;
    if(grid_zone_idx >= 21) {
      std::ostringstream oss;
      oss << "USNG: Invalid USNG UTM grid zone: " << this->grid_zone << ".";
      throw(std::runtime_error(oss.str()));
    }
    
    const double min_northing = GridZonesNorthing[grid_zone_idx]; // Unwrap northing to ~ [0, 10000000]
    utm_northing += 2000000 * ceil((min_northing - utm_northing) / 2000000);
    
    // Check that the coordinate is within the utm zone and grid zone specified:
    const char ll_grid_zone1 = UTM::calc_ll_grid_zone(
						LL84(
						     UTM(
							 this->utm_zone,
							 utm_easting,
							 utm_northing
							 )
						     )
						);
    // If error from the above TODO mattered... then need to move north a grid
    if( ll_grid_zone1 != this->grid_zone) {
      utm_northing -= 2000000;
    }

    // Verify UTM based USNG Coordinate is valid
    const LL84 ll(UTM(this->utm_zone, utm_easting, utm_northing));
    const char ll_grid_zone = UTM::calc_ll_grid_zone(ll);	  
    const unsigned int ll_utm_zone = UTM::calc_ll_utm_zone(ll);
    
    if(strict) {
      std::ostringstream oss;
      if(ll.lat > 84.0 || ll.lat < -80.0) {
	oss << "USNG: Latitude " << ll.lat << " outside valid UTM range.";
	throw(std::runtime_error(oss.str()));
      }
      if(ll_utm_zone != utm_zone) {
	std::ostringstream oss;
	oss << "USNG: calculated coordinate not in correct UTM zone! Supplied: "
	    << this->utm_zone << this->grid_zone
	    << " Calculated: " << ll_utm_zone << ll_grid_zone;
	throw(std::runtime_error(oss.str()));
      }
      if(ll_grid_zone != grid_zone) {
	std::ostringstream oss;
	oss << "USNG: calculated coordinate not in correct grid zone! Supplied: "
	    << this->utm_zone << this->grid_zone
	    << " Calculated: " << ll_utm_zone << ll_grid_zone;
	throw(std::runtime_error(oss.str()));
      }
    } else {
      // Loosen requirements to allow for grid extensions that don't introduce ambiguity.
      
      // "The UTM grid extends to 80°30'S and 84°30'N, providing a 30-minute overlap with the UPS grid."
      // -- http://earth-info.nga.mil/GandG/publications/tm8358.1/tr83581b.html Section 2-6.3.1
      if(ll.lat > 84.5 || ll.lat < -79.5) {
	std::ostringstream oss;
	oss << "USNG: Latitude " << ll.lat << " outside valid UTM range.";
	throw(std::runtime_error(oss.str()));
      }
      // 100km grids E-W unique +/- 2 UTM zones of the correct UTM zone.
      // 100km grids unique for 800,000m in one UTM zone.
      // Thus, two limiting conditions for uniqueness:
      //		UTM zone max width = 665,667m at equator => 800,000m is 1.2 UTM 6* zones wide at 0*N. => 67000m outside zone.
      //			=> utm_easting in [100,000, 900,000] (800,000m wide centered at 500,000m (false easting) 
      //		UTM zone min width = 63,801m at 84.5* N. => 12 UTM 6* zones.  => 2 UTM zones.
      if(utm_easting < 100000 || utm_easting > 900000) {
	std::ostringstream oss;
	oss << "USNG: calculated coordinate not in correct UTM zone! Supplied: "
	    << this->utm_zone << this->grid_zone
	    << " Calculated: " << ll_utm_zone << ll_grid_zone;
	throw(std::runtime_error(oss.str()));
      }
      int utm_zone_diff = abs(ll_utm_zone - this->utm_zone);
      if(utm_zone_diff > 2 && utm_zone_diff < 58) { // utm_zone wraps 1..60,1
	std::ostringstream oss;
	oss << "USNG: calculated coordinate not in correct UTM zone! Supplied: "
	    << utm_zone << grid_zone
	    << " Calculated: " << ll_utm_zone << ll_grid_zone;
	throw(std::runtime_error(oss.str()));
      }
      // 100km grids N-S unique +/- 2,000,000 meters
      // A grid zone is roughly 887,570 meters N-S
      // => unique +/- 1 grid zone.
      int ll_idx = std::find(NSLetters135, NSLetters135 + 20, ll_grid_zone) - NSLetters135; // 135 or 246 doesn't matter
      int gz_idx = std::find(NSLetters135, NSLetters135 + 20, grid_zone) - NSLetters135;    // letters in same order and circular subtraction.
      int gz_diff = abs(ll_idx - gz_idx);
      if(gz_diff > 1 && gz_diff < 19) {
	std::ostringstream oss;
	oss << "USNG: calculated coordinate not in correct grid zone! Supplied: "
	    << utm_zone << grid_zone
	    << " Calculated: " << ll_utm_zone << ll_grid_zone;
	throw(std::runtime_error(oss.str()));
      }
    }
    
    return UTM(this->utm_zone, utm_easting, utm_northing);
  }


  USNG::operator UTM() const
  {
    return this->toUTM();
  }
  
  
  UPS
  USNG::toUPS() const
  {
    /* Start at the pole */
    double ups_x = 2000000;
    double ups_y = 2000000;
    bool is_north = false;
    
    /* Offset based on 100km grid square */
    int x_idx = std::find(XLetters, XLetters + 18, grid_square[0]) - XLetters;
    if(x_idx < 0 || x_idx >= 18)
      throw(std::runtime_error("USNG: Invalid grid square."));
    
    int y_idx;
    switch(grid_zone) {
    case 'A': // South West half-hemisphere
      x_idx = x_idx - 18;
    case 'B': // South East half-hemisphere
      y_idx = std::find(YSLetters, YSLetters + 24, grid_square[1]) - YSLetters;
      if(x_idx < -12 || x_idx > 11 || y_idx >= 24 )
	throw(std::runtime_error("USNG: Invalid grid square."));
      
      if(y_idx > 11)
	y_idx = y_idx - 24;
      break;
      
    case 'Y': // North West half-hemisphere
      x_idx = x_idx - 18;
    case 'Z': // North East half-hemisphere
      y_idx = std::find(YNLetters, YNLetters + 14, grid_square[1]) - YNLetters;
      if(x_idx < -7 || x_idx > 6 || y_idx >= 14)
	throw(std::runtime_error("USNG: Invalid grid square."));
      
      if(y_idx > 6)
	y_idx = y_idx - 14;

      is_north = true;
      break;
      
    default:
      throw(std::runtime_error("UPS only valid in zones A, B, Y, and Z" ));
    };

    ups_x += x_idx * 100000.0;
    ups_y += y_idx * 100000.0;

    /* Offset based on grid_x,y */
    ups_x += this->grid_easting;
    ups_y += this->grid_northing;

    UPS ups(is_north, ups_x, ups_y);
    
    // Check that the coordinate is within the ups zone and grid zone specified:
    LL84 ll(ups);
    if (grid_zone == 'A' || grid_zone == 'B') {
      if (ll.lat > -80.0)
	throw(std::runtime_error("USNG: Grid Zone A or B but Latitude > -80."));
    } else {
      if (ll.lat < 84.0)
	throw(std::runtime_error("USNG: Grid Zone Y or Z but Latitude < 84."));
    }
    return ups;
  }


  USNG::operator UPS() const
  {
    return this->toUPS();
  }


  
  LL84
  USNG::toLL84() const
  {
    if (this->grid_zone == 'A' || this->grid_zone == 'B' || this->grid_zone == 'Y' || this->grid_zone == 'Z')
      return LL84(this->toUPS());
    else
      return LL84(this->toUTM());
  }


  USNG::operator LL84() const
  {
    return this->toLL84();
  }

  
} /* Namespace USNG2 */

