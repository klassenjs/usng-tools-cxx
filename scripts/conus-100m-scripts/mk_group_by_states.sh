# Script to generate the group_by_states script to sort 100km grid squares into state folders

for i in *100km*.shp ; do 
	ogr2ogr -append -f PostgreSQL PG: . -nln usng100km -t_srs EPSG:4269 ${i%%.shp}; 
done

psql <<EOF
select distinct 'mkdir -p '||stusps||' && ln '||replace(substring(usng,1,6), ' ', '')||'* '||stusps 
  from usng100km, cb_2015_us_state_500k
 where st_intersects(usng100km.wkb_geometry, cb_2015_us_state_500k.wkb_geometry)
order by 1;
EOF
