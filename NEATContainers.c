#include "NEATContainers.h"

static int poolAlloc;
static int poolInputAlloc;
static int poolOutputAlloc;

static int speciesListAlloc;
static int speciesAlloc;

static int genomeListAlloc;
static int genomeAlloc;

static int geneListAlloc;
static int geneAlloc;

static int neuronAlloc;
static int networkAlloc;
static int neuronInputAlloc;

//start a debug thing
void neatInitMemInfo()
{
	poolAlloc = 0;
	poolInputAlloc = 0;
	poolOutputAlloc = 0;
	speciesListAlloc = 0;
	speciesAlloc = 0;
	genomeListAlloc = 0;
	genomeAlloc = 0;
	geneListAlloc = 0;
	geneAlloc = 0;
	neuronAlloc = 0;
	networkAlloc = 0;
	neuronInputAlloc = 0;
}

//print debug info
void neatMemReport()
{
	char msg[256];
	consolePrint("***** Mem Stats *****", CON_MSG);
	sprintf(msg, "Pool: %d", poolAlloc);
	consolePrint(msg, CON_MSG);
	sprintf(msg, "Pool Input: %d", poolInputAlloc);
	consolePrint(msg, CON_MSG);
	sprintf(msg, "Pool Output: %d", poolOutputAlloc);
	consolePrint(msg, CON_MSG);
	sprintf(msg, "Species List: %d", speciesListAlloc);
	consolePrint(msg, CON_MSG);
	sprintf(msg, "Species: %d", speciesAlloc);
	consolePrint(msg, CON_MSG);
	sprintf(msg, "Genome List: %d", genomeListAlloc);
	consolePrint(msg, CON_MSG);
	sprintf(msg, "Genomes: %d", genomeAlloc);
	consolePrint(msg, CON_MSG);
	sprintf(msg, "Gene List: %d", geneListAlloc);
	consolePrint(msg, CON_MSG);
	sprintf(msg, "Genes: %d", geneAlloc);
	consolePrint(msg, CON_MSG);
	sprintf(msg, "Neurons: %d", neuronAlloc);
	consolePrint(msg, CON_MSG);
	sprintf(msg, "Networks: %d", networkAlloc);
	consolePrint(msg, CON_MSG);
	sprintf(msg, "Neuron Inputs: %d", neuronInputAlloc);
	consolePrint(msg, CON_MSG);
	consolePrint("---------------------", CON_MSG);
}

void initPool(Pool** pool)
{
	*(pool) = (Pool*)calloc(1, sizeof(Pool));
	poolAlloc++;

	(*pool)->species = (Species**)calloc(MAX_SPECIES, sizeof(Species*));
	speciesListAlloc++;

	(*pool)->maxSpecies = MAX_SPECIES;
	(*pool)->speciesCount = 0;
	(*pool)->globalInnovation = 0;
	(*pool)->generation = 0;
	(*pool)->population = POPULATION;

	(*pool)->currentGenome = 0;
	(*pool)->currentSpecies = 0;
	(*pool)->timeout = POOL_TIMEOUT;
	(*pool)->alive = 0;
	(*pool)->requestReset = 0;

	(*pool)->input = (float*)calloc(INPUT_SIZE, sizeof(float));
	poolInputAlloc++;

	(*pool)->output = (float*)calloc(OUTPUT_SIZE, sizeof(float));
	poolOutputAlloc++;

	(*pool)->inputSize = INPUT_SIZE;
	(*pool)->outputSize = OUTPUT_SIZE;
	(*pool)->bias = 1.f;

	(*pool)->speciesIDCTR = 0;
	(*pool)->topFitness = 0;
}

void destroyPool(Pool** pool)
{
	free((*pool)->input);
	poolInputAlloc--;

	free((*pool)->output);
	poolOutputAlloc--;

	for(int i = 0; i < (*pool)->maxSpecies; i++)
	{
		if((*pool)->species[i] == NULL) continue;
		destroySpecies(&((*pool)->species[i]));
	}

	free((*pool)->species);
	speciesListAlloc--;

	free(*pool);
	poolAlloc--;
	(*pool) = NULL;
}

