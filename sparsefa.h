/*
 *  sparsefa.h
 *  peer
 *  Sparse Factor analysis variant of VBFA
 *
 *  Created by Oliver Stegle on 3/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __SPARSEFA_H__
#define __SPARSEFA_H__


#include "vbfa.h"
#include "bayesnet.h"


namespace PEER
{
	
	
	/*Sparse Factor analysis node for W*/
	class cWNodeSparse : cWNode {
		
	};
	
	
	class cSPARSEFA : cVBFA { 
		//declare friends:
		friend class cNode;
		friend class cWNode;
		friend class cXNode;
		friend class cPhenoNode;
		friend class cEpsNode;
		friend class cAlphaNode;
		
	protected:
		//test: matrix with pror information about relations?
		PMatrix pi;
		
		void init_params();
		
	public:
		//updated, sparse node for W
		cWNodeSparse W;
		
		//constructor
		cSPARSEFA();
		
#ifndef SWIG
		//constructors that take matrices in 
		
			
#endif
		
		void init_net();	
		void update();
		
		
	};// :: sparseFA
	
	
	
}


#endif