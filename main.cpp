/*******************************************************************************
 * Copyright (c) 2018,
 *  Folding Rain LLC
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of Sony Pictures Imageworks, nor Industrial Light & Magic,
 *   nor the names of their contributors may be used to endorse or promote
 *   products derived from this software without specific prior written
 *   permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <stdlib.h>

#include "Life.h"

using namespace std;


const unsigned int DEFAULT_GENERATIONS = 100;
const unsigned int DEFAULT_WORLD_SIZE = 50;

const unsigned int SEED_1 = 1517181883;


void usage(const char *argv0)
{
	cout << "usage: " << argv0 << " <world size> <generations>\n";
	exit(1);
}


int main(int argc, char *argv[])
{
	LifeRenderer *renderer = NULL;
	unsigned int generations = 0;
	unsigned int world_size = 0;
	Simulation *sim = NULL;
	World *world = NULL;

	if (argc == 1) {
		generations = DEFAULT_GENERATIONS;
		world_size = DEFAULT_WORLD_SIZE;
	} else if (argc == 3) {
		generations = atoi(argv[2]);
		world_size = atoi(argv[1]);
	} else {
		usage(argv[0]);
		// NOTREACHED
	}

	world = new World(world_size, SEED_1);
	renderer = new TerminalLifeRenderer();
	sim = new Simulation(generations, world, renderer);

	sim->run();

	delete sim;
	delete world;
	delete renderer;
}
