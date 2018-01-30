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

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <time.h>

#include "Life.h"

using namespace std;


const unsigned int LIFE_DEFAULT_GENERATIONS = 5;
const unsigned int LIFE_DEFAULT_WORLD_LEN = 10;
const unsigned int LIFE_MAX_WORLD_LEN = 10000;
const unsigned int SLEEP_TIME = 90000;


/**
 * Given the size of the square 2D world "plane" and a row/column, compute the
 * offset into the the 1D array which stores all the Creature pointers.
 *
 * @param unsigned int world_size - size of the square world
 * @param unsigned int row - row offset of the desired creature
 * @param unsigned int col - column offset of the desired creature
 * @return unsigned int - offset of the creature position in the 1D array
 * backing the world
 */
inline unsigned int offset_2d(unsigned int world_size, unsigned int row,
  unsigned int col)
{
	return ((world_size * row) + col);
}


/**
 * Returns true if the given value falls within the bounds of the given world
 * size, false otherwise.
 *
 * @param unsigned int world_size - size of the square world
 * @param int val - signed value to bounds-check
 * @return bool - true if the given value falls within the bounds of the given
 * world size, false otherwise
 */
inline bool inbounds_2d(unsigned int world_size, int val)
{
	stringstream ss;

	// paranoia checks since we are comparing signed/unsigned types
	
	if (world_size > LIFE_MAX_WORLD_LEN) {
		ss << "world_size " << world_size << " out of bounds " << 
		  LIFE_MAX_WORLD_LEN;
		throw std::runtime_error(ss.str());
	}

	return (((val >= 0) && (val < world_size)));
}


/*******************************************************************************
 * Creature
 */


Creature::Creature() : was_alive(false), is_alive(false)
{
}


Creature::~Creature()
{
}


void Creature::init(bool is_alive)
{
	this->is_alive = is_alive;
	was_alive = this->is_alive;
}


/*******************************************************************************
 * World
 */


/**
 * Creates a World with default parameters and a chaotic, time-based seed.
 */
World::World() : world_len(LIFE_DEFAULT_WORLD_LEN),
  world_size(world_len * world_len), rand_seed(time(NULL))
{
	init_world();
}


/**
 * Creates a square world of the given length and with the given random seed.
 *
 * @param unsigned int world_len - the "length" of the world, the square of
 * which describes the full size of the world
 * @param unsigned int rand_seed - seed for the RNG
 */
World::World(unsigned int world_len, unsigned int rand_seed) :
  world_len(world_len), world_size(world_len * world_len), rand_seed(rand_seed)
{
	init_world();
}


World::~World()
{
	unsigned int i;

	for (i = 0; i < world_size; i++)
		delete creatures[i];

	delete [] creatures;
}


/**
 * Performs allocation operations to instantiate the Conway world.
 */
void World::init_world()
{
	bool is_alive = false;
	Creature *creature;
	unsigned int i;

	creatures = new Creature*[world_size];

	srand(rand_seed);

	for (i = 0; i < world_size; i++) {
		if (rand() % 2)
			is_alive = true;
		else
			is_alive = false;

		creature = new Creature();
		creature->is_alive = is_alive;
		creature->was_alive = is_alive;
		creatures[i] = creature;
	}
}


/**
 * Given row and colum offsets, update the living state of the creature at that
 * position for the next generation.
 *
 * @param unsigned int row - row offset of the desired creature
 * @param unsigned int col - col offset of the desired creature
 */
void World::advance_creature(unsigned int row, unsigned int col)
{
	unsigned int live_neighbours = 0;
	Creature *target;
	stringstream ss;

	if (!inbounds_2d(world_len, row)) {
		ss << "row " << row << " is not within bound [0, " << world_len << "]"
		  << endl;
		throw std::runtime_error(ss.str());
	}

	if (!inbounds_2d(world_len, col)) {
		ss << "col " << col << " is not within bound [0, " << world_len << "]"
		  << endl;
		throw std::runtime_error(ss.str());
	}

	target = get_creature(row, col);
	live_neighbours = count_live_neighbours(row, col);
	if (col == world_len)
		cout << endl;

	if (live_neighbours < 2) {
		target->is_alive = false;
	} else if (live_neighbours > 3) {
		target->is_alive = false;
	} else if (live_neighbours == 3) {
		target->is_alive = true;
	}
}


