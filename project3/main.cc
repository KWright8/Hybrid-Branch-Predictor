#include <stdio.h>
#include "HybridPredictor.h"

int main(int argc, char *argv[]) {
	int chooserTablePCBits = std::stoi(argv[2]);
	int gsharePCBits = std::stoi(argv[3]);
	int gbh = std::stoi(argv[4]);
	int bimodalPCBits = std::stoi(argv[5]);
	string trace_file = argv[6];

	// printf("DEBUG: chooser bits %d, gsharepcb %d, gbh %d, bimodalPCB %d, trace_file %s\n",chooserTablePCBits,gsharePCBits,gbh,bimodalPCBits, trace_file.c_str());

	HybridPredictor(bimodalPCBits, chooserTablePCBits, gsharePCBits, gbh, trace_file);
	
}
