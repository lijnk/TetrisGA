#include "NEAT.h"

#define WEIGHT_MUTATE 0.8 //p(weight mutation)
#define PERTURB_WEIGHT 0.9 //p(perturb chance)

#define CROSSOVER_REPRO 0.25 //p(asexual reproduction)
#define REENABLE_GENE 0.25 //p(reenabling a gene)
#define INTERSPECIES_MATE 0.001 //p(interspecies mating)

#define NODE_MUTATE 0.03 //p(new node)
#define CONNECT_MUTATE 0.05 //p(new connection)

#define MAX_STAGNATE 15 //max stagnate level before species is removed

//Internal NEAT functions
void updatePopulation(Pool* pool); //updates population number

void updateSpeciesData(Species* species); //updates stagnation and fitness info

void newGenome(Pool* pool, Genome** genome); //creates dummy genome

float sigmoid(float x); //for evaluating neurons easier
float recursiveEval(Pool* pool, Genome* genome, Neuron* neuron); //recursively evaluate network
void evaluateNetwork(Pool* pool, Genome* genome); //top level network evaluate (calls recursiveEval)
int networkCC(Genome* genome, int check, int in); //check network for loops

void crossover(Genome* g1, Genome* g2, Genome** child); //breed two genomes
void breed(Pool* pool, Species* species, Genome** child); //crossover or copy/mutate

int chance(float x); //return true via a probability of x
float randWeight(); //return a random weight

//mutation code
void mutateWeight(Genome* genome);
void mutateConnection(Pool* pool, Genome* genome, int isBias);
void mutateNode(Pool* pool, Genome* genome);
void mutate(Pool* pool, Genome* genome);

//cull code
void cullHalf(Pool* pool);
void cullAllButOne(Pool* pool);
void cullStagnate(Pool* pool);
void newGeneration(Pool* pool);

//updates the population count
void updatePopulation(Pool* pool)
{
	pool->population = 0;
	for(int sp = 0; sp < pool->speciesCount; sp++)
	{
		pool->population += pool->species[sp]->genomeSize;
	}
}

//updates top fitness, avg fitness and stagnation level
void updateSpeciesData(Species* species)
{
	int max = 0;
	int sum = 0;
	//grab max fitness
	for(int i = 0; i < species->genomeSize; i++)
	{
		sum += species->genomes[i]->fitness;
		if(species->genomes[i]->fitness > max)
			max = species->genomes[i]->fitness;
	}
	species->avgFitness = sum / species->genomeSize; //set the average

	//this part might be why the system does not grow properly
	if(max > species->topFitness) //set stagnation level and max fitness
	{
		species->stagnate = 0;
		species->topFitness = max;
	}
	else
	{
		species->stagnate++;
	}
}

//creates new genome
void newGenome(Pool* pool, Genome** genome)
{
	initGenome(genome);
	mutate(pool, *genome);
}

//sigmoid function in evaluating neuron weights
float sigmoid(float x)
{
	return (1.f / (expf(-4.9f * x))) - 1.f;
}

//recursively evaluate neuron stream
float recursiveEval(Pool* pool, Genome* genome, Neuron* neuron)
{
	float sum = 0.f;
	for(int i = 0; i < neuron->inputSize; i++)
	{
		//next neuron is the input neuron of the gene inside the neuron's input list
		//input neuron is inside of the gene; which is inside the neuron's input list
		Gene* gene = genome->genes[neuron->inputList[i]];
		Neuron* next = genome->network[gene->input];

		if(pool->input[gene->input] > 0)
		{
			int t = 0;
		}

		if(gene->input == pool->inputSize) sum += pool->bias * gene->weight; //check bias case
		else if(gene->input < pool->inputSize) sum += pool->input[gene->input] * gene->weight; //check input neuron case
		else if(next->solved) sum += next->output; //check solved case
		else sum += recursiveEval(pool, genome, next) * gene->weight; //recurse
	}

	//so we don't apply the sigmoid to input neurons
	if(neuron->inputSize > 0)
	{
		sum = sigmoid(sum);
	}
	neuron->output = sum;
	neuron->solved++;
	return sum;
}

