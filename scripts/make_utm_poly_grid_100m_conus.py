import os, sys
import multiprocessing as mp
from osgeo import gdal
from osgeo import ogr
from osgeo import osr
import string
import USNGC

GridZones = [ 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'X' ]

def mk100m(u100kmSq):
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


	outputShapefile = "conus-100m/%s_100m_grid_poly.shp" % u100kmSq
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

	print "Starting " + u100kmSq
	for northing in range(min_northing, min_northing+100000, 100):
		#print "%s %3d%% complete" % (u100kmSq, (100 * (northing-min_northing) / 100000))
		for easting in range(min_easting, min_easting+100000, 100):
			ring = ogr.Geometry(ogr.wkbLinearRing)
			ring.AddPoint(easting     , northing)
			ring.AddPoint(easting+100, northing)
			ring.AddPoint(easting+100, northing+100)
			ring.AddPoint(easting     , northing+100)
			ring.CloseRings()

			geom = ogr.Geometry(ogr.wkbPolygon)
			geom.AddGeometry(ring)

			geom.Transform(utmTollTrans)
			geom2 = geom.Intersection(utmZoneBoundary)

			#utm = USNGC.UTM(utmZone, easting, northing)
			#usng = USNGC.USNG.fromUTM(utm, 3)
			usng = u100kmSq + (str(easting % 100000))[0:3] + (str(northing % 100000))[0:3]
			if (geom2 != None and geom2.GetArea() > 0):
				feat = ogr.Feature(lyr.GetLayerDefn())
				feat.SetField("USNG", usng)
				feat.SetGeometryDirectly(geom2)
				if lyr.CreateFeature(feat) != 0:
					print "Failed to create feature in shapefile.\n"
					sys.exit( 1 )

				feat.Destroy()
	print "Done " + u100kmSq
	ds = None

