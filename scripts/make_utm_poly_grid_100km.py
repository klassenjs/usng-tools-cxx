import os, sys
from osgeo import gdal
from osgeo import ogr
from osgeo import osr
import string
import USNGC

GridZones = [ 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'X' ]

def mk100km(u100kmSq, grid_size=100000):
	usng100kmSq = USNGC.USNG.fromString(u100kmSq,False)
	utmZone = usng100kmSq.toUTM().zone
	gridZone = u100kmSq[2] #usng100kmSq.toUTM().grid_zone
	min_northing = int(usng100kmSq.toUTM().northing)
	min_easting = int(usng100kmSq.toUTM().easting)

	min_latitude = -80 + (GridZones.index(gridZone) * 8)

	baseLon = -180 + (6 * (utmZone - 1))

	utmSpatialRef = osr.SpatialReference()
	utmSpatialRef.ImportFromEPSG(26900 + utmZone)

	llSpatialRef = osr.SpatialReference()
	llSpatialRef.ImportFromEPSG(4269)

	utmTollTrans = osr.CoordinateTransformation(utmSpatialRef, llSpatialRef)
	llToutmTrans = osr.CoordinateTransformation(llSpatialRef, utmSpatialRef)

	utmZoneBoundaryRing = ogr.Geometry(ogr.wkbLinearRing)
	utmZoneBoundaryRing.AddPoint(baseLon, min_latitude)
	utmZoneBoundaryRing.AddPoint(baseLon, min_latitude+8)
	utmZoneBoundaryRing.AddPoint(baseLon+6, min_latitude+8)
	utmZoneBoundaryRing.AddPoint(baseLon+6, min_latitude)
	utmZoneBoundaryRing.CloseRings()

	utmZoneBoundary = ogr.Geometry(ogr.wkbPolygon)
	utmZoneBoundary.AddGeometry(utmZoneBoundaryRing)

	driverName = "ESRI Shapefile"
	drv = ogr.GetDriverByName( driverName )
	if drv is None:
		print "%s driver not available.\n" % driverName
		sys.exit( 1 )


	outputShapefile = "%s_100km_grid_poly.shp" % u100kmSq
	if os.path.exists(outputShapefile):
		drv.DeleteDataSource(outputShapefile)
	ds = drv.CreateDataSource(outputShapefile)
	if ds is None:
		print "Creation of output file failed.\n"
		sys.exit( 1 )

	lyr = ds.CreateLayer( "point_out", llSpatialRef, ogr.wkbPolygon )
	if lyr is None:
		print "Layer creation failed.\n"
		sys.exit( 1 )

	field_defn = ogr.FieldDefn( "USNG", ogr.OFTString )
	field_defn.SetWidth( 32 )
	if lyr.CreateField ( field_defn ) != 0:
		print "Creating USNG field failed.\n"
		sys.exit( 1 )

	for northing in range(min_northing, min_northing+100000, grid_size):
		print "%s %3d%% complete" % (u100kmSq, (100 * (northing-min_northing) / 100000))
		for easting in range(min_easting, min_easting+100000, grid_size):
			ring = ogr.Geometry(ogr.wkbLinearRing)
			ring.AddPoint(easting     , northing)
			ring.AddPoint(easting+grid_size, northing)
			ring.AddPoint(easting+grid_size, northing+grid_size)
			ring.AddPoint(easting     , northing+grid_size)
			ring.CloseRings()

			geom = ogr.Geometry(ogr.wkbPolygon)
			geom.AddGeometry(ring)

			geom.Transform(utmTollTrans)
			geom2 = geom.Intersection(utmZoneBoundary)

			utm = USNGC.UTM(utmZone, easting, northing)
			usng = USNGC.USNG.fromUTM(utm, 3)

			if (geom2 != None and geom2.GetArea() > 0):
				feat = ogr.Feature(lyr.GetLayerDefn())
				feat.SetField("USNG", u100kmSq)
				feat.SetGeometryDirectly(geom2)
				if lyr.CreateFeature(feat) != 0:
					print "Failed to create feature in shapefile.\n"
					sys.exit( 1 )

				feat.Destroy()

	ds = None

def tryMk100km(grid):
	try:
		mk100km(grid, 100000)
	except RuntimeError,e:
		print(e)


grids = []
for utm_zone in range(10, 20):
	for grid_zone in "RSTU":
		for e in "ABCDEFGHJKLMNPQRSTUVWXYZ":
			for n in "ABCDEFGHJKLMNPQRSTUVWXYZ":
				print(str(utm_zone)+grid_zone+e+n)
				grids.append(str(utm_zone)+grid_zone+e+n)

map(tryMk100km, grids)

# for i in *100km*.shp ; do ogr2ogr -append -f PostgreSQL PG: . -nln usng100km -t_srs EPSG:4269 ${i%%.shp}; done
# select distinct replace(substring(usng,1,6), ' ', '') from usng100km, covid19.cb_2015_us_state_500k where st_intersects(usng100km.wkb_geometry, cb_2015_us_state_500k.wkb_geometry) order by 1;
# select distinct 'mkdir -p '||stusps||' && ln '||replace(substring(usng,1,6), ' ', '')||'* '||stusps from usng100km, covid19.cb_2015_us_state_500k where st_intersects(usng100km.wkb_geometry, cb_2015_us_state_500k.wkb_geometry) order by 1;