//network evaluation (resolves the inputs to outputs)
void evaluateNetwork(Pool* pool, Genome* genome)
{	
	//reset network
	for(int i = pool->inputSize + 1; i < genome->neuronSize; i++)
	{
		genome->network[i]->solved = 0;
	}

	//recursively evaluate outputs
	for(int i = pool->inputSize + 1; i < genome->neuronSize; i++)
	{
		recursiveEval(pool, genome, genome->network[i]);
	}

	//apply the outputs to the pool's output
	for(int i = 0; i < pool->outputSize; i++)
	{
		pool->output[i] = genome->network[i + (pool->inputSize + 1)]->output;
	}
}

//checks if a gene from check to in will create a circle. returns 1 if loop is found
int networkCC(Genome* genome, int check, int in) {
	if(in == check) return 1;

	for(int i = 0; i < genome->network[in]->inputSize; i++)
	{
		int next = genome->genes[genome->network[in]->inputList[i]]->input;
		//printf("Chk: %d, In: %d, NSz: %d, iter: %d, InSz: %d\n", check, in, genome->neuronSize, i, genome->network[in]->inputSize);
		if(networkCC(genome, check, next)) return 1;
	}

	return 0;
}

void crossover(Genome* g1, Genome* g2, Genome** child)
{
	//g1 must have higher fitness (must be better)
	if(g2->fitness > g1->fitness)
	{
		Genome* temp = g2;
		g2 = g1;
		g1 = g2;
	}
	copyGenome(&g1, child);
	int gp = 0; //gene pointer
	for(int i = 0; i < (*child)->geneSize; i++, gp++)
	{
		if(gp >= g2->geneSize) break;

		if((*child)->genes[i]->innovation == g2->genes[gp]->innovation)
		{
			if(!g2->genes[gp]->enabled) continue; //g2's disabled genes don't get passed
			if(chance(0.5f)) //randomely choose gene weight
			{
				(*child)->genes[i]->weight = g2->genes[gp]->weight;
			}
			if(!(*child)->genes[i]->enabled) //whether we renable a disabled gene (is a mutation, but not really)
			{
				if(chance(REENABLE_GENE))
				{
					(*child)->genes[i]->enabled = 1;
				}
				else
				{
					(*child)->genes[i]->enabled = 0;
				}
			}
		}
		else if((*child)->genes[i]->innovation < g2->genes[gp]->innovation)
			gp--; //hold back gp if i needs to catch up
		else
			i--; //hold back i if gp needs to catch up
	}
}

//breeds species
void breed(Pool* pool, Species* species, Genome** child)
{
	if(chance(CROSSOVER_REPRO))
	{
		//crossover
		Genome* g1 = species->genomes[rand() % species->genomeSize];
		Genome* g2 = species->genomes[rand() % species->genomeSize];
		crossover(g1, g2, child);
	}
	else
	{
		//mutate
		Genome* g1 = species->genomes[rand() % species->genomeSize];
		copyGenome(&g1, child);
		mutate(pool, *child);
	}
}

//returns true based on given probability
int chance(float x)
{
	return ((float)rand()/(float)RAND_MAX) < x;
}

//give a random weight (for gene weights)
float randWeight()
{
	return (((float)rand()/(float)RAND_MAX) * 2.f) - 1.f;
}

//mutates gene weights
void mutateWeight(Genome* genome)
{
	//iterate through all genes and set new random weight
	//also perturb chance on some (extra weighting)
	for(int i = 0; i < genome->geneSize; i++)
	{
		if(chance(PERTURB_WEIGHT))
		{
			//agitate by adding a random value from -0.1 to 0.1
			genome->genes[i]->weight += randWeight() * .1f;
		}
		else
		{
			//non-agitated will simply be reset with a random value between -1.f and 1.f
			genome->genes[i]->weight = randWeight();
		}
	}
}

//inserts connection
void mutateConnection(Pool* pool, Genome* genome, int isBias)
{
	//get two random neuron connection IDs
	int g1, g2;
	int ng = genome->neuronSize - pool->inputSize - pool->outputSize - 1; //neurons made by the genes

	g2 = (rand() % (ng + pool->outputSize)) + pool->inputSize + 1; //g2 can't be an in

	//if isBias is true, set g1 to the ID of bias
	if(isBias)
	{
		g1 = pool->inputSize;
	}
	else
	{
		g1 = rand() % (pool->inputSize + ng + 1); //g1 can't be an out
		
		//adjust to skip output space
		if(g1 > pool->inputSize)
		{
			g1 += pool->outputSize;
		}
	}

	if(g1 == g2) return; //no connecting to self

	//return if the gene connection exists
	for(int i = 0; i < genome->geneSize; i++)
	{
		if(genome->genes[i]->input == g1 && genome->genes[i]->output == g2) return;
	}

	//something broke
	initNetwork(&genome);
	if(genome->network == NULL)
	{
		destroyNetwork(&genome);
		return;
	}

	//return if the new gene will create a loop
	if(networkCC(genome, g2, g1))
	{
		destroyNetwork(&genome);
		return;
	}
	destroyNetwork(&genome);

	//create the new gene with the new connection, innovation, etc
	Gene* temp = NULL;
	initGene(&temp);
	temp->input = g1;
	temp->output = g2;
	temp->innovation = pool->globalInnovation++;
	temp->enabled = 1;
	temp->weight = randWeight();
	insertGene(&genome, &temp);
}

