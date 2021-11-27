////////////////////////////////////////////////////////////////////////
// filename - HlsGen.cpp
// author - Dillon Gregerson
// project - Ece574 Assignment 2
////////////////////////////////////////////////////////////////////////

#include "HlsGen.h"

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
HlsGen::HlsGen(void)
{
	typesMap_["Int1"] = { 1, true };
	typesMap_["Int2"] = { 2, true };
	typesMap_["Int4"] = { 4, true };
	typesMap_["Int8"] = { 8, true };
	typesMap_["Int16"] = { 16, true };
	typesMap_["Int32"] = { 32, true };
	typesMap_["Int64"] = { 64, true };

	typesMap_["UInt1"] = { 1, true };
	typesMap_["UInt2"] = { 2, true };
	typesMap_["UInt4"] = { 4, true };
	typesMap_["UInt8"] = { 8, true };
	typesMap_["UInt16"] = { 16, true };
	typesMap_["UInt32"] = { 32, true };
	typesMap_["UInt64"] = { 64, true };

	opsMap_["+"] = Vertex::Operation::ADD;
	opsMap_["-"] = Vertex::Operation::SUB;
	opsMap_["*"] = Vertex::Operation::MULT;
	opsMap_["=="] = Vertex::Operation::COMP_EQ;
	opsMap_[">"] = Vertex::Operation::COMP_GT;
	opsMap_["<"] = Vertex::Operation::COMP_LT;
	opsMap_["?"] = Vertex::Operation::MUX2x1;
	opsMap_[">>"] = Vertex::Operation::SHR;
	opsMap_["<<"] = Vertex::Operation::SHL;
	opsMap_["/"] = Vertex::Operation::DIV;
	opsMap_["%"] = Vertex::Operation::MOD;
	opsMap_["++"] = Vertex::Operation::INC;
	opsMap_["--"] = Vertex::Operation::DEC;
	opsMap_["="] = Vertex::Operation::REG;

	//Name 1 - bit 2 - bit 8 - bit 16 - bit 32 - bit 64 - bit
	latencyMap_[Vertex::Operation::REG] = { 2.616, 2.644, 2.879, 3.061, 3.602, 3.966 };
	latencyMap_[Vertex::Operation::ADD] = { 2.704, 3.713, 4.924, 5.638, 7.270, 9.566 };
	latencyMap_[Vertex::Operation::SUB] = { 3.024, 3.412, 4.890, 5.569, 7.253, 9.566 };
	latencyMap_[Vertex::Operation::MULT] = { 2.438, 3.651, 7.453, 7.811, 12.395, 15.354 };
	latencyMap_[Vertex::Operation::COMP_EQ] = { 3.031, 3.934, 5.949, 6.256, 7.264, 8.416 };
	latencyMap_[Vertex::Operation::COMP_LT] = { 3.031, 3.934, 5.949, 6.256, 7.264, 8.416 };
	latencyMap_[Vertex::Operation::COMP_GT] = { 3.031, 3.934, 5.949, 6.256, 7.264, 8.416 };
	latencyMap_[Vertex::Operation::MUX2x1] = { 4.083, 4.115, 4.815, 5.623, 8.079, 8.766 };
	latencyMap_[Vertex::Operation::SHR] = { 3.644, 4.007, 5.178, 6.460, 8.819, 11.095 };
	latencyMap_[Vertex::Operation::SHL] = { 3.614, 3.980, 5.152, 6.549, 8.565, 11.220 };
	latencyMap_[Vertex::Operation::DIV] = { 0.619, 2.144, 15.439, 33.093, 86.312, 243.233 };
	latencyMap_[Vertex::Operation::MOD] = { 0.758, 2.149, 16.078, 35.563, 88.142, 250.583 };
	latencyMap_[Vertex::Operation::INC] = { 1.792, 2.218, 3.111, 3.471, 4.347, 6.200 };
	latencyMap_[Vertex::Operation::DEC] = { 1.792, 2.218, 3.108, 3.701, 4.685, 6.503 };

	bitsMapping_[1] = 0;
	bitsMapping_[2] = 1;
	bitsMapping_[8] = 2;
	bitsMapping_[16] = 3;
	bitsMapping_[32] = 4;
	bitsMapping_[64] = 5;
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
double HlsGen::getLatency(Vertex::Operation op, int bits)
{
	return latencyMap_[op][bitsMapping_[bits]];
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
unsigned int HlsGen::parseFile(std::string filename)
{
	std::vector<unsigned int>invalidLines;
	std::string file = filename;
	filename.erase(filename.end() - 4, filename.end());
	circuitName_ = filename;

	unsigned int retVal = 0;
	if (!parseConditionals(invalidLines, filename))
	{
		retVal = 1;
	}
	else if (!parseData(invalidLines, file))
	{
		retVal = 1;
	}
	else if (!parseOps(invalidLines, file))
	{
		retVal = 2;
	}
	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
bool HlsGen::parseData(std::vector<unsigned int>& invalidLines, std::string fileName)
{
	std::ifstream file(fileName.c_str());
	bool retVal(false);
	// first open the file to parse out the data types
	if (file.is_open())
	{
		retVal = true;
		std::string line;
		unsigned int a = 0;
		while (getline(file, line))
		{
			if (!parseInputLine(line, dataDefs_))
			{
				invalidLines.push_back(a);
			}
			a++;
		}
	}
	else
	{
		std::cout << "\n couldn't open file\n";
	}
	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
bool HlsGen::parseOps(std::vector<unsigned int>invalidLines, std::string fileName)
{
	std::ifstream file(fileName.c_str());
	bool retVal(true);

	// first open the file to parse out the data types
	if (file.is_open())
	{
		std::string line;
		unsigned int a = 0;
		while (getline(file, line))
		{
			if (std::count(invalidLines.begin(), invalidLines.end(), a))
			{
				retVal = (parseOpsLine(line, opsDefs_) && retVal);
				//the previous parse data function would have considered anything that was not 
				//data related an invalid line. Therefore we only want to look at 'invalid' lines
			}
			a++;
		}
	}
	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
bool HlsGen::parseInputLine(std::string line, std::map<std::string, BaseType>& dataDefs)
{
	std::string s = line + " "; //add ' ' for string parsing
	BaseType::DataType dType(BaseType::DataType::NONE);
	bool lineValid(false);
	unsigned int dataSize(0);
	bool isSigned;
	DataParseState parseState(DataParseState::IO_TYPE);
	std::vector<std::string>varNames;
	std::string strName("");

	std::string delim = " ";
	//std::map<>
	auto start = 0U;
	auto end = s.find(delim);
	while (end != std::string::npos)
	{
		switch (parseState)
		{
		case DataParseState::IO_TYPE:
			if (s.substr(start, end - start) == "input")
			{
				dType = BaseType::DataType::INPUT;
				parseState = DataParseState::DATA_TYPE;
			}
			else if (s.substr(start, end - start) == "wire")
			{
				dType = BaseType::DataType::WIRE;
				parseState = DataParseState::DATA_TYPE;
			}
			else if (s.substr(start, end - start) == "output")
			{
				dType = BaseType::DataType::OUTPUT;
				parseState = DataParseState::DATA_TYPE;
			}
			else if (s.substr(start, end - start) == "register")
			{
				dType = BaseType::DataType::REGISTER;
				parseState = DataParseState::DATA_TYPE;
			}
			break;
		case DataParseState::DATA_TYPE:
			if (typesMap_.find(s.substr(start, end - start)) != typesMap_.end())
			{
				dataSize = typesMap_[s.substr(start, end - start)].dataSize;
				isSigned = typesMap_[s.substr(start, end - start)].isSigned;
				parseState = DataParseState::VAR_NAMES;
			}
			break;
		case DataParseState::VAR_NAMES:
			strName = s.substr(start, end - start);
			strName.erase(std::remove(strName.begin(), strName.end(), ','), strName.end());
			varNames.push_back(strName);
			break;
		default:
			break;
		}

		start = end + delim.length();
		end = s.find(delim, start);
	}

	for (std::vector<std::string>::iterator itr = varNames.begin(); itr < varNames.end(); itr++)
	{
		if (*itr != "")
		{
			BaseType data(isSigned, dataSize, dType);
			dataDefs[*itr] = data;
		}
	}
	return (dType != BaseType::DataType::NONE) && (dataSize != 0);
}

//////////////////////////////////////////////////////////////////////////////
//@brief parse individual line for operations
//@param std::string line
//@param std::map<unsigned int, Vertex>&condionalMap
//@param std::map<unsigned int, Vertex>& opsDefs
//@return bool returns true on success, false otherwise
//////////////////////////////////////////////////////////////////////////////


bool HlsGen::parseConditionals(std::vector<unsigned int>& invalidLines, std::string fileName)
{
	std::map<std::string, std::vector<std::string>>conditionalMap;
	unsigned int stateNum;

	ConditionalParseState state = ConditionalParseState::IF;

	std::ifstream file(fileName.c_str());
	bool retVal(true);

	// first open the file to parse out the data types
	if (file.is_open())
	{
		std::string line;
		unsigned int a = 0;
		while (getline(file, line))
		{
			retVal = (parseConditionalLine(line, state, stateNum) && retVal);
			if (retVal == 1)
			{
				invalidLines.push_back(a);
			}
			//the previous parse data function would have considered anything that was not 
			//data related an invalid line. Therefore we only want to look at 'invalid' lines
			a++;
		}
	}
	return retVal;
}

bool HlsGen::parseConditionalLine(std::string line, HlsGen::ConditionalParseState& currState,
	unsigned int nested)
{
	std::string s = line + " "; //add ' ' for string parsing
	bool lineValid(false);

	std::vector<std::string>varNames;
	std::string strName("");
	std::string conditional("");
	std::string operation("");

	std::string delim = " ";
	auto start = 0U;
	auto end = s.find(delim);
	while (end != std::string::npos)
	{
		std::string substring = s.substr(start, end - start);
		switch (currState)
		{
			std::cout << "state: " << (int)currState << std::endl;
			std::cout << "substring: " << substring << std::endl;
			std::cout << "conditional: " << conditional << std::endl;
		case ConditionalParseState::IDLE:
			if (substring == "if")
			{
				currState = ConditionalParseState::PARENTHESIS1;
			}
			break;
		case ConditionalParseState::IF:
			if (substring == "if")
			{
				currState = ConditionalParseState::PARENTHESIS1;
			}
			break;
		case ConditionalParseState::PARENTHESIS1:
			if (substring == "(")
			{
				currState = ConditionalParseState::CONDITIONAL;
			}
		case ConditionalParseState::CONDITIONAL:
			if (substring != ")")
			{
				conditional += substring;
				currState = ConditionalParseState::CONDITIONAL;
			}
			else
			{
				currState = ConditionalParseState::BRACKETS;
			}
			break;
		case ConditionalParseState::BRACKETS:
			if (substring == "{")
			{
				currState = ConditionalParseState::OPERATION;
			}
			break;
		case ConditionalParseState::OPERATION:
			if (substring == "}")
			{
				currState = ConditionalParseState::OPERATION;
			}
			else
			{
				conditional += substring;
			}
			break;
		default:
			break;
		}

		start = end + delim.length();
		end = s.find(delim, start);
	}

	// for (std::vector<std::string>::iterator itr = varNames.begin(); itr < varNames.end(); itr++)
	// {
	// 	if (*itr != "")
	// 	{
	// 		BaseType data(isSigned, dataSize, dType);
	// 		dataDefs[*itr] = data;
	// 	}
	// }
	// return (dType != BaseType::DataType::NONE) && (dataSize != 0);
	return false;
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
bool HlsGen::parseOpsLine(std::string line, std::map<unsigned int, Vertex>& opsDefs)
{
	bool retVal(true);
	std::string s = line + " |"; //add ' ' for string parsing
	std::string delim = " ";
	//std::map<>
	auto start = 0u;
	auto end = s.find(delim);
	OpsParseState parseState(OpsParseState::OUTPUT_VAR);
	Vertex::Operation op(Vertex::Operation::NOP);
	std::string outputVar("");
	std::string dataVar1("");
	std::string dataVar2("");
	std::string dataVar3("");

	while (end != std::string::npos)
	{

		std::string substring = s.substr(start, end - start);
		switch (parseState)
		{
		case OpsParseState::OUTPUT_VAR:
			if (substring != "")
			{

				outputVar = substring;
				parseState = OpsParseState::EQUAL_SIGN;
			}
			break;
		case OpsParseState::EQUAL_SIGN:
			if (substring == "=")
			{
				parseState = OpsParseState::DATA1_EXTRACT;
			}
			break;
		case OpsParseState::DATA1_EXTRACT:
			if (substring != "")
			{
				dataVar1 = substring;
				parseState = OpsParseState::OP_EXTRACT;
			}

			break;
		case OpsParseState::OP_EXTRACT:
			if (opsMap_.find(substring) != opsMap_.end())
			{
				op = opsMap_[substring];
				parseState = OpsParseState::DATA2_EXTRACT;
				retVal = true;
			}
			else
			{
				//invalid operator
				retVal = false;
			}
			break;
		case OpsParseState::DATA2_EXTRACT:
			if (substring != "")
			{
				dataVar2 = substring;
				if (op == Vertex::Operation::MUX2x1)
				{
					parseState = OpsParseState::MUX_LOGIC;
				}
				else
				{
					parseState = OpsParseState::COMPLETE;
				}
			}
			break;
		case OpsParseState::MUX_LOGIC:
			if (substring == ":")
			{
				parseState = OpsParseState::DATA3_EXTRACT;
			}
			break;
		case OpsParseState::DATA3_EXTRACT:
			if (substring != " " && substring != "")
			{
				dataVar3 = substring;
				parseState = OpsParseState::COMPLETE;
			}
			break;
		default:
			break;
		}

		start = end + delim.length();
		end = s.find(delim, start);
	}

	if (parseState == OpsParseState::OP_EXTRACT)
	{
		op = Vertex::Operation::REG;
		parseState = OpsParseState::COMPLETE;
	}
	if (parseState == OpsParseState::COMPLETE)
	{
		Vertex newElement;
		newElement.output_ = outputVar;
		newElement.inputs_[0] = dataVar1;
		newElement.inputs_[1] = dataVar2;
		newElement.inputs_[2] = dataVar3;
		newElement.op_ = op;
		int vertexNum(0);
		if (!opsDefs.empty())
		{
			vertexNum = (--opsDefs.end())->first + 1;
		}
		opsDefs[vertexNum] = newElement;
		opsDefs2_[outputVar] = newElement;
	}
	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
void HlsGen::writeHeader(std::ofstream& outputStream)
{
	outputStream << "`timescale 1ns / 1ps\n";
	outputStream << "//////////////////////////////////////////////////////////////////////////////////\n";
	outputStream << "// Company: \n";
	outputStream << "// Engineer: Dillon Gregerson\n";
	outputStream << "// \n";
	outputStream << "// Create Date: \n";
	outputStream << "// Design Name: \n";
	outputStream << "// Module Name: " << circuitName_ << std::endl;
	outputStream << "// Project Name: \n";
	outputStream << "// Target Devices: \n";
	outputStream << "// Tool Versions: \n";
	outputStream << "// Description: \n";
	outputStream << "// \n";
	outputStream << "// Dependencies: \n";
	outputStream << "// \n";
	outputStream << "// Revision: \n";
	outputStream << "// Revision 0.01 - File Created \n";
	outputStream << "// Additional Comments: \n";
	outputStream << "//\n";
	outputStream << "//////////////////////////////////////////////////////////////////////////////////\n\n";
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
std::string HlsGen::moduleSignature(std::string outputFile)
{
	std::string writeLine = "module " + outputFile + " (Clk, Rst, ";
	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::INPUT)
		{
			writeLine += it->first;
			writeLine += ", ";
		}
	}

	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::OUTPUT)
		{
			writeLine += it->first;
			writeLine += ", ";
		}
	}
	writeLine.erase(writeLine.end() - 2, writeLine.end());
	writeLine += ");\n";
	return writeLine;

}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
void HlsGen::writeDataDefinitions(std::ofstream& outputStream)
{
	outputStream << "input Clk, Rst; \n";
	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::INPUT)
		{
			outputStream << it->second.printDataType() << it->second.printDataWidth() << it->first << ";\n";
		}
	}
	outputStream << "\n";
	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::WIRE)
		{
			outputStream << it->second.printDataType() << it->second.printDataWidth() << it->first << ";\n";
		}
	}
	outputStream << "\n";
	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::OUTPUT)
		{
			outputStream << it->second.printDataType() << it->second.printDataWidth() << it->first << ";\n";
		}
	}
	outputStream << "\n";
	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::REGISTER)
		{
			outputStream << it->second.printDataType() << it->second.printDataWidth() << it->first << ";\n";
		}
	}
	outputStream << "\n";
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
bool HlsGen::isNumber(const std::string& str)
{
	for (char const& c : str) {
		if (std::isdigit(c) == 0) return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
bool HlsGen::writeOperations(std::ofstream& outputStream)
{
	unsigned int uniqueNum(0);
	bool retVal = true;
	for (std::map<unsigned int, Vertex>::iterator it = opsDefs_.begin(); it != opsDefs_.end(); it++)
	{
		switch (it->second.op_)
		{
		case Vertex::Operation::NOP:
			break;
		case Vertex::Operation::REG:
			if (dataDefs_.count(it->second.output_) == 0)
			{
				retVal = false;
			}

			if (dataDefs_.count(it->second.inputs_[0]) == 0 && !isNumber(it->second.inputs_[0]))
			{
				retVal = false;
			}
			break;
		case Vertex::Operation::ADD:
		case Vertex::Operation::SUB:
		case Vertex::Operation::MULT:
		case Vertex::Operation::SHR:
		case Vertex::Operation::SHL:
		case Vertex::Operation::DIV:
		case Vertex::Operation::MOD:
		case Vertex::Operation::INC:
		case Vertex::Operation::DEC:
		case Vertex::Operation::COMP_EQ:
		case Vertex::Operation::COMP_LT:
		case Vertex::Operation::COMP_GT:
			if (dataDefs_.count(it->second.output_) == 0)
			{
				retVal = false;
			}
			if (dataDefs_.count(it->second.inputs_[0]) == 0 && !isNumber(it->second.inputs_[0]))
			{
				retVal = false;
			}
			if (dataDefs_.count(it->second.inputs_[1]) == 0 && !isNumber(it->second.inputs_[1]))
			{
				retVal = false;
			}
			break;
		case Vertex::Operation::MUX2x1:
			if (dataDefs_.count(it->second.output_) == 0)
			{
				retVal = false;
			}
			if (dataDefs_.count(it->second.inputs_[0]) == 0 && !isNumber(it->second.inputs_[0]))
			{
				retVal = false;
			}
			if (dataDefs_.count(it->second.inputs_[1]) == 0 && !isNumber(it->second.inputs_[1]))
			{
				retVal = false;
			}
			if (dataDefs_.count(it->second.inputs_[2]) == 0 && !isNumber(it->second.inputs_[2]))
			{
				retVal = false;
			}
			break;
		default:
			std::cout << "\nERROR!\n";
		}
	}

	for (std::map<unsigned int, Vertex>::iterator it = opsDefs_.begin(); it != opsDefs_.end(); it++)
	{
		if ((dataDefs_[it->second.inputs_[0]].dataType_ == BaseType::DataType::INPUT) &&
			dataDefs_[it->second.inputs_[1]].dataType_ == BaseType::DataType::INPUT)
		{
			outputStream << it->second.printMixedExtension(dataDefs_) << "\n";
			uniqueNum++;
		}
	}

	for (std::map<unsigned int, Vertex>::iterator it = opsDefs_.begin(); it != opsDefs_.end(); it++)
	{
		outputStream << it->second.printOp(dataDefs_, uniqueNum) << "\n";
		uniqueNum++;
	}
	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
bool HlsGen::generateVerilog(std::string outputFile)
{
	std::ofstream outdata; // outdata is like cin
	int i; // loop index

	outdata.open(std::string(outputFile)); // opens the file
	if (!outdata) { // file couldn't be opened
		std::cerr << "Error: file could not be opened" << std::endl;
		exit(1);
	}

	writeHeader(outdata);
	outputFile.erase(outputFile.end() - 2, outputFile.end()); //removing the '.v'
	outdata << moduleSignature(outputFile) << "\n";
	writeDataDefinitions(outdata);
	bool retVal = false;
	retVal = writeOperations(outdata);
	if (retVal == false)
	{
		std::cout << "error: missing data declaration detected\n";
	}
	outdata << "\nendmodule\n";
	outdata.close();
	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
double HlsGen::getCriticalPath(void)
{
	double longestPath(0.0);
	for (std::map<unsigned int, Vertex>::iterator it = opsDefs_.begin(); it != opsDefs_.end(); it++)
	{
		double componentLatency = getLatency(it->second.op_, dataDefs_[it->second.output_].dataWidth_);
		longestPath = longestPath > componentLatency ? longestPath : componentLatency;
	}
	return longestPath;
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
bool HlsGen::getDependencies(std::string vtx)
{
	vtxStack_.push(vtx);
	bool retVal = false;
	num_iteration_++;
	//if we are not in the terminal state (ie register, output etc)
	if (outputStates_.find(vtx) == outputStates_.end())
	{
		for (std::vector<std::string>::iterator it = dag_[vtx].begin(); it < dag_[vtx].end(); it++)
		{
			retVal = false;
			retVal = getDependencies(*it);
		}
	}
	else
	{
		paths_.push_back(vtxStack_);
		retVal = true;
	}
	vtxStack_.pop();

	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
void HlsGen::createDag()
{
	//iterate over all of the ops definitions. 
	//we want to construct the map such that all nodes, including initial inputs, have a vector of 
	//Vertexes which are dependent on them. We will do this by iterating through all of the operations.
	//For each vertex we will append it's inputs to the vector at the value of the map.

	for (std::map<std::string, Vertex>::iterator it = opsDefs2_.begin(); it != opsDefs2_.end(); it++)
	{
		for (int i = 0; i < 3; i++)
		{
			if (it->second.inputs_[i] != "")
			{
				if (dag_.count(it->second.inputs_[i]) == 0)
				{
					//if the input does not exist as a key in the map yet, make it. the value will be the current
					//key (since this is what denotes the vertex that is dependent on it).
					dag_[it->second.inputs_[i]] = { it->first };
				}
				else
				{
					//if it already exists we will push to the vector
					dag_[it->second.inputs_[i]].push_back(it->first);
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
double HlsGen::determineCriticalPath(void)
{
	// we need to delineate the following to check the critical paths:
	// input to register
	// register to register
	// output to register

	double longestPath(0.0);
	std::unordered_set<std::string>registers;

	//First we will check input to register, to do this we need to first create an unordered set
	//of the vertecies that will be used as our terminating condition for the recursive function
	for (std::map<std::string, Vertex>::iterator it = opsDefs2_.begin(); it != opsDefs2_.end(); it++)
	{
		if (it->second.op_ == Vertex::Operation::REG)
		{
			outputStates_.insert(it->second.inputs_[0]); //because the we only need to check that the input to the reg has arrived
														 //in our critical path calculation
			registers.insert(it->second.inputs_[0]);
		}
	}
	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::INPUT)
		{
			num_iteration_ = 0;
			getDependencies(it->first);
		}
	}

	//iterate through each of the stacks and calculate the overall latency of the path
	for (std::vector<std::stack<std::string>>::iterator it = paths_.begin(); it < paths_.end(); it++)
	{
		double pathLength = 0.0;
		while (!it->empty())
		{
			if (opsDefs2_.count(it->top()) != 0)
			{
				double componentLatency = getLatency(opsDefs2_[it->top()].op_, dataDefs_[opsDefs2_[it->top()].output_].dataWidth_);
				pathLength += componentLatency;
			}
			it->pop();
		}
		longestPath = pathLength > longestPath ? pathLength : longestPath;
	}

	//Now we need to check for input to output- since multibit outputs are registered in verilog
	//we will follow the same pattern as for the registers, the only difference being we will be filtering
	//for outputs instead of registers.

	outputStates_.clear();

	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::OUTPUT)
		{
			//here we look up the operation using the key we are iterating through (ie 'x'), then we check
			//if the input to that output was added to our list of registers, if it was not then we will add it 
			//to our list of outputs
			if (registers.count(opsDefs2_[it->first].inputs_[0]) == 0)
			{
				outputStates_.insert(it->first);
			}
		}
	}

	paths_.clear();
	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::INPUT)
		{
			num_iteration_ = 0;
			getDependencies(it->first);
		}
	}

	//iterate through each of the stacks and calculate the overall latency of the path
	for (std::vector<std::stack<std::string>>::iterator it = paths_.begin(); it < paths_.end(); it++)
	{
		double pathLength = 0.0;
		while (!it->empty())
		{
			if (opsDefs2_.count(it->top()) != 0)
			{
				double componentLatency = getLatency(opsDefs2_[it->top()].op_, dataDefs_[opsDefs2_[it->top()].output_].dataWidth_);
				pathLength += componentLatency;
			}
			it->pop();
		}
		longestPath = pathLength > longestPath ? pathLength : longestPath;
	}
	return longestPath;
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
unsigned int HlsGen::getAsapTimes(std::string vtx, unsigned int layer)
{
	unsigned int retVal = 0;
	//if we are not in the terminal state (ie register, output etc)
	//std::cout << "entering \n";
	//std::cout << vtx << ", opsDefs2_[vtx].ASAPtimeFrame_: " << opsDefs2_[vtx].ASAPtimeFrame_ << ", layer: " << layer << std::endl;
	if (outputStates_.find(vtx) == outputStates_.end())
	{
		for (std::vector<std::string>::iterator it = dag_[vtx].begin(); it < dag_[vtx].end(); it++)
		{
			unsigned int componentLatency;
			if (opsDefs2_[vtx].op_ == Vertex::Operation::MULT)
			{
				componentLatency = 2;
			}
			else if (opsDefs2_[vtx].op_ == Vertex::Operation::DIV ||
				opsDefs2_[vtx].op_ == Vertex::Operation::MOD)
			{
				componentLatency = 3;
			}
			else
			{
				componentLatency = 1;
			}

			retVal = getAsapTimes(*it, layer + componentLatency);
		}
	}
	else
	{
		retVal = 0;
	}

	//update the vertex's asap and alap time frames (we want to update to the largest)
	//opsDefs2_[vtx].ALAPtimeFrame_ = retVal > opsDefs2_[vtx].ALAPtimeFrame_? retVal: opsDefs2_[vtx].ALAPtimeFrame_;
	opsDefs2_[vtx].ASAPtimeFrame_ = layer > opsDefs2_[vtx].ASAPtimeFrame_ ? layer : opsDefs2_[vtx].ASAPtimeFrame_;
	//std::cout << vtx << ", opsDefs2_[vtx].ASAPtimeFrame_: " << opsDefs2_[vtx].ASAPtimeFrame_ << ", layer: " << layer << std::endl;
	//std::cout << "exiting\n";
	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
unsigned int HlsGen::getAlapTimes(std::string vtx, unsigned int layer)
{
	unsigned int retVal = 0;
	//if we are not in the terminal state (ie register, output etc)
	//std::cout << "entering\n";
	//std::cout << vtx << ", opsDefs2_[vtx].ASAPtimeFrame_: " << opsDefs2_[vtx].ASAPtimeFrame_ << ", layer: " << layer << std::endl;
	unsigned int componentLatency(0);
	if (opsDefs2_[vtx].op_ == Vertex::Operation::MULT)
	{
		componentLatency = 2;
	}
	else if (opsDefs2_[vtx].op_ == Vertex::Operation::DIV ||
		opsDefs2_[vtx].op_ == Vertex::Operation::MOD)
	{
		componentLatency = 3;
	}
	else
	{
		componentLatency = 1;
	}
	if (outputStates_.find(vtx) == outputStates_.end())
	{
		for (std::vector<std::string>::iterator it = invDag_[vtx].begin(); it < invDag_[vtx].end(); it++)
		{
			retVal = getAlapTimes(*it, layer + componentLatency);
		}
	}
	else
	{
		retVal = 0;
	}
	layer = layer + componentLatency - 1; //because we need to factor in the amount of time it takes to run this component

	//update the vertex's asap and alap time frames (we want to update to the largest)
	opsDefs2_[vtx].ALAPtimeFrame_ = layer > opsDefs2_[vtx].ALAPtimeFrame_ ? layer : opsDefs2_[vtx].ALAPtimeFrame_;
	//std::cout << "exiting\n";
	//std::cout << vtx << ", opsDefs2_[vtx].ALAPtimeFrame_: " << opsDefs2_[vtx].ALAPtimeFrame_ << ", layer: " << layer << std::endl;
	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
void HlsGen::addToInvDag(std::string vtx)
{
	for (std::map<std::string, std::vector<std::string>>::iterator it = dag_.begin(); it != dag_.end(); it++)
	{
		for (std::vector<std::string>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			if (*it2 == vtx)
			{
				if (std::count(invDag_[vtx].begin(), invDag_[vtx].end(), it->first) == 0)
				{
					invDag_[vtx].push_back(it->first);
					addToInvDag(it->first);
				}
			}
		}
	}
}

bool HlsGen::invertDag(void)
{
	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		//iterate through all of the output variables
		if (it->second.dataType_ == BaseType::DataType::OUTPUT)
		{
			addToInvDag(it->first);
		}
	}
	for (std::map<std::string, std::vector<std::string>>::iterator it = invDag_.begin(); it != invDag_.end(); it++)
	{
		std::cout << it->first << " : ";
		for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			std::cout << *it2 << " ";
		}
		std::cout << std::endl;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
bool HlsGen::populateTimeFrames(void)
{
	outputStates_.clear();

	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::OUTPUT)
		{
			//here we look up the operation using the key we are iterating through (ie 'x'), then we check
			//if the input to that output was added to our list of registers, if it was not then we will add it 
			//to our list of outputs
			outputStates_.insert(it->first);
		}
	}

	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::INPUT)
		{
			num_iteration_ = 0;
			unsigned int layer(0);
			getAsapTimes(it->first, layer);
		}
	}

	outputStates_.clear();

	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::INPUT)
		{
			//here we look up the operation using the key we are iterating through (ie 'x'), then we check
			//if the input to that output was added to our list of registers, if it was not then we will add it 
			//to our list of outputs
			outputStates_.insert(it->first);
		}
	}

	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::OUTPUT)
		{
			num_iteration_ = 0;
			unsigned int layer(0);
			getAlapTimes(it->first, layer);
		}
	}

	bool validLatency = true;
	
	for (std::map<std::string, Vertex>::iterator it = opsDefs2_.begin(); it != opsDefs2_.end(); it++)
	{
		if ((it->second.ALAPtimeFrame_ > latency_) || (it->second.ASAPtimeFrame_ > latency_))
		{
			validLatency = false;
		}
		else
		{
			it->second.ALAPtimeFrame_ = latency_ - it->second.ALAPtimeFrame_;
			it->second.timeFrame_[0] = it->second.ASAPtimeFrame_;
			it->second.timeFrame_[1] = it->second.ALAPtimeFrame_;
		}
		std::cout << it->first << " : [" << it->second.timeFrame_[0] << ", " << it->second.timeFrame_[1] << "]" << std::endl;
	}
	return validLatency;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////