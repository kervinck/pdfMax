#!/bin/sh

for exponent in 1 2 3 4 5 6 7 8 9 10 11
do
        for coef in 5 2 1
        do
                significance="${coef}e-${exponent}"
                echo "Testing significance: $significance"
                bunzip2 -c testcases.json.bz2 | python testPdfMax.py $significance
                echo
        done
done
