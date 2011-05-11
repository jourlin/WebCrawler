DROP TABLE IF EXISTS node CASCADE;
CREATE TABLE node (
    id SERIAL PRIMARY KEY,
    url text NOT NULL UNIQUE,
    effectiveurl text,
    checked timestamp with time zone
);

DROP TABLE IF EXISTS links CASCADE;
CREATE TABLE links (
    id SERIAL PRIMARY KEY,
    "from" integer NOT NULL REFERENCES node(id) ON UPDATE CASCADE ON DELETE CASCADE,
    "to" integer NOT NULL REFERENCES node(id) ON UPDATE CASCADE ON DELETE CASCADE,
    UNIQUE ("from", "to"),
    checked timestamp with time zone
);
