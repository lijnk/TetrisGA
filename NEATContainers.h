#ifndef NEATCONTAINER_H
#define NEATCONTAINER_H

#include <stdio.h>
#include <math.h>
#include "console.h"

#define POPULATION 125 //population count
#define MAX_SPECIES 50 //max species (will automatically adjust)
#define MAX_GENOMES 50 //max genomes (will automatically adjust)
#define MAX_GENES 500 //max genes (will automatically adjust)

#define POOL_TIMEOUT 200

#define DISJOINT_COEFF 1.0 //disjointed coefficient (for shared species function)
#define WEIGHT_COEFF 0.4 //weight coefficient (for shared species function)
#define SH_THRESHOLD 3.0 //shared species threshold (lower number means stricter rating)

#define INPUT_SIZE (10*18) + (10*18)
#define OUTPUT_SIZE 5

struct _neuron {
	int* inputList; //list of neuron inputs - these are genes
	int inputSize; //size of list
	float output; //neuron output
	int solved; //whether the neuron has been calculated
};

typedef struct _neuron Neuron;

struct _gene {
	int input; //input connection
	int output; //output connection
	float weight; //weight
	int innovation; //global innovation number (id)
	int enabled; //phenotype
};

typedef struct _gene Gene;

struct _genome {
	Gene** genes; //list of genes
	int geneSize; //number of genes
	int neuronSize; //number of neurons
	int maxGenes; //max number of genes
	int fitness; //how well this genome is doing (higher is better)
	Neuron** network; //network of neurons

	int id; //our id
};

typedef struct _genome Genome;

struct _species {
	Genome** genomes; //list of genome indexes
	int genomeSize; //number of genomes
	int maxGenomes; //max number of genomes
	int stagnate; //stagnation level
	int topFitness; //fitness level of best genome
	int avgFitness; //average fitness level

	int id; //our id
	int genomeIDCTR; //genome ID counter
};

typedef struct _species Species;

struct _pool {
	Species** species; //list of species
	int speciesCount; //number of species
	int maxSpecies; //max number of species
	int globalInnovation; //global innovation number
	int generation; //current generation
	int population; //current population

	int speciesIDCTR; //species ID counter

	int currentGenome; //current genome being evaluated
	int currentSpecies; //current species being evaluated
	int timeout; //timeout for current genome
	int alive; //how long the genome has been alive for
	int requestReset; //1 if pool requests game to restart; 0 to continue game

	float* input; //input for neurons
	int inputSize; //amount of input neurons
	float* output; //output for neurons
	int outputSize; //amount of output neurons
	float bias; //bias neuron
};

typedef struct _pool Pool;

void neatInitMemInfo(); //init memory info (for debugging purposes)
void neatMemReport(); //print memory report to console

void initPool(Pool** pool); //initialise new pool
void destroyPool(Pool** pool); //destroy the pool
void resizeSpeciesList(Pool** pool); //resize list
void adjustSpeciesList(Pool** pool); //adjust list after loading from file

void initSpecies(Species** species); //init species
void destroySpecies(Species** species); //destroy species
void resizeGenomeList(Species** species); //resize list
void adjustGenomeList(Species** species); //adjust list after loading from file
void insertSpecies(Pool** pool, Genome** genome); //insert new species
void removeSpecies(Pool** pool, int num); //remove a species

void initGenome(Genome** genome); //init genome
void destroyGenome(Genome** genome);//destroy genome
void copyGenome(Genome** g1, Genome** g2); //copy a genome from g1 to g2
void resizeGeneList(Genome** genome); //resize list
void adjustGeneList(Genome** genome); //adjust list after loading from file

void initGene(Gene** gene); //init gene
void destroyGene(Gene** gene); //destroy gene
void copyGene(Gene** g1, Gene** g2); //copy a gene from g1 to g2
void insertGene(Genome** genome, Gene** gene); //insert gene

void initNetwork(Genome** genome); //init network
void destroyNetwork(Genome** genome); //destroy network

void initNeuron(Neuron** neuron); //init neuron
void destroyNeuron(Neuron** neuron); //destroy neuron

#endif
