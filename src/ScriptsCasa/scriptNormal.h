#!/bin/bash
cd ..
make clean
make
cd Scripts
for i in `seq 1 3`;
        do
	        ./../fast-mrmr -f ../../../../../Datasets/Mrmr/breast_preprocessed.mrmr -a 50 >> ../../Output-Casa/BreastCancerMicroarray/Normal/breastMicroArray50FeaturesTotal
                ./../fast-mrmr -f ../../../../../Datasets/Mrmr/breast_preprocessed.mrmr -a 100 >> ../../Output-Casa/BreastCancerMicroarray/Normal/breastMicroArray100FeaturesTotal
	        ./../fast-mrmr -f ../../../../../Datasets/Mrmr/breast_preprocessed.mrmr -a 200 >> ../../Output-Casa/BreastCancerMicroarray/Normal/breastMicroArray200FeaturesTotal
        done 
