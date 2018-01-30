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

#ifndef LIFE_H
#define LIFE_H


/**
 * This library generates a simulation of Conway's "Life". The inputs are
 * simple:
 *
 * - World Size: the "world" is a square, so this is a single integer value
 * - Generation Count: the number of generations to simulate
 * - Seed: optional seed for the RNG
 *
 * There are a couple of simple renderers available, but the general idea behind
 * this library is to generate a simulation which can be realized in a 3D
 * package such as Maya and virtually photographed in a beautiful way.
 */


/**
 * Represents a single Conway creature in the world.
 */
class Creature
{
	public:
		bool was_alive;
		bool is_alive;

		Creature();
		~Creature();

		void init(bool is_alive);
};


/**
 * Represents the 2-dinemsional "world" that the Conway creatures live in.
 */
class World
{
public:
	// The world is square, world_len describes the length of a side.
	unsigned int world_len;

	World();
	World(unsigned int world_len, unsigned int rand_seed);
	~World();
	
	Creature * get_creature(unsigned int row, unsigned int col);
	void advance_creature(unsigned int row, unsigned int col);
	unsigned int count_live_neighbours(unsigned int row, unsigned int col);
	void save_last_generation();

private:
	Creature **creatures;

	// Total number of creatures in the world (world_len ^ 2).
	unsigned int world_size;

	unsigned int rand_seed;

	void init_world();
};


/**
 * Abstract class which provides a common interface to the library to hook into
 * a rendering function.
 */
class LifeRenderer
{
public:
	virtual ~LifeRenderer() {}

	virtual void render_generation(unsigned int generation, World *world) = 0;
};


/**
 * Renderer which produces output on the command line.
 */
class TerminalLifeRenderer : public LifeRenderer
{
public:
	~TerminalLifeRenderer() {}

	void render_generation(unsigned int generation, World *world);
};


/**
 * Encapsulates the notion of advancing the simulation through the vector of
 * generations, as well as the actual Conway "life & death" logic.
 */
class Simulation
{
public:
	Simulation();
	Simulation(unsigned int generations, World *world, LifeRenderer *renderer);
	~Simulation();

	void run();

private:
	unsigned int generations;
	LifeRenderer *renderer;
	World *world;

	void advance();
};


#endif /* LIFE_H */
