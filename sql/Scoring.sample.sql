CREATE OR REPLACE FUNCTION ScoreLink(context text) RETURNS int AS 
$$
DECLARE
score INT;
normcontext TEXT;
BEGIN
normcontext=normalize(context);
score=0;
IF (substring(normcontext, 'hollande') IS NOT NULL) THEN
	score = score +1;
END IF;
IF 	(substring(normcontext, 'le pen') IS NOT NULL) THEN
	score = score +1;
END IF;
IF 	(substring(normcontext, 'de villepin') IS NOT NULL) THEN
	score = score +1;
END IF;
IF 	(substring(normcontext, 'joly') IS NOT NULL) THEN
	score = score +1;
END IF;
IF 	(substring(normcontext, 'melanchon') IS NOT NULL) THEN
	score = score +1;
END IF;
IF 	(substring(normcontext, 'sarkozy') IS NOT NULL) THEN
	score = score +1;
END IF;
IF 	(substring(normcontext, 'dupont-aignan') IS NOT NULL) THEN
	score = score +1;
END IF;
IF 	(substring(normcontext, 'bayrou') IS NOT NULL) THEN
	score = score +1;
END IF;
IF 	(substring(normcontext, 'chevenement') IS NOT NULL) THEN
	score = score +1;
END IF;
IF 	(substring(normcontext, 'morin') IS NOT NULL) THEN
	score = score +1;
END IF;
IF 	(substring(normcontext, 'lepage') IS NOT NULL) THEN
	score = score +1;
END IF;
IF 	(substring(normcontext, 'nihous') IS NOT NULL) THEN
	score = score +1;
END IF;
IF 	(substring(normcontext, 'boutin') IS NOT NULL) THEN
	score = score +1;
END IF;
IF 	(substring(normcontext, 'poutou') IS NOT NULL) THEN
	score = score +1;
END IF;
IF 	(substring(normcontext, 'arthaud') IS NOT NULL) THEN
	score = score +1;
END IF;
RETURN score;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION ScoreURL(url url) RETURNS bigint AS 
$$
DECLARE
score INT;
normurl TEXT;
BEGIN
normurl=normalize(CAST(url AS text));
IF CAST(url_top(url) AS TEXT) ='fr' THEN
	score=1;
ELSE
	score=0;
END IF;
IF substring(normurl, 'hollande') IS NOT NULL THEN
	score=score+1;
END IF;
IF substring(normurl, 'lepen') IS NOT NULL OR substring(normurl, 'le-pen') IS NOT NULL THEN
	score=score+1;
END IF;
IF substring(normurl, 'villepin') IS NOT NULL THEN
	score=score+1;
END IF;
IF substring(normurl, 'joly') IS NOT NULL THEN
	score=score+1;
END IF;
IF substring(normurl, 'melanchon') IS NOT NULL OR substring(normurl, 'm%e8lanchon') IS NOT NULL THEN
	score=score+1;
END IF;
IF substring(normurl, 'sarkozy') IS NOT NULL THEN
	score=score+1;
END IF;
IF substring(normurl, 'dupontaignan') IS NOT NULL OR substring(normurl, 'dupont-aignan') IS NOT NULL THEN
	score=score+1;
END IF;
IF substring(normurl, 'bayrou') IS NOT NULL THEN
	score=score+1;
END IF;
IF substring(normurl, 'chevenement') IS NOT NULL OR substring(normurl, 'chev%e8nement') IS NOT NULL  THEN
	score=score+1;
END IF;
IF substring(normurl, 'morin') IS NOT NULL THEN
	score=score+1;
END IF;
IF substring(normurl, 'lepage') IS NOT NULL THEN
	score=score+1;
END IF;
IF substring(normurl, 'nihous') IS NOT NULL THEN
	score=score+1;
END IF;
IF substring(normurl, 'boutin') IS NOT NULL THEN
	score=score+1;
END IF;
IF substring(normurl, 'poutou') IS NOT NULL THEN
	score=score+1;
END IF;
IF substring(normurl, 'arthaud') IS NOT NULL THEN
	score=score+1;
END IF;
RETURN score;
END;
$$ LANGUAGE plpgsql;

