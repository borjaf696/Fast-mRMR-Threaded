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

#include "RawData.h"
#include "omp.h"
#include <string.h>


/**
 * Constructor that creates a rawData object.
 *
 * @param data_table this is a matrix of bytes containing the translated csv data.
 * @param ds the number of data samples.
 * @param fs the number of features that each sample has.
 */
RawData::RawData(char * filename) {
	dataFile = fopen(filename, "rb"); //fopen devuelve un puntero al fichero filename
	calculateDSandFS();
	loadData();
	calculateVR(); //Esto es todavia un misterio.
}

RawData::~RawData() {

}

size_t result;
/**
 *
 */
void RawData::destroy() {
	free(valuesRange);
	free(data);
}

/**Calculates
 *	DataSize: Number of patterns or samples
 *  FeaturesSize: Number of features
 */
/*Metodo que calcula el numero de Features y el numero de samples. (Para esto lee los 2 primeros valores que se encuentran
en el fichero de datos, .mrmr que se han colocado al principio FS y el DS -> luego hay que saltarlos de ahi el avance de
8 posiciones)*/
void RawData::calculateDSandFS() {
	uint featuresSizeBuffer[1];
	uint datasizeBuffer[1];
	//fread avanza el puntero en el fichero.
	result = fread(datasizeBuffer, sizeof(uint), 1, dataFile);
	if (result != 1)
		{fputs ("Reading error bufferDatafile\n",stderr); exit (3);}
	result = fread(featuresSizeBuffer, sizeof(uint), 1, dataFile);
	if (result != 1)
		{fputs ("Reading error featuresDatafile\n",stderr); exit (3);}
	datasize = datasizeBuffer[0];
	featuresSize = featuresSizeBuffer[0];
}

/*Rellena el atributo data con la informacion del fichero incluido en dataFile. Usa el tipo t_data -> que no es mas que
que un caracter. Se rellena data la info con ese valor para no limitarse a valores numericos lo que esta realmente bien.
dataFile parece que guarda la informacion contigua relativa a las features -> es decir el valor 0 leido contiene el valor
de la feature 0 para el sample 0, el valor 1 el valor 0 para la feature 1 para el sample 0. Es decir, parece que esta
dispuesto de modo FeaturesXData -> la traspuesta de la que se pide.*/
void RawData::loadData() {
	uint i, j;
	t_data buffer[1];
	data = (t_data*) calloc(featuresSize, sizeof(t_data) * datasize);
	//Avanza 8 bits en el fichero de datos, para saltar metaData
	fseek(dataFile, 8, 0);
	for (i = 0; i < datasize; i++) {
		for (j = 0; j < featuresSize; j++) {
			//Lee un bit de informacion
			result = fread(buffer, sizeof(t_data), 1, dataFile);
			if (result != 1)
				{fputs ("Reading error loadFile\n",stderr); exit (3);}
			//Se va rellenando data -> para 1 mismo sample los distintos valores que toma para cada feature
			data[j * datasize + i] = buffer[0];
		}
	}
}

/**
 * Calculates how many different values each feature has.
 */
/*Si que cuenta los valores diferentes -> caso extremo 1 solo valor 16, 100 veces. Vr llegara como maximo
a 16 -> pues vr == valor. Pero, y si lees primero 100 y luego veinte veces el valor 16 -> tienes 2 valores
y vas a crear un histograma con 16 valores. Hacer mi propia version*/
void RawData::calculateVR() {
	uint i, j;
	t_data dataReaded;
	uint vr;
	valuesRange = (uint*) calloc(featuresSize, sizeof(uint));
	#pragma omp parallel for
	for (i = 0; i < featuresSize; i++) {
		vr = 0;
		for (j = 0; j < datasize; j++) {
			//Ahora avanza por filas mirando los valores de cada feature: i*datasize+j -> cada iteracion incrementa 1 y no datasize
			dataReaded = data[i * datasize + j];
			if (dataReaded > vr) {
				vr++;
			}
		}
		valuesRange[i] = vr + 1;
	}
}

uint RawData::getDataSize() {
	return datasize;
}

uint RawData::getFeaturesSize() {
	return featuresSize;
}

/**
 * Returns how much values has a features FROM 1 to VALUES;
 */
uint RawData::getValuesRange(uint index) {
	return valuesRange[index];
}

/**
 * Returns an array with the number of possible values for each feature.
 */
uint * RawData::getValuesRangeArray() {
	return this->valuesRange;
}

/**
 * Returns a vector containing a feature. Realmente devuelve un puntero
 */
t_feature RawData::getFeature(int index) {
	return data + index * datasize;
	/*Como se apuntaba anteriormente cada feature contiene su informacion de manera contigua en memoria.*/
}
