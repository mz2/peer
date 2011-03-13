#include "vbfa.h"
#include "io.h"

using namespace PEER;

int main (int argc, char * const argv[]) {
	sPeerArgs args = parseCmdlineArgs(argc, argv);
	cVBFA vb = getInstance(args);
	vb.update();
	write_output(vb, args);
	return 0;
}