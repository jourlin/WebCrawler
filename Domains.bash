psql -o domains.list -n -q -t -c "select distinct substring(url from '(www\\\.[^\\\./]+\\\.[^/%]+)') from node"
 
