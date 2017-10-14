#!/bin/bash
cd ..
make clean
make
cd Scripts
export OMP_NUM_THREADS=1
for i in `seq 1 3`;
do
	./../fast-mrmr -f ../../../../../Datasets/Mrmr/breast_preprocessed.mrmr -a 50 >> ../../Output-Casa/BreastCancerMicroarray/MPI/breastMicroArray50FeaturesTotal
	./../fast-mrmr -f ../../../../../Datasets/Mrmr/breast_preprocessed.mrmr -a 100 >> ../../Output-Casa/BreastCancerMicroarray/MPI/breastMicroArray100FeaturesTotal
	./../fast-mrmr -f ../../../../../Datasets/Mrmr/breast_preprocessed.mrmr -a 200 >> ../../Output-Casa/BreastCancerMicroarray/MPI/breastMicroArray200FeaturesTotal
done
