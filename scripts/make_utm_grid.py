import sys
from osgeo import gdal
from osgeo import ogr
import string

driverName = "ESRI Shapefile"
drv = ogr.GetDriverByName( driverName )
if drv is None:
    print "%s driver not available.\n" % driverName
    sys.exit( 1 )
    
ds = drv.CreateDataSource( "grid_utm.shp" )
if ds is None:
    print "Creation of output file failed.\n"
    sys.exit( 1 )
    
lyr = ds.CreateLayer( "point_out", None, ogr.wkbLineString )
if lyr is None:
    print "Layer creation failed.\n"
    sys.exit( 1 )
    
field_defn = ogr.FieldDefn( "UTMGrid", ogr.OFTInteger )
field_defn.SetWidth( 9 )
if lyr.CreateField ( field_defn ) != 0:
    print "Creating Name field failed.\n"
    sys.exit( 1 )

field_defn = ogr.FieldDefn( "NG1K2", ogr.OFTString )
field_defn.SetWidth( 2 )
if lyr.CreateField ( field_defn ) != 0:
    print "Creating Name field failed.\n"
    sys.exit( 1 )

field_defn = ogr.FieldDefn( "NG1K", ogr.OFTString )
field_defn.SetWidth( 1 )
if lyr.CreateField ( field_defn ) != 0:
    print "Creating Name field failed.\n"
    sys.exit( 1 )

field_defn = ogr.FieldDefn( "NG10K", ogr.OFTString )
field_defn.SetWidth( 1 )
if lyr.CreateField ( field_defn ) != 0:
    print "Creating Name field failed.\n"
    sys.exit( 1 )

field_defn = ogr.FieldDefn( "NGPRE", ogr.OFTInteger )
field_defn.SetWidth( 2 )
if lyr.CreateField ( field_defn ) != 0:
    print "Creating Name field failed.\n"
    sys.exit( 1 )

field_defn = ogr.FieldDefn( "CODE", ogr.OFTInteger )
field_defn.SetWidth( 4 )
if lyr.CreateField ( field_defn ) != 0:
    print "Creating Name field failed.\n"
    sys.exit( 1 )
    
    
field_defn = ogr.FieldDefn( "Direction", ogr.OFTString )
field_defn.SetWidth( 2 )

if lyr.CreateField ( field_defn ) != 0:
    print "Creating Direction field failed.\n"
    sys.exit( 1 )


# Make horizontal lines
for northing in range(0, 8884000, 1000):
    feat = ogr.Feature(lyr.GetLayerDefn())

    northing_str = "%07d" % northing
    code = 0
    if (northing_str[3:4] == "0"):
        code = 1
    
    feat.SetField("UTMGrid", northing)
    feat.SetField("NG1K2", northing_str[2:4])
    feat.SetField("NG1K", northing_str[3:4])
    feat.SetField("NG10K", northing_str[2:3])
    feat.SetField("NGPRE", northing_str[0:2])
    feat.SetField("CODE", code)    
    feat.SetField("Direction", "EW")
    geom = ogr.Geometry(ogr.wkbLineString)
    for easting in range(166000, 834000, 1000):
        geom.AddPoint(easting, northing)

    feat.SetGeometryDirectly(geom)
    if lyr.CreateFeature(feat) != 0:
        print "Failed to create feature in shapefile.\n"
        sys.exit( 1 )
        
    feat.Destroy()

# Make vertical lines
for easting in range(166000, 834000, 1000):
    feat = ogr.Feature(lyr.GetLayerDefn())

    easting_str = "%07d" % easting
    code = 0
    if (easting_str[3:4] == "0"):
        code = 1
        
    feat.SetField("UTMGrid", easting)
    feat.SetField("NG1K2", easting_str[2:4])
    feat.SetField("NG1K", easting_str[3:4])
    feat.SetField("NG10K", easting_str[2:3])
    feat.SetField("NGPRE", easting_str[0:2])
    feat.SetField("CODE", code)    
    feat.SetField("Direction", "NS")
    geom = ogr.Geometry(ogr.wkbLineString)
    
    for northing in range(0, 8884000, 1000):
        geom.AddPoint(easting, northing)
        
    feat.SetGeometryDirectly(geom)
    if lyr.CreateFeature(feat) != 0:
        print "Failed to create feature in shapefile.\n"
        sys.exit( 1 )
        
    feat.Destroy()

ds = None
