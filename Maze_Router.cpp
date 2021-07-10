#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include<string>

using namespace std;

//Struct for pins
struct cell {
	int x, y, layer;
	string net_name;
};

//Functions
void input_data(vector<vector<cell>>& nets);		//Reads the input file
void initialize_arrays(ifstream& in);				//Sets the layers arrays
void record_net(string line, vector<cell>& net);	//Records the pins of the nets

//Global variables
int** M1;	//Layer 1 array
int** M2;	//Layer 2 array
int m, n;	//Array dimensions


int main() {
	vector<vector<cell>> nets;	//List of nets
	input_data(nets);			//Reading the input file

	//Testing
	/*
	for (int r = 0; r < nets.size(); r++) {
		cout << nets[r][0].net_name << ": ";
		for (int c = 0; c < nets[r].size(); c++) {
			cout << "(" << nets[r][c].layer << ", " << nets[r][c].x << ", " << nets[r][c].y << ") ";
		}
		cout << endl;
	}
	*/
}

void input_data(vector<vector<cell>>& nets) {
	ifstream in;
	in.open("DEF_file.txt");
	if (in.fail()) {
		cout << "Can't open DEF file";
		exit(0);
	}

	initialize_arrays(in);	//Initializing the array

	//Testing
	//cout << m << " " << n << endl;

	string line;
	while (!in.eof()) {	//Reading each line in the input file
		getline(in, line);

		if (line.substr(0, 3) == "OBS") {	//If the file refers to an obstacle
			//Extracting its coordinates
			line.erase(0, 5);
			stringstream coordinates(line);
			string x_coordinates, y_coordinates;
			getline(coordinates, x_coordinates, ',');
			getline(coordinates, y_coordinates, ')');

			int x = stoi(x_coordinates);
			int y = stoi(y_coordinates);

			//Testing
			//cout << x << " " << y << endl;

			//Adding the obstacles in the array in case they are in range
			if (0 <= x && x < m && 0 <= y && y < n) {
				M1[x][y] = -1;
				M2[x][y] = -1;
			}
			else {
				cout << "The obstacle courdinates of (" << x << ", " << y << ") is not in range!\n";
			}
		}
		else {	//If the file refers to a net
			nets.resize(nets.size() + 1);				//Add a net in the vector
			record_net(line, nets[nets.size() - 1]);	//Obtaining the net's pins
			if (nets[nets.size() - 1].size() <= 1) {	//If the net includes no pins or one pin, then erase net
				nets.resize(nets.size() - 1);
			}
		}
	}
}

void initialize_arrays(ifstream& in) {
	string size;
	getline(in, size);
	
	stringstream size_ss(size);

	//Extracting size
	string x, y;
	getline(size_ss, x, 'x');
	getline(size_ss, y);
	m = stoi(x);
	n = stoi(y);

	//If size within range, intitialize array dimensions
	if (m <= 1000 && n <= 1000 && m > 0 && n > 0) {
		M1 = new int* [m];
		M2 = new int* [m];

		for (int i = 0; i < m; i++) {
			*(M1 + i) = new int[n];
			*(M2 + i) = new int[n];
		}
	}
	else {
		cout << "The maximum size for the grid is 1000x1000!";
		exit(1);
	}

}

void record_net(string line, vector<cell>& net) {
	stringstream net_details(line);
	string net_name;
	getline(net_details, net_name, ' ');	//Obtaining net name

	//Looping over each pin
	while (!line.empty()) {
		cell pin;
		string x, y, layer;

		//Extracting pin details
		int brak_open_idx = line.find('(');
		int brak_clos_idx = line.find(')');
		string pin_details = line.substr(brak_open_idx + 1, brak_clos_idx);
		stringstream pin_ss(pin_details);
		getline(pin_ss, layer, ',');
		getline(pin_ss, x, ',');
		getline(pin_ss, y, ')');
		line.erase(0, brak_clos_idx + 1);

		//Setting pin details
		pin.layer = stoi(layer);
		pin.net_name = net_name;
		pin.x = stoi(x);
		pin.y = stoi(y);

		//If details are realistic, add to the net
		if (pin.x >= 0 && pin.x < m && pin.y >= 0 && pin.y < n && pin.layer > 0 && pin.layer < 3) {
			net.push_back(pin);
		}
		else {
			cout << "The pin " << pin.net_name << " has invalid coordinates of (" << pin.layer << ", " << pin.x << ", " << pin.y << ")!\n";
		}
	}
}