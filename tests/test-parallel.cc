/* 
 * File: test.cc: Test program for USNG2 C++ library.
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

#include "usng2.h"
#include <iostream>
#include <chrono>
#include <stdexcept>
#include <future>
#include <vector>

using namespace USNG2;


void test_coordinate(double lat, double lon)
{
  try {
    LL84 ll(lat, lon);
    std::cout << "LL84( " << lat << ", " << lon << " ) = \n\t";
    std::cout << "toD(): " << ll.toD() << "\n\t";
    std::cout << "toDM(): " << ll.toDM() << "\n\t";
    std::cout << "toDMS(): " << ll.toDMS() << "\n\t";

    if( lat > 84.0 || lat < -80.0) {
      UPS ups(ll);
      std::cout << "UPS(ll): " << ups.toString() << "\n\t";
      std::cout << "LL84(ups): " << LL84(ups).toD() << "\n\t";
    }
    if ( lat < 84.5 && lat > -80.5) {
      UTM utm(ll);
      std::cout << "UTM(ll): " << utm.toString() << "\n\t";
      std::cout << "LL84(utm): " << LL84(utm).toD() << "\n\t";
    }

    USNG usng = USNG::fromLL84(ll);
    std::cout << "USNG(ll): " << usng.toString() << "\n\t";
    std::cout << "LL84(usng): " << LL84(usng).toD() << "\n\t";
    std::cout << std::endl;
  } catch (std::invalid_argument& e) {
    std::cout << e.what() << std::endl << std::endl;
  } catch (std::runtime_error& e) {
    std::cout << e.what() << std::endl << std::endl;
  }
}



void test_usng_parse(const std::string &s)
{
  try {
    std::cout << "USNG::parseString( " << s << " ) = \n\t";
    USNG u = USNG::fromString(s);
    std::cout << "USNG: " << u.toString() << "\n\t";
    std::cout << "LL84: " << u.toLL84().toD() << "\n\t";
    std::cout << "LL84: " << u.toLL84().toDM() << std::endl << std::endl;
  } catch (std::invalid_argument& e) {
    std::cout << e.what() << std::endl << std::endl;
  } catch (std::runtime_error& e) {
    std::cout << e.what() << std::endl << std::endl;
  }
}


void test_usng_parse(const std::string &s, double lat, double lon)
{
  try {
    std::cout << "USNG::parseString( " << s << ", " << lat << ", " << lon << " ) = \n\t";
    USNG u = USNG::fromString(s, LL84(lat, lon));
    std::cout << "USNG: " << u.toString() << "\n\t";
    std::cout << "LL84: " << u.toLL84().toD() << "\n\t";
    std::cout << "LL84: " << u.toLL84().toDM() << std::endl << std::endl;
  } catch (std::invalid_argument& e) {
    std::cout << e.what() << std::endl << std::endl;
  } catch (std::runtime_error& e) {
    std::cout << e.what() << std::endl << std::endl;
  }
}


bool benchmark(const std::string name, int iterations, std::string lambda(int))
{
  //try {
    std::cout << "Benchmarking " << name << " (" << iterations << ") = "
	      << lambda(0) << std::endl;

		std::vector<std::future<std::string>> f(8);
		int num_per_thread = iterations / 8;
		
    auto clock_start = std::chrono::system_clock::now();

		int i = 0;
		for(int t = 0; t < 8; t++) {
				f[t] = std::async(std::launch::async, [&lambda,&iterations,&num_per_thread](int i)->std::string
					{
						for(int j = 0; j < num_per_thread && i+j < iterations; i++, j++) {
							lambda(i+j);
						}
						return(lambda(i));
					}, i);
			i += num_per_thread;
    }
		for(auto &future : f) {
			std::cout << future.get() << std::endl;
		}
    auto clock_end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = clock_end - clock_start;
    std::cout << elapsed_seconds.count() << std::endl;
  //} catch (...) {
  //  return true;
  //}
  return false;
}

int main(int argc, char** argv)
{
  test_coordinate(44.999995, -93.0);
  test_coordinate(45, 267.11);

  test_coordinate(70, 40);
  test_coordinate(-70, 40);
  test_coordinate(70, -40);
  test_coordinate(-70, -40);
  
  test_coordinate(84.1, 40);
  test_coordinate(-80.1, 40);
  test_coordinate(84.1, -40);
  test_coordinate(-80.1, -40);
  
  test_coordinate(88, 40);
  test_coordinate(-88, 40);
  test_coordinate(88, -40);
  test_coordinate(-88, -40);
  
  test_coordinate(0,0);
  test_coordinate(44.876, -93.12456789);
  test_coordinate(38.894, -77.043); // "18S UJ 22821 06997"
  
  test_usng_parse("15t vk 1234 5678");
  test_usng_parse("vk 1234 5678", 44, -93);
  test_usng_parse("1234 5678", 44, -93);
  test_usng_parse("vk", 44, -93);
  test_usng_parse("15t vk");
  test_usng_parse("A VK 0 0"); // invalid
  
  test_usng_parse("18S UJ 228 070"); //  lon: -77.04324684425941, lat: 38.8940174428622, precision: 3
  test_usng_parse("UJ 228 070", 38.894, -77.043); // same
  test_usng_parse("228 070", 38.894, -77.043); // same
  test_usng_parse("B AN"); // lon: 0, lat: -90, precision: 0
  test_usng_parse("Y ZP 12345 12345"); // lon: -171.85365493260602, lat: 84.43254784831868, precision: 5
  
  
  
  USNG usng2 = USNG::fromLL84(LL84(44.876, -93.124));
  std::cout << usng2.toString() << std::endl;
	
  auto clock_start = std::chrono::system_clock::now();

  for(int i = 0; i < 5000000; i++) {
    //usng2 = USNG(UTM(LL84(44.876 + (i/1000000.0), -93.124)));
    auto s = USNG::fromLL84(LL84(44.876 + (i/5000000.0), -93.124)).toString();
  }
  //std::cout << usng2.toString(4) << std::endl;
  auto clock_end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = clock_end - clock_start;
  std::cout << elapsed_seconds.count() << std::endl;
  std::cout << usng2.toString() << std::endl;

  benchmark("USNG::fromLL84", 5000000,
	    [](int i)->std::string
	    {
	      return USNG::fromLL84(LL84(44.876 + (i/5000000.0), -93.124)).toString();
	    }
	  );

  return 0;
}
