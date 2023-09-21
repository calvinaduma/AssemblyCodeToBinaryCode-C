/*
 * Calvin Aduma
 * October 27
 * CPSC3300
 * Project2
*/

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <math.h>

using namespace std;
/*
 * GLOBAL VARIABLES
 * 	- for easier binary conversion
 * 	assumptions: words are spelled correctly
*/
string outputFile = "";
unordered_map<string, int> instructionNums = { {"lw",6},{"sw",7},{"beq",8},{"j",9} };
// {instruction, Function#}
unordered_map<string,int> rType = { {"add",32},{"sub",34},{"and",36},{"or",37},{"slt",42} };
// extra r-type: {"addu","100001"},{"subu","100011"},{"xor","100110"},{"nor","100111"},{"sltu","101011"},{"sll","000000"},{"srl","000010"},{"sllv","000100"},{"sra","000011"},{"srav","000111"}

// {instruction, Opcode#}
unordered_map<string,int> iType = { {"beq",4},{"lw",35},{"j",2},{"sw",43} };
// extra i-type: {"addi","001000"},{"addiu","001001"},{"andi","001100"},{"bne","000101"},{"lb","100000"},{"lbu","100100"},{"lh","100001"},{"lhu","100101"},{"lui","001111"},{"lwcl","110001"},{"ori","001101"},{"jal","000011"}

// Function declaration
vector<vector<string> > readFile(string &filename);
void writeFile(string &filename, int num);
vector<vector<int> > assembly2binary(vector<vector<string> > &v);
string int2binary(int num);
void fivebitPadder(string &s);
void sixteenbitPadder(string &s, int num);
void twentysixbitPadder(string &s);
void print2DVector(vector<vector<int> > &v);

/* input: [string] filename.
 * output: 2D vector of input from a file.
 * Gets input from file and stores each word separated by
 * a space into its own element in vector.
*/
vector<vector<string> > readFile(string &filename)
{
	ifstream file(filename);
	vector<vector<string> > input;
	string data;

	// reads each line
	while(getline(file, data))
	{
		string word;
		istringstream ss(data);
		vector<string> line;

		// reads each word and places in vector
		while (getline(ss, word, ' '))
			line.push_back(word);

		// adds to input vector
		input.push_back(line);
	}
	file.close();
	return input;
}

/* input: [string] filename, [vector<string>>] v.
 * output: NONE.
 * Writes components of v into filename.
*/
void writeFile(string &filename, int num)
{
	ofstream file(filename,ios::binary | ios::out);
/*
	for (int i=0; i<v.size(); i++)
	{
		for (int j=0; j<v[i].size(); j++){
			file.write((char*)&v[i][j], sizeof(int)); // Binary version
			//outputFile << v[i] << endl;    //ASCII Version to test output of correct binary values
		}
	}
	*/
	file.write(reinterpret_cast<const char *>(&num), sizeof(num));
	file.close();
	return;
}

