////////////////////////////////////////////////////////////////////////
// filename - BaseType.h
// author - Dillon Gregerson
// project - Ece574 Assignment 2
////////////////////////////////////////////////////////////////////////

#pragma once

#include <iostream>
#include <string>

class BaseType
{
public:
	enum class DataType
	{
		NONE,
		INPUT,
		OUTPUT,
		WIRE,
		REGISTER
	};

	BaseType(void) {}

	explicit BaseType(bool sign, unsigned int dataWidth, BaseType::DataType dType)
		: signed_(sign)
		, dataWidth_(dataWidth)
		, dataType_(dType)
	{}

	virtual ~BaseType(void) {}
	bool signed_;
	unsigned int dataWidth_;
	DataType dataType_;

	std::string printDataType(void);

	std::string printDataWidth(void);

	void print(void);
};
