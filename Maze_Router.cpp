#include<iostream>
#include<vector>
#include<fstream>
#include<string>

using namespace std;

struct cell {
	int x, y, layer;

};

void initialize_arrays(ifstream &in);


int** M1;
int** M2;


int main() {
	
	ifstream in;
	in.open("DEF_file.txt");
	if (!in.fail()) {
		cout << "Can't open DEF file";
		exit(0);
	}
	
	initialize_arrays(in);
	

}

void initialize_arrays(ifstream &in) {
	string size;

	in >> size;
	cout << size;
	int x_idx = size.find('x');
	string M = size.substr(0, x_idx - 1);
	string N = size.substr(x_idx + 1, size.size() - x_idx);
	M1 = new int* [stoi(M)];
	M2 = new int* [stoi(M)];

	for (int i = 0; i < stoi(M); i++) {
		*(M1 + i) = new int[stoi(N)];
		*(M2 + i) = new int[stoi(N)];
	}

}