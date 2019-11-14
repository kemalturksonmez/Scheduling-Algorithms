all:
	mkdir -p simulationlogs
	mkdir -p results
	chmod +x runGnu.sh
	gcc schedAlgo.c -lgsl -lgslcblas -lm -o schedAlgo
clean:
	rm -rf simulationlogs/
	rm -rf results/
	rm schedAlgo

cleanfiles:
	rm -rf simulationlogs/*
	rm -rf results/*