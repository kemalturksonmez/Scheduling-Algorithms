#!/bin/bash

for FILE in results/*; do
	filename="${FILE%.*}"

    gnuplot <<- EOF
        set xlabel "d"
        set ylabel "d/ATT"
        set title "${filename}"   
        set term png
        set output "${filename}.png"
        plot "${FILE}"
EOF
done