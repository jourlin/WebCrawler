CREATE LANGUAGE plpgsql;

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
    score double precision,
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

DROP FUNCTION IF EXISTS ScoreURL(id bigint);
CREATE FUNCTION ScoreURL(id bigint) RETURNS bigint AS 
$$
DECLARE
score bigint;
tmp bigint;

BEGIN
select count(*) INTO tmp FROM links where (
substring(lower(midcontext), 'presidentielle') is not NULL OR 
substring(lower(midcontext), 'politique') is not NULL OR 
substring(lower(midcontext), 'election') is not NULL OR 
substring(lower(midcontext), 'primaires') is not NULL OR 
substring(lower(midcontext), 'socialistes') is not NULL OR
substring(lower(midcontext), 'hollande') is not NULL OR
substring(lower(midcontext), 'aubry') is not NULL OR
substring(lower(midcontext), 'royal') is not NULL OR
substring(lower(midcontext), 'vals') is not NULL OR
substring(lower(midcontext), 'montebourg') is not NULL OR
substring(lower(midcontext), 'bailet') is not NULL) AND
"to"=id;
score:=tmp;
select (CASE WHEN (substring(lower("url"), 'presidentielle') IS NULL) THEN 0 ELSE 2 END)+
(CASE WHEN (substring(lower("url"), 'politique') IS NULL) THEN 0 ELSE 1 END)+
(CASE WHEN (substring(lower("url"), 'election') IS NULL) THEN 0 ELSE 1 END)+
(CASE WHEN (substring(lower("url"), 'primaires') IS NULL) THEN 0 ELSE 3 END)+
(CASE WHEN (substring(lower("url"), 'socialistes') IS NULL) THEN 0 ELSE 3 END)+
(CASE WHEN (substring(lower("url"), 'hollande') IS NULL) THEN 0 ELSE 1 END)+
(CASE WHEN (substring(lower("url"), 'aubry') IS NULL) THEN 0 ELSE 4 END)+
(CASE WHEN (substring(lower("url"), 'royal') IS NULL) THEN 0 ELSE 1 END)+
(CASE WHEN (substring(lower("url"), 'vals') IS NULL) THEN 0 ELSE 1 END)+
(CASE WHEN (substring(lower("url"), 'montebourg') IS NULL) THEN 0 ELSE 4 END)+
(CASE WHEN (substring(lower("url"), 'bailet') IS NULL) THEN 0 ELSE 4 END)
INTO tmp FROM node where node."id"=id;
score:=score+tmp;
SELECT case WHEN tld='fr' THEN 1 ELSE 0 END INTO tmp FROM node, domain WHERE node."id"=id AND domainid=domain.id;
score:=score+tmp;
RETURN score;
END;
$$ LANGUAGE plpgsql;

CREATE TEXT SEARCH CONFIGURATION public.pg ( COPY = pg_catalog.simple); 
CREATE TEXT SEARCH DICTIONARY scratch (TEMPLATE = simple);
ALTER TEXT SEARCH CONFIGURATION public.pg ALTER MAPPING FOR asciiword WITH scratch;