//inserts neuron
void mutateNode(Pool* pool, Genome* genome)
{
	//if there's no nodes to mutate, return
	if(genome->geneSize == 0) return;

	//Grab a random gene
	int mg = rand() % genome->geneSize;

	//if disabled, return
	if(!genome->genes[mg]->enabled) return;
	genome->genes[mg]->enabled = 0;

	//copy gene into g1, set output as a new neuron and create new weight
	Gene* g1;
	initGene(&g1);
	g1->input = genome->genes[mg]->input;
	g1->output = genome->neuronSize;
	g1->weight = 1.f;
	g1->innovation = pool->globalInnovation++;
	g1->enabled = 1;
	insertGene(&genome, &g1);

	//copy gene into g2, set input as new neuron
	Gene* g2;
	initGene(&g2);
	g2->output = genome->genes[mg]->output;
	g2->input = genome->neuronSize++;
	g2->weight = genome->genes[mg]->weight;
	g2->innovation = pool->globalInnovation++;
	g2->enabled = 1;
	insertGene(&genome, &g2);
}

//mutate genome
void mutate(Pool* pool, Genome* genome)
{
	//all our mutation functions
	//mutate weight
	if(chance(WEIGHT_MUTATE))
	{
		mutateWeight(genome);
	}
	//mutate connection
	if(chance(CONNECT_MUTATE))
	{
		mutateConnection(pool, genome, 0);
	}
	//mutate gene
	if(chance(NODE_MUTATE))
	{
		mutateNode(pool, genome);
	}
}

//culls worst half of species
void cullHalf(Pool* pool)
{
	//iterate through and remove the worst half of species
	for(int sp = 0; sp < pool->speciesCount/2; sp++)
	{
		int min = sp;
		for(int i = sp; i < pool->speciesCount; i++)
		{
			if(pool->species[i]->avgFitness < min)
			{
				min = i;
			}
		}
		removeSpecies(&pool, min); //genocide
	}
}

//culls all but one in each species
void cullAllButOne(Pool* pool)
{
	//go through each species
	for(int sp = 0; sp < pool->speciesCount; sp++)
	{
		Species* ptr = pool->species[sp];
		int max = 0;
		for(int i = 0; i < ptr->genomeSize; i++) //find the best fitness genome
		{
			if(ptr->genomes[i]->fitness > ptr->genomes[max]->fitness)
				max = i; 
		}

		//set the max to the beginning of the list if it isn't already
		if(max != 0)
		{
			destroyGenome(&(ptr->genomes[0]));
			ptr->genomes[0] = ptr->genomes[max];
			ptr->genomes[max] = NULL;
			ptr->genomeSize--;
		}

		for(int i = 1; i < ptr->genomeSize; i++) //kill everybody else
		{
			destroyGenome(&(ptr->genomes[i]));
		}
		ptr->genomeSize = 1;
	}
}

//cull stagnated species
void cullStagnate(Pool* pool)
{
	if(pool->speciesCount < 2) return; //don't kill last species

	for(int sp = 0; sp < pool->speciesCount; sp++)
	{
		if(pool->species[sp]->stagnate >= MAX_STAGNATE)
		{
			removeSpecies(&pool, sp); //stagnated species
		}
	}
}

