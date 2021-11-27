////////////////////////////////////////////////////////////////////////
// filename - HlsGen.h
// author - Dillon Gregerson
// project - Ece574 Assignment 2
////////////////////////////////////////////////////////////////////////

#pragma once

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

#include "Vertex.h"
#include "BaseType.h"

class HlsGen
{
public:
	enum class DataParseState
	{
		IO_TYPE,
		DATA_TYPE,
		VAR_NAMES,
		INVALID,
		COMPLETE
	};

	enum class OpsParseState
	{
		INIT,
		OUTPUT_VAR,
		EQUAL_SIGN,
		DATA1_EXTRACT,
		OP_EXTRACT,
		DATA2_EXTRACT,
		MUX_LOGIC,
		DATA3_EXTRACT,
		COMPLETE
	};

	enum class ConditionalParseState
	{
		IDLE,
		IF,
		ELSE,
		PARENTHESIS1,
		CONDITIONAL,
		BRACKETS,
		OPERATION
	};

	struct DataProp
	{
		unsigned int dataSize;
		bool isSigned;
		DataProp() :dataSize(0), isSigned(false) {}
		DataProp(unsigned int dataSize, bool isSigned) :dataSize(dataSize), isSigned(isSigned) {}
	};

	std::map<std::string, DataProp>typesMap_;
	std::map<std::string, Vertex::Operation>opsMap_;
	std::map<Vertex::Operation, std::vector<double>> latencyMap_;
	std::map<std::string, Vertex> opsDefs2_;
	std::map<unsigned int, Vertex> opsDefs_;
	std::unordered_set < std::string > componentNames_;
	std::map<std::string, BaseType>dataDefs_;

	std::unordered_set<std::string> outputStates_;
	std::vector<std::stack<std::string>>paths_;
	int num_iteration_;
	std::stack<std::string> vtxStack_;
	std::map <std::string, std::vector<std::string>>dag_;
	std::string fileName_;
	std::string circuitName_;
	std::map<int, int> bitsMapping_;

	//////////////////////////////////////////////////////////////////////////////
	//@brief default constructor
	//@param void
	//@return N/A
	//////////////////////////////////////////////////////////////////////////////
	HlsGen(void);

	//////////////////////////////////////////////////////////////////////////////
	//@brief calculate the latency of an operation
	//@param Vertex::Operation op - operation to be performed
	//@return double overall latency
	//////////////////////////////////////////////////////////////////////////////
	double getLatency(Vertex::Operation op, int bits);

	//////////////////////////////////////////////////////////////////////////////
	//@brief parse the input netlist file 
	//@param std::string filename
	//@return unsigned int - status of file parsing
	//////////////////////////////////////////////////////////////////////////////
	unsigned int parseFile(std::string filename);

	//////////////////////////////////////////////////////////////////////////////
	//@brief parse the input file for it's data constituents
	//@param std::vector<unsigned int>&invalidLines
	//@param std::string filename
	//@return bool returns true on success, false otherwise
	//////////////////////////////////////////////////////////////////////////////
	bool parseData(std::vector<unsigned int>& invalidLines, std::string fileName);

	//////////////////////////////////////////////////////////////////////////////
	//@brief parse the input file for it's operations
	//@param std::vector<unsigned int>invalidLines
	//@param std::string filename
	//@return bool returns true on success, false otherwise
	//////////////////////////////////////////////////////////////////////////////
	bool parseOps(std::vector<unsigned int>invalidLines, std::string fileName);

	//////////////////////////////////////////////////////////////////////////////
	//@brief parse individual line
	//@param std::string line
	//@param std::map<std::string, BaseType>& dataDef
	//@return bool returns true on success, false otherwise
	//////////////////////////////////////////////////////////////////////////////
	bool parseInputLine(std::string line, std::map<std::string, BaseType>& dataDefs);

	//////////////////////////////////////////////////////////////////////////////
	//@brief parse individual line for operations
	//@param std::string line
	//@param std::map<std::string, BaseType>& opsDef
	//@return bool returns true on success, false otherwise
	//////////////////////////////////////////////////////////////////////////////
	bool parseOpsLine(std::string line, std::map<unsigned int, Vertex>& opsDefs);

