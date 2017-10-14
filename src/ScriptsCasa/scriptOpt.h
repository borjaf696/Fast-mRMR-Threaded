#!/bin/bash
cd ..
make clean
make
cd Scripts
for i in `seq 1 3`;
do
	./../fast-mrmr -f ../../../../../Datasets/Mrmr/breast_preprocessed.mrmr -a 50 >> ../../Output-Casa/BreastCancerMicroarray/Optimized/breastMicroArray50FeaturesMain3Total
	./../fast-mrmr -f ../../../../../Datasets/Mrmr/breast_preprocessed.mrmr -a 100 >> ../../Output-Casa/BreastCancerMicroarray/Optimized/breastMicroArray100FeaturesMain3Total
	./../fast-mrmr -f ../../../../../Datasets/Mrmr/breast_preprocessed.mrmr -a 200 >> ../../Output-Casa/BreastCancerMicroarray/Optimized/breastMicroArray200FeaturesMain3Total
done
