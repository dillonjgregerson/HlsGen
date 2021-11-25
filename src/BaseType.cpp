////////////////////////////////////////////////////////////////////////
// filename - BaseType.cpp
// author - Dillon Gregerson
// project - Ece574 Assignment 2
////////////////////////////////////////////////////////////////////////

#include "BaseType.h"

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
		retString = "output";
		break;
	case DataType::WIRE:
		retString = "wire";
		break;
	case DataType::REGISTER:
		retString = "wire";
		break;
	default:
		break;
	}
	return retString;
}

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