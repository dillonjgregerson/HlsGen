////////////////////////////////////////////////////////////////////////
// filename - HlsGen.cpp
// author - Dillon Gregerson
// project - Ece574 Assignment 2
// dpgen.cpp : This file contains the 'main' function. 
////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <utility>
#include <map>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <math.h>
#include <chrono>
#include <stack>

#include "HlsGen.h"

int main(int argc, char* argv[])
{
	HlsGen hlsgen;
	unsigned int retVal = hlsgen.parseFile(argv[1]);
	if (argc != 3)
	{
		std::cout << "error: invalid input arguments\n Correct usage: dpgen <netlist file> <output file (.v)>\n";
		return 0;
	}
	else if (retVal == 1)
	{
		std::cout << "error: annot find input file (argument 1), please ensure correct file path to netlist file\n";
		return 0;
	}
	else if (retVal == 2)
	{
		std::cout << "error: invalid operator in netlist\n";
	}

	else
	{
		bool validCode = hlsgen.generateVerilog(argv[2]);
		if (!validCode)
		{
			std::cout << "error occurred, HLS failed\n";
			return 0;
		}
		hlsgen.createDag();
		std::cout << "Critical Path : " << hlsgen.determineCriticalPath() << " ns\n";
	}
}