//creates new generation
void newGeneration(Pool* pool)
{
	//temporary holding pool for next generation
	Genome** nextGen = (Genome**)calloc(POPULATION, sizeof(Genome*));
	int ngc = 0;

	//update the species fitness/stagnation data; aka prepping for mass extinction
	int sum = 0; //this is for breeding
	for(int sp = 0; sp < pool->speciesCount; sp++)
	{
		updateSpeciesData(pool->species[sp]);
		sum += pool->species[sp]->avgFitness;
	}
	//kill off the bad ones
	cullHalf(pool);
	cullStagnate(pool);

	//breed top species
	for(int sp = 0; sp < pool->speciesCount; sp++)
	{
		int bc = (int)((float)pool->species[sp]->avgFitness / (float)sum * (float)POPULATION);
		for(int i = 0; i < bc; i++)
		{
			breed(pool, pool->species[sp], &(nextGen[ngc++]));
		}
	}

	//prep for next generation
	cullAllButOne(pool);
	updatePopulation(pool);

	//breed until we reach the population cap
	for(int i = pool->population + ngc; i < POPULATION; i++)
	{
		if(chance(INTERSPECIES_MATE)) //interspecies mating (something might come of it, probably not though)
		{
			Species* s1 = pool->species[rand() % pool->speciesCount];
			Species* s2 = pool->species[rand() % pool->speciesCount];
			Genome* g1 = s1->genomes[rand() % s1->genomeSize];
			Genome* g2 = s2->genomes[rand() % s2->genomeSize];
			crossover(g1, g2, &(nextGen[ngc++]));
		}
		else
		{
			Species* s1 = pool->species[rand() % pool->speciesCount];
			breed(pool, s1, &(nextGen[ngc++]));
		}
	}

	//categorise into species
	for(int i = 0; i < ngc; i++)
	{
		insertSpecies(&pool, &(nextGen[i]));
		destroyGenome(&(nextGen[i]));
	}

	updatePopulation(pool);
	pool->generation++;
	free(nextGen);
}