//general resize for having too many species
void resizeSpeciesList(Pool** pool)
{
	Species** newList = (Species**)calloc(MAX_SPECIES + (*pool)->maxSpecies, sizeof(Species*));
	speciesListAlloc++;

	for(int i = 0; i < (*pool)->maxSpecies; i++)
	{
		newList[i] = (*pool)->species[i];
	}
	free((*pool)->species);
	speciesListAlloc--;

	(*pool)->species = newList;
	(*pool)->maxSpecies += MAX_SPECIES;
}

//readjusts species list after loading
void adjustSpeciesList(Pool** pool)
{
	Species** newList = (Species**)calloc((*pool)->maxSpecies, sizeof(Species*));
	speciesListAlloc++;

	for(int i = 0; i < MAX_SPECIES; i++)
	{
		newList[i] = (*pool)->species[i];
	}
	free((*pool)->species);
	speciesListAlloc--;

	(*pool)->species = newList;
}

void initSpecies(Species** species)
{
	(*species) = (Species*)calloc(1, sizeof(Species));
	speciesAlloc++;

	(*species)->genomes = (Genome**)calloc(MAX_GENOMES, sizeof(Genome*));
	genomeListAlloc++;

	(*species)->genomeSize = 0;
	(*species)->maxGenomes = MAX_GENOMES;

	(*species)->stagnate = 0;
	(*species)->topFitness = 0;
	(*species)->avgFitness = 0;

	(*species)->id = 0;
	(*species)->genomeIDCTR = 0;
}

void destroySpecies(Species** species)
{
	for(int i = 0; i < (*species)->maxGenomes; i++)
	{
		if((*species)->genomes[i] == NULL) continue;
		destroyGenome(&((*species)->genomes[i]));
	}
	free((*species)->genomes);
	genomeListAlloc--;
	free(*species);
	speciesAlloc--;
	(*species) = NULL;
}

//resize genome list if it's too big
void resizeGenomeList(Species** species)
{
	Genome** newList = (Genome**)calloc(MAX_GENOMES + (*species)->maxGenomes, sizeof(Genome*));
	genomeListAlloc++;

	for(int i = 0; i < (*species)->maxGenomes; i++)
	{
		newList[i] = (*species)->genomes[i];
	}

	free((*species)->genomes);
	genomeListAlloc--;
	(*species)->genomes = newList;
	(*species)->maxGenomes += MAX_GENOMES;
}

//readjust genome list after loading
void adjustGenomeList(Species** species)
{
	Genome** newList = (Genome**)calloc((*species)->maxGenomes, sizeof(Genome*));
	genomeListAlloc++;

	for(int i = 0; i < MAX_GENOMES; i++)
	{
		newList[i] = (*species)->genomes[i];
	}

	free((*species)->genomes);
	genomeListAlloc--;
	(*species)->genomes = newList;
}

int sharedSpecies(Genome** g1, Genome** g2)
{
	float D, W, N; //disjoint, weights, number

	//if they both have 0, they're equal
	if((*g1)->geneSize == 0 && (*g2)->geneSize == 0) return 1;

	//make g1 be the genome with the most genes
	if((*g1)->geneSize < (*g2)->geneSize)
	{
		Genome** t = g1;
		g1 = g2;
		g2 = g1;
	}

	//grab initial disjoint, largest N values and init weight
	D = (float)((*g1)->geneSize - (*g2)->geneSize);
	N = (float)((*g1)->geneSize);
	W = 0.f;

	//since innovation numbers are incremental in the list, we can exploit that
	int gp = 0; //temp pointer
	for(int i = 0; i < (*g1)->geneSize; i++)
	{
		if(gp >= (*g2)->geneSize) break; //if we've exhausted g2's gene list
		if((*g2)->genes[gp]->innovation > (*g1)->genes[i]->innovation) //found a disjoint. increment i
		{
			D += 1.f;
			continue;
		}
		else if((*g2)->genes[gp]->innovation < (*g1)->genes[i]->innovation) //found a disjoint. increment gp (decrement i due to the for loop incrementing it)
		{
			D += 1.f;
			gp++;
			i--;
			continue;
		}
		else //found a match. increase weight
		{
			W += fabsf((*g1)->genes[gp]->weight - (*g2)->genes[i]->weight);
			gp++;
			continue;
		}
	}

	return (((DISJOINT_COEFF * D) / N) + (WEIGHT_COEFF * W)) < SH_THRESHOLD;
}

