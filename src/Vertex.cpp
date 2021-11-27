////////////////////////////////////////////////////////////////////////
// filename - Vertex.cpp
// author - Dillon Gregerson
// project - Ece574 Assignment 2
////////////////////////////////////////////////////////////////////////

#include "Vertex.h"

//////////////////////////////////////////////////////////////////////////////
//@brief default constructor
//@param void
//@return N/A
//////////////////////////////////////////////////////////////////////////////
Vertex::Vertex(void) :op_(Operation::NOP), currDelay_(0.0), ALAPtimeFrame_(0u), ASAPtimeFrame_(0u)
{
}

//////////////////////////////////////////////////////////////////////////////
//@brief default destructor
//@param void
//@return N/A
//////////////////////////////////////////////////////////////////////////////
Vertex::~Vertex(void) 
{
}

//////////////////////////////////////////////////////////////////////////////
//@brief print details of the vertex
//@param void
//@return N/A
//////////////////////////////////////////////////////////////////////////////
void Vertex::print()
{
	std::cout << output_ << " = " << inputs_[0] << " ";

	switch (op_)
	{
		case Operation::NOP: std::cout << "NOP "; break;
		case Operation::REG: std::cout << "REG "; break;
		case Operation::ADD: std::cout << "ADD "; break;
		case Operation::SUB: std::cout << "SUB "; break;
		case Operation::MULT: std::cout << "MULT "; break;
		case Operation::COMP_EQ: std::cout << "COMP_EQ "; break;
		case Operation::COMP_LT: std::cout << "COMP_LT "; break;
		case Operation::COMP_GT: std::cout << "COMP_GT "; break;
		case Operation::MUX2x1: std::cout << "MUX2x1 "; break;
		case Operation::SHR: std::cout << "SHR "; break;
		case Operation::SHL: std::cout << "SHL "; break;
		case Operation::DIV: std::cout << "DIV "; break;
		case Operation::MOD: std::cout << "MOD "; break;
		case Operation::INC: std::cout << "INC "; break;
		case Operation::DEC: std::cout << "DEC "; break;
		case Operation::ASSIGN: std::cout << "(REG) "; break;
	}
	if (op_ != Operation::ASSIGN)
	{
		std::cout << inputs_[1];
		if (op_ == Operation::MUX2x1)
		{
			std::cout << " : " << inputs_[2];
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
//@brief prints the mixed sign extension
//@param std::map<std::string, BaseType>& dataDefs - map of data definitions
//@return string - string of sign extension
//////////////////////////////////////////////////////////////////////////////
std::string Vertex::printMixedExtension(std::map<std::string, BaseType>& dataDefs)
{
	std::string retString("");
	unsigned int width1 = dataDefs[inputs_[0]].dataWidth_;

	//note you may want to go back through all this and make sure you are updating the maps corrrectly
	//to utilize the correct extended input
	if (dataDefs[inputs_[0]].signed_ && (dataDefs[inputs_[0]].dataWidth_ < dataDefs[inputs_[1]].dataWidth_))
	{
		retString += "wire [" + std::to_string(dataDefs[inputs_[1]].dataWidth_ - 1) + ":0] extended_" + inputs_[0] + ";\n"
			+ "assign " + "extended_" + inputs_[0] + " = { {" + std::to_string(dataDefs[inputs_[1]].dataWidth_ - dataDefs[inputs_[0]].dataWidth_) + "{" + inputs_[0] + "[" + std::to_string(dataDefs[inputs_[0]].dataWidth_ - 1) + "]}}, " + inputs_[0] + "};\n";
		inputs_[0] = "extended_" + inputs_[0];
		dataDefs[inputs_[0]].dataWidth_ = dataDefs[inputs_[1]].dataWidth_;
	}

	if (dataDefs[inputs_[1]].signed_ && (dataDefs[inputs_[0]].dataWidth_ > dataDefs[inputs_[1]].dataWidth_))
	{
		//sign extend the first input
		retString += "wire [" + std::to_string(dataDefs[inputs_[0]].dataWidth_ - 1) + ":0] extended_" + inputs_[1] + ";\n" +
			"assign " + "extended_" + inputs_[1] + " = { {" + std::to_string(dataDefs[inputs_[0]].dataWidth_ - dataDefs[inputs_[1]].dataWidth_) + "{" + inputs_[1] + "[" + std::to_string(dataDefs[inputs_[1]].dataWidth_ - 1) + "]}}, " + inputs_[1] + "};\n";
		inputs_[1] = "extended_" + inputs_[1];
		dataDefs[inputs_[1]].dataWidth_ = dataDefs[inputs_[0]].dataWidth_;
	}

	if (!dataDefs[inputs_[0]].signed_ && (dataDefs[inputs_[0]].dataWidth_ < dataDefs[inputs_[1]].dataWidth_))
	{
		retString += "wire [" + std::to_string(dataDefs[inputs_[1]].dataWidth_ - 1) + ":0] extended_" + inputs_[0] + ";\n" +
			"assign " + "extended_" + inputs_[0] + " = { {" + std::to_string(dataDefs[inputs_[1]].dataWidth_ - dataDefs[inputs_[0]].dataWidth_) + "{" + "1'b0" + "}}, " + inputs_[0] + "};\n";
		inputs_[0] = "extended_" + inputs_[0];
		dataDefs[inputs_[0]].dataWidth_ = dataDefs[inputs_[1]].dataWidth_;
	}

	if (!dataDefs[inputs_[1]].signed_ && (dataDefs[inputs_[0]].dataWidth_ > dataDefs[inputs_[1]].dataWidth_))
	{
		retString += "wire [" + std::to_string(dataDefs[inputs_[0]].dataWidth_ - 1) + ":0] extended_" + inputs_[1] + ";\n" +
			"assign " + "extended_" + inputs_[1] + " = { {" + std::to_string(dataDefs[inputs_[0]].dataWidth_ - dataDefs[inputs_[1]].dataWidth_) + "{" + "1'b0" + "}}, " + inputs_[1] + "};\n";
		inputs_[1] = "extended_" + inputs_[1];
		dataDefs[inputs_[1]].dataWidth_ = dataDefs[inputs_[0]].dataWidth_;
	}
	return retString;
}

//////////////////////////////////////////////////////////////////////////////
//@brief prints the mixed sign extension
//@param std::map<std::string, BaseType>& dataDefs - map of data definitions
//@return string - string of sign extension
//////////////////////////////////////////////////////////////////////////////
bool Vertex::isNumber(const std::string& str)
{
	for (std::string::size_type i = 0; i < str.size(); i++)
	{
		if (std::isdigit(i) == 0) return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//@brief print the operation the vertex performs
//@param std::map<std::string, BaseType>& dataDefs - map of data definitions
//@param unsigned int num
//@return string - string of vertex's operation
//////////////////////////////////////////////////////////////////////////////
std::string Vertex::printOp(std::map<std::string, BaseType>& dataDefs, unsigned int num)
{

	//The width of a datapath component (except comparators) should be determined by the size of the output,
	// register, or wire to which the output of the component connects. (DONE)
	// Signed integer inputs with fewer bits
	//	should be sign extended, and unsigned integer inputs should be padded with 0's. Inputs with greater bits
	//	should connect the least significant bits to the input. 
	// The size of comparators should be determined by the size of the largest input. (DONE)

	std::string retString("");
	unsigned int dataWidth(0);
	if (dataDefs[output_].signed_ || dataDefs[inputs_[0]].signed_ || dataDefs[inputs_[1]].signed_)
	{
		retString = "S";
	}

	switch (op_)
	{
	case Operation::NOP:break;
	case Operation::REG:
		retString += "REG #(.DATAWIDTH(" + std::to_string(dataDefs[output_].dataWidth_) + ")) reg" + std::to_string(num) + "(.Clk(Clk), .Rst(Rst), .d(" + inputs_[0] + "), .q(" + output_ + "));";
		break;
	case Operation::ADD:
		retString += "ADD #(.DATAWIDTH(" + std::to_string(dataDefs[output_].dataWidth_) + ")) add" + std::to_string(num) + "(.a(" + inputs_[0] + "), .b(" + inputs_[1] + "), .sum(" + output_ + "));";
		break;
	case Operation::SUB:
		retString += "SUB #(.DATAWIDTH(" + std::to_string(dataDefs[output_].dataWidth_) + ")) sub" + std::to_string(num) + "(.a(" + inputs_[0] + "), .b(" + inputs_[1] + "), .diff(" + output_ + "));";
		break;
	case Operation::MULT:
		retString += "MUL #(.DATAWIDTH(" + std::to_string(dataDefs[output_].dataWidth_) + ")) mul" + std::to_string(num) + "(.a(" + inputs_[0] + "), .b(" + inputs_[1] + "), .prod(" + output_ + "));";
		break;
	case Operation::COMP_EQ:
		if (!isNumber(inputs_[0]) && !isNumber(inputs_[1]))
		{
		    dataWidth = std::max(dataDefs[inputs_[0]].dataWidth_, dataDefs[inputs_[1]].dataWidth_);
		}
		else if (isNumber(inputs_[0]) && !isNumber(inputs_[1]))
		{
			dataWidth = dataDefs[inputs_[1]].dataWidth_;
		}
		else if (!isNumber(inputs_[0]) && isNumber(inputs_[1]))
		{
			dataWidth = dataDefs[inputs_[1]].dataWidth_;
		}
		else
		{
			dataDefs[output_].dataWidth_;
		}

		retString += "COMP #(.DATAWIDTH(" + std::to_string(dataWidth) + ")) comp" + std::to_string(num) + "(.a(" + inputs_[0] + "), .b(" + inputs_[1] + "), .eq(" + output_ + "));";
		break;
	case Operation::COMP_LT:
		if (!isNumber(inputs_[0]) && !isNumber(inputs_[1]))
		{
			dataWidth = std::max(dataDefs[inputs_[0]].dataWidth_, dataDefs[inputs_[1]].dataWidth_);
		}
		else if (isNumber(inputs_[0]) && !isNumber(inputs_[1]))
		{
			dataWidth = dataDefs[inputs_[1]].dataWidth_;
		}
		else if (!isNumber(inputs_[0]) && isNumber(inputs_[1]))
		{
			dataWidth = dataDefs[inputs_[1]].dataWidth_;
		}
		else
		{
			dataDefs[output_].dataWidth_;
		}
		dataWidth = std::max(dataDefs[inputs_[0]].dataWidth_, dataDefs[inputs_[1]].dataWidth_);
		retString += "COMP #(.DATAWIDTH(" + std::to_string(dataWidth) + ")) comp" + std::to_string(num) + "(.a(" + inputs_[0] + "), .b(" + inputs_[1] + "), .lt(" + output_ + "));";
		break;
	case Operation::COMP_GT:
		if (!isNumber(inputs_[0]) && !isNumber(inputs_[1]))
		{
			dataWidth = std::max(dataDefs[inputs_[0]].dataWidth_, dataDefs[inputs_[1]].dataWidth_);
		}
		else if (isNumber(inputs_[0]) && !isNumber(inputs_[1]))
		{
			dataWidth = dataDefs[inputs_[1]].dataWidth_;
		}
		else if (!isNumber(inputs_[0]) && isNumber(inputs_[1]))
		{
			dataWidth = dataDefs[inputs_[1]].dataWidth_;
		}
		else
		{
			dataDefs[output_].dataWidth_;
		}
		dataWidth = std::max(dataDefs[inputs_[0]].dataWidth_, dataDefs[inputs_[1]].dataWidth_);
		retString += "COMP #(.DATAWIDTH(" + std::to_string(dataWidth) + ")) comp" + std::to_string(num) + "(.a(" + inputs_[0] + "), .b(" + inputs_[1] + "), .gt(" + output_ + "));";
		break;
	case Operation::MUX2x1:
		if (!isNumber(inputs_[0]) && !isNumber(inputs_[1]))
		{
			dataWidth = std::max(dataDefs[inputs_[0]].dataWidth_, dataDefs[inputs_[1]].dataWidth_);
		}
		else if (isNumber(inputs_[0]) && !isNumber(inputs_[1]))
		{
			dataWidth = dataDefs[inputs_[1]].dataWidth_;
		}
		else if (!isNumber(inputs_[0]) && isNumber(inputs_[1]))
		{
			dataWidth = dataDefs[inputs_[1]].dataWidth_;
		}
		else
		{
			dataDefs[output_].dataWidth_;
		}
		retString += "MUX2x1 #(.DATAWIDTH(" + std::to_string(dataWidth) + ")) mux2x1" + std::to_string(num) + "(.a(" + inputs_[0] + "), .b(" + inputs_[1] + "), .sel(" + inputs_[2] + "), .d(" + output_ + "));";
		break;
	case Operation::SHR:
		retString += "SHR #(.DATAWIDTH(" + std::to_string(dataDefs[output_].dataWidth_) + ")) shr" + std::to_string(num) + "(.a(" + inputs_[0] + "), .sh_amt(" + inputs_[1] + "), .d(" + output_ + "));";
		break;
	case Operation::SHL:
		retString += "SHL #(.DATAWIDTH(" + std::to_string(dataDefs[output_].dataWidth_) + ")) shl" + std::to_string(num) + "(.a(" + inputs_[0] + "), .sh_amt(" + inputs_[1] + "), .d(" + output_ + "));";
		break;
	case Operation::DIV:
		retString += "DIV #(.DATAWIDTH(" + std::to_string(dataDefs[output_].dataWidth_) + ")) div" + std::to_string(num) + "(.a(" + inputs_[0] + "), .b(" + inputs_[1] + "), .quot(" + output_ + "));";
		break;
	case Operation::MOD:
		retString += "MOD #(.DATAWIDTH(" + std::to_string(dataDefs[output_].dataWidth_) + ")) mod" + std::to_string(num) + "(.a(" + inputs_[0] + "), .b(" + inputs_[1] + "), .rem(" + output_ + "));";
		break;
	case Operation::INC:
		retString += "INC #(.DATAWIDTH(" + std::to_string(dataDefs[output_].dataWidth_) + ")) inc" + std::to_string(num) + "(.a(" + inputs_[0] + "), .d(" + output_ + "));";
		break;
	case Operation::DEC:
		retString += "DEC #(.DATAWIDTH(" + std::to_string(dataDefs[output_].dataWidth_) + ")) dec" + std::to_string(num) + "(.a(" + inputs_[0] + "), .d(" + output_ + "));";
		break;
	default:
		std::cout << "\nERROR!\n";
	}
	return retString;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////