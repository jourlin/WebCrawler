DROP TABLE IF EXISTS node CASCADE;
CREATE TABLE node (
    id BIGSERIAL PRIMARY KEY,
    url text NOT NULL UNIQUE,
    effectiveurl text,
    checked timestamp with time zone
);

DROP TABLE IF EXISTS links CASCADE;
CREATE TABLE links (
    id BIGSERIAL PRIMARY KEY,
    "from" bigint NOT NULL REFERENCES node(id) ON UPDATE CASCADE ON DELETE CASCADE,
    "to" bigint NOT NULL REFERENCES node(id) ON UPDATE CASCADE ON DELETE CASCADE,
    UNIQUE ("from", "to"),
    checked timestamp with time zone
);

DROP TABLE IF EXISTS domain CASCADE;
CREATE TABLE domain (
    id BIGSERIAL PRIMARY KEY,
    name text NOT NULL UNIQUE,
);

DROP TABLE IF EXISTS tld CASCADE;
CREATE TABLE node (

    id BIGSERIAL PRIMARY KEY,
    url text NOT NULL UNIQUE,
    effectiveurl text,
    checked timestamp with time zone
);