/* input: [vector<vector<string>>] v.
 * output: vector of strings.
 * Converts assembly language as string, into binary language as string.
*/
vector<vector<int> > assembly2binary(vector<vector<string> > &v)
{
	vector<vector<int> > answer;
	vector<int> line;
	for (size_t i=0; i<v.size(); i++)
	{
		string key = v[i][0]; // operation performed e.g. add, sub, beq
		int type = 2;
		if (rType.find(key) != rType.end()) type = 1;
		else if (iType.find(key) != iType.end()) type = 2;
		else
		{
			cout << "Error on line " << i << " unsupported instuction" << endl;
			exit(1);
		}
		switch (type)
		{
			case 1: // rType
			{
				line.push_back(0);
				writeFile(outputFile,0);
				int rtInt, rdInt, rsInt;
				rtInt=0;
				rdInt=0;
				rsInt = 0;
				string rd = v[i][1];
				string rs = v[i][2];
				string rt = v[i][3];

				// convert string->int
				rdInt = stoi(rd.substr(1,rd.size()-1));
				rsInt = stoi(rs.substr(1,rs.size()-1));
				rtInt = stoi(rt.substr(1,rt.size()));

				//checks valid register number
				if (rdInt<0 || rdInt>31){
					cout << "Error on line " << i << " regsiter number is out of range" << endl;
					exit(1);
				} else if (rsInt<0 || rsInt>31){
					cout << "Error on line " << i << " regsiter number is out of range" << endl;
					exit(1);
				} else if (rtInt<0 || rtInt>31){
					cout << "Error on line " << i << " regsiter number is out of range" << endl;
					exit(1);
				}
				line.push_back(rsInt);
				line.push_back(rtInt);
				line.push_back(rdInt);
				line.push_back(0);
				line.push_back(rType[key]);

				writeFile(outputFile,rsInt);
				writeFile(outputFile,rtInt);
				writeFile(outputFile,rdInt);
				writeFile(outputFile,0);
				writeFile(outputFile,rType[key]);

/*
				// convert to binary
				rt = int2binary(rtInt);
				rd = int2binary(rdInt);
				rs = int2binary(rsInt);

				//pads 0s
				if (rt.size() < 5) fivebitPadder(rt);
				if (rd.size() < 5) fivebitPadder(rd);
				if (rs.size() < 5) fivebitPadder(rs);

				// rs + rt + rd + shiftAmount + Function
				binarySoln = binarySoln + rs + rt + rd + "00000" + rType[key];*/
				break;
			}
			case 2: // iType
			{
				line.push_back(iType[key]); // Opcode
				writeFile(outputFile,iType[key]);
				int val = instructionNums[key];
				switch (val)
				{
					case 8: // beq
					{
						int rtInt, rsInt, labelInt;
						rtInt=0;
						rsInt=0;
						labelInt = 0;
						string rt = v[i][2];
						string rs = v[i][1];
						string label = v[i][3];

						// convert string->int
						rtInt = stoi(rt.substr(1, rt.size()-1));
						rsInt = stoi(rs.substr(1, rs.size()-1));
						labelInt = stoi(label);

						// checks register number
						if (rsInt<0 || rsInt>31){
							cout << "Error on line " << i << " regsiter number is out of range" << endl;
							exit(1);
						} else if (rtInt<0 || rtInt>31){
							cout << "Error on line " << i << " regsiter number is out of range" << endl;
							exit(1);
						}
						line.push_back(rsInt);
						line.push_back(rtInt);
						line.push_back(labelInt);

						writeFile(outputFile,rsInt);
						writeFile(outputFile,rtInt);
						writeFile(outputFile,labelInt);

/*
						// convert to binary
						rt = int2binary(rtInt);
						label = int2binary(labelInt);
						rs = int2binary(rsInt);

						// pad with 5 or 16 bits
						if (rt.size() < 5) fivebitPadder(rt);
						if (rs.size() < 5) fivebitPadder(rs);
						if (label.size() < 16) sixteenbitPadder(label,labelInt);

						// rs + rt + address
						binarySoln = binarySoln + rs + rt + label;*/
						break;
					}
					case 6: // lw
					{
						int rtInt, rsInt, immInt;
						rtInt=0;
						rsInt=0;
						immInt = 0;
						string rt = v[i][1];
						size_t pos = 0;
						string imm, buffer;
						imm = buffer = "";
						buffer = v[i][2];
						string delimeter = "(";
						while ((pos=buffer.find(delimeter)) != string::npos){
							imm += buffer.substr(0,pos);
							buffer.erase(0,pos + delimeter.size());
							break;
						}
						string rs = buffer.substr(0,buffer.size()-1);

						// convert string->int
						rtInt = stoi(rt.substr(1,rt.size()-1));
						rsInt = stoi(rs.substr(1, rs.size()-1));
						immInt = stoi(imm);

						line.push_back(rsInt);
						line.push_back(rtInt);
						line.push_back(immInt);

						writeFile(outputFile,rsInt);
						writeFile(outputFile,rtInt);
						writeFile(outputFile,immInt);
/*
						// convert to binary
						rt = int2binary(rtInt);
						rs = int2binary(rsInt);
						imm = int2binary(immInt);

						// checks register number
						if (rsInt<0 || rsInt>31){
							cout << "Error on line " << i << " regsiter number is out of range" << endl;
							exit(1);
						} else if (rtInt<0 || rtInt>31){
							cout << "Error on line " << i << " regsiter number is out of range" << endl;
							exit(1);
						}

						// pad with 5 or 16 bits
						if (rt.size() < 5) fivebitPadder(rt);
						if (rs.size() < 5) fivebitPadder(rs);
						if (imm.size() < 16) sixteenbitPadder(imm, immInt);

						// rs + rt + address
						binarySoln = binarySoln + rs + rt + imm;*/
						break;
					}
					case 7: //sw
					{
						int rtInt, rsInt, immInt;
						rtInt=0;
					        rsInt=0;
						immInt = 0;
						string rt = v[i][1];
						size_t pos = 0;
						string imm, buffer;
						imm = buffer = "";
						buffer = v[i][2];
						string delimeter = "(";
						while ((pos=buffer.find(delimeter)) != string::npos){
							imm += buffer.substr(0,pos);
							buffer.erase(0,pos + delimeter.size());
							break;
						}
						string rs = buffer.substr(0,buffer.size()-1);

						// convert string->int
						rtInt = stoi(rt.substr(1,rt.size()-1));
						rsInt = stoi(rs.substr(1, rs.size()-1));
						immInt = stoi(imm);

						line.push_back(rsInt);
						line.push_back(rtInt);
						line.push_back(immInt);

						writeFile(outputFile,rsInt);
						writeFile(outputFile,rtInt);
						writeFile(outputFile,immInt);

/*						
						// convert to binary
						rt = int2binary(rtInt);
						rs = int2binary(rsInt);
						imm = int2binary(immInt);

						// checks register number
						if (rsInt<0 || rsInt>31){
							cout << "Error on line " << i << " regsiter number is out of range" << endl;
							exit(1);
						} else if (rtInt<0 || rtInt>31){
							cout << "Error on line " << i << " regsiter number is out of range" << endl;
							exit(1);
						}

						// pad with 5 or 16 bits
						if (rt.size() < 5) fivebitPadder(rt);
						if (rs.size() < 5) fivebitPadder(rs);
						if (imm.size() < 16) sixteenbitPadder(imm,immInt);

						// rs + rt + address
						binarySoln = binarySoln + rs + rt + imm;*/
						break;
					}
					case 9: // j 2 opcode 26 bit address
					{
						string address;
						int addressInt;
						address = v[i][1];
						addressInt = stoi(address);
						line.push_back(addressInt);
						writeFile(outputFile,addressInt);

						//address = int2binary(addressInt);
						//if (address.size() < 26) twentysixbitPadder(address);
						//binarySoln = binarySoln + address;
						break;
					}
				}
				break;
			}
		}
		//finalSoln.push_back(binarySoln);
		writeFile(outputFile,0x0D0A);
		answer.push_back(line);
	}
	return answer;
}

