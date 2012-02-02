#-------------------------------------------------------------------------
#
# Makefile--
#    Makefile for tutorial
#
# By default, this builds against an existing PostgreSQL installation
# (the one identified by whichever pg_config is first in your path).
# Within a configured source tree, you can say "gmake NO_PGXS=1 all"
# to build using the surrounding source tree.
#
# IDENTIFICATION
#    src/tutorial/Makefile
#
#-------------------------------------------------------------------------

MODULES = url 

# Usual path for pg_config :
 PG_CONFIG = pg_config
# Specific path for our 9.1 server :
#PG_CONFIG = /usr/local/pgsql/bin/pg_config

PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

