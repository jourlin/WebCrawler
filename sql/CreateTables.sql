DROP TABLE IF EXISTS domain CASCADE;
CREATE TABLE domain (
    id BIGSERIAL PRIMARY KEY,
    checked timestamp with time zone,
    protocol VARCHAR(50) NOT NULL,
    subdomains VARCHAR(256),
    domain VARCHAR(256) NOT NULL,
    tld VARCHAR(10) NOT NULL,
    ipadr inet,
    UNIQUE(protocol, subdomains, domain, tld)
);

DROP TABLE IF EXISTS node CASCADE;
CREATE TABLE node (
    id BIGSERIAL PRIMARY KEY,
    url text NOT NULL UNIQUE,
    effectiveurl text,
    checked timestamp with time zone,
    score double,
    domainid bigint NOT NULL REFERENCES domain(id) ON UPDATE CASCADE ON DELETE CASCADE,
    urlpath text,
    content text
);

DROP TABLE IF EXISTS links CASCADE;
CREATE TABLE links (
    id BIGSERIAL PRIMARY KEY,
    "from" bigint NOT NULL REFERENCES node(id) ON UPDATE CASCADE ON DELETE CASCADE,
    "to" bigint NOT NULL REFERENCES node(id) ON UPDATE CASCADE ON DELETE CASCADE,
    leftcontext text,
    midcontext text,
    rightcontext text,
    UNIQUE ("from", "to"),
    checked timestamp with time zone
);


CREATE TEXT SEARCH CONFIGURATION public.pg ( COPY = pg_catalog.simple); 
CREATE TEXT SEARCH DICTIONARY scratch (TEMPLATE = simple);
ALTER TEXT SEARCH CONFIGURATION public.pg ALTER MAPPING FOR asciiword WITH scratch;

