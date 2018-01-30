#include <unistd.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "Life.h"

using namespace std;


const unsigned int LIFE_DEFAULT_GENERATIONS = 5;
const unsigned int LIFE_DEFAULT_WORLD_LEN = 10;
const unsigned int SLEEP_TIME = 90000;


inline unsigned int offset_2d(unsigned int len, unsigned int row,
  unsigned int col)
{
	return ((len * row) + col);
}


inline bool inbounds_2d(unsigned int len, int val)
{
	return (((val >= 0) && (val < len)));
}


/*******************************************************************************
 * Creature
 */


/**
 */
Creature::Creature() : was_alive(false), is_alive(false)
{
}


/**
 */
Creature::~Creature()
{
}


/**
 */
void Creature::init(bool is_alive)
{
	this->is_alive = is_alive;
	was_alive = this->is_alive;
}


/*******************************************************************************
 * World
 */


/**
 */
World::World() : len(LIFE_DEFAULT_WORLD_LEN), world_size(len * len),
  rand_seed(time(NULL))
{
	init_world();
}


/**
 */
World::World(unsigned int len, unsigned int rand_seed) : len(len),
  world_size(len * len), rand_seed(rand_seed)
{
	init_world();
}


/**
 */
World::~World()
{
	unsigned int i;

	for (i = 0; i < world_size; i++)
		delete creatures[i];

	delete [] creatures;
}


/**
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
 */
void World::advance_creature(unsigned int row, unsigned int col)
{
	unsigned int live_neighbours = 0;
	Creature *target;

	if (!inbounds_2d(len, row)) {
		cerr << "row " << row << " is not within bound [0, " << len << "]"
		  << endl;
		return;
	}

	if (!inbounds_2d(len, col)) {
		cerr << "col " << col << " is not within bound [0, " << len << "]"
		  << endl;
		return;
	}

	target = get_creature(row, col);
	live_neighbours = count_live_neighbours(row, col);
	if (col == len)
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
 */
unsigned int World::count_live_neighbours(unsigned int row, unsigned int col)
{
	int a_col, t_col, a_row, t_row;
	unsigned int count = 0;
	Creature *target;

	if (!inbounds_2d(len, row)) {
		cerr << "row " << row << " is not within bound [0, " << len << "]"
		  << endl;
		return (0);
	}

	if (!inbounds_2d(len, col)) {
		cerr << "col " << col << " is not within bound [0, " << len << "]"
		  << endl;
		return (0);
	}

	a_row = (int)row;
	a_col = (int)col;

	for (t_row = a_row - 1; t_row <= a_row + 1; t_row++) {
		if (!inbounds_2d(len, t_row))
			continue;

		for (t_col = a_col - 1; t_col <= a_col + 1; t_col++) {
			if (!inbounds_2d(len, t_col))
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
 */
Creature * World::get_creature(unsigned int row, unsigned int col)
{
	return (creatures[offset_2d(len, row, col)]);
}


/**
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


/**
 */
Simulation::Simulation() : generations(LIFE_DEFAULT_GENERATIONS), world(NULL)
{
}


/**
 */
Simulation::Simulation(unsigned int generations, World *world,
  LifeRenderer *renderer) : generations(generations), renderer(renderer),
  world(world)
{
}


/**
 */
Simulation::~Simulation()
{
	renderer = NULL;
	world = NULL;
}


/**
 */
void Simulation::run()
{
	unsigned int i = 0;

	if (world == NULL) {
		cerr << "cannot run simulation, no world\n\n";
		return;
	}

	//world->dump();

	for (i = 0; i < generations; i++) {
		renderer->render_frame(i, world);
		usleep(SLEEP_TIME);
		advance();
	}
}


/**
 */
void Simulation::advance()
{
	unsigned int i, j;

	if (world == NULL) {
		cerr << "cannot advance simulation, no world\n\n";
		return;
	}

	for (i = 0; i < world->len; i++) {
		for (j = 0; j < world->len; j++) {
			world->advance_creature(i, j);
		}
	}

	world->save_last_generation();
}


/*******************************************************************************
 * TerminalLifeRenderer
 */


/**
 */
void TerminalLifeRenderer::render_frame(unsigned int frame, World *world)
{
	Creature *creature;
	unsigned int i, j;
	char c;

	for (i = 0; i < world->len; i++) {
		for (j = 0; j < world->len; j++) {
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

