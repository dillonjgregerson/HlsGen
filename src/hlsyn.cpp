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
	hlsgen.setLatency(std::stoi(argv[3]));
	unsigned int prevLatency = std::stoi(argv[3]);

	if (argc != 4)
	{
		std::cout << "error: invalid input arguments\n Correct usage: dpgen <netlist file> <output file (.v)>\n";
		return 0;
	}
	else if (retVal == 1)
	{
		std::cout << "error: cannot find input file (argument 1), please ensure correct file path to netlist file\n";
		return 0;
	}
	else if (retVal == 2)
	{
		std::cout << "error occurred while parsing file\n";
	}

	else
	{
		hlsgen.createDag();
		//std::cout << "Critical Path : " << hlsgen.determineCriticalPath() << " ns\n";

		hlsgen.invertDag();

		if (!hlsgen.populateTimeFrames())
		{
			std::cout <<"error: " << prevLatency << " cycles is not sufficient to meet latency constriants! \nIncreasing to " 
				<< hlsgen.latency_ << " cycles to meet latency constraints.\n";
		}
	
		if(!hlsgen.performScheduling())
		{
			std::cout << "Error occurred while scheduling\n";
		}
		bool validCode = hlsgen.generateVerilog(argv[2]);
		if (!validCode)
		{
			std::cout << "error occurred, HLS failed\n";
			return 0;
		}
		else
		{
			std::cout << "Verilog generated!\n";
		}
	}
	//todo add error checking


}
