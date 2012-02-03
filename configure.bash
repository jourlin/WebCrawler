#!/bin/bash

echo "Type your PostgreSQL database, followed by [ENTER]:"
read DB
echo "Type your PostgreSQL server host, followed by [ENTER]:"
read HOST
echo "Type your PostgreSQL server port, followed by [ENTER]:"
read PORT
echo "Type your PostgreSQL user name, followed by [ENTER]:"
read USER

sed "s:__USER__:"$USER":" makefile.uc |sed "s:__DB__:"$DB":"|sed "s:__HOST__:"$HOST":"|sed "s:__PORT__:"$PORT":" > makefile


