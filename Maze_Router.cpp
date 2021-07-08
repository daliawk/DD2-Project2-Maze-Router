#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include<string>

using namespace std;

struct cell {
	int x, y, layer;

};

void initialize_arrays(ifstream &in);
void add_obstacles(ifstream& in);

int** M1;
int** M2;


int main() {
	ifstream in;
	add_obstacles(in);
	

}

void initialize_arrays(ifstream &in) {
	string size;

		in.open("DEF_file.txt");
		if (in.fail()) {
			cout << "Can't open DEF file";
			exit(0);
		}
		in >> size;

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
void add_obstacles(ifstream& in) {

	string line,size;
	cin >> size;
	in.open("DEF_file.txt");
	if (in.fail()) {
		cout << "Can't open DEF file";
		exit(0);
	}
	line = "OBS,() ";
	while (!in.eof()) {
		getline(in, line);
		for (int i = 0, len = line.size(); i < len; i++)
		{
			// check whether parsing character is punctuation or not
			if (ispunct(line[i]))
			{
				line.erase(i--, 1);
				len = line.size();
			}
		}
		cout << line.data() << endl;
	}
}