void insertSpecies(Pool** pool, Genome** genome)
{
	//ridiculously long pointer redirections
	//Basically find the species the genome should belong to using the sharedSpecies function (inside the for loop)
	//if there isn't an appropriate species, create a new one (after the for loop)
	for(int i = 0; i < (*pool)->speciesCount; i++)
	{
		if(sharedSpecies(genome, &((*pool)->species[i]->genomes[0])))
		{
			if((*pool)->species[i]->genomeSize >= (*pool)->species[i]->maxGenomes)
				resizeGenomeList(&((*pool)->species[i]));

			copyGenome(genome, &((*pool)->species[i]->genomes[(*pool)->species[i]->genomeSize]));
			(*pool)->species[i]->genomes[(*pool)->species[i]->genomeSize]->id = (*pool)->species[i]->genomeIDCTR++;
			(*pool)->species[i]->genomeSize++;
			return;
		}
	}

	if((*pool)->speciesCount >= (*pool)->maxSpecies)
		resizeSpeciesList(pool);

	int spcnt = (*pool)->speciesCount; //lets make these lines shorter. they're already confusing enough as it is

	initSpecies(&((*pool)->species[spcnt]));
	copyGenome(genome, &((*pool)->species[spcnt]->genomes[(*pool)->species[spcnt]->genomeSize]));

	(*pool)->species[spcnt]->id = (*pool)->speciesIDCTR++;
	(*pool)->species[spcnt]->genomes[(*pool)->species[spcnt]->genomeSize]->id = (*pool)->species[spcnt]->genomeIDCTR++;
	(*pool)->species[spcnt]->genomeSize++;
	(*pool)->speciesCount++;
}

//remove the species from the pool list (for use in culling)
void removeSpecies(Pool** pool, int num)
{
	destroySpecies(&((*pool)->species[num]));

	for(int i = num + 1; i < (*pool)->speciesCount; i++)
	{
		(*pool)->species[i-1] = (*pool)->species[i];
	}
	(*pool)->speciesCount -= 1;
}

void initGenome(Genome** genome)
{
	(*genome) = (Genome*)calloc(1, sizeof(Genome));
	genomeAlloc++;

	(*genome)->genes = (Gene**)calloc(MAX_GENES, sizeof(Gene*));
	geneListAlloc++;

	(*genome)->network = NULL;
	(*genome)->geneSize = 0;
	(*genome)->neuronSize = INPUT_SIZE + OUTPUT_SIZE + 1;
	(*genome)->maxGenes = MAX_GENES;
	(*genome)->fitness = 0;
	(*genome)->id = 0;
}

void destroyGenome(Genome** genome)
{
	if(*genome == NULL) return;

	for(int i = 0; i < (*genome)->maxGenes; i++)
	{
		if((*genome)->genes[i] == NULL) continue;
		destroyGene(&((*genome)->genes[i]));
	}
	destroyNetwork(genome);
	free((*genome)->genes);
	geneListAlloc--;
	free(*genome);
	genomeAlloc--;
	(*genome) = NULL;
}

//copy from g1 to g2
void copyGenome(Genome** g1, Genome** g2)
{
	initGenome(g2);
	(*g2)->fitness = (*g1)->fitness;
	(*g2)->geneSize = (*g1)->geneSize;
	(*g2)->maxGenes = (*g1)->maxGenes;
	adjustGeneList(g2);
	(*g2)->network = NULL;
	(*g2)->neuronSize = (*g1)->neuronSize;

	for(int i = 0; i < (*g1)->maxGenes; i++)
	{
		copyGene(&((*g1)->genes[i]), &((*g2)->genes[i]));
	}
}

//resize if the gene list is too big
void resizeGeneList(Genome** genome)
{
	Gene** newList = (Gene**)calloc(MAX_GENES + (*genome)->maxGenes, sizeof(Gene*));
	geneListAlloc++;

	for(int i = 0; i < (*genome)->maxGenes; i++)
	{
		newList[i] = (*genome)->genes[i];
	}

	free((*genome)->genes);
	geneListAlloc--;
	(*genome)->genes = newList;
	(*genome)->maxGenes += MAX_GENES;
}

//adjust gene list from loading
void adjustGeneList(Genome** genome)
{
	Gene** newList = (Gene**)calloc((*genome)->maxGenes, sizeof(Gene*));
	geneListAlloc++;

	for(int i = 0; i < MAX_GENES; i++)
	{
		newList[i] = (*genome)->genes[i];
	}

	free((*genome)->genes);
	geneListAlloc--;
	(*genome)->genes = newList;
}

