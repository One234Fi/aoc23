#! /bin/bash

DAY=$1

if [[ -z "$DAY" ]]; then
    DAY=$(date +%d)
fi

curl -b session=$(cat ${HOME}/aoc23/cookie.config) https://adventofcode.com/2023/day/${DAY}/input > input
