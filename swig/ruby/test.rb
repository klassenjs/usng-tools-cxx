#!/usr/bin/ruby
require './USNGC'

def benchmark(name, cnt = 1000000)
	print "Running #{name} #{cnt} times.\n"
	start = Time.now()
  puts yield
	(1..cnt).each do
		yield
	end
	tend = Time.now()
	p tend - start
end

def test_coordinate(lat, lon)
	ll = USNGC::LL84.new(lat, lon)
	print "LL84( #{lat}, #{lon} )\n\t"
	print "toD(): #{ll.toD()}\n\t"
	print "toDM(): #{ll.toDM()}\n\t"
	print "toDMS(): #{ll.toDMS()}\n\t"
	
	usng = USNGC::USNG.fromLL84(ll)
	print "USNG(ll): #{usng.toString()}\n\t"
	print "LL84(usng): #{usng.toLL84.toD()}\n\t"
	print "\n"
rescue StandardError => e
	print "#{e.to_s}\n"
	print "\n"
end

def test_usng_parse(s)
	print "USNG::fromString( #{s} )\n\t"
	u = USNGC::USNG.fromString(s)
	print "USNG: #{u.toString()}\n\t"
	print "LL84: #{u.toLL84.toD()}\n\t"
	print "LL84: #{u.toLL84.toDM()}\n\t"
	print "\n"
rescue StandardError => e
	print "#{e.to_s}\n"
	print "\n"
end


def test_usng_parse2(s, lat, lon)
	print "USNG::fromString( #{s}, LL84(#{lat}, #{lon}) )\n\t"
	u = USNGC::USNG.fromString(s, USNGC::LL84.new(lat, lon))
  print "USNG: #{u.toString()}\n\t"
  print "LL84: #{u.toLL84.toD()}\n\t"
  print "LL84: #{u.toLL84.toDM()}\n\t"
	print "\n"
rescue StandardError => e
	print "#{e.to_s}\n"
	print "\n"
end

begin
  test_coordinate(45, 267.11)

  test_coordinate(70, 40)
  test_coordinate(-70, 40)
  test_coordinate(70, -40)
  test_coordinate(-70, -40)
  
  test_coordinate(84.1, 40)
  test_coordinate(-80.1, 40)
  test_coordinate(84.1, -40)
  test_coordinate(-80.1, -40)
  
  test_coordinate(88, 40)
  test_coordinate(-88, 40)
  test_coordinate(88, -40)
  test_coordinate(-88, -40)
  
  test_coordinate(0,0)
  test_coordinate(44.876, -93.12456789)
  test_coordinate(38.894, -77.043) # "18S UJ 22821 06997"
  
  test_usng_parse("15t vk 1234 5678")
  test_usng_parse2("vk 1234 5678", 44, -93)
  test_usng_parse2("1234 5678", 44, -93)
  test_usng_parse2("vk", 44, -93)
  test_usng_parse("15t vk")
  test_usng_parse("A VK 0 0") # invalid
  
  test_usng_parse("18S UJ 228 070") #  lon: -77.04324684425941, lat: 38.8940174428622, precision: 3
  test_usng_parse2("UJ 228 070", 38.894, -77.043) # same
  test_usng_parse2("228 070", 38.894, -77.043) # same
  test_usng_parse("B AN") # lon: 0, lat: -90, precision: 0
  test_usng_parse("Y ZP 12345 12345") # lon: -171.85365493260602, lat: 84.43254784831868, precision: 5


	benchmark "ruby_c" do
		usng = USNGC::fromll84( -93.124, 44.876 )
	end

	benchmark "ruby_c++" do
		usng = USNGC::USNG.fromLL84(USNGC::LL84.new(44.876,-93.124)).toString()
	end

	benchmark "ruby_c++2" do
		usng = USNGC::USNG.fromLL84(USNGC::LL84.new(44.876,-93.124),4).toString()
	end
end
