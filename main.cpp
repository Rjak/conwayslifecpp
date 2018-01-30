#include <iostream>
#include <stdlib.h>

#include "Life.h"

using namespace std;


const unsigned int DEFAULT_WORLD_SIZE = 100;

const unsigned int SEED_1 = 1517181883;


int main(int argc, char *argv[]) {
	LifeRenderer *renderer = NULL;
	Simulation *sim = NULL;
	World *world = NULL;
	int world_size = 0;

	if (argc == 2)
		world_size = atoi(argv[1]);
	else
		world_size = DEFAULT_WORLD_SIZE;

	world = new World(world_size, SEED_1);
	renderer = new TerminalLifeRenderer();
	sim = new Simulation(100, world, renderer);

	sim->run();

	delete sim;
	delete world;
	delete renderer;
}