/* input: [int] num
 * outpout: string
 * converts the integer to its binary number as a string
*/
string int2binary(int num)
{
	string binaryString = "";
	if (num==0)
	{
		binaryString = "1";
	}
	while (num!=0)
	{
		binaryString = (num%2==0 ? "0" : "1") + binaryString;
		floor(num/=2);
	}
	return binaryString;
}

/* input: [string] s
 * output: NONE
 * pads the binary string with as many 0s are necessary
*/
void fivebitPadder(string &s)
{
	int offset = s.size();
	if (offset != 5)
	{
		string pad5 = "00000";
		int missingZs = 5-offset;
		string pad = pad5.substr(0,missingZs);
		pad += s;
		s = pad;
	}
}

/* input: [string] s
 * output: NONE
 * pads the binary string with as many 0s are necessary
*/
void sixteenbitPadder(string &s, int num)
{
	int offset = s.size();
	if (offset != 16)
	{
		string pad16 = "0000000000000000";
		int missingZs = 16-offset;
		string pad = pad16.substr(0,missingZs);
		pad += s;
		s = pad;
	}
	// 2's compliment for negative numbers
	if (num < 0)
	{
		for (size_t i=0; i<s.size(); i++)
		{
			if (s[i] == '0') s[i] = '1';
			else s[i] = '0';
		}

		// add 1 to least sig bit
		if (s[s.size()-1] == '1')
		{
			bool carry = true;
			int i = s.size()-1;
			while (i>=0)
			{
				if (s[i] == '1' && carry)
				{
					carry = true;
					s[i] = '0';
				}
				else
				{
					carry = false;
					s[i] = '1';
				}
				i--;
			}
		}
		else
		{
			s[s.size()-1] = '1';
		}
	}
}

/* input: [string] s
 * output: NONE
 * pads the binary string with as many 0s are necessary
*/
void twentysixbitPadder(string &s)
{
	int offset = s.size();
	if (offset != 26)
	{
		string pad16 = "00000000000000000000000000";
		int missingZs = 26-offset;
		string pad = pad16.substr(0,missingZs);
		pad += s;
		s = pad;
	}
}

/* input: [vector<vector<string>>] v.
 * output: NONE.
 * Prints contents of vector v.
*/
void print2DVector(vector<vector<int> > &v)
{
	for (size_t i=0; i<v.size(); i++)
	{
		for (size_t j=0; j<v[i].size(); j++)
		{
			cout << v[i][j] << " ";
		}
		cout << endl;
	}
}

int main(int argc, char *argv[])
{
// 1st step: fetch instruction mem
// 2nd step: read registers
// 3rd step: use ALU for address calculation
// 4th step: use ALU for operation execution
// 5th step: use ALU for comparisons

	string filename = argv[1]; // input file
	vector<vector<string> > dataVector;
	dataVector = readFile(filename);
	//print2DVector(dataVector);

	outputFile = argv[2]; // output file
	vector<vector<int> > solnVector = assembly2binary(dataVector);
	print2DVector(solnVector);
	return 0;

}
