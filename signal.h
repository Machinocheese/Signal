#ifndef SIGNAL_H
#define SIGNAL_H
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
using namespace std;

int exp(const int& num){
	int base = 2;
	if(num == 0)
		return 1;
	for(int i = 1; i < num; i++){
		base *= 2;
	}
	return base;
}

vector<int> hexToBin(uint8_t num){
	vector<int> bin;
	while(num != 0){
		bin.push_back(num % 2);
		num /= 2;
	}
	reverse(bin.begin(), bin.end());
	return bin;
}

int binToHex(vector<int> hex){
	int sum = 0;
	reverse(hex.begin(), hex.end());
	for(int i = 0; i < hex.size(); i++){
		sum += hex[i] * exp(i);
	}
	return sum;
}

int tillNextZero(vector<uint8_t> hex, const int& position){
	int num = 0;
	while(hex[position + num] != 0x0){
		num++;
	}
	return num;
}

void print(vector<int> vec){
	for(int i = 0; i < vec.size(); i++){
		cout << vec[i];
	}
	cout << endl;
}

vector<int> getCodeStream(vector<uint8_t> input, int offset, int size){
	int codeSize = input[offset] + 1; //minimum size for gifs
	int clearCode, endCode, codeOffset = 0, counter, skippable, temp;
	stringstream ss;
	//cout << codeSize << endl;

	vector<string> codeTable;
	vector<int> codeStream;
	vector<int> intermed;
	vector<int> binaryInput;
	for(int i = 0; i < size; i++){
		ss << i;
		codeTable.push_back(ss.str());
		ss.str("");
	}
	clearCode = codeTable.size();
	endCode = codeTable.size() + 1;
	codeTable.push_back("4");
	codeTable.push_back("5");

	//this consumes a lot of memory, but makes the code easier to write/read
	offset += 1;
	skippable = input[offset] - 1;
	offset += 1;
	for(int i = offset; i < offset + skippable; i++){
		intermed = hexToBin(input[i]);
		for(int i = 0; i < 8; i++){
			if(intermed.empty()) binaryInput.push_back(0);
			else{
				binaryInput.push_back(intermed.back());
				intermed.pop_back();
			}
		}
	}

	intermed.clear();

	while(!binaryInput.empty()){
		counter = 0;
		intermed = vector<int>(binaryInput.begin(), binaryInput.begin() + codeSize);
		reverse(intermed.begin(), intermed.end());
		temp = binToHex(intermed);
		binaryInput.erase(binaryInput.begin(), binaryInput.begin() + codeSize);
		codeStream.push_back(temp);
		for(int i = 0; i < codeTable.size(); i++){
			
		}
		//don't know what code is, so just fill in with # and position
	}
	
////////////////////////////////////////////////loop based on hex number, calc all at once from one hex # in one loop

	codeTable[clearCode] = "clear code";
	codeTable[endCode] = "end of information code";
	cout << "\nCode Table: \n------------------------\n";
	for(int i = 0; i < codeTable.size(); i++){
		printf("Position: %2d | ", i);
		cout << codeTable[i] << endl;
	}
	print(codeStream);
	return codeStream;
}

vector<string> getColorTable(vector<uint8_t> input){
	vector<int> packed;
	vector<string> colorTable;
	stringstream ss;
	int test = 0, a, b, c, counter = 0;
	packed = hexToBin(input[10]);

	if(packed[0] == 0)
		return colorTable;

	test = binToHex(vector<int>(packed.end() - 3, packed.end())); //last 3 bits
	test = exp(test + 1); //there are 2^(N+1) colors
	test *= 3; //each color has 3 bytes

	for(int i = 13; i < 13 + test; i += 3){
		a = input[i];
		b = input[i+1];
		c = input[i+2];
		
		if(a < 0x10) ss << 0;
		ss << hex << a;
		if(b < 0x10) ss << 0;
		ss << hex << b;
		if(c < 0x10) ss << 0;
		ss << hex << c;
		//this is code in an attempt to clean the color table
		if(counter == 1 && ss.str() == "000000"){
			ss.str("");
			continue;
		} 
		colorTable.push_back(ss.str());
		if(ss.str() == "000000")
			counter = 1;
		ss.str("");
	}
/*	make debugging easier
	for(int i = 0; i < colorTable.size(); i++){
		cout << colorTable[i] << endl;
	}
	cout << "SIZE: " << colorTable.size() << endl;
*/
	return colorTable;
}

int calculateOffset(vector<uint8_t> input){

	vector<int> packed;
	int totalOffset = 13; //header + logical screen descriptor = 13 bytes
	int test = 0;

	//global color table offset
	packed = hexToBin(input[10]);

	if(packed[0] == 0)
		totalOffset += 0;
	else{
		test = binToHex(vector<int>(packed.end() - 3, packed.end())); //last 3 bits
		test = exp(test + 1); //there are 2^(N+1) colors
		test *= 3; //each color has 3 bytes
		totalOffset += test;
	}
	
	while(input[totalOffset] != 0x2c){
		if(input[totalOffset] == 0x21){
			//f9 = graphics control, fe = comments, 01 = plain text, ff = application
			if(input[totalOffset + 1] == 0xf9 || input[totalOffset + 1] == 0xfe)
				totalOffset += input[totalOffset + 2] + 4;
			if(input[totalOffset + 1] == 0x01 || input[totalOffset + 1] == 0xff){
				totalOffset += input[totalOffset + 2] + 2;
				totalOffset += tillNextZero(input, totalOffset) + 1;
			}
		} else if(input[totalOffset] == 0x0){
			totalOffset += 1;
		}
	}

	//skip over Image Descriptor
	totalOffset += 10;
	cout << totalOffset << endl;
	return totalOffset;
}
#endif