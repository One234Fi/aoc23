#recompile script

DAY=$1

rm ./bin/day${DAY}
make ./bin/day${DAY}