void initGene(Gene** gene)
{
	(*gene) = (Gene*)calloc(1, sizeof(Gene));
	geneAlloc++;
	(*gene)->input = 0;
	(*gene)->output = 0;
	(*gene)->weight = 0.f;
	(*gene)->innovation = 0;
	(*gene)->enabled = 0;
}

void destroyGene(Gene** gene)
{
	if(*gene == NULL) return;

	free(*gene);
	geneAlloc--;
	(*gene) = NULL;
}

//copy gene from g1 to g2
void copyGene(Gene** g1, Gene** g2)
{
	if(*g1 == NULL) return;
	initGene(g2);
	(*g2)->enabled = (*g1)->enabled;
	(*g2)->innovation = (*g1)->innovation;
	(*g2)->input = (*g1)->input;
	(*g2)->output = (*g1)->output;
	(*g2)->weight = (*g1)->weight;
}

void insertGene(Genome** genome, Gene** gene)
{
	//insert new gene, sorted by innovation number
	if((*genome)->geneSize >= (*genome)->maxGenes) resizeGeneList(genome);

	for(int i = 0; i < (*genome)->geneSize; i++)
	{
		if((*genome)->genes[i]->innovation == (*gene)->innovation) return;
		if((*genome)->genes[i]->innovation > (*gene)->innovation)
		{
			Gene* temp1 = *gene;
			Gene* temp2;
			for(int j = i; j <= (*genome)->geneSize; j++)
			{
				temp2 = (*genome)->genes[j];
				(*genome)->genes[j] = temp1;
				temp1 = temp2;
			}
			(*genome)->geneSize++;
			return;
		}
	}
	(*genome)->genes[(*genome)->geneSize] = *gene;
	(*genome)->geneSize++;
}

void initNetwork(Genome** genome)
{
	//init all neurons
	(*genome)->network = (Neuron**)calloc((*genome)->neuronSize, sizeof(Neuron*));
	networkAlloc++;
	for(int i = 0; i < (*genome)->neuronSize; i++)
	{
		initNeuron(&((*genome)->network[i]));
	}

	//set input sizes
	for(int i = 0; i < (*genome)->geneSize; i++)
	{
		if(!(*genome)->genes[i]->enabled) continue; //ignore disabled genes
		int output = (*genome)->genes[i]->output;

		if(output >= (*genome)->neuronSize) //Some edge case with a mutation that I have no idea what it is right now
		{
			//printf("ERROR: Invalid Gene Found (%d/%d)\n", output, (*genome)->neuronSize);
			destroyNetwork(genome);
			return;
		}
		(*genome)->network[output]->inputSize++;
	}

	//allocate their inputs
	for(int i = 0; i < (*genome)->neuronSize; i++)
	{
		int inputSize = (*genome)->network[i]->inputSize;
		(*genome)->network[i]->inputList = (int*)calloc(inputSize, sizeof(int));
		neuronInputAlloc++;
	}

	//set values
	int* count = (int*)calloc((*genome)->neuronSize, sizeof(int)); //used as a counter for input lists
	for(int i = 0; i < (*genome)->geneSize; i++)
	{
		if(!(*genome)->genes[i]->enabled) continue; //ignore disabled genes
		int output = (*genome)->genes[i]->output;
		(*genome)->network[output]->inputList[count[output]] = i;
		count[output]++;
	}
	free(count);
}

void destroyNetwork(Genome** genome)
{
	if(*genome == NULL || (*genome)->network == NULL) return;

	for(int i = 0; i < (*genome)->neuronSize; i++)
	{
		destroyNeuron(&((*genome)->network[i]));
	}
	free((*genome)->network);
	networkAlloc--;
	(*genome)->network = NULL;
}

void initNeuron(Neuron** neuron)
{
	(*neuron) = (Neuron*)calloc(1, sizeof(Neuron));
	neuronAlloc++;
	(*neuron)->inputList = NULL;
	(*neuron)->inputSize = 0;
	(*neuron)->output = 0.f;
	(*neuron)->solved = 0;
}

void destroyNeuron(Neuron** neuron)
{
	if(*neuron == NULL) return;

	if((*neuron)->inputList != NULL)
	{
		free((*neuron)->inputList);
		neuronInputAlloc--;
	}

	free(*neuron);
	neuronAlloc--;
	(*neuron) = NULL;
}
