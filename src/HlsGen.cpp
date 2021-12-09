////////////////////////////////////////////////////////////////////////
// filename - HlsGen.cpp
// author - Dillon Gregerson
// project - Ece574 Assignment 2
////////////////////////////////////////////////////////////////////////

#include "HlsGen.h"
#include <regex>

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
HlsGen::HlsGen(void):currState_(ConditionalParseState::IF)
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

void HlsGen::printOps(void)
{
	for (std::map<std::string, Vertex>::iterator itr = opsDefs2_.begin(); itr != opsDefs2_.end(); itr++)
	{
		std::cout << itr->first << ": " << itr->second.rawLine << "\n";
	}
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
	if (!parseData(invalidLines, file))
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
	bool isConditionalLine(false);
	bool opsValid(true);
	// first open the file to parse out the data types
	if (file.is_open())
	{
		std::string line;
		unsigned int a = 0;
		while (getline(file, line))
		{
			if (std::count(invalidLines.begin(), invalidLines.end(), a))
			{
				//the previous parse data function would have considered anything that was not 
				//data related an invalid line. Therefore we only want to look at 'invalid' lines
				if (!parseConditionals(line)) //if it is not a conditional line, parse as ops line
				{
					isConditionalLine = parseConditionalLine(line);
					if (!isConditionalLine)
					{
						retVal = parseOpsLine(line, opsDefs_) ;
						if (!retVal)
						{
							opsValid = false;
						}
					}
				}
			}
			a++;
		}
	}
	return opsValid;
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
			else if (s.substr(start, end - start) == "variable")
			{
				dType = BaseType::DataType::VARIABLE;
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
			dataDefs[*itr].original = true;

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


bool HlsGen::parseConditionals(std::string line)
{

	return false;
}

bool HlsGen::parseConditionalLine(std::string line)
{
	std::string s = line + " "; //add ' ' for string parsing
	bool lineValid(false);

	std::vector<std::string>varNames;
	std::string strName("");
	std::string operation("");

	std::string delim = " ";
	auto start = 0U;
	auto end = s.find(delim);
	ConditionalParseState prevState = currState_;
	std::pair<std::string, std::string> conditionalPair; //contains an 'if/else' element as well as the conditional itself

	while (end != std::string::npos)
	{
    	std::string substring = s.substr(start, end - start);

		substring.erase(std::remove_if(substring.begin(), substring.end(), ::isspace), substring.end());

		switch (currState_)
		{
		case ConditionalParseState::IF:
			if (substring == "if")
			{
				lineValid = true;
				conditionalPair.first = "if";
				currState_ = ConditionalParseState::PARENTHESIS1;
			}
			if (substring == "else")
			{
				lineValid = true;
				conditionalPair.first = "else";
				conditionalPair.second = conditional_;
				conditionalStack_.push(conditionalPair);
			}
			if (substring == "}")
			{
				lineValid = true;
				conditionalStack_.pop();//since we are not longer in the conditional, pop it off
				currState_ = ConditionalParseState::IF;
			}
			break;
		case ConditionalParseState::PARENTHESIS1:
			if (substring == "(")
			{
				currState_ = ConditionalParseState::CONDITIONAL;
			}
			break;
		case ConditionalParseState::CONDITIONAL:
			if (substring != ")")
			{
				conditional_ = substring;
				conditionalPair.second = conditional_;
				conditionalStack_.push(conditionalPair);
				currState_ = ConditionalParseState::PARENTHESIS2;
			}
			break;
		case ConditionalParseState::PARENTHESIS2:
			if (substring == ")")
			{
				currState_ = ConditionalParseState::BRACKETS;
			}
		case ConditionalParseState::BRACKETS:
			if (substring == "{")
			{
				currState_ = ConditionalParseState::IF;
			}
			break;
		case ConditionalParseState::OPERATION:
			if (substring == "}")
			{
				try
				{
				    conditionalStack_.pop();//since we are not longer in the conditional, pop it off
				}
				catch(...)
				{
					//do some exception handling and error logging here
				}
				currState_ = ConditionalParseState::IF;
			}
			break;
		default:
			break;
		}

		start = end + delim.length();
		end = s.find(delim, start);
	}

	return lineValid;
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
		substring.erase(std::remove_if(substring.begin(), substring.end(), ::isspace), substring.end());
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

	Vertex newElement;
	if (parseState == OpsParseState::OP_EXTRACT)
	{
		op = Vertex::Operation::REG;
		parseState = OpsParseState::COMPLETE;
	}
	if (parseState == OpsParseState::COMPLETE)
	{
		std::stack<std::pair<std::string, std::string>>conditionalStack = conditionalStack_;
		newElement.conditionals_ = conditionalStack_;
		newElement.output_ = outputVar;
		newElement.outputVar_ = outputVar;
		newElement.inputs_[0] = dataVar1;
		newElement.inputs_[1] = dataVar2;
		newElement.inputs_[2] = dataVar3;
		int idx = 3;
		while(!conditionalStack.empty())
		{
			if (idx < 8)
			{
				newElement.inputs_[idx] = conditionalStack.top().second;
				newElement.output_ = newElement.output_ + "_" + newElement.inputs_[idx];
			}
			conditionalStack.pop();
			idx++;
		}
		if (newElement.conditionals_.size() > 0 && dataDefs_.count(newElement.output_) == 0)
		{
			BaseType newData;
			newData.dataType_ = dataDefs_[outputVar].dataType_;
			newData.dataWidth_ = dataDefs_[outputVar].dataWidth_;
			dataDefs_[newElement.output_] = newData;
			dataDefs_[newElement.output_].original = false;
			if (conditionalDependencies_.find(outputVar) == conditionalDependencies_.end())
			{
				conditionalDependencies_[outputVar] = { newElement.output_ };
			}
			else
			{
				conditionalDependencies_[outputVar].push_back(newElement.output_);
			}
		}

		newElement.op_ = op;
		int vertexNum(0);
		if (!opsDefs.empty())
		{
			vertexNum = (--opsDefs.end())->first + 1;
		}
		opsDefs[vertexNum] = newElement;
		if (newElement.output_ != " ")
		{
		opsDefs2_[newElement.output_] = newElement;
		}
		opsDefs2_[newElement.output_].rawLine = line;
		if (dataDefs_.count(outputVar) == 0)
		{
			retVal = false;
			std::cout << "error: " << outputVar << " not defined\n";
		}
		if (dataDefs_.count(dataVar1) == 0)
		{
			retVal = false;
			std::cout << "error: " << dataVar1 << " not defined\n";
		}
		if(dataDefs_.count(dataVar2) ==0)
		{
			retVal = false;
			std::cout << "error: " << dataVar2 << " not defined\n";
		}
	}

	std::remove_if(line.begin(), line.end(), ::isspace), line.end();
	if (line == "")
	{
		retVal = true;
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
	std::string writeLine = "module HLSM(Clk, Rst, Start, Done, ";
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
		if (it->second.dataType_ == BaseType::DataType::OUTPUT && it->second.original)
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
	outputStream << "input Clk, Rst, Start; \n";
	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::INPUT)
		{
			if(it->second.original)
			{
			    outputStream << it->second.printDataType() << it->second.printDataWidth() << it->first << ";\n";
			}
		}
	}
	outputStream << "\n";
	std::unordered_set<std::string>writeSetWire;

	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
			if (it->second.dataType_ == BaseType::DataType::WIRE)
			{
				if (it->second.original)
				{
					outputStream << it->second.printDataType() << it->second.printDataWidth() << it->first << ";\n";
					writeSetWire.insert(it->first);
				}
		}
	}
	outputStream << "\n";
	std::unordered_set<std::string>writeSetOutput;
	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::OUTPUT)
		{
			if (it->second.original)
			{
				outputStream << it->second.printDataType() << it->second.printDataWidth() << it->first << ";\n";
				writeSetOutput.insert(it->first);
			}
		}
	}
	outputStream << "output reg Done; \n\n";
	outputStream << "reg[" << std::ceil(std::log2(latency_)) << ":0] " "State; \n";
	std::unordered_set<std::string>writeSetReg;
	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::REGISTER)
		{
			if (it->second.original)
			{
				outputStream << it->second.printDataType() << it->second.printDataWidth() << it->first << ";\n";
				writeSetReg.insert(it->first);
			}
		}
	}

	outputStream << "\n";
	for (int i = 0; i <= latency_+1; i++)
	{
	    outputStream << "parameter["<< std::ceil(std::log2(latency_)) << ":0]" << " S" << i <<"="<< i << ";\n"; 
	}
	outputStream << "\n";
	std::unordered_set<std::string>writeSetVar;

	for (std::map<std::string, BaseType>::iterator it = dataDefs_.begin(); it != dataDefs_.end(); it++)
	{
		if (it->second.dataType_ == BaseType::DataType::VARIABLE)
		{
			if (it->second.original)
			{
				outputStream << it->second.printDataType() << it->second.printDataWidth() << it->first << ";\n";
			}
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

    outputStream << "always @(posedge Clk) begin\n" << "if (Rst == 1) begin\n" << "\t\tDone <= 0;\nend\nelse begin\ncase (State)\n";
	outputStream << "S0: begin\n";
	outputStream << "\t\tif(Start == 1 ) begin\n";
	outputStream << "\t\t\tState <= S1;\n";
	outputStream << "\t\tend\n";
	outputStream << "\t\telse begin\n";
	outputStream << "\t\t\tState <= S0;\n";
	outputStream << "\t\tend\n";
	outputStream << "end\n";

	for(int i = 0; i < latency_; i++)
	{
		outputStream << "S" << i+1 << ": begin\n";
		
		for(std::vector<std::string>::iterator itr = schedule_[i].begin(); itr!= schedule_[i].end(); itr++)
		{
			int numTabs = 2;
			std::stack<std::pair<std::string, std::string>> conditionals = opsDefs2_[*itr].conditionals_;
			while (!conditionals.empty())
			{

				int tabIndex = numTabs;
				while (tabIndex > 0)
				{
					tabIndex--;
					outputStream << "\t";
				}
				if (conditionals.top().first == "if")
				{
					outputStream << conditionals.top().first << "(" << conditionals.top().second << ") begin\n";
				}
				else
				{
					outputStream << "if (~" << conditionals.top().second << ") begin\n";
				}
				conditionals.pop();

				numTabs++;
			}
			outputStream << "\t\t";
			bool firstEq(false);
			opsDefs2_[*itr].rawLine = std::regex_replace(opsDefs2_[*itr].rawLine, std::regex("^ +"), "");

			for(int j = 0; j < opsDefs2_[*itr].rawLine.length(); j++)
			{
				if(!firstEq && opsDefs2_[*itr].rawLine[j] == '=')
				{
					firstEq=true;
					outputStream << "<";
				}
				outputStream << opsDefs2_[*itr].rawLine[j];
			}
			outputStream << ";\n";
			numTabs--;
			conditionals = opsDefs2_[*itr].conditionals_;
			while (!conditionals.empty())
			{
				int tabIndex = numTabs;
				while (tabIndex > 0)
				{
					outputStream << "\t";
					tabIndex--;
				}
				outputStream << "end\n";
				conditionals.pop();

				numTabs--;
			}
		}
		outputStream << "\t\tState <= S" << i+1 <<";\n";
		outputStream << "\tend\n";
	}
	outputStream << "S" << latency_+1 << ": begin\n";
	outputStream << "\t\tDone <= 1;\n";
	outputStream << "\t\tState <= S0" <<";\n";
	outputStream << "\tend\n";

	outputStream << "endcase\n";
	outputStream << "end\n";
	outputStream << "end\n";
	outputStream << "endmodule\n";
	return true;
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
		for (int i = 0; i < 8; i++)
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

	for (std::map<std::string, Vertex>::iterator it = opsDefs2_.begin(); it != opsDefs2_.end(); it++)
	{
		if (dataDefs_[it->first].dataType_ == BaseType::DataType::OUTPUT)
		{
			//here we look up the operation using the key we are iterating through (ie 'x'), then we check
			//if the input to that output was added to our list of registers, if it was not then we will add it 
			//to our list of outputs
			if (registers.count(it->second.inputs_[0]) == 0)
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
	unsigned int componentLatency = 1;
	//if we are not in the terminal state (ie register, output etc)

	if (opsDefs2_.count(vtx) > 0)
	{
		if (opsDefs2_.at(vtx).op_ == Vertex::Operation::MULT)
		{
			componentLatency = 2;
		}
		else if (opsDefs2_.at(vtx).op_ == Vertex::Operation::DIV ||
			opsDefs2_.at(vtx).op_ == Vertex::Operation::MOD)
		{
			componentLatency = 3;
		}
		else
		{
			componentLatency = 1;
		}
	}

	if (outputStates_.find(vtx) == outputStates_.end())
	{
		for (std::vector<std::string>::iterator it = dag_[vtx].begin(); it < dag_[vtx].end(); it++)
		{
			retVal = getAsapTimes(*it, layer + componentLatency);
		}
	}
	else
	{
		retVal = 0;
	}

	//update the vertex's asap and alap time frames (we want to update to the largest)
	//opsDefs2_[vtx].ALAPtimeFrame_ = retVal > opsDefs2_[vtx].ALAPtimeFrame_? retVal: opsDefs2_[vtx].ALAPtimeFrame_;
	if (opsDefs2_.count(vtx) > 0)
	{
		if (dataDefs_.count(vtx) > 0)
		{
			if (dataDefs_.at(vtx).dataType_ != BaseType::DataType::INPUT)
			{
				opsDefs2_.at(vtx).ASAPtimeFrame_ = layer > opsDefs2_.at(vtx).ASAPtimeFrame_-1 ? layer : opsDefs2_.at(vtx).ASAPtimeFrame_-1;
			}
		}
	}

	return retVal;
}

unsigned int HlsGen::getAsapTimes2(std::string vtx, unsigned int layer)
{
	unsigned int retVal = 0;
	unsigned int componentLatency = 1;
	//if we are not in the terminal state (ie register, output etc)

	if (opsDefs2_.count(vtx) > 0)
	{
		if (opsDefs2_.at(vtx).op_ == Vertex::Operation::MULT)
		{
			componentLatency = 2;
		}
		else if (opsDefs2_.at(vtx).op_ == Vertex::Operation::DIV ||
			opsDefs2_.at(vtx).op_ == Vertex::Operation::MOD)
		{
			componentLatency = 3;
		}
		else
		{
			componentLatency = 1;
		}
	}

	if (outputStates_.find(vtx) == outputStates_.end())
	{
		for (std::vector<std::string>::iterator it = dag_[vtx].begin(); it < dag_[vtx].end(); it++)
		{
			if (opsDefs2_.count(*it) > 0)
			{
			retVal = getAsapTimes2(*it, layer + componentLatency);
			}
		}
	}
	else
	{
		retVal = 0;
	}

	//update the vertex's asap and alap time frames (we want to update to the largest)
	//opsDefs2_[vtx].ALAPtimeFrame_ = retVal > opsDefs2_[vtx].ALAPtimeFrame_? retVal: opsDefs2_[vtx].ALAPtimeFrame_;
	if (opsDefs2_.count(vtx) > 0)
	{
		if (dataDefs_.count(vtx) > 0)
		{
			if (dataDefs_.at(vtx).dataType_ != BaseType::DataType::INPUT)
			{
				opsDefs2_.at(vtx).ASAPtimeFrame_ = layer > opsDefs2_.at(vtx).ASAPtimeFrame_ ? layer : opsDefs2_.at(vtx).ASAPtimeFrame_;
			}
		}
	}

	return retVal;

}
//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
unsigned int HlsGen::getAlapTimes(std::string vtx, unsigned int layer)
{
	unsigned int retVal = 0;
	//if we are not in the terminal state (ie register, output etc)

	unsigned int componentLatency(0);
	if (opsDefs2_.count(vtx) > 0)
	{
		if (opsDefs2_.at(vtx).op_ == Vertex::Operation::MULT)
		{
			componentLatency = 2;
		}
		else if (opsDefs2_.at(vtx).op_ == Vertex::Operation::DIV ||
			opsDefs2_.at(vtx).op_ == Vertex::Operation::MOD)
		{
			componentLatency = 3;
		}
		else
		{
			componentLatency = 1;
		}
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
	if (opsDefs2_.count(vtx) > 0)
	{
		if (dataDefs_[vtx].dataType_ != BaseType::DataType::INPUT)
		{
			opsDefs2_.at(vtx).ALAPtimeFrame_ = layer > opsDefs2_.at(vtx).ALAPtimeFrame_ ? layer : opsDefs2_.at(vtx).ALAPtimeFrame_;
		}
	}

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
					if (opsDefs2_.count(it->first) > 0)
					{
						invDag_[vtx].push_back(it->first);
						addToInvDag(it->first);
					}
				}
			}
		}
	}
}