void neatSave(Pool* pool, const char* filename)
{
	updatePopulation(pool);

	consolePrint("Saving...", CON_MSG);

	if(filename == NULL)
	{
		consolePrint("Error saving file", CON_ERR);
		return;
	}

	int spcount = 0, gmcount = 0, gncount = 0;
	//Jump values
	unsigned int SPJMP, GMJMP, GNJMP, GNDJMP;

	//Block buffers
	unsigned int* PBlock = (unsigned int*)calloc(PBLOCKSIZE, sizeof(unsigned int));
	unsigned int* SPBlock = (unsigned int*)calloc(pool->speciesCount*SPBLOCKSIZE, sizeof(unsigned int));
	unsigned int* GMBlock = (unsigned int*)calloc(pool->population*GMBLOCKSIZE, sizeof(unsigned int));
	unsigned int* GNBlock = NULL; //to be determined later
	unsigned int* GNDBlock = (unsigned int*)calloc(pool->globalInnovation*GNDBLOCKSIZE, sizeof(unsigned int));

	//set jump locations
	SPJMP = PBLOCKSIZE;
	GMJMP = SPJMP + (pool->speciesCount*SPBLOCKSIZE);
	GNJMP = GMJMP + (pool->population*GMBLOCKSIZE);

	PBlock[0] = MAGICBLOCK;
	PBlock[1] = SPJMP;
	PBlock[2] = GMJMP;
	PBlock[3] = GNJMP;

	//size counters
	unsigned int GMSZ = 0;
	unsigned int GNSZ = 0;

	//write the species and genomes to their buffers
	for(int i = 0; i < pool->speciesCount; i++)
	{
		unsigned int value;
		//fill SP block data
		memcpy(&value, &(pool->species[i]->genomeSize), sizeof(unsigned int));
		SPBlock[(i*SPBLOCKSIZE)+0] = value;
		memcpy(&value, &(pool->species[i]->maxGenomes), sizeof(unsigned int));
		SPBlock[(i*SPBLOCKSIZE)+1] = value;
		memcpy(&value, &(pool->species[i]->stagnate), sizeof(unsigned int));
		SPBlock[(i*SPBLOCKSIZE)+2] = value;
		memcpy(&value, &(pool->species[i]->topFitness), sizeof(unsigned int));
		SPBlock[(i*SPBLOCKSIZE)+3] = value;
		memcpy(&value, &(pool->species[i]->avgFitness), sizeof(unsigned int));
		SPBlock[(i*SPBLOCKSIZE)+4] = value;
		memcpy(&value, &(pool->species[i]->id), sizeof(unsigned int));
		SPBlock[(i*SPBLOCKSIZE)+5] = value;
		memcpy(&value, &(pool->species[i]->genomeIDCTR), sizeof(unsigned int));
		SPBlock[(i*SPBLOCKSIZE)+6] = value;
		spcount++;

		for(int j = 0; j < pool->species[i]->genomeSize; j++)
		{
			//fill GM block data
			memcpy(&value, &(pool->species[i]->genomes[j]->geneSize), sizeof(unsigned int));
			GMBlock[GMSZ + (j*GMBLOCKSIZE) + 0] = value;
			memcpy(&value, &(pool->species[i]->genomes[j]->maxGenes), sizeof(unsigned int));
			GMBlock[GMSZ + (j*GMBLOCKSIZE) + 1] = value;
			memcpy(&value, &(pool->species[i]->genomes[j]->neuronSize), sizeof(unsigned int));
			GMBlock[GMSZ + (j*GMBLOCKSIZE) + 2] = value;
			memcpy(&value, &(pool->species[i]->genomes[j]->id), sizeof(unsigned int));
			GMBlock[GMSZ + (j*GMBLOCKSIZE) + 3] = value;

			//gather gene size data
			GNSZ += pool->species[i]->genomes[j]->geneSize * GNBLOCKSIZE;
			gmcount++;
		}
		GMSZ += pool->species[i]->genomeSize * GMBLOCKSIZE;
	}

	GNBlock = (unsigned int*)calloc(GNSZ, sizeof(unsigned int)); //finally allocate gene block
	
	GNDJMP = GNJMP + GNSZ; //set final jump
	PBlock[4] = GNDJMP;

	GNSZ = 0; //restart count for the loop

	//write Gene and Gene Data block buffers
	for(int i = 0; i < pool->speciesCount; i++)
	{
		for(int j = 0; j < pool->species[i]->genomeSize; j++)
		{
			for(int k = 0; k < pool->species[i]->genomes[j]->geneSize; k++)
			{
				unsigned int value;
				//need the innovation for the GND block
				int innovation = pool->species[i]->genomes[j]->genes[k]->innovation;

				//fill GN block data
				memcpy(&value, &innovation, sizeof(unsigned int));
				GNBlock[(GNSZ*GNBLOCKSIZE)+0] = value;
				memcpy(&value, &(pool->species[i]->genomes[j]->genes[k]->weight), sizeof(unsigned int));
				GNBlock[(GNSZ*GNBLOCKSIZE)+1] = value;
				memcpy(&value, &(pool->species[i]->genomes[j]->genes[k]->enabled), sizeof(unsigned int));
				GNBlock[(GNSZ*GNBLOCKSIZE)+2] = value;

				//fill GND block data
				memcpy(&value, &(pool->species[i]->genomes[j]->genes[k]->input), sizeof(unsigned int));
				GNDBlock[(innovation*GNDBLOCKSIZE)+0] = value;
				memcpy(&value, &(pool->species[i]->genomes[j]->genes[k]->output), sizeof(unsigned int));
				GNDBlock[(innovation*GNDBLOCKSIZE)+1] = value;

				GNSZ++; //increment for use with GN block
				gncount++;
			}
		}
	}

	//finish writing the P block data
	PBlock[5] = (unsigned int)pool->speciesCount;
	PBlock[6] = (unsigned int)pool->maxSpecies;
	PBlock[7] = (unsigned int)pool->globalInnovation;
	PBlock[8] = (unsigned int)pool->generation;
	PBlock[9] = (unsigned int)pool->speciesIDCTR;

	//write save file
	FILE* fp = fopen(filename, "wb");
	fwrite(PBlock, sizeof(unsigned int), PBLOCKSIZE, fp);
	fwrite(SPBlock, sizeof(unsigned int), pool->speciesCount*SPBLOCKSIZE, fp);
	fwrite(GMBlock, sizeof(unsigned int), pool->population*GMBLOCKSIZE, fp);
	fwrite(GNBlock, sizeof(unsigned int), GNSZ*GNBLOCKSIZE, fp);
	fwrite(GNDBlock, sizeof(unsigned int), pool->globalInnovation*GNDBLOCKSIZE, fp);

	fclose(fp);

	free(PBlock);
	free(SPBlock);
	free(GMBlock);
	free(GNBlock);
	free(GNDBlock);

	char msg[256];
	sprintf(msg, "%d Species, %d Genomes, %d Genes", spcount, gmcount, gncount);
	consolePrint(msg, CON_MSG);
}

