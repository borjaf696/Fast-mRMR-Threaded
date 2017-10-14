#!/bin/bash
cd ..
make clean
make
cd Scripts
for i in `seq 1 3`;
do
	./../fast-mrmr -f ../../../../../Datasets/Mrmr/breast_preprocessed.mrmr -a 10 >> ../../Output-BreastCancerMicroarray/OpenMp/breastMicroArray10FeaturesTotal
	./../fast-mrmr -f ../../../../../Datasets/Mrmr/breast_preprocessed.mrmr -a 100 >> ../../Output-BreastCancerMicroarray/OpenMp/breastMicroArray100FeaturesTotal
	./../fast-mrmr -f ../../../../../Datasets/Mrmr/breast_preprocessed.mrmr -a 150 >> ../../Output-BreastCancerMicroarray/OpenMp/breastMicroArray150FeaturesTotal
done

