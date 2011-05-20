psql -o domains.list -n -q -t -c "select distinct substring(url from '([^\\\.\\\/]+\\\.[a-z]+\\\/)') from node"
 