/**
 * Given the row and column of the desired creature, returns the number of
 * living neighbours.
 *
 * @param unsigned int row - row offset of the desired creature
 * @param unsigned int col - col offset of the desired creature
 * @return unsigned int - count of the number of living neighbours the desired
 * creature has
 */
unsigned int World::count_live_neighbours(unsigned int row, unsigned int col)
{
	int a_col, t_col, a_row, t_row;
	unsigned int count = 0;
	Creature *target;

	if (!inbounds_2d(world_len, row)) {
		cerr << "row " << row << " is not within bound [0, " << world_len << "]"
		  << endl;
		return (0);
	}

	if (!inbounds_2d(world_len, col)) {
		cerr << "col " << col << " is not within bound [0, " << world_len << "]"
		  << endl;
		return (0);
	}

	a_row = (int)row;
	a_col = (int)col;

	for (t_row = a_row - 1; t_row <= a_row + 1; t_row++) {
		if (!inbounds_2d(world_len, t_row))
			continue;

		for (t_col = a_col - 1; t_col <= a_col + 1; t_col++) {
			if (!inbounds_2d(world_len, t_col))
				continue;

			if ((t_row == a_row) && (t_col == a_col))
				continue;

			target = get_creature(t_row, t_col);
			if (target->was_alive)
				count++;
		}
	}

	return (count);
}


/**
 * @param unsigned int row - row offset of the desired creature
 * @param unsigned int col - col offset of the desired creature
 * @return Creature * - pointer to the creature at the given row and column
 */
Creature * World::get_creature(unsigned int row, unsigned int col)
{
	return (creatures[offset_2d(world_len, row, col)]);
}


/**
 * Persists the the current notion of life and death in the world to the
 * previous generation storage.
 */
void World::save_last_generation()
{
	Creature *creature;
	unsigned int i;

	for (i = 0; i < world_size; i++) {
		creature = creatures[i];
		creature->was_alive = creature->is_alive;
	}
}


/*******************************************************************************
 * Simulation
 */


Simulation::Simulation() : generations(LIFE_DEFAULT_GENERATIONS), world(NULL)
{
}


/**
 * Instantiates a simulator with the given World and Renderer, which will run
 * for the given number of generations.
 *
 * @param unsigned int generations - number of generations the simulation will
 * run for
 * @param World * world - the World the simulation is taking place in
 * @param LifeRenderer * renderer - the renderer whick will handle persisting
 * each generation to a view format
 */
Simulation::Simulation(unsigned int generations, World *world,
  LifeRenderer *renderer) : generations(generations), renderer(renderer),
  world(world)
{
}


Simulation::~Simulation()
{
	renderer = NULL;
	world = NULL;
}


/**
 * Runs the simulation.
 *
 * This method has the side effect of calling on the LifeRenderer to render each
 * generation.
 */
void Simulation::run()
{
	unsigned int i = 0;

	if (world == NULL)
		throw std::runtime_error("world is NULL, cannot run simulation");

	for (i = 0; i < generations; i++) {
		if (renderer != NULL)
			renderer->render_generation(i, world);
		usleep(SLEEP_TIME);
		advance();
	}
}


/**
 * Advances the simulation one generation.
 */
void Simulation::advance()
{
	unsigned int i, j;

	if (world == NULL)
		throw std::runtime_error("world is NULL, cannot advance simulation");

	for (i = 0; i < world->world_len; i++) {
		for (j = 0; j < world->world_len; j++) {
			world->advance_creature(i, j);
		}
	}

	world->save_last_generation();
}


/*******************************************************************************
 * TerminalLifeRenderer
 */


/**
 * Renders the current generation of the simulation to stdout.
 *
 * @param unsigned int generation - the generation we must render
 * @param World * - the World we are rendering
 */
void TerminalLifeRenderer::render_generation(unsigned int generation,
  World *world)
{
	Creature *creature;
	unsigned int i, j;
	char c;

	for (i = 0; i < world->world_len; i++) {
		for (j = 0; j < world->world_len; j++) {
			creature = world->get_creature(i, j);
			if (creature->is_alive)
				c = '*';
			else
				c = '.';
			cout << c << "  ";
		}
		cout << endl;
	}

	cout << "---------------" << endl;
}