//loads pool data from file
void neatLoad(Pool** pool, const char* filename)
{
	consolePrint("Loading...", CON_MSG);

	if(filename == NULL)
	{
		consolePrint("Error loading file", CON_ERR);
		return;
	}

	int spcount = 0, gmcount = 0, gncount = 0;

	unsigned int SPJMP, GMJMP, GNJMP, GNDJMP; //jump addresses
	unsigned int* buffer; //file buffer
	unsigned int magic;
	int fsize;

	FILE* fp = fopen(filename, "rb");

	if(fp == NULL)
	{
		char msg[256];
		sprintf(msg, "Error loading file: %s", filename);
		consolePrint(msg, CON_ERR);
		return;
	}

	fread(&magic, sizeof(unsigned int), 1, fp);
	if(magic != MAGICBLOCK)
	{
		char msg[256];
		sprintf(msg, "Error loading file: %s", filename);
		consolePrint(msg, CON_ERR);
		return;
	}

	//destroy and reinitialise pool
	destroyPool(pool);
	initPool(pool);

	//simply dump the file into a buffer
	fseek(fp, 0, SEEK_END);
	fsize = ftell(fp) / sizeof(unsigned int);
	rewind(fp);

	buffer = (unsigned int*)malloc(fsize * sizeof(unsigned int));

	fread(buffer, sizeof(unsigned int), fsize, fp);

	fclose(fp);

	//get jump addresses
	SPJMP = buffer[1];
	GMJMP = buffer[2];
	GNJMP = buffer[3];
	GNDJMP = buffer[4];

	//initialise pool
	(*pool)->speciesCount = (int)buffer[5];
	(*pool)->maxSpecies = (int)buffer[6];
	(*pool)->globalInnovation = (int)buffer[7];
	(*pool)->generation = (int)buffer[8];
	(*pool)->speciesIDCTR = (int)buffer[9];

	adjustSpeciesList(pool);

	for(int i = 0; i < (*pool)->speciesCount; i++)
	{
		//create new species
		initSpecies(&((*pool)->species[i]));
		(*pool)->species[i]->genomeSize = (int)buffer[SPJMP+(i*SPBLOCKSIZE)+0];
		(*pool)->species[i]->maxGenomes = (int)buffer[SPJMP+(i*SPBLOCKSIZE)+1];
		(*pool)->species[i]->stagnate = (int)buffer[SPJMP+(i*SPBLOCKSIZE)+2];
		(*pool)->species[i]->topFitness = (int)buffer[SPJMP+(i*SPBLOCKSIZE)+3];
		(*pool)->species[i]->avgFitness = (int)buffer[SPJMP+(i*SPBLOCKSIZE)+4];
		(*pool)->species[i]->id = (int)buffer[SPJMP+(i*SPBLOCKSIZE)+5];
		(*pool)->species[i]->genomeIDCTR = (int)buffer[SPJMP+(i*SPBLOCKSIZE)+6];
		adjustGenomeList(&((*pool)->species[i]));
		spcount++;

		for(int j = 0; j < (*pool)->species[i]->genomeSize; j++)
		{
			//create new genome
			initGenome(&((*pool)->species[i]->genomes[j]));
			(*pool)->species[i]->genomes[j]->geneSize = (int)buffer[GMJMP+(j*GMBLOCKSIZE)+0];
			(*pool)->species[i]->genomes[j]->maxGenes = (int)buffer[GMJMP+(j*GMBLOCKSIZE)+1];
			(*pool)->species[i]->genomes[j]->neuronSize = (int)buffer[GMJMP+(j*GMBLOCKSIZE)+2];
			(*pool)->species[i]->genomes[j]->id = (int)buffer[GMJMP+(j*GMBLOCKSIZE)+3];
			adjustGeneList(&((*pool)->species[i]->genomes[j]));
			gmcount++;

			for(int k = 0; k < (*pool)->species[i]->genomes[j]->geneSize; k++)
			{
				//prelim steps we need to do before retreiving gene data
				int innovation = (int)buffer[GNJMP+(k*GNBLOCKSIZE)]; //innovation is used to gather i/o gene data
				float weight;
				memcpy(&weight, &(buffer[GNJMP+(k*GNBLOCKSIZE)+1]), sizeof(float)); //weight is a float type which needs to be directly grabbed to preserve data

				//create new gene
				initGene(&((*pool)->species[i]->genomes[j]->genes[k]));
				(*pool)->species[i]->genomes[j]->genes[k]->innovation = innovation;
				(*pool)->species[i]->genomes[j]->genes[k]->weight = weight;
				(*pool)->species[i]->genomes[j]->genes[k]->enabled = (int)buffer[GNJMP+(k*GNBLOCKSIZE)+2]; //enabled can be grabbed directly

				(*pool)->species[i]->genomes[j]->genes[k]->input = (int)buffer[GNDJMP+(innovation*GNDBLOCKSIZE)+0]; //grab i/o gene data
				(*pool)->species[i]->genomes[j]->genes[k]->output = (int)buffer[GNDJMP+(innovation*GNDBLOCKSIZE)+1];
				gncount++;
			}
		}
	}

	free(buffer);

	char msg[256];
	sprintf(msg, "%d Species, %d Genomes, %d Genes", spcount, gmcount, gncount);
	consolePrint(msg, CON_MSG);
}

