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

int tillNextZero(const vector<uint8_t>& hex, const int& position){
	int num = 0;
	while(hex[position + num] != 0x0){
		num++;
	}
	return num;
}

void print(const vector<int>& vec){
	for(int i = 0; i < vec.size(); i++){
		cout << vec[i];
	}
	cout << endl;
}

vector<int> extract(vector<int>& input, const int& codeSize){
	vector<int> intermed;
	if(codeSize > input.size()){
		/*for(int i = 0; i < codeSize - input.size(); i++){
			intermed.push_back(0);
		}*/
		//print(input);
		intermed = input;
		reverse(intermed.begin(), intermed.end());
		input.clear();
	}
	else{
		intermed = vector<int>(input.begin(), input.begin() + codeSize);
		reverse(intermed.begin(), intermed.end());
		input.erase(input.begin(), input.begin() + codeSize);
	}
	return intermed;
}

vector<int> getCodeStream(const vector<uint8_t>& input, int offset, const int& size){
	int codeSize = input[offset] + 1; //minimum size for gifs
	int clearCode, endCode, codeOffset = 0, counter = 0, skippable, temp;
	stringstream ss;

	vector<string> codeTable;
	vector<int> codeStream;
	vector<int> indexStream;
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

	//sets up the data in binary form
	offset += 1;
	skippable = input[offset];
	offset += 1;
	for(int i = offset; i < offset + skippable; i++){
		intermed = hexToBin(input[i]);
		if(i == offset + skippable - 1){
			reverse(intermed.begin(), intermed.end());
			binaryInput.insert(binaryInput.end(), intermed.begin(), intermed.end());
			break;
		}
		for(int i = 0; i < 8; i++){
			if(intermed.empty()) binaryInput.push_back(0);
			else{
				binaryInput.push_back(intermed.back());
				intermed.pop_back();
			}
		}
	}

	int pW, cW;
	string p = "", c = "", result;
	cW = binToHex(extract(binaryInput, codeSize));
	codeStream.push_back(cW);
	//if(cW != 4) inputStream.push_back(codeTable[cW]); //this line shouldn't run
	cW = binToHex(extract(binaryInput, codeSize)); //this should be the first legit output
	codeStream.push_back(cW);
	//indexStream.push_back(codeTable[cW]);
	
	while(!binaryInput.empty()){
		pW = cW;
		cW = binToHex(extract(binaryInput, codeSize));
		if(cW == 0x5){
			codeStream.push_back(cW);
			break;
		}
		if(codeTable.size() == (exp(codeSize) - 1)) codeSize++;
		if(cW > codeTable.size() - 1){ //aka cW isn't in the dictionary
			ss << codeTable[pW];
			p = ss.str();
			c = ss.str()[0];
			codeStream.push_back(codeTable.size());
			codeTable.push_back(p+c);
			ss.str("");
			//inputStream.push_back(atoi(p+c));
		} else {
			codeStream.push_back(cW);
			//indexStream.push_back(codeTable[cW]);
			ss << codeTable[pW];
			p = ss.str();
			ss.str("");
			ss << codeTable[cW];
			c = ss.str()[0];
			ss.str("");
			codeTable.push_back(p + c);
		}
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

vector<string> getColorTable(const vector<uint8_t>& input){
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

int calculateOffset(const vector<uint8_t>& input){

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