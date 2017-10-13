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

extern "C" {
#include "postgres.h"
#include "fmgr.h"
#include "utils/geo_decls.h"
}

#include <stdexcept>
#include <usng2.h>

extern "C" {

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

// Define prototype
Datum usng2fromll84_(PG_FUNCTION_ARGS);
Datum usng2toll84_(PG_FUNCTION_ARGS);

// API Macros
PG_FUNCTION_INFO_V1(usng2fromll84_);
PG_FUNCTION_INFO_V1(usng2toll84_);

// Functions

Datum
usng2fromll84_(PG_FUNCTION_ARGS)
{
	float8 lon;
	float8 lat;
  int32  precision;
	int32 usng_len;
	text* new_t;

	// Check for NULLs (not needed because strict, but safer)
	if (PG_ARGISNULL(0) || PG_ARGISNULL(1))
		PG_RETURN_NULL();

	// Get arguments.
	lon = PG_GETARG_FLOAT8(0);
	lat = PG_GETARG_FLOAT8(1);
	
	if (PG_ARGISNULL(2))
		precision = 4;
	else
		precision = PG_GETARG_INT32(2);

	// Get USNG as owned char*
	try {
		std::string s = USNG2::USNG::fromLL84(USNG2::LL84(lat, lon), precision).toString();
		usng_len = s.length();

		// Allocate a zeroed text object
		new_t = (text *) palloc0(VARHDRSZ + usng_len);

		// Return NULL on failure
		if (new_t == NULL) {
			PG_RETURN_NULL();
		}

		// Set the size and bytes of the text object
		SET_VARSIZE(new_t, VARHDRSZ + usng_len);
		memcpy((void *) VARDATA(new_t), s.c_str(), usng_len);
	} catch (const std::exception &e)	{
		ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("usng exception: %s", e.what())));
	} catch (...) {
		ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("usng: unknown error")));
		PG_RETURN_NULL();
	}	

	// Return the text object
	PG_RETURN_TEXT_P(new_t);
}


Datum
usng2toll84_(PG_FUNCTION_ARGS)
{
	float8 lat = 0.0;
	float8 lon = 0.0;
	text* new_t;

	// Check for NULLs (not needed because strict, but safer)
	if (PG_ARGISNULL(0))
		PG_RETURN_NULL();

	// Get arguments.
	new_t = PG_GETARG_TEXT_P(0);
	
	Point *new_point = (Point *) palloc(sizeof(Point));
	if (new_point == NULL)
		PG_RETURN_NULL();

	try {
		std::string s((char*)VARDATA(new_t), (size_t)VARSIZE(new_t)-VARHDRSZ);
	
		//ereport(INFO, (errcode(ERRCODE_SUCCESSFUL_COMPLETION), errmsg("usng exception: %s", s.c_str())));

		if (!PG_ARGISNULL(1) && !PG_ARGISNULL(2)) {
			lon = PG_GETARG_FLOAT8(1);
			lat = PG_GETARG_FLOAT8(2);
		}

		auto u = (PG_ARGISNULL(1) || PG_ARGISNULL(2))?
			USNG2::USNG::fromString(s) :
			USNG2::USNG::fromString(s, USNG2::LL84(lat, lon));

		auto ll = u.toLL84();

		new_point->x = ll.lon;
		new_point->y = ll.lat;
	} catch (const std::exception &e)	{
		ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("usng exception: %s", e.what())));
	} catch (...) {
		ereport(ERROR, (errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("usng: unknown error")));
		PG_RETURN_NULL();
	}

	PG_RETURN_POINT_P(new_point);
}

}
