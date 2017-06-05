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
		cout << vec[i] << ' ';
	}
	cout << endl;
}

vector<int> extract(vector<int>& input, const int& codeSize){
	vector<int> intermed;
	if(codeSize > input.size()){
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

vector<int> initializeBinary(const vector<uint8_t>& input, int offset){
	vector<int> intermed;
	vector<int> binaryInput;
	int skippable;

	offset += 1;
	int counter = 0;
	while(input[offset] != 0){
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
		offset += skippable;
	}
/*
	for(int i = 0; i < binaryInput.size(); i++){
		cout << binaryInput[i] << ' ';
	}
*/
	return binaryInput;
}

vector<string> initializeCodeTable(const int& tableSize, const int& codeSize, int &clearCode, int &endCode){
	stringstream ss;
	vector<string> codeTable;
	for(int i = 0; i < tableSize; i++){
		ss << i;
		codeTable.push_back(ss.str());
		ss.str("");
	}
	int trueSize = exp(codeSize - 1);
	for(int i = codeTable.size(); i < trueSize; i++){
		codeTable.push_back("emptyspace");
	}
	clearCode = trueSize;
	endCode = trueSize + 1;
	ss << trueSize;
	codeTable.push_back(ss.str());
	ss.str("");
	ss << trueSize + 1;
	codeTable.push_back(ss.str());
	ss.str("");
	return codeTable;
}

void organizeData(const vector<string>& indexStream, const vector<uint8_t>& input){
	stringstream ss;
	int a = input[7];
	int b = input[6];
		
	if(a < 0x10) ss << 0;
	ss << hex << a;
	if(b < 0x10) ss << 0;
	ss << hex << b;
	int width = (int)strtol(ss.str().c_str(), NULL, 16);
	ss.str("");

	a = input[9];
	b = input[8];
	if(a < 0x10) ss << 0;
	ss << hex << a;
	if(b < 0x10) ss << 0;
	ss << hex << b;
	int height = (int)strtol(ss.str().c_str(), NULL, 16);
	ss.str("");


	char organizedStream[width][height];
	int currentX = 0, currentY = 0;
	
	for(int i = 0; i < indexStream.size(); i++){
		for(int j = 0; j < indexStream[i].size(); j++){
			organizedStream[currentX][currentY] = indexStream[i][j];
			currentX++;
			if(currentX == width){
				currentY++;
				currentX = 0;
			}
		}
	}

	ofstream myfile;
	myfile.open("data.txt");
	
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			myfile << organizedStream[j][i] << ' ';
		}
		myfile << endl;
	}
	myfile.close();
}

vector<int> getImageData(const vector<uint8_t>& input, int offset, const int& size){
	int codeSize = input[offset] + 1; //minimum size for gifs
	int clearCode, endCode;
	stringstream ss;
	int counter = 0;

	vector<string> codeTable;
	vector<string> originalCodeTable;
	vector<string> indexStream;
	vector<int> codeStream;
	vector<int> binaryInput;

	//initializes code table with color table values
	codeTable = initializeCodeTable(size, codeSize, clearCode, endCode);
	originalCodeTable = codeTable;

	//sets up the data in binary form
	binaryInput = initializeBinary(input, offset);

	int pW, cW;
	string p = "", c = "";
	cW = binToHex(extract(binaryInput, codeSize));
	codeStream.push_back(cW);
	if(cW != clearCode) indexStream.push_back(codeTable[cW]); //this line shouldn't run
	cW = binToHex(extract(binaryInput, codeSize)); //this should be the first legit output
	codeStream.push_back(cW);
	indexStream.push_back(codeTable[cW]);

	while(!binaryInput.empty()){
		pW = cW;
		cW = binToHex(extract(binaryInput, codeSize));

		if(cW == endCode){
			codeStream.push_back(cW);
			continue;
		}
		if(cW == clearCode){
			cout << "ACTIVATED" << endl << endl;
			codeTable = originalCodeTable;
			codeSize = input[offset] + 1;
		}
		if(codeTable.size() == (exp(codeSize) - 1)){
			codeSize++; // <- i think this is the problem
		} 
		if(cW > codeTable.size() - 1){ //aka cW isn't in the dictionary
			if(pW >= codeTable.size()) {
				c = p[0];
			} else {
				ss << codeTable[pW];
				p = ss.str();
				c = ss.str()[0];
			}
			codeStream.push_back(codeTable.size());
			codeTable.push_back(p+c);
			ss.str("");
			indexStream.push_back(p+c);
			p += c;
		} else {
			codeStream.push_back(cW);
			indexStream.push_back(codeTable[cW]);
			if(pW >= codeTable.size()){
				//do nothing to p
			} else {
				ss << codeTable[pW];
				p = ss.str();
				ss.str("");
			}
			ss << codeTable[cW];
			c = ss.str()[0];
			ss.str("");
			codeTable.push_back(p + c);
			p += c;
		}
	}
	
	
	codeTable[clearCode] = "clear code";
	codeTable[endCode] = "end of information code";
	cout << "\nCode Table: \n------------------------\n";
	for(int i = 0; i < codeTable.size(); i++){
		printf("Position: %2d | ", i);
		cout << codeTable[i] << endl;
	}
	/*print(codeStream);
	for(int i = 0; i < indexStream.size(); i++){
		cout << indexStream[i] << ' ';
	}
	cout << endl;*/

	organizeData(indexStream, input);
	return codeStream;
}

vector<string> getColorTable(const vector<uint8_t>& input){
	//account for local color tables
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
	/*
	//make debugging easier
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
	//cout << totalOffset << endl;
	return totalOffset;
}
#endif