	//////////////////////////////////////////////////////////////////////////////
	//@brief parse individual line for operations
	//@param std::string line
	//@param std::map<unsigned int, Vertex>&condionalMap
	//@param std::map<unsigned int, Vertex>& opsDefs
	//@return bool returns true on success, false otherwise
	//////////////////////////////////////////////////////////////////////////////
    bool parseConditionals(std::vector<unsigned int>&invalidLines, std::string fileName);

	//////////////////////////////////////////////////////////////////////////////
	//@brief parse individual line for operations
	//@param std::string line
	//@param std::map<unsigned int, Vertex>&condionalMap
	//@param std::map<unsigned int, Vertex>& opsDefs
	//@return bool returns true on success, false otherwise
	//////////////////////////////////////////////////////////////////////////////
	bool parseConditionalLine(std::string, HlsGen::ConditionalParseState& currState, unsigned int);

	//////////////////////////////////////////////////////////////////////////////
	//@brief write the verilog header to file
    //@param std::ofstream& outputStream
	//@return void
	//////////////////////////////////////////////////////////////////////////////
	void writeHeader(std::ofstream& outputStream);

	//////////////////////////////////////////////////////////////////////////////
	//@brief populate and return a string with module signature
    //@param std::string outputFile - string name of output file, which becomes name 
	//                                of module 
	//@return std::string module signature
	//////////////////////////////////////////////////////////////////////////////
	std::string moduleSignature(std::string outputFile);

	//////////////////////////////////////////////////////////////////////////////
	//@brief write data definitions in verilog
    //@param std::ofstream& outputStream
	//@return std::string module signature
	//////////////////////////////////////////////////////////////////////////////
	void writeDataDefinitions(std::ofstream& outputStream);

	//////////////////////////////////////////////////////////////////////////////
	//@brief write oprerations in verilog
    //@param std::ofstream& outputStream
	//@return bool - true on write success
	//////////////////////////////////////////////////////////////////////////////
	bool writeOperations(std::ofstream& outputStream);

	//////////////////////////////////////////////////////////////////////////////
	//@brief wrapper method that calls other class methods for verilog generation
    //@param std::string outputFile - output file name
	//@return bool - true on write success
	//////////////////////////////////////////////////////////////////////////////
	bool generateVerilog(std::string outputFile);

	//////////////////////////////////////////////////////////////////////////////
	//@brief determine the critical path from operations map
    //@param void
	//@return double critical path
	//////////////////////////////////////////////////////////////////////////////
	double getCriticalPath(void);

	//////////////////////////////////////////////////////////////////////////////
	//@brief recursive function to get all of the paths of the DAG
    //@param void
	//@return bool after reaching end of each path
	//////////////////////////////////////////////////////////////////////////////
	bool getDependencies(std::string vtx);

	//////////////////////////////////////////////////////////////////////////////
	//@brief creates the Directed Acyclic Graph from the operations map
    //@param void
	//@return void
	//////////////////////////////////////////////////////////////////////////////
	void createDag();

	//////////////////////////////////////////////////////////////////////////////
	//@brief calls recursive method then calculates the actual forward latency of 
	//       the critical path
    //@param void
	//@return double critical path
	//////////////////////////////////////////////////////////////////////////////
	double determineCriticalPath(void);

	//////////////////////////////////////////////////////////////////////////////
	//@brief checks to see if a string is a number
    //@param const std::string& str
	//@return bool - true if input string is a number, false otherwise
	//////////////////////////////////////////////////////////////////////////////
	bool isNumber(const std::string& str);
	
	bool populateTimeFrames(void);
	unsigned int getAsapTimes(std::string input, unsigned int layer);
	unsigned int getAlapTimes(std::string vtx, unsigned int layer);
	bool invertDag(void);
	void addToInvDag(std::string);
	std::map<std::string, std::vector<std::string>>invDag_;
	unsigned int latency_;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////