bool HlsGen::updateDag(void)
{
	std::map<std::string, std::vector<std::string>>prevDag = dag_;

	for (auto itrn = prevDag.begin(); itrn != prevDag.end(); itrn++)
	{
		if (conditionalDependencies_.count(itrn->first) > 0)
		{
			for (std::vector<std::string>::iterator itr2 = conditionalDependencies_[itrn->first].begin();
				itr2 != conditionalDependencies_[itrn->first].end(); itr2++)
			{
				dag_[*itr2] = dag_[itrn->first];
			}
		}
	}
	return true;
}
bool HlsGen::invertDag(void)
{
	updateDag();

	for (std::map<std::string, Vertex>::iterator it = opsDefs2_.begin(); it != opsDefs2_.end(); it++)
	{
		//iterate through all of the output variables
		if (dataDefs_[it->first].dataType_ == BaseType::DataType::OUTPUT)
		{
			addToInvDag(it->first);
		}
	}

	//now iterate through the dag we just populated, if its dependency is in the dependency map, push those dependencies into dag too
	std::map<std::string, std::vector<std::string>>prevDag = invDag_;

	for (auto itrn = prevDag.begin(); itrn != prevDag.end(); itrn++)
	{
		for (auto i = itrn->second.begin(); i != itrn->second.end(); i++)
		{
			if (conditionalDependencies_.count(*i) > 0)
			{
				for (std::vector<std::string>::iterator itr2 = conditionalDependencies_[*i].begin();
					itr2 != conditionalDependencies_[*i].end(); itr2++)
				{
					invDag_[itrn->first].push_back(*itr2);
				}
			}
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// see header file for method desription
//////////////////////////////////////////////////////////////////////////////
bool HlsGen::populateTimeFrames(void)
{
	outputStates_.clear();

	for (std::map<std::string, Vertex>::iterator it = opsDefs2_.begin(); it != opsDefs2_.end(); it++)
	{
		if (dataDefs_[it->first].dataType_ == BaseType::DataType::OUTPUT)
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
			getAsapTimes2(it->first, layer);
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
		num_iteration_ = 0;
		unsigned int layer(0);
		getAlapTimes(it->first, layer);
	}

	bool validLatency = true;
	unsigned int minLatency = latency_;
	for (std::map<std::string, Vertex>::iterator it = opsDefs2_.begin(); it != opsDefs2_.end(); it++)
	{
		if (it->second.ASAPtimeFrame_ > minLatency)
		{
			validLatency = false;
			minLatency = it->second.ASAPtimeFrame_;
		}
	}
	latency_ = minLatency;
	for (std::map<std::string, Vertex>::iterator it = opsDefs2_.begin(); it != opsDefs2_.end(); it++)
	{
		if ((it->second.ALAPtimeFrame_ > latency_) || (it->second.ASAPtimeFrame_ > latency_))
		{
			validLatency = false;
			it->second.ASAPtimeFrame_ -= 1;// so it can be used to index into arrays/vectors
			it->second.ALAPtimeFrame_ = it->second.ASAPtimeFrame_;
			it->second.timeFrame_[0] = it->second.ASAPtimeFrame_;
			it->second.timeFrame_[1] = it->second.ALAPtimeFrame_;
		}
		else
		{
			it->second.ASAPtimeFrame_ -= 1;// so it can be used to index into arrays/vectors
			it->second.ALAPtimeFrame_ = latency_ - it->second.ALAPtimeFrame_-1;
			it->second.timeFrame_[0] = it->second.ASAPtimeFrame_;
			it->second.timeFrame_[1] = it->second.ALAPtimeFrame_;
		}

	}
	return validLatency;
}
//auxiliary method to determine if the op is grouped into ALU/MULT/ or other

short HlsGen::isALU(Vertex::Operation op)
{
	short retVal = -1;
	switch (op)
	{
		case Vertex::Operation::ADD:
		case Vertex::Operation::SUB:
		case Vertex::Operation::COMP:
		case Vertex::Operation::COMP_EQ:
		case Vertex::Operation::COMP_LT:
		case Vertex::Operation::COMP_GT:
		case Vertex::Operation::MUX2x1:	    
		case Vertex::Operation::SHR:
		case Vertex::Operation::SHL:
		case Vertex::Operation::INC:
		case Vertex::Operation::DEC:
			retVal = 1;
			break;
		case Vertex::Operation::MULT:
		case Vertex::Operation::DIV:
		case Vertex::Operation::MOD:
			retVal = 0;
			break;
		case Vertex::Operation::ASSIGN:
		case Vertex::Operation::NOP:
		case Vertex::Operation::REG:
			retVal = 1;
			break;
	}
	return retVal;	
}

void HlsGen::setLatency(const unsigned int latency)
{
	latency_ = latency;
	for (int i = 0; i < latency; i++)
	{
		aluProbDistVec_.push_back(0.0);
		multProbDistVec_.push_back(0.0);
		otherProbDistVec_.push_back(0.0);
		schedule_[i] = {};
	}
}

bool HlsGen::performScheduling(void)
{
	calculateDistributions();
	for (std::map<std::string, Vertex>::iterator itr = opsDefs2_.begin(); itr != opsDefs2_.end(); itr++)
	{
		if (dataDefs_[itr->first].dataType_ != BaseType::DataType::INPUT)
		{
			calculateSelfForces();
 			scheduleNode(itr->first, itr->second);
		}
	}
	//for (int i = 0; i < latency_; i++)
	//{
	//	std::cout << "timeframe " << i << ": ";
	//	for (std::vector<std::string>::iterator itr = schedule_[i].begin(); itr != schedule_[i].end(); itr++)
	//	{
	//		std::cout << *itr << " ";
	//	}
	//	std::cout << std::endl;
	//}
	return true;
}

void HlsGen::scheduleNode(std::string vtxName, Vertex& vtx)
{
	std::vector<float> totalForce;
	for (int i = 0; i < latency_; i++)
	{
		//initialize total forces as the self force at each time frame
		totalForce.push_back(vtx.selfForceVector[i]);
	}

	for (int i = 0; i < latency_; i++)
	{
		//get precedessor forces at each time step using the inverted DAG 
		float predecessorForces = 0.0;
		for (std::vector<std::string>::iterator itr = invDag_[vtxName].begin(); itr != invDag_[vtxName].end(); itr++)
		{
			// if scheduling the current vertex at time frame i limits the predecessor's ability to schedule
			// at timeFrame i, add in the self force of it's predecessor at time frame i.
			if (opsDefs2_.count(*itr) != 0)
			{
				if (opsDefs2_[*itr].inRange(i))
				{
					predecessorForces += opsDefs2_[*itr].selfForceVector[i];
				}
			}
		}
		totalForce.at(i) += predecessorForces;

	    //get successor forces at each time step using the inverted DAG 
		float successorForces = 0.0;
		for (std::vector<std::string>::iterator itr = dag_[vtxName].begin(); itr != dag_[vtxName].end(); itr++)
		{
			// if scheduling the current vertex at time frame i limits the predecessor's ability to schedule
			// at timeFrame i, add in the self force of it's predecessor at time frame i.
			if (opsDefs2_.count(*itr) != 0)
			{
				if (opsDefs2_[*itr].inRange(i))
				{
					successorForces += opsDefs2_[*itr].selfForceVector[i];
				}
			}
		}
		totalForce.at(i) += successorForces;
	}
	int minIndex = opsDefs2_[vtxName].ASAPtimeFrame_;

	if (opsDefs2_[vtxName].ASAPtimeFrame_ == opsDefs2_[vtxName].ALAPtimeFrame_)
	{
		schedule_[opsDefs2_[vtxName].ASAPtimeFrame_].push_back(vtxName);
	}
	else
	{
		for (int i = opsDefs2_[vtxName].ALAPtimeFrame_; i >= opsDefs2_[vtxName].ASAPtimeFrame_ || i > 0 ; i--)
		{
			if (i >= 0)
			{
				if (totalForce[i] <= totalForce[minIndex])
				{
					minIndex = i;
				}
			}
			else
			{
				break;
			}
		}
		schedule_[minIndex].push_back(vtxName);
		vtx.ALAPtimeFrame_ = minIndex;
		vtx.ASAPtimeFrame_ = minIndex;
	}


	for (std::vector<std::string>::iterator itr = invDag_[vtxName].begin(); itr != invDag_[vtxName].end(); itr++)
	{
		// if scheduling the current vertex at time frame i limits the predecessor's ability to schedule
		// at timeFrame i, add in the self force of it's predecessor at time frame i.
		if (opsDefs2_.count(*itr) != 0)
		{
			if (opsDefs2_[*itr].inRange(minIndex))
			{
				opsDefs2_[*itr].ALAPtimeFrame_ = minIndex - 1; //its frame no longer includes minElementIndex + 1
			}
		}
	}

	for (std::vector<std::string>::iterator itr = dag_[vtxName].begin(); itr != dag_[vtxName].end(); itr++)
	{
		// if scheduling the current vertex at time frame i limits the predecessor's ability to schedule
		// at timeFrame i, add in the self force of it's predecessor at time frame i.
		if (opsDefs2_[*itr].inRange(minIndex))
		{
			opsDefs2_[*itr].ASAPtimeFrame_ = minIndex + 1;
		}
	}
}

void HlsGen::calculateSelfForces(void)
{
	for (std::map<std::string, Vertex>::iterator itr = opsDefs2_.begin(); itr != opsDefs2_.end(); itr++)
	{
		//initialize vectors
		if (isALU(itr->second.op_) == 1)
		{
			for (int i = 0; i < latency_; i++)
			{
				float selfForce = 0.0f;
				for (int j = 0; j < latency_; j++)
				{
					if (i == j)
					{
						selfForce += aluProbDistVec_[j] * (1 - itr->second.prob(j + 1));
					}
					else
					{
						selfForce += aluProbDistVec_[j] * (0 - itr->second.prob(j + 1));
					}
				}

			itr->second.setSelfForce(selfForce, i);
			}
		}
		else if (isALU(itr->second.op_) == 0)
		{
			for (int i = 0; i < latency_; i++)
			{
				float selfForce = 0.0f;
				for (int j = 0; j < latency_; j++)
				{
					if (i == j)
					{
						selfForce += multProbDistVec_[j] * (1 - itr->second.prob(j + 1));
					}
					else
					{
						selfForce += multProbDistVec_[j] * (0 - itr->second.prob(j + 1));
					}
				}
				itr->second.setSelfForce(selfForce, i);
			}
		}
		else if (isALU(itr->second.op_) == -1)
		{
			for (int i = 0; i < latency_; i++)
			{
				itr->second.setSelfForce(0.0, i);
			}
		}
	}
}
void HlsGen::calculateDistributions(void)
{
	setLatency(latency_);

	for (int i = 0; i < latency_; i++)
	{
		aluProbDistVec_[i] = 0.0f;
		multProbDistVec_[i] = 0.0f;
		otherProbDistVec_[i] = 0.0f;
	}
	for(std::map<std::string, Vertex>::iterator itr = opsDefs2_.begin(); itr != opsDefs2_.end(); itr++)
	{
        if(isALU(itr->second.op_) == 1)
		{
			for(int i = 0 ; i < latency_; i++)
			{
            	aluProbDistVec_[i] += itr->second.prob(i+1);
			}
		}
	    else if(isALU(itr->second.op_) == 0)
		{
			for(int i = 0 ; i < latency_; i++)
			{
            	multProbDistVec_[i] += itr->second.prob(i+1);
			}
		}
		else if(isALU(itr->second.op_) == -1)
		{
			for(int i = 0 ; i < latency_; i++)
			{
            	otherProbDistVec_[i] += itr->second.prob(i+1);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////