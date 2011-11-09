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

-- Note : When table "domain" is already filled, the two following queries insert root URLs into table node
--        This can be useful to start a crawl from previous collected data.
-- INSERT INTO node (url, score, domainid, urlpath) SELECT protocol||'://'||subdomains||'.'||domain||'.'||tld, 1.0, id, '' FROM domain WHERE subdomains is not NULL AND length(subdomains)!=0;
-- INSERT INTO node (url, score, domainid, urlpath) SELECT protocol||'://'||domain||'.'||tld, 1.0, id, '' FROM domain WHERE subdomains is NULL OR length(subdomains)=0;


DROP TABLE IF EXISTS node CASCADE;
CREATE TABLE node (
    id BIGSERIAL PRIMARY KEY,
    url text NOT NULL,
    effectiveurl text,
    checked timestamp with time zone,
    score double precision,
    domainid bigint NOT NULL REFERENCES domain(id) ON UPDATE CASCADE ON DELETE CASCADE,
    urlpath text,
    content text,
    UNIQUE(url,urlpath)
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
substring(lower(midcontext), 'socialistes') is not NULL) AND
"to"=id;
score:=tmp;
select (CASE WHEN (substring(lower("url"), 'presidentielle') IS NULL) THEN 0 ELSE 2 END)+
(CASE WHEN (substring(lower("url"), 'politique') IS NULL) THEN 0 ELSE 1 END)+
(CASE WHEN (substring(lower("url"), 'election') IS NULL) THEN 0 ELSE 1 END)+
(CASE WHEN (substring(lower("url"), 'primaires') IS NULL) THEN 0 ELSE 3 END)+
(CASE WHEN (substring(lower("url"), 'socialistes') IS NULL) THEN 0 ELSE 3 END)
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

