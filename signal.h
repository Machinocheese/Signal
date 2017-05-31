#ifndef SIGNAL_H
#define SIGNAL_H
#include <vector>
#include <algorithm>
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

	cout << totalOffset;
	printf("\n%x", input[totalOffset]);
}
#endif