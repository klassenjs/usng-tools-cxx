DROP FUNCTION usngfromll84(float8, float8);
DROP FUNCTION usngfromll84(float8, float8, integer);

CREATE FUNCTION usngfromll84(float8, float8, integer) RETURNS text
  AS '/home/jimk/DevWorld/USNG/usng_tools/pg/pg_usng', 'usng2fromll84_'
	LANGUAGE C STRICT;

CREATE FUNCTION usngfromll84(lon float8, lat float8) RETURNS text
AS $$
BEGIN
	RETURN usngfromll84(lon, lat, 4);
END;
$$ LANGUAGE plpgsql;


DROP FUNCTION usngtoll84(text);
DROP FUNCTION usngtoll84(text, float8, float8);

CREATE FUNCTION usngtoll84(text, float8, float8) RETURNS Point
  AS '/home/jimk/DevWorld/USNG/usng_tools/pg/pg_usng', 'usng2toll84_'
  LANGUAGE C;


CREATE FUNCTION usngtoll84(usng text) RETURNS Point
AS $$
BEGIN
	RETURN usngtoll84(usng, NULL, NULL);
END;
$$ LANGUAGE plpgsql;

select usngtoll84('WK00', -93,45), usngfromll84(-93,44.25);

