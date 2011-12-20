DROP TYPE IF EXISTS url CASCADE;

CREATE OR REPLACE FUNCTION url_in(cstring)
   RETURNS url
   AS '_OBJWD_/url'
   LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION url_out(url)
   RETURNS cstring
   AS '_OBJWD_/url'
   LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION url_pro(url)
   RETURNS cstring
   AS '_OBJWD_/url'
   LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION url_sub(url)
   RETURNS cstring
   AS '_OBJWD_/url'
   LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION url_dom(url)
   RETURNS cstring
   AS '_OBJWD_/url'
   LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION url_top(url)
   RETURNS cstring
   AS '_OBJWD_/url'
   LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION url_pat(url)
   RETURNS cstring
   AS '_OBJWD_/url'
   LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION url_len(url)
   RETURNS INTEGER
   AS '_OBJWD_/url'
   LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE url (
   internallength = 266,
   input = url_in,
   output = url_out
);

CREATE FUNCTION url_lt(url, url) RETURNS bool
   AS '_OBJWD_/url' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION url_le(url, url) RETURNS bool
   AS '_OBJWD_/url' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION url_eq(url, url) RETURNS bool
   AS '_OBJWD_/url' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION url_ge(url, url) RETURNS bool
   AS '_OBJWD_/url' LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION url_gt(url, url) RETURNS bool
   AS '_OBJWD_/url' LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR < (
   leftarg = url, rightarg = url, procedure = url_lt,
   commutator = > , negator = >= ,
   restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR <= (
   leftarg = url, rightarg = url, procedure = url_le,
   commutator = >= , negator = > ,
   restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR = (
   leftarg = url, rightarg = url, procedure = url_eq,
   commutator = = ,
   -- leave out negator since we didn't create <> operator
   -- negator = <> ,
   restrict = eqsel, join = eqjoinsel
);
CREATE OPERATOR >= (
   leftarg = url, rightarg = url, procedure = url_ge,
   commutator = <= , negator = < ,
   restrict = scalargtsel, join = scalargtjoinsel
);
CREATE OPERATOR > (
   leftarg = url, rightarg = url, procedure = url_gt,
   commutator = < , negator = <= ,
   restrict = scalargtsel, join = scalargtjoinsel
);

-- create the support function too
CREATE FUNCTION url_cmp(url, url) RETURNS int4
   AS '_OBJWD_/url' LANGUAGE C IMMUTABLE STRICT;

-- now we can make the operator class
CREATE OPERATOR CLASS url_ops
    DEFAULT FOR TYPE url USING btree AS
        OPERATOR        1       < ,
        OPERATOR        2       <= ,
        OPERATOR        3       = ,
        OPERATOR        4       >= ,
        OPERATOR        5       > ,
        FUNCTION        1       url_cmp(url, url);

