////////////////////////////////////////////////////////////////////////
// filename - Vertex.h
// author - Dillon Gregerson
// project - Ece574 Assignment 2
////////////////////////////////////////////////////////////////////////

//TODO class members should be protected

#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <stack>
#include "BaseType.h"

class Vertex
{
public:
	//enum of the potential operations this vertext can perform
	enum class Operation
	{
		NOP,
		REG,
		ADD,
		SUB,
		MULT,
		COMP,
		COMP_EQ,
		COMP_LT,
		COMP_GT,
		MUX2x1,
		SHR,
		SHL,
		DIV,
		MOD,
		INC,
		DEC,
		ASSIGN
	};
	Operation op_;

	//////////////////////////////////////////////////////////////////////////////
	//@brief default constructor
	//@param void
	//@return N/A
	//////////////////////////////////////////////////////////////////////////////
	Vertex(void);

	//////////////////////////////////////////////////////////////////////////////
	//@brief default destructor
	//@param void
	//@return N/A
	//////////////////////////////////////////////////////////////////////////////
	virtual ~Vertex(void);

	//////////////////////////////////////////////////////////////////////////////
	//@brief print details of the vertex
	//@param void
	//@return N/A
	//////////////////////////////////////////////////////////////////////////////
	void print();

	//////////////////////////////////////////////////////////////////////////////
	//@brief prints the mixed sign extension
	//@param std::map<std::string, BaseType>& dataDefs - map of data definitions
	//@return string - string of sign extension
	//////////////////////////////////////////////////////////////////////////////
	std::string printMixedExtension(std::map<std::string, BaseType>& dataDefs);

	//////////////////////////////////////////////////////////////////////////////
	//@brief print the operation the vertex performs
	//@param std::map<std::string, BaseType>& dataDefs - map of data definitions
	//@param unsigned int num
	//@return string - string of vertex's operation
	//////////////////////////////////////////////////////////////////////////////
	std::string printOp(std::map<std::string, BaseType>& dataDefs, unsigned int num);

	//////////////////////////////////////////////////////////////////////////////
	//@brief print the operation the vertex performs
	//@param std::string& str
	//@return bool - true input string is a number, false otherwise
	//////////////////////////////////////////////////////////////////////////////
	static bool isNumber(const std::string& str);

	double prob(unsigned int timeFrame);

	void initSelfForce(unsigned int numTimeFrames);

	void setSelfForce(float selfForce, unsigned int idx);
	bool inRange(unsigned int timeFrame) const;
    //class members
    std::string inputs_[8];
	std::string output_;
	std::string outputVar_;
	unsigned int timeFrame_[2];
	double currDelay_;
	unsigned int ALAPtimeFrame_;
	unsigned int ASAPtimeFrame_;

	float selfForceVector[100];
	std::string rawLine;
	std::stack<std::pair<std::string, std::string>>conditionals_;
	std::vector<std::pair<std::string, std::string>>conditionalVec_;
	std::string conditionalAr[8];
	unsigned int conditionalDepth_;
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////