grids=[
		"10SDG",
		"10SDH",
		"10SDJ",
		"10SDK",
		"10SEF",
		"10SEG",
		"10SEH",
		"10SEJ",
		"10SEK",
		"10SFD",
		"10SFE",
		"10SFF",
		"10SFG",
		"10SFH",
		"10SFJ",
		"10SFK",
		"10SGC",
		"10SGD",
		"10SGE",
		"10SGF",
		"10SGG",
		"10SGH",
		"10SGJ",
		"10SGJ",
		"10SGK",
		"10SGK",
		"10TCK",
		"10TCL",
		"10TCM",
		"10TCM",
		"10TCN",
		"10TCP",
		"10TCT",
		"10TCU",
		"10TDK",
		"10TDL",
		"10TDM",
		"10TDM",
		"10TDN",
		"10TDP",
		"10TDQ",
		"10TDR",
		"10TDS",
		"10TDS",
		"10TDT",
		"10TDU",
		"10TEK",
		"10TEL",
		"10TEM",
		"10TEM",
		"10TEN",
		"10TEP",
		"10TEQ",
		"10TER",
		"10TER",
		"10TES",
		"10TES",
		"10TET",
		"10TEU",
		"10TFK",
		"10TFL",
		"10TFM",
		"10TFM",
		"10TFN",
		"10TFP",
		"10TFQ",
		"10TFR",
		"10TFR",
		"10TFS",
		"10TFT",
		"10TFU",
		"10TGK",
		"10TGL",
		"10TGM",
		"10TGM",
		"10TGN",
		"10TGP",
		"10TGQ",
		"10TGR",
		"10TGR",
		"10TGS",
		"10TGT",
		"10TGU",
		"10UCU",
		"10UDU",
		"10UDV",
		"10UEU",
		"10UEV",
		"10UFU",
		"10UFV",
		"10UGU",
		"10UGV",
		"11SKA",
		"11SKB",
		"11SKC",
		"11SKC",
		"11SKD",
		"11SKD",
		"11SKE",
		"11SKE",
		"11SKS",
		"11SKT",
		"11SKU",
		"11SKV",
		"11SLA",
		"11SLB",
		"11SLB",
		"11SLC",
		"11SLC",
		"11SLD",
		"11SLE",
		"11SLS",
		"11SLT",
		"11SLU",
		"11SLV",
		"11SMA",
		"11SMA",
		"11SMB",
		"11SMB",
		"11SMC",
		"11SMD",
		"11SME",
		"11SMR",
		"11SMS",
		"11SMT",
		"11SMU",
		"11SMV",
		"11SNA",
		"11SNA",
		"11SNB",
		"11SNC",
		"11SND",
		"11SNE",
		"11SNS",
		"11SNT",
		"11SNU",
		"11SNV",
		"11SNV",
		"11SPA",
		"11SPB",
		"11SPC",
		"11SPD",
		"11SPE",
		"11SPS",
		"11SPT",
		"11SPU",
		"11SPU",
		"11SPV",
		"11SPV",
		"11SQA",
		"11SQA",
		"11SQA",
		"11SQB",
		"11SQB",
		"11SQC",
		"11SQC",
		"11SQD",
		"11SQD",
		"11SQE",
		"11SQE",
		"11SQR",
		"11SQS",
		"11SQS",
		"11SQT",
		"11SQT",
		"11SQU",
		"11SQU",
		"11SQU",
		"11SQV",
		"11SQV",
		"11TKE",
		"11TKE",
		"11TKF",
		"11TKF",
		"11TKG",
		"11TKG",
		"11TKG",
		"11TKH",
		"11TKJ",
		"11TKK",
		"11TKL",
		"11TKL",
		"11TKM",
		"11TKN",
		"11TKP",
		"11TLE",
		"11TLF",
		"11TLG",
		"11TLG",
		"11TLH",
		"11TLJ",
		"11TLK",
		"11TLL",
		"11TLL",
		"11TLM",
		"11TLN",
		"11TLP",
		"11TME",
		"11TMF",
		"11TMG",
		"11TMG",
		"11TMG",
		"11TMH",
		"11TMH",
		"11TMJ",
		"11TMJ",
		"11TMK",
		"11TMK",
		"11TML",
		"11TML",
		"11TMM",
		"11TMM",
		"11TMN",
		"11TMN",
		"11TMP",
		"11TMP",
		"11TNE",
		"11TNF",
		"11TNG",
		"11TNG",
		"11TNH",
		"11TNJ",
		"11TNJ",
		"11TNK",
		"11TNK",
		"11TNL",
		"11TNL",
		"11TNL",
		"11TNM",
		"11TNM",
		"11TNN",
		"11TNN",
		"11TNP",
		"11TNP",
		"11TPE",
		"11TPF",
		"11TPG",
		"11TPG",
		"11TPH",
		"11TPJ",
		"11TPK",
		"11TPL",
		"11TPL",
		"11TPM",
		"11TPM",
		"11TPN",
		"11TPN",
		"11TPP",
		"11TQE",
		"11TQE",
		"11TQF",
		"11TQF",
		"11TQG",
		"11TQG",
		"11TQG",
		"11TQH",
		"11TQJ",
		"11TQK",
		"11TQL",
		"11TQL",
		"11TQM",
		"11TQM",
		"11TQN",
		"11TQP",
		"11UKP",
		"11UKQ",
		"11ULP",
		"11ULQ",
		"11UMP",
		"11UMP",
		"11UMQ",
		"11UMQ",
		"11UNP",
		"11UNP",
		"11UNQ",
		"11UNQ",
		"11UPP",
		"11UPQ",
		"11UQP",
		"11UQQ",
		"12RTA",
		"12RUA",
		"12RVA",
		"12RVV",
		"12RWA",
		"12RWV",
		"12RXA",
		"12RXA",
		"12RXV",
		"12RXV",
		"12RYA",
		"12RYV",
		"12STA",
		"12STB",
		"12STC",
		"12STD",
		"12STE",
		"12STF",
		"12STF",
		"12STG",
		"12STH",
		"12STJ",
		"12STK",
		"12SUA",
		"12SUB",
		"12SUC",
		"12SUD",
		"12SUE",
		"12SUF",
		"12SUF",
		"12SUG",
		"12SUH",
		"12SUJ",
		"12SUK",
		"12SVA",
		"12SVB",
		"12SVC",
		"12SVD",
		"12SVE",
		"12SVF",
		"12SVF",
		"12SVG",
		"12SVH",
		"12SVJ",
		"12SVK",
		"12SWA",
		"12SWB",
		"12SWC",
		"12SWD",
		"12SWE",
		"12SWF",
		"12SWF",
		"12SWG",
		"12SWH",
		"12SWJ",
		"12SWK",
		"12SXA",
		"12SXA",
		"12SXB",
		"12SXB",
		"12SXC",
		"12SXC",
		"12SXD",
		"12SXD",
		"12SXE",
		"12SXE",
		"12SXF",
		"12SXF",
		"12SXF",
		"12SXF",
		"12SXG",
		"12SXG",
		"12SXH",
		"12SXH",
		"12SXJ",
		"12SXJ",
		"12SXK",
		"12SXK",
		"12SYA",
		"12SYB",
		"12SYC",
		"12SYD",
		"12SYE",
		"12SYF",
		"12SYF",
		"12SYG",
		"12SYH",
		"12SYJ",
		"12SYK",
		"12TTK",
		"12TTL",
		"12TTM",
		"12TTM",
		"12TTN",
		"12TTP",
		"12TTQ",
		"12TTQ",
		"12TTR",
		"12TTR",
		"12TTS",
		"12TTT",
		"12TTU",
		"12TUK",
		"12TUL",
		"12TUM",
		"12TUM",
		"12TUN",
		"12TUP",
		"12TUQ",
		"12TUQ",
		"12TUR",
		"12TUS",
		"12TUT",
		"12TUU",
		"12TVK",
		"12TVL",
		"12TVL",
		"12TVM",
		"12TVM",
		"12TVM",
		"12TVN",
		"12TVN",
		"12TVP",
		"12TVP",
		"12TVQ",
		"12TVQ",
		"12TVQ",
		"12TVR",
		"12TVS",
		"12TVT",
		"12TVU",
		"12TWK",
		"12TWL",
		"12TWL",
		"12TWM",
		"12TWN",
		"12TWP",
		"12TWQ",
		"12TWQ",
		"12TWR",
		"12TWS",
		"12TWT",
		"12TWU",
		"12TXK",
		"12TXK",
		"12TXL",
		"12TXL",
		"12TXL",
		"12TXM",
		"12TXN",
		"12TXP",
		"12TXQ",
		"12TXQ",
		"12TXR",
		"12TXS",
		"12TXT",
		"12TXU",
		"12TYK",
		"12TYL",
		"12TYL",
		"12TYM",
		"12TYN",
		"12TYP",
		"12TYQ",
		"12TYQ",
		"12TYR",
		"12TYS",
		"12TYT",
		"12TYU",
		"12UTU",
		"12UTV",
		"12UUU",
		"12UUV",
		"12UVU",
		"12UVV",
		"12UWU",
		"12UWV",
		"12UXU",
		"12UXV",
		"12UYU",
		"12UYV",
		"13RBR",
		"13RCQ",
		"13RCR",
		"13RCR",
		"13RDP",
		"13RDQ",
		"13RDR",
		"13REN",
		"13REP",
		"13REQ",
		"13RER",
		"13RFN",
		"13RFP",
		"13RFQ",
		"13RFR",
		"13RGN",
		"13RGP",
		"13RGQ",
		"13RGR",
		"13SBA",
		"13SBA",
		"13SBB",
		"13SBC",
		"13SBD",
		"13SBE",
		"13SBR",
		"13SBS",
		"13SBT",
		"13SBU",
		"13SBV",
		"13SCA",
		"13SCA",
		"13SCB",
		"13SCC",
		"13SCD",
		"13SCE",
		"13SCR",
		"13SCR",
		"13SCS",
		"13SCT",
		"13SCU",
		"13SCV",
		"13SDA",
		"13SDA",
		"13SDB",
		"13SDC",
		"13SDD",
		"13SDE",
		"13SDR",
		"13SDR",
		"13SDS",
		"13SDT",
		"13SDU",
		"13SDV",
		"13SEA",
		"13SEA",
		"13SEB",
		"13SEC",
		"13SED",
		"13SEE",
		"13SER",
		"13SER",
		"13SES",
		"13SET",
		"13SEU",
		"13SEV",
		"13SFA",
		"13SFA",
		"13SFA",
		"13SFA",
		"13SFB",
		"13SFC",
		"13SFD",
		"13SFE",
		"13SFR",
		"13SFR",
		"13SFS",
		"13SFS",
		"13SFT",
		"13SFT",
		"13SFU",
		"13SFU",
		"13SFV",
		"13SFV",
		"13SGA",
		"13SGA",
		"13SGA",
		"13SGA",
		"13SGB",
		"13SGB",
		"13SGC",
		"13SGC",
		"13SGD",
		"13SGD",
		"13SGE",
		"13SGE",
		"13SGR",
		"13SGS",
		"13SGT",
		"13SGU",
		"13SGV",
		"13TBE",
		"13TBF",
		"13TBF",
		"13TBG",
		"13TBH",
		"13TBJ",
		"13TBK",
		"13TBK",
		"13TBL",
		"13TBM",
		"13TBN",
		"13TBP",
		"13TCE",
		"13TCF",
		"13TCF",
		"13TCG",
		"13TCH",
		"13TCJ",
		"13TCK",
		"13TCK",
		"13TCL",
		"13TCM",
		"13TCN",
		"13TCP",
		"13TDE",
		"13TDF",
		"13TDF",
		"13TDG",
		"13TDH",
		"13TDJ",
		"13TDK",
		"13TDK",
		"13TDL",
		"13TDM",
		"13TDN",
		"13TDP",
		"13TEE",
		"13TEF",
		"13TEF",
		"13TEF",
		"13TEG",
		"13TEG",
		"13TEH",
		"13TEH",
		"13TEH",
		"13TEJ",
		"13TEJ",
		"13TEK",
		"13TEK",
		"13TEK",
		"13TEL",
		"13TEL",
		"13TEL",
		"13TEM",
		"13TEM",
		"13TEN",
		"13TEN",
		"13TEP",
		"13TEP",
		"13TFE",
		"13TFF",
		"13TFF",
		"13TFG",
		"13TFH",
		"13TFH",
		"13TFJ",
		"13TFK",
		"13TFL",
		"13TFL",
		"13TFM",
		"13TFN",
		"13TFP",
		"13TGE",
		"13TGE",
		"13TGE",
		"13TGF",
		"13TGF",
		"13TGG",
		"13TGH",
		"13TGH",
		"13TGJ",
		"13TGK",
		"13TGL",
		"13TGL",
		"13TGM",
		"13TGN",
		"13TGP",
		"13UBP",
		"13UBQ",
		"13UCP",
		"13UCQ",
		"13UDP",
		"13UDQ",
		"13UEP",
		"13UEP",
		"13UEQ",
		"13UEQ",
		"13UFP",
		"13UFQ",
		"13UGP",
		"13UGQ",
		"14RKA",
		"14RKT",
		"14RKU",
		"14RKV",
		"14RLA",
		"14RLS",
		"14RLT",
		"14RLU",
		"14RLV",
		"14RMA",
		"14RMQ",
		"14RMR",
		"14RMS",
		"14RMT",
		"14RMU",
		"14RMV",
		"14RNA",
		"14RNP",
		"14RNQ",
		"14RNR",
		"14RNS",
		"14RNT",
		"14RNU",
		"14RNV",
		"14RPA",
		"14RPP",
		"14RPQ",
		"14RPR",
		"14RPS",
		"14RPT",
		"14RPU",
		"14RPV",
		"14RQA",
		"14RQR",
		"14RQS",
		"14RQT",
		"14RQU",
		"14RQV",
		"14SKA",
		"14SKB",
		"14SKC",
		"14SKD",
		"14SKE",
		"14SKF",
		"14SKF",
		"14SKF",
		"14SKG",
		"14SKH",
		"14SKJ",
		"14SKK",
		"14SLA",
		"14SLB",
		"14SLC",
		"14SLD",
		"14SLE",
		"14SLF",
		"14SLF",
		"14SLF",
		"14SLG",
		"14SLH",
		"14SLJ",
		"14SLK",
		"14SMA",
		"14SMB",
		"14SMC",
		"14SMC",
		"14SMD",
		"14SMD",
		"14SME",
		"14SME",
		"14SMF",
		"14SMF",
		"14SMF",
		"14SMG",
		"14SMH",
		"14SMJ",
		"14SMK",
		"14SNA",
		"14SNB",
		"14SNC",
		"14SNC",
		"14SND",
		"14SNE",
		"14SNF",
		"14SNF",
		"14SNG",
		"14SNH",
		"14SNJ",
		"14SNK",
		"14SPA",
		"14SPB",
		"14SPC",
		"14SPC",
		"14SPD",
		"14SPE",
		"14SPF",
		"14SPF",
		"14SPG",
		"14SPH",
		"14SPJ",
		"14SPK",
		"14SQA",
		"14SQB",
		"14SQC",
		"14SQC",
		"14SQD",
		"14SQE",
		"14SQF",
		"14SQF",
		"14SQG",
		"14SQH",
		"14SQJ",
		"14SQK",
		"14TKK",
		"14TKK",
		"14TKL",
		"14TKM",
		"14TKN",
		"14TKN",
		"14TKP",
		"14TKQ",
		"14TKR",
		"14TKR",
		"14TKS",
		"14TKT",
		"14TKU",
		"14TLK",
		"14TLK",
		"14TLL",
		"14TLM",
		"14TLN",
		"14TLN",
		"14TLP",
		"14TLQ",
		"14TLR",
		"14TLR",
		"14TLS",
		"14TLT",
		"14TLU",
		"14TMK",
		"14TMK",
		"14TML",
		"14TMM",
		"14TMN",
		"14TMN",
		"14TMP",
		"14TMQ",
		"14TMR",
		"14TMR",
		"14TMS",
		"14TMT",
		"14TMU",
		"14TNK",
		"14TNK",
		"14TNL",
		"14TNM",
		"14TNN",
		"14TNN",
		"14TNP",
		"14TNQ",
		"14TNR",
		"14TNR",
		"14TNS",
		"14TNT",
		"14TNU",
		"14TPK",
		"14TPK",
		"14TPL",
		"14TPM",
		"14TPN",
		"14TPN",
		"14TPN",
		"14TPP",
		"14TPP",
		"14TPQ",
		"14TPR",
		"14TPR",
		"14TPR",
		"14TPS",
		"14TPS",
		"14TPT",
		"14TPT",
		"14TPU",
		"14TPU",
		"14TQK",
		"14TQK",
		"14TQL",
		"14TQL",
		"14TQM",
		"14TQM",
		"14TQN",
		"14TQN",
		"14TQN",
		"14TQP",
		"14TQP",
		"14TQP",
		"14TQQ",
		"14TQQ",
		"14TQR",
		"14TQR",
		"14TQS",
		"14TQT",
		"14TQU",
		"14UKU",
		"14UKV",
		"14ULU",
		"14ULV",
		"14UMU",
		"14UMV",
		"14UNU",
		"14UNV",
		"14UPU",
		"14UPU",
		"14UPV",
		"14UPV",
		"14UQU",
		"14UQV",
		"15RTM",
		"15RTN",
		"15RTP",
		"15RTQ",
		"15RTR",
		"15RUN",
		"15RUP",
		"15RUQ",
		"15RUR",
		"15RVN",
		"15RVN",
		"15RVP",
		"15RVP",
		"15RVQ",
		"15RVQ",
		"15RVR",
		"15RVR",
		"15RWN",
		"15RWP",
		"15RWQ",
		"15RWR",
		"15RXN",
		"15RXP",
		"15RXQ",
		"15RXQ",
		"15RXR",
		"15RXR",
		"15RYN",
		"15RYP",
		"15RYQ",
		"15RYQ",
		"15RYR",
		"15STA",
		"15STA",
		"15STB",
		"15STC",
		"15STD",
		"15STE",
		"15STE",
		"15STR",
		"15STS",
		"15STT",
		"15STT",
		"15STU",
		"15STV",
		"15SUA",
		"15SUA",
		"15SUA",
		"15SUA",
		"15SUB",
		"15SUB",
		"15SUC",
		"15SUC",
		"15SUD",
		"15SUD",
		"15SUE",
		"15SUE",
		"15SUE",
		"15SUR",
		"15SUS",
		"15SUT",
		"15SUT",
		"15SUT",
		"15SUU",
		"15SUU",
		"15SUV",
		"15SUV",
		"15SVA",
		"15SVA",
		"15SVB",
		"15SVC",
		"15SVD",
		"15SVE",
		"15SVR",
		"15SVR",
		"15SVS",
		"15SVS",
		"15SVS",
		"15SVT",
		"15SVT",
		"15SVU",
		"15SVV",
		"15SWA",
		"15SWA",
		"15SWB",
		"15SWC",
		"15SWD",
		"15SWE",
		"15SWR",
		"15SWS",
		"15SWS",
		"15SWT",
		"15SWU",
		"15SWV",
		"15SXA",
		"15SXA",
		"15SXB",
		"15SXC",
		"15SXD",
		"15SXD",
		"15SXE",
		"15SXE",
		"15SXR",
		"15SXR",
		"15SXS",
		"15SXS",
		"15SXS",
		"15SXT",
		"15SXT",
		"15SXU",
		"15SXV",
		"15SYA",
		"15SYA",
		"15SYB",
		"15SYC",
		"15SYC",
		"15SYD",
		"15SYD",
		"15SYE",
		"15SYR",
		"15SYS",
		"15SYT",
		"15SYT",
		"15SYU",
		"15SYU",
		"15SYU",
		"15SYV",
		"15SYV",
		"15SYV",
		"15TTE",
		"15TTE",
		"15TTE",
		"15TTE",
		"15TTF",
		"15TTF",
		"15TTG",
		"15TTG",
		"15TTH",
		"15TTJ",
		"15TTJ",
		"15TTK",
		"15TTL",
		"15TTM",
		"15TTN",
		"15TTP",
		"15TUE",
		"15TUE",
		"15TUE",
		"15TUF",
		"15TUG",
		"15TUH",
		"15TUJ",
		"15TUJ",
		"15TUK",
		"15TUL",
		"15TUM",
		"15TUN",
		"15TUP",
		"15TVE",
		"15TVE",
		"15TVF",
		"15TVG",
		"15TVH",
		"15TVJ",
		"15TVJ",
		"15TVK",
		"15TVL",
		"15TVM",
		"15TVN",
		"15TVP",
		"15TWE",
		"15TWE",
		"15TWF",
		"15TWG",
		"15TWH",
		"15TWJ",
		"15TWJ",
		"15TWJ",
		"15TWK",
		"15TWK",
		"15TWL",
		"15TWL",
		"15TWM",
		"15TWM",
		"15TWN",
		"15TWP",
		"15TXE",
		"15TXE",
		"15TXE",
		"15TXF",
		"15TXF",
		"15TXG",
		"15TXH",
		"15TXH",
		"15TXH",
		"15TXJ",
		"15TXJ",
		"15TXJ",
		"15TXK",
		"15TXL",
		"15TXM",
		"15TXM",
		"15TXN",
		"15TXN",
		"15TXP",
		"15TYE",
		"15TYF",
		"15TYF",
		"15TYG",
		"15TYG",
		"15TYH",
		"15TYH",
		"15TYH",
		"15TYJ",
		"15TYK",
		"15TYL",
		"15TYM",
		"15TYM",
		"15TYN",
		"15TYP",
		"15UTP",
		"15UTQ",
		"15UUP",
		"15UUQ",
		"15UVP",
		"15UWP",
		"15UXP",
		"15UYP",
		"16RBA",
		"16RBT",
		"16RBU",
		"16RBU",
		"16RBV",
		"16RBV",
		"16RCA",
		"16RCA",
		"16RCT",
		"16RCU",
		"16RCU",
		"16RCU",
		"16RCV",
		"16RCV",
		"16RDA",
		"16RDU",
		"16RDU",
		"16RDV",
		"16RDV",
		"16REA",
		"16REU",
		"16REV",
		"16REV",
		"16RFA",
		"16RFA",
		"16RFT",
		"16RFU",
		"16RFV",
		"16RFV",
		"16RFV",
		"16RGA",
		"16RGT",
		"16RGU",
		"16RGU",
		"16RGV",
		"16RGV",
		"16SBA",
		"16SBB",
		"16SBC",
		"16SBD",
		"16SBD",
		"16SBE",
		"16SBE",
		"16SBE",
		"16SBF",
		"16SBF",
		"16SBF",
		"16SBF",
		"16SBG",
		"16SBG",
		"16SBH",
		"16SBH",
		"16SBJ",
		"16SBK",
		"16SCA",
		"16SCA",
		"16SCB",
		"16SCB",
		"16SCC",
		"16SCC",
		"16SCD",
		"16SCD",
		"16SCD",
		"16SCE",
		"16SCF",
		"16SCF",
		"16SCF",
		"16SCF",
		"16SCG",
		"16SCG",
		"16SCG",
		"16SCH",
		"16SCJ",
		"16SCK",
		"16SDA",
		"16SDB",
		"16SDC",
		"16SDD",
		"16SDD",
		"16SDE",
		"16SDF",
		"16SDF",
		"16SDG",
		"16SDG",
		"16SDG",
		"16SDH",
		"16SDH",
		"16SDH",
		"16SDJ",
		"16SDJ",
		"16SDK",
		"16SDK",
		"16SEA",
		"16SEB",
		"16SEC",
		"16SED",
		"16SED",
		"16SEE",
		"16SEF",
		"16SEF",
		"16SEG",
		"16SEG",
		"16SEH",
		"16SEH",
		"16SEJ",
		"16SEK",
		"16SFA",
		"16SFA",
		"16SFB",
		"16SFB",
		"16SFC",
		"16SFC",
		"16SFD",
		"16SFD",
		"16SFD",
		"16SFE",
		"16SFF",
		"16SFF",
		"16SFG",
		"16SFH",
		"16SFH",
		"16SFJ",
		"16SFJ",
		"16SFJ",
		"16SFK",
		"16SFK",
		"16SGA",
		"16SGB",
		"16SGC",
		"16SGD",
		"16SGD",
		"16SGD",
		"16SGE",
		"16SGE",
		"16SGF",
		"16SGF",
		"16SGG",
		"16SGH",
		"16SGH",
		"16SGJ",
		"16SGJ",
		"16SGK",
		"16TBK",
		"16TBL",
		"16TBM",
		"16TBN",
		"16TBN",
		"16TBP",
		"16TBQ",
		"16TBR",
		"16TBS",
		"16TBS",
		"16TBU",
		"16TCK",
		"16TCL",
		"16TCM",
		"16TCN",
		"16TCN",
		"16TCP",
		"16TCQ",
		"16TCR",
		"16TCR",
		"16TCS",
		"16TCS",
		"16TCT",
		"16TCU",
		"16TCU",
		"16TDK",
		"16TDK",
		"16TDL",
		"16TDL",
		"16TDM",
		"16TDM",
		"16TDN",
		"16TDN",
		"16TDP",
		"16TDQ",
		"16TDQ",
		"16TDR",
		"16TDR",
		"16TDS",
		"16TDT",
		"16TEK",
		"16TEL",
		"16TEM",
		"16TEM",
		"16TEN",
		"16TEP",
		"16TEQ",
		"16TER",
		"16TER",
		"16TES",
		"16TFK",
		"16TFK",
		"16TFL",
		"16TFL",
		"16TFM",
		"16TFM",
		"16TFM",
		"16TFN",
		"16TFP",
		"16TFQ",
		"16TFR",
		"16TFS",
		"16TGK",
		"16TGL",
		"16TGM",
		"16TGM",
		"16TGN",
		"16TGP",
		"16TGQ",
		"16TGR",
		"16TGS",
		"16UBU",
		"16UCU",
		"16UCU",
		"16UDU",
		"17RKN",
		"17RKP",
		"17RKP",
		"17RKQ",
		"17RKR",
		"17RLH",
		"17RLK",
		"17RLL",
		"17RLM",
		"17RLN",
		"17RLP",
		"17RLP",
		"17RLQ",
		"17RLR",
		"17RMH",
		"17RMJ",
		"17RMK",
		"17RML",
		"17RMM",
		"17RMN",
		"17RMP",
		"17RMP",
		"17RMQ",
		"17RMQ",
		"17RMR",
		"17RNH",
		"17RNJ",
		"17RNK",
		"17RNL",
		"17RNM",
		"17RNN",
		"17RNR",
		"17SKA",
		"17SKA",
		"17SKA",
		"17SKB",
		"17SKC",
		"17SKC",
		"17SKD",
		"17SKE",
		"17SKR",
		"17SKS",
		"17SKT",
		"17SKU",
		"17SKU",
		"17SKU",
		"17SKV",
		"17SKV",
		"17SLA",
		"17SLA",
		"17SLA",
		"17SLA",
		"17SLB",
		"17SLB",
		"17SLB",
		"17SLC",
		"17SLC",
		"17SLC",
		"17SLD",
		"17SLE",
		"17SLR",
		"17SLS",
		"17SLT",
		"17SLT",
		"17SLU",
		"17SLU",
		"17SLU",
		"17SLV",
		"17SLV",
		"17SMA",
		"17SMA",
		"17SMA",
		"17SMB",
		"17SMB",
		"17SMB",
		"17SMC",
		"17SMC",
		"17SMD",
		"17SMD",
		"17SME",
		"17SMR",
		"17SMR",
		"17SMS",
		"17SMS",
		"17SMT",
		"17SMT",
		"17SMU",
		"17SMU",
		"17SMV",
		"17SMV",
		"17SNA",
		"17SNA",
		"17SNB",
		"17SNB",
		"17SNC",
		"17SNC",
		"17SND",
		"17SND",
		"17SND",
		"17SNE",
		"17SNE",
		"17SNE",
		"17SNR",
		"17SNR",
		"17SNS",
		"17SNT",
		"17SNU",
		"17SNU",
		"17SNV",
		"17SPA",
		"17SPA",
		"17SPB",
		"17SPC",
		"17SPC",
		"17SPD",
		"17SPD",
		"17SPD",
		"17SPD",
		"17SPE",
		"17SPS",
		"17SPT",
		"17SPT",
		"17SPU",
		"17SPU",
		"17SPV",
		"17SQA",
		"17SQA",
		"17SQB",
		"17SQC",
		"17SQD",
		"17SQD",
		"17SQD",
		"17SQD",
		"17SQE",
		"17SQE",
		"17SQT",
		"17SQT",
		"17SQU",
		"17SQV",
		"17TKE",
		"17TKF",
		"17TKG",
		"17TKG",
		"17TKH",
		"17TKJ",
		"17TKK",
		"17TKL",
		"17TKM",
		"17TLE",
		"17TLF",
		"17TLG",
		"17TLG",
		"17TLH",
		"17TLJ",
		"17TLK",
		"17TLL",
		"17TLM",
		"17TME",
		"17TMF",
		"17TMG",
		"17TNE",
		"17TNE",
		"17TNE",
		"17TNF",
		"17TNF",
		"17TNG",
		"17TNG",
		"17TPE",
		"17TPF",
		"17TPG",
		"17TPG",
		"17TPH",
		"17TPJ",
		"17TQE",
		"17TQF",
		"17TQG",
		"17TQG",
		"17TQH",
		"17TQJ",
		"18STC",
		"18STD",
		"18STE",
		"18STF",
		"18STF",
		"18STG",
		"18STH",
		"18STJ",
		"18STJ",
		"18STJ",
		"18STJ",
		"18STK",
		"18STK",
		"18SUD",
		"18SUE",
		"18SUF",
		"18SUF",
		"18SUG",
		"18SUH",
		"18SUH",
		"18SUH",
		"18SUJ",
		"18SUJ",
		"18SUJ",
		"18SUJ",
		"18SUK",
		"18SVD",
		"18SVE",
		"18SVF",
		"18SVF",
		"18SVG",
		"18SVG",
		"18SVH",
		"18SVH",
		"18SVH",
		"18SVJ",
		"18SVJ",
		"18SVJ",
		"18SVJ",
		"18SVK",
		"18SVK",
		"18SVK",
		"18SWJ",
		"18SWK",
		"18TTK",
		"18TTL",
		"18TTM",
		"18TTM",
		"18TTN",
		"18TTP",
		"18TUK",
		"18TUL",
		"18TUM",
		"18TUM",
		"18TUN",
		"18TUP",
		"18TVK",
		"18TVK",
		"18TVL",
		"18TVL",
		"18TVL",
		"18TVM",
		"18TVM",
		"18TVN",
		"18TVP",
		"18TVQ",
		"18TWK",
		"18TWK",
		"18TWK",
		"18TWL",
		"18TWL",
		"18TWL",
		"18TWM",
		"18TWN",
		"18TWP",
		"18TWQ",
		"18TXK",
		"18TXL",
		"18TXL",
		"18TXM",
		"18TXM",
		"18TXM",
		"18TXN",
		"18TXN",
		"18TXN",
		"18TXN",
		"18TXP",
		"18TXP",
		"18TXQ",
		"18TXQ",
		"18TYL",
		"18TYL",
		"18TYM",
		"18TYM",
		"18TYN",
		"18TYN",
		"18TYN",
		"18TYP",
		"18TYP",
		"18TYQ",
		"18TYQ",
		"19TBF",
		"19TBF",
		"19TBF",
		"19TBG",
		"19TBG",
		"19TBG",
		"19TBH",
		"19TBH",
		"19TBJ",
		"19TBK",
		"19TBK",
		"19TCF",
		"19TCF",
		"19TCG",
		"19TCG",
		"19TCH",
		"19TCH",
		"19TCH",
		"19TCJ",
		"19TCJ",
		"19TCK",
		"19TCK",
		"19TCK",
		"19TCL",
		"19TCL",
		"19TCM",
		"19TDF",
		"19TDG",
		"19TDJ",
		"19TDK",
		"19TDL",
		"19TDM",
		"19TDN",
		"19TEJ",
		"19TEK",
		"19TEL",
		"19TEM",
		"19TEN",
		"19TFK",
		"19TFL"
]


pool = mp.Pool(mp.cpu_count())
pool.map(mk100m, grids)
#map(mk100m, grids)