# HlsGen
This toolset generates Verilog code from a netlist (performs High Level Synthesis)

1) name: Dillon Gregerson
   NETID: DillonJGregerson

2) course: ECE574a

3) The program contains 3 main objects

class BaseType: Contains data for each data type (input, output, wire, register, signed/unsigned, data width). 
		Throughout the program this class is often used as the key for a map. This class also contains
		printing functions that aid with the generation of Verilog.

class Vertex:   This class represents each operation to be performed. It contains enumerations for the type of operations
		to be performed, the output as well as the input(s) (These output / inputs are strings).

class HlsGen:   This is the primary class that does the all of the netlist parsing, Verilog generation, and critical path
                calculation. Each of these functionalities is described as follows
		1) Netlist parsing: the netlist is parsed by iterating over each line of the file twice.
                   The general pattern of the parsing was to use a state machine to detect syntax errors and appropriately
                   populate the necessary data structures.
                   The first pass through parses out the data fields and populates a map called dataDefs.
                   The second pass through parses out the operations to be performed on this data and populates
                   a map called opsDefs.
                   The end result of the parsing are these two data structures that will be used by the rest of the program.
                2) Verilog generation: The code first iterates over the map of dataDefs and writes the verilog code to a file.
                   It then iterates through all of the verticies in the opsDefs map and writes out each of the corresponding operations
                   as Verilog code. Note: this is greatly oversimplified, there are also stages of logic that check if additional
                   sections of Verilog code need to be written to do sign extension and other checks for syntax errors. 
                3) Critical path calculation. There were main challenges for this method. The first was the generation of all of the 
		   possible paths. To do this I first used the opsDefs map to create another map to represent the Directed Acyclic Graph.
                   The structure of this DAG (named 'dag_') was a map<string, vector of strings>.  To traverse the
                   dag I made a recursive function (getDependencies()), which calls itself until it lands on a designated state (ie output, register etc..).
		   At this terminating condition it pushes a stack of the nodes it visited upstream as an element of a vector, then visits
                   visits its adjacent nodes. The second challenge was determining where the start points and end points (ie input-register, 
		   register-register, and register-output. needed to be. To do this I added elements to an vector (ie all the 
		   data elements that were 'inputs'), then iterated through the vector using each element as a starting point, 
                   and added the desired endpoints (ie all outputs, registers etc) to an unordered set that could be checked in the recursive
                   function. After all the paths were generated I looped through the vector of stacks paths (named paths_), and used a
                   map to get the latency for the corresponding operation, summing all these up resulted in the estimated critical path for the circuit.

4) Distribution of work: My group partner dropped the course so this project was done by myself.