//debug to print basic gene info
void printGenes(Genome* genome)
{
	const char* TAG[] = {
	"X",
	"E",
	"B",
	"O",
	"N"
	};

	if(genome->geneSize == 0)
	{
		printf("No genes\n");
		return;
	}

	for(int i = 0; i < genome->geneSize; i++)
	{
		int a, b;
		if(genome->genes[i]->input < 180) a = 0; //tiles
		else if(genome->genes[i]->input < 360) a = 1; //extended tiles
		else if(genome->genes[i]->input == 360) a = 2; //bias
		else if(genome->genes[i]->input < 366) a = 3; //output
		else a = 4;

		if(genome->genes[i]->output < 180) b = 0; //tiles
		else if(genome->genes[i]->output < 360) b = 1; //extended tiles
		else if(genome->genes[i]->output == 360) b = 2; //bias
		else if(genome->genes[i]->output < 366) b = 3; //output
		else b = 4;
		
		printf("%d: %s(%d), %s(%d), %f, %d | ", genome->genes[i]->innovation, TAG[a], genome->genes[i]->input, TAG[b], genome->genes[i]->output, genome->genes[i]->weight, genome->genes[i]->enabled);
	}
	printf("\n");
}

/**********************************************
 *********** User Defined Functions ***********
 **********************************************/

//initialises NEAT system
int neatInit(Pool** pool)
{
	initPool(pool);

	Genome** gen0 = (Genome**)calloc(POPULATION, sizeof(Genome*));

	//fill to the population cap
	for(int i = 0; i < POPULATION; i++)
	{
		newGenome(*pool, &(gen0[i]));
		insertSpecies(pool, &(gen0[i]));
		destroyGenome(&(gen0[i]));
	}

	free(gen0);
}

//forces the next genome to go
void neatForceNext(Pool* pool)
{
	pool->timeout = -(pool->alive);
}

//draws neurons
void neatDraw(Pool* pool)
{
	Genome* cg;
	Species* cs;
	char v_string[30];
	char data_string[40];
	Pix c = {1.f, 1.f, 1.f, 1.f};

	if(pool->species == NULL) return;
	cs = pool->species[pool->currentSpecies];

	if(cs->genomes == NULL) return;
	cg = cs->genomes[pool->currentGenome];

	if(cg == NULL || cg->genes == NULL) return;

	sprintf(v_string, "v%d.%d.%d", (cs->id)%100, (pool->generation)%10000, (cg->id)%10000);
	drawText(NET_X+CELL_SIZE*2, NET_Y+NET_HEIGHT+BORDER-(TEXT_HEIGHT/2), v_string, c); //genome v_string

	//data screen
	sprintf(data_string, "Generation: %d", pool->generation);
	drawText(NET_X+CELL_SIZE*2, NET_Y+BORDER+(TEXT_HEIGHT*3), data_string, c);
	sprintf(data_string, "   Species: %d", cs->id);
	drawText(NET_X+CELL_SIZE*2, NET_Y+BORDER+(TEXT_HEIGHT*2), data_string, c);
	sprintf(data_string, "    Genome: %d", cg->id);
	drawText(NET_X+CELL_SIZE*2, NET_Y+BORDER+TEXT_HEIGHT, data_string, c);

	//Replace this with network drawing code or whatever is listed in the todo (only here for reference)
	/*
	for(int i = 0; i < cg->geneSize; i++)
	{
		if(cg->genes[i]->input > pool->inputSize - 1) continue;

		Pix c = {0.f, 0.f, 0.f, 1.f};
		int x = cg->genes[i]->input % 10;
		int y = (cg->genes[i]->input / 10) % 18;

		if(cg->genes[i]->weight > 0.f)
		{
			c.g = 1.f;
		}
		else
		{
			c.r = 1.f;
		}

		if(cg->genes[i]->input > (10*18))
		{
			drawText(GB_X + (x * CELL_SIZE) + CELL_SIZE/2, GB_Y + ((y+1) * CELL_SIZE), "E", c);
		}
		else
		{
			drawText(GB_X + (x * CELL_SIZE) + CELL_SIZE/2, GB_Y + ((y+1) * CELL_SIZE), "X", c);
		}
	}
	*/
}

