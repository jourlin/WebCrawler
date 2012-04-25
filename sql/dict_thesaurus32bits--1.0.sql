/* contrib/dict_thesaurus32bits/dict_thesaurus32bits--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION dict_thesaurus32bits" to load this file. \quit

CREATE FUNCTION thesaurus_init32(internal)
        RETURNS internal
        AS 'MODULE_PATHNAME'
        LANGUAGE C STRICT;

CREATE FUNCTION thesaurus_lexize32(internal,internal,internal,internal)
        RETURNS internal
        AS 'MODULE_PATHNAME'
        LANGUAGE C STRICT;

CREATE TEXT SEARCH TEMPLATE thesaurus_template32 (
        LEXIZE = thesaurus_lexize32,
	INIT   = thesaurus_init32
);

CREATE TEXT SEARCH DICTIONARY thesaurus32 (
	TEMPLATE = thesaurus_template32
);

COMMENT ON TEXT SEARCH DICTIONARY thesaurus32 IS '32-bits thesaurus dictionary template';
