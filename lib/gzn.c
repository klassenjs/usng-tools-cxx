/* 
 * File: gzn.c
 *
 * This is a program to generate the values for the GridZonesNorthing
 * array in usng2.cc.  This is not needed for the build process as the
 * values are in the source file, but left here for completeness.
 *
 * Copyright (c) 2008-2016 James Klassen
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the “Software”), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies of this Software or works derived from this Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include <stdio.h>
#include <math.h>

inline double gzn(double deg)
{
	return  2 * M_PI * 6356752.3 * (deg / 360.0);
}


int main(int argc, char** argv)
{
	double northing;	
	int deg;

	printf("{");
	for(deg = -80; deg < 80; deg += 8) {
		printf( "%lf, " , gzn((double)deg) );
	}
	printf("}\n");

	return 0;
}