//ticks NEAT system
void neatTick(Pool** pool, int currentFitness)
{
	Species* cs = (*pool)->species[(*pool)->currentSpecies];
	Genome* cg = cs->genomes[(*pool)->currentGenome];

	if(cg->geneSize == 0)
	{
		cg->fitness = 0;
		neatForceNext(*pool); //don't let "dead" genomes get tested
	}

	if(cg->network == NULL)
	{
		initNetwork(&cg);
	}

	if(cg->network == NULL)
	{
		//either it mutated the wrong way (more likely) or it became sentient (less likely). either way, kill it
		consolePrint("*** Killing Invalid Genome ***", CON_WARN);
		cg->fitness = -1;
		neatForceNext(*pool);
	}
	else
	{
		evaluateNetwork(*pool, cg);
	}

	if(((*pool)->timeout + ((*pool)->alive / 4)) <= 0)
	{
		cg->fitness = (currentFitness*1000) + ((*pool)->alive); //fitness is based on points and time alive
		(*pool)->currentGenome++;
		if(currentFitness > 0)
		{
			char msg[256];
			sprintf(msg, "Score: %d\n", currentFitness);
			consolePrint(msg, CON_MSG);
		}

		if((*pool)->currentGenome >= cs->genomeSize)
		{
			(*pool)->currentGenome = 0;
			(*pool)->currentSpecies++;

			if((*pool)->currentSpecies >= (*pool)->speciesCount)
			{
				(*pool)->currentSpecies = 0;

				newGeneration(*pool);
				neatSave(*pool, "tempbk.dat"); //save data in case of crash

				char msg[256];
				sprintf(msg, "*** Generation %d ***\n", (*pool)->generation);
				consolePrint(msg, CON_MSG);
			}
		}
		(*pool)->requestReset = 1;
		(*pool)->timeout = POOL_TIMEOUT; //reset timeout
		(*pool)->alive = 0;

		char v_string[30];
		Pix c = {1.f, 1.f, 1.f, 1.f};
		cs = (*pool)->species[(*pool)->currentSpecies];
		cg = cs->genomes[(*pool)->currentGenome];

		sprintf(v_string, "V%d.%d.%d", (cs->id)%100, ((*pool)->generation)%10000, (cg->id)%10000);
		consolePrint(v_string, CON_MSG);
		
		//printGenes((*pool)->species[(*pool)->currentSpecies]->genomes[(*pool)->currentGenome]);

		if(!GRAPHICS_HEADLESS)
		{
			//drawClearScreen((*pool)->screen);
			//This is where we draw other things. When that will be coded, I have no idea
			//This also probably shouldn't be here...
		}
	}
	(*pool)->timeout--;
	(*pool)->alive++;
}

//destroys NEAT system
int neatDestroy(Pool** pool)
{
	destroyPool(pool);
}

//sets NEAT input (game board)
int neatSetInput(Pool* pool, int* b1, int* b2)
{
	for(int i = 0; i < (10*18); i++)
	{
		pool->input[i] = b1[i] > 0 ? 1.f : 0.f; //tile
		pool->input[i+(10*18)] = b2[i] > 0 ? 1.f : 0.f; //extended tile
	}
}

//grabs NEAT output (controller input)
int neatGetOutput(Pool* pool)
{
	if(pool->requestReset)
	{
		pool->requestReset = 0;
		return -1;
	}

	int result = 0;
	for(int i = 0; i < pool->outputSize; i++)
	{
		if(pool->output[i] > 0.f)
			result |= 1 << i;
	}
	if(result != 0) pool->timeout = POOL_TIMEOUT;

	return result;
}

//for a malloc somewhere
size_t neatSizeof()
{
	return sizeof(Pool);
}
