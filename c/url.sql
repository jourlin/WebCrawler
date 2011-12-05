DROP TYPE IF EXISTS url CASCADE;

CREATE OR REPLACE FUNCTION url_in(cstring)
   RETURNS url
   AS '/tmp/url'
   LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION url_out(url)
   RETURNS cstring
   AS '/tmp/url'
   LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION url_top(url)
   RETURNS cstring
   AS '/tmp/url'
   LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE url (
   internallength = 265,
   input = url_in,
   output = url_out
);

