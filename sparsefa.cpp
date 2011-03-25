/*
 *  sparsefa.cpp
 *  peer
 *
 *  Created by Oliver Stegle on 3/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "sparsefa.h"

using namespace PEER;

cSPARSEFA::cSPARSEFA() { 
	//default settings
	init_params();
};


void cSPARSEFA::init_params()
{
	//call enhirted method first
	cVBFA::init_params();
		
}


void cSPARSEFA::init_net()
{
	cVBFA::init_net();
}


void cSPARSEFA::update()
{
	cVBFA::update();
}