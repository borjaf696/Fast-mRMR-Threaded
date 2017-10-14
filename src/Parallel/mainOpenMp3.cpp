/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../../plibOp/JointProb.h"
#include "../../plibOp/MutualInfo.h"
#include "../../plibOp/utils.h"

#include "omp.h"
#include <vector>
#include <algorithm>
#include <limits>
#include <boost/bind.hpp>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <stdlib.h>


//Returns the index of the higher value in the classRelevances Vector different from classIndex
/*uint getMaxRelevance(vector<double> classRelevances, uint classIndex) {
	uint i = 0;
	uint newFeature = -1;
	double relevance = 0;
	for (i = 0; i < classRelevances.size(); ++i) {
		if (classRelevances[i] > relevance && i != classIndex) {
			relevance = classRelevances[i];
			newFeature = i;
		}
	}
	return newFeature;
}*/
uint getMaxRelevance(double* classRelevances, uint classIndex, uint rawDataSize) {
	uint i = 0;
	uint newFeature = -1;
	double relevance = 0;
	for (i = 0; i < rawDataSize ; ++i) {
		if (classRelevances[i] > relevance && i != classIndex) {
			relevance = classRelevances[i];
			newFeature = i;
		}
	}
	return newFeature;
}
options parseOptions(int argc, char*argv[]) {
	options opts;
	opts.classIndex = 0;
	opts.selectedFeatures = 10;
	opts.file = (char *) malloc(20);
	strcpy(opts.file,"../data.mrmr");
	//opts.file = "../data.mrmr";

	if (argc > 1) {
		for (int i = 0; i < argc; ++i) {
			if (strcmp(argv[i], "-f") == 0) {
				opts.file = argv[i + 1];
			}
			if (strcmp(argv[i], "-a") == 0) {
				opts.selectedFeatures = atoi(argv[i + 1]) - 1;
			}
			if (strcmp(argv[i], "-c") == 0) {
				opts.classIndex = atoi(argv[i + 1]) - 1;
			}
			if (strcmp(argv[i], "-h") == 0) {
				printf(
						"fast-mrmr:\nOptions:\n -f <inputfile>\t\tMRMR file generated using mrmrReader (default: data.mrmr).\n-c <classindex>\t\tIndicates the class index in the dataset (default: 0).\n-a <nfeatures>\t Indicates the number of features to select (default: 10).\n-h Prints this message");
				exit(0);
			}
		}
	}
	return opts;
}

int main(int argc, char* argv[]) {
	options opts;
	uint i = 0;
	uint j = 0;
	uint newFeatureIndex = 0;
	uint lastFeatureIndex = 0;
    	int threadsMaximos = omp_get_max_threads();
	double mrmr = 0;
	double acum = 0;
    	double *acumThread = (double*)malloc(threadsMaximos*sizeof(double));
    	int *numFeature = (int*)malloc(threadsMaximos*sizeof(int));
	/*vector<double> relevances;
	vector<double> redundances;*/
	double * relevances;
	double * redundances;
	std::vector<int> selectedFeatures;
	int* visited;
	int sizeofBlock;

	Timer tm;
	opts = parseOptions(argc, argv);
	tm.start();
	RawData rawData = RawData(opts.file);
	ProbTable prob = ProbTable(rawData);
	MutualInfo mutualInfo = MutualInfo(rawData, prob);
	visited = (int*) malloc(rawData.getFeaturesSize()*sizeof(int));

	for (uint z = 0;z < rawData.getFeaturesSize();z++)
		visited[z] = 0;

	sizeofBlock = (int)(((double)rawData.getFeaturesSize())*0.02);
	printf("%d\n",sizeofBlock);
	//Get relevances between all features and class.
	Timer timeRel;
	timeRel.start();
	//Mirar a ver si se puede optimizar.
	relevances = (double*) malloc (rawData.getFeaturesSize()*sizeof(double));
	redundances = (double*) malloc (rawData.getFeaturesSize()*sizeof(double));
	#pragma omp parallel for schedule(dynamic,sizeofBlock)
	for (i = 0; i < rawData.getFeaturesSize(); ++i) {
		/*relevances.push_back(mutualInfo.get(opts.classIndex, i));
		redundances.push_back(0);*/
		relevances[i] = mutualInfo.get(opts.classIndex,i);
		redundances[i] = 0;
	}

	/*for (uint i = 0; i < rawData.getFeaturesSize(); i++)
		printf("Relevances %d %f\n",i,relevances[i]);*/

	double timeMeasureRel = timeRel.stop();

	// Max relevance feature is added because no redundancy is possible.
	//newFeatureIndex = getMaxRelevance(relevances, opts.classIndex);
	newFeatureIndex = getMaxRelevance(relevances, opts.classIndex, rawData.getFeaturesSize());
	selectedFeatures.push_back(newFeatureIndex);
	visited[newFeatureIndex] = 1;
	lastFeatureIndex = newFeatureIndex;

	// Modo de salvar cada caracteristica seleccionada.
	std::cout << newFeatureIndex << ",";

	//Pruebas de tiempo sobre el original:
	Timer timeMrmr;
	timeMrmr.start();
	//MRMR Original
	while (selectedFeatures.size() < rawData.getFeaturesSize() - 1 /*-1 because class is discarded*/ and selectedFeatures.size() < opts.selectedFeatures) {
		//acum = infinito inicializacion simple y logica para meter el primer valor directamente.
		acum = -std::numeric_limits<double>::infinity();
        for (int z = 0; z < threadsMaximos; ++z)
            acumThread[z] = -std::numeric_limits<double>::infinity();

		#pragma omp parallel for schedule(dynamic,sizeofBlock) private(mrmr)
		for (j = 0; j < rawData.getFeaturesSize(); ++j) {
			//If feature not in selected selectedFeatures -> se podrian usar conjuntos o eliminar de verdad la caracteristica -> ¿supone un ahorro sustancial?
			if (visited[j] == 0 && j != opts.classIndex) {
				redundances[j] += mutualInfo.get(lastFeatureIndex, j);
                mrmr = relevances[j] - (redundances[j] / selectedFeatures.size());
                if (mrmr > acumThread[omp_get_thread_num()]) {
                    acumThread[omp_get_thread_num()] = mrmr;
                    numFeature[omp_get_thread_num()] = j;
                }
			}
		}
        for (int z = 0; z < threadsMaximos;++z){
            if (acumThread[z] > acum){
                acum = acumThread[z];
                newFeatureIndex = numFeature[z];
            }
        }
		//Last feature doesn't prints comma.
		if ( (selectedFeatures.size() == (opts.selectedFeatures - 1)) or (selectedFeatures.size() == (rawData.getFeaturesSize() -2)) ){
			std::cout << newFeatureIndex << "\n";
		}else{
			std::cout << newFeatureIndex << ",";
		}
		selectedFeatures.push_back(newFeatureIndex);
		visited[newFeatureIndex] = 1;
		lastFeatureIndex = newFeatureIndex;
	}
	//Obtencion de tiempos estandar para Mrmr;Calculo de Relevancia;ProgramaTotal.
	double timeMeasureMrmr = timeMrmr.stop();
	printf("Tiempos medidos: tiempoRelevance: %f, tiempoMrmr %f\n",timeMeasureRel, timeMeasureMrmr);

    double timeTotal = tm.stop();
    printf("Tiempos totales medidos: %f\n",timeTotal);

    free(acumThread);free(numFeature);
    free(visited);free(relevances);free(redundances);
	rawData.destroy();prob.destroy();
	printf("\n");
	return (0);
}
