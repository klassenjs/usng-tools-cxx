#!/usr/bin/ruby
require './USNGC'


def get_pt(easting, northing)
	base_coordinate = "15TVK00000000"
	#base_coordinate = "15WVS00000000"
	u = USNGC::USNG.fromString(base_coordinate)
	utm = u.toUTM()
	utm = USNGC::UTM.new(utm.zone, utm.easting + easting, utm.northing + northing)
	return utm.toLL84()
end


def plot_pt(easting, northing)
	ll = get_pt(easting, northing)
	u2 = USNGC::USNG.fromLL84(ll)

	print "\"POINT(#{ll.lon} #{ll.lat})\", \"#{u2.toString()}\"\n"
end


begin
	de = 1
	dn = 0

	while(de < 100000)
		plot_pt(de, dn)
		de = de + 10
  end
	while(dn < 100000)
		plot_pt(de, dn)
		dn = dn + 10
	end
	while(de >= 10)
		plot_pt(de, dn)
		de = de - 10
	end
	while(dn >= 0)
		plot_pt(de, dn)
		dn = dn - 10
	end

	ll_1 = get_pt(0,0)
	ll_2 = get_pt(100000,0)
	ll_3 = get_pt(100000,100000)
	ll_4 = get_pt(0,100000)
	print "\"POLYGON ((#{ll_1.lon} #{ll_1.lat}, #{ll_2.lon} #{ll_2.lat}, #{ll_3.lon} #{ll_3.lat}, #{ll_4.lon} #{ll_4.lat}, #{ll_1.lon} #{ll_1.lat} ))\",  \"15TVK00000000\"\n"

end
