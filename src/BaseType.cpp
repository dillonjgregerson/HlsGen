////////////////////////////////////////////////////////////////////////
// filename - BaseType.cpp
// author - Dillon Gregerson
// project - Ece574 Assignment 2
////////////////////////////////////////////////////////////////////////

#include "BaseType.h"

//////////////////////////////////////////////////////////////////////////////
//@brief default constructor
//@param void
//@return N/A
//////////////////////////////////////////////////////////////////////////////
BaseType::BaseType(void) 
{

}

//////////////////////////////////////////////////////////////////////////////
//@brief constructor
//@param bool sign - sign of the data type (unsigned or signed)
//@param unsigned int dataWidth - width in bits of data type
//@param BaseType::DataType dtype - type of data (enum)
//@return N/A
//////////////////////////////////////////////////////////////////////////////
BaseType::BaseType(bool sign, unsigned int dataWidth, BaseType::DataType dType)
	: signed_(sign)
	, dataWidth_(dataWidth)
	, dataType_(dType)
	, original(false)
{

}

//////////////////////////////////////////////////////////////////////////////
//@brief default destructor
//@param void
//@return N/A
//////////////////////////////////////////////////////////////////////////////
/*virtual*/ BaseType::~BaseType(void) 
{

}

//////////////////////////////////////////////////////////////////////////////
//@brief return string type of data
//@param void
//@return std::string the type of data
//////////////////////////////////////////////////////////////////////////////
std::string BaseType::printDataType()
{
	std::string retString;
	switch (dataType_)
	{
	case DataType::NONE:
		retString = "NONE";
		break;
	case DataType::INPUT:
		retString = "input";
		break;
	case DataType::OUTPUT:
		retString = "output reg";
		break;
	case DataType::WIRE:
		retString = "reg";
		break;
	case DataType::REGISTER:
		retString = "reg";
		break;
	case DataType::VARIABLE:
		retString = "reg";
		break;
	default:
		break;
	}
	return retString;
}

//////////////////////////////////////////////////////////////////////////////
//@brief return string width of data
//@param void
//@return std::string the width of data
//////////////////////////////////////////////////////////////////////////////
std::string BaseType::printDataWidth()
{
	std::string retString("");
	if (dataWidth_ > 1)
	{
		retString = "[" + std::to_string(dataWidth_ - 1) + ":0]";
	}
	else
	{
		retString = " ";
	}
	return retString;
}

//////////////////////////////////////////////////////////////////////////////
//@brief print details about the data type
//@param void
//@return void
//////////////////////////////////////////////////////////////////////////////
void BaseType::print()
{
	switch (dataType_)
	{
	case DataType::NONE:
		std::cout << "NONE ";
		break;
	case DataType::INPUT:
		std::cout << "INPUT ";
		break;
	case DataType::OUTPUT:
		std::cout << "OUTPUT ";
		break;
	case DataType::WIRE:
		std::cout << "WIRE ";
		break;
	case DataType::REGISTER:
		std::cout << "REGISTER ";
		break;
	default:
		break;
	}

	switch (signed_)
	{
	case 1: std::cout << "SIGNED ";
		break;
	case 0: std::cout << "UNSIGNED ";
		break;
	}
	std::cout << dataWidth_ << " bits";
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////