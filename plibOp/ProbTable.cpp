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

#include "ProbTable.h"
#include "omp.h"
#include <typeinfo>

ProbTable::ProbTable(RawData & rd): rawData(rd) {
	this->datasize = rawData.getDataSize();
	this->featuresSize = rawData.getFeaturesSize();
	this->valuesRange = rawData.getValuesRangeArray();
	this->table = (t_prob_table) malloc(featuresSize * sizeof(t_prob *));
	calculate();
	//calculate2();
}

ProbTable::~ProbTable() {
}

//Calculates the marginal probability table for each posible value in a feature.
//This table will be cached in memory to avoid repeating calculations.
void ProbTable::calculate() {
	Histogram histogram = Histogram(rawData);
	uint i = 0;
	uint j = 0;
	t_prob value = 0;
	t_histogram hist_data;
	#pragma omp parallel for
	for (i = 0; i < featuresSize; ++i) {
		table[i] = (t_prob*) malloc(valuesRange[i] * sizeof(t_prob));
		//Para cada feature se almacena su histograma
		hist_data = histogram.getHistogram(i);
		for (j = 0; j < valuesRange[i]; ++j) {
			/*para cada valor j de la feature i se calcula la probabilidad
			asociada a ese valor -> hist_data[j] = numero de ocurrencias de ese valor j;
			datasize = numero de samples*/
			value = (t_prob) hist_data[j] / (t_prob) datasize;
			table[i][j] = value;
		}
		free(hist_data);
	}
	/*Table contiene la probabilidad relativa a cada valor de cada feature*/
}

//Calculo alternativo de la tabla de probabilidades
void ProbTable::calculate2() {
	Histogram histogram = Histogram(rawData);
	uint i = 0;
	double* probabilidades;
	#pragma omp parallel for private(probabilidades)
	for (i = 0; i < featuresSize; ++i) {
		table2[i] = (t_prob*) malloc(valuesRange[i] * sizeof(t_prob));
		//Para cada feature se almacena su histograma
		probabilidades = histogram.getHistogram2(i);
		table2[i] = probabilidades;
		free(probabilidades);
	}
	printf("Termine\n");
	/*Table contiene la probabilidad relativa a cada valor de cada feature*/
}

/**
 * @param featureIndex The index of the feature you want to get the probability.
 * @return The selected feature prob value.
 */
double ProbTable::getProbability(uint index, t_data value) {
	return table[index][value];
}

void ProbTable::destroy() {
	/*for (uint i = 0; i < featuresSize; i++) {
		free(table[i]);
	}
	free(table);*/
}
