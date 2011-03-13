/*
 *  io.h
 *  peer
 *
 *  Created by Leopold Parts on 13/03/2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "vbfa.h"

using namespace PEER;

struct sPeerArgs{
	bool no_residuals, no_X, no_W, no_Alpha, keep_mean, has_header;
	string out_dir, expr_file, cov_file, conf_file;
	int n_factors, n_iter;
	float alpha_pa, alpha_pb, eps_pa, eps_pb, bound_tolerance, var_tolerance;
};


sPeerArgs parseCmdlineArgs(int argc, char * const argv[]);

PMatrix parseCsv(const char *filename, bool header);

cVBFA getInstance(sPeerArgs args);

void write_output(cVBFA vb, sPeerArgs args);

