#include <iostream>
#include <fstream>
#include <vector>
#include "signal.h"
using namespace std;

int main(int argc, char* argv[]){
	//std::ifstream in (argv[1],std::ios::binary);
	std::ifstream in ("seismogram.gif",std::ios::binary);
	uint8_t a;
	vector<uint8_t> input;
	std::noskipws(in);
	if(!in){
		cout << "Could not open file for reading.";
		return 0;
	}
	in.seekg(0, in.end);
	int sizeInBytes = in.tellg();
	in.seekg(0, in.beg);

	for(int i = 0; i < sizeInBytes; i++){
		in >> a;
		input.push_back(a);
		//printf("%8.2x \n", a);
	}

	if(input[0] != 0x47 || input[1] != 0x49 || input[2] != 0x46){
		cout << "Please give a .GIF file.";
  		return 0;
  	}
  	cout << "Successfully read in a .GIF.\n";
  	if(input[3] != 0x38 || input[4] != 0x39 || input[5] != 0x61){
  		cout << "This version of .GIF file is not supported.";
  		return 0;
  	}

  	calculateOffset(input);
	return 0;
}