#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include<string>
#include<queue>
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
void Fill(vector<cell>S, vector<cell>T);            //Fill the array 
//Global variables
int** M1;	//Layer 1 array
int** M2;	//Layer 2 array
int m, n;	//Array dimensions
int MAX_INT = 1e9;	//Infinity
int via_cost, wrong_dir;


int main() {
	vector<vector<cell>> nets;	//List of nets
	input_data(nets);			//Reading the input file

	cout << "Please enter the cost of using vias: ";
	cin >> via_cost;

	cout << "Please enter the cost of unpreferred routing direction: ";
	cin >> wrong_dir;

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

	//Testing Fill Function
	/*
	vector<cell> S;  vector<cell> T;
	S = { { 3,5,1,"net1"} };
	T = { { 1,4,2,"net2"} };

	Fill(S, T);
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
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < n; j++) {
				if (M1[i][j] != -1) {
					M1[i][j] = MAX_INT;
					M2[i][j] = MAX_INT;
				}
			}
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

void Fill(vector<cell>S, vector<cell>T) {
	int** Copy_M1;
	int** Copy_M2;

	Copy_M1 = new int* [m];
	Copy_M2 = new int* [m];

	for (int i = 0; i < m; i++) {
		*(Copy_M1 + i) = new int[n];
		*(Copy_M2 + i) = new int[n];
	}

	for (int j = 0; j < m; j++) {
		memcpy(Copy_M1[j], M1[j], n * sizeof(int));
		memcpy(Copy_M2[j], M2[j], n * sizeof(int));
	}



	queue<cell>q;
	bool found = false;

	for (int i = 0; i < S.size(); i++) {
		q.push(S[i]);
		if (S[i].layer == 1) {
			Copy_M1[S[i].x][S[i].y] = 0;
		}
		else
			Copy_M2[S[i].x][S[i].y] = 0;
	}


	cell target;
	while (!found) {
		cell current_cell = q.front();
		q.pop();

		//Checking if we reached one of the targets
		for (int i = 0; i < T.size(); i++) {
			if (T[i].x == current_cell.x && T[i].y == current_cell.y && T[i].layer == current_cell.layer) {
				target = T[i];
				found = true;
				break;
			}
		}

		//Layer 1: Horizontal
		if (current_cell.layer == 1) {
			

			//In case the cell on the left have not been given a value
			if (current_cell.y > 0 && Copy_M1[current_cell.x][current_cell.y - 1] > (Copy_M1[current_cell.x][current_cell.y] + 1)) {
				Copy_M1[current_cell.x][current_cell.y - 1] = Copy_M1[current_cell.x][current_cell.y] + 1;	//Adding cost to new cell

				//Adding cell to queue
				cell temp;
				temp.x = current_cell.x;
				temp.y = current_cell.y - 1;
				temp.layer = 1;
				q.push(temp);
			}

			//In case the cell on the right have not been given a value
			if (current_cell.y < (n - 1) && Copy_M1[current_cell.x][current_cell.y + 1] >(Copy_M1[current_cell.x][current_cell.y] + 1)) {
				Copy_M1[current_cell.x][current_cell.y + 1] = Copy_M1[current_cell.x][current_cell.y] + 1;	//Adding cost to new cell

				//Adding cell to queue
				cell temp;
				temp.x = current_cell.x;
				temp.y = current_cell.y + 1;
				temp.layer = 1;
				q.push(temp);
			}

			//In case the cell on the opposite layer has not been given a value
			if (Copy_M2[current_cell.x][current_cell.y] > (Copy_M1[current_cell.x][current_cell.y] + via_cost)) {
				Copy_M2[current_cell.x][current_cell.y] = Copy_M1[current_cell.x][current_cell.y] + via_cost;	//Adding cost to new cell

				//Adding cell to queue
				cell temp;
				temp.x = current_cell.x;
				temp.y = current_cell.y;
				temp.layer = 2;
				q.push(temp);
			}

			//In case the cell on the top have not been given a value
			if (current_cell.x > 0 && Copy_M1[current_cell.x - 1][current_cell.y] > (Copy_M1[current_cell.x][current_cell.y] + wrong_dir)) {
				Copy_M1[current_cell.x - 1][current_cell.y] = Copy_M1[current_cell.x][current_cell.y] + wrong_dir;	//Adding cost to new cell

				//Adding cell to queue
				cell temp;
				temp.x = current_cell.x - 1;
				temp.y = current_cell.y;
				temp.layer = 1;
				q.push(temp);
			}

			//In case the cell on the bottom have not been given a value
			if (current_cell.x < (m - 1) && Copy_M1[current_cell.x + 1][current_cell.y] >(Copy_M1[current_cell.x][current_cell.y] + wrong_dir)) {
				Copy_M1[current_cell.x + 1][current_cell.y] = Copy_M1[current_cell.x][current_cell.y] + wrong_dir;	//Adding cost to new cell

				//Adding cell to queue
				cell temp;
				temp.x = current_cell.x + 1;
				temp.y = current_cell.y;
				temp.layer = 1;
				q.push(temp);
			}
		}
		else {	//Layer 2: Vertical
			//In case the cell on the top have not been given a value
			if (current_cell.x > 0 && Copy_M2[current_cell.x - 1][current_cell.y] > (Copy_M2[current_cell.x][current_cell.y] + 1)) {
				Copy_M2[current_cell.x - 1][current_cell.y] = Copy_M2[current_cell.x][current_cell.y] + 1;	//Adding cost to new cell

				//Adding cell to queue
				cell temp;
				temp.x = current_cell.x - 1;
				temp.y = current_cell.y;
				temp.layer = 2;
				q.push(temp);
			}

			//In case the cell on the bottom have not been given a value
			if (current_cell.x < (m - 1) && Copy_M2[current_cell.x + 1][current_cell.y] >(Copy_M2[current_cell.x][current_cell.y] + 1)) {
				Copy_M2[current_cell.x + 1][current_cell.y] = Copy_M2[current_cell.x][current_cell.y] + 1;	//Adding cost to new cell

				//Adding cell to queue
				cell temp;
				temp.x = current_cell.x + 1;
				temp.y = current_cell.y;
				temp.layer = 2;
				q.push(temp);
			}

			//In case the cell on the opposite layer has not been given a value
			if (Copy_M1[current_cell.x][current_cell.y] > (Copy_M2[current_cell.x][current_cell.y] + via_cost)) {
				Copy_M1[current_cell.x][current_cell.y] = Copy_M2[current_cell.x][current_cell.y] + via_cost;	//Adding cost to new cell

				//Adding cell to queue
				cell temp;
				temp.x = current_cell.x;
				temp.y = current_cell.y;
				temp.layer = 1;
				q.push(temp);
			}

			//In case the cell on the left have not been given a value
			if (current_cell.y > 0 && Copy_M2[current_cell.x][current_cell.y - 1] > (Copy_M2[current_cell.x][current_cell.y] + wrong_dir)) {
				Copy_M2[current_cell.x][current_cell.y - 1] = Copy_M2[current_cell.x][current_cell.y] + wrong_dir;	//Adding cost to new cell

				//Adding cell to queue
				cell temp;
				temp.x = current_cell.x;
				temp.y = current_cell.y - 1;
				temp.layer = 2;
				q.push(temp);
			}

			//In case the cell on the right have not been given a value
			if (current_cell.y < (n - 1) && Copy_M2[current_cell.x][current_cell.y + 1] >(Copy_M2[current_cell.x][current_cell.y] + wrong_dir)) {
				Copy_M2[current_cell.x][current_cell.y + 1] = Copy_M2[current_cell.x][current_cell.y] + wrong_dir;	//Adding cost to new cell

				//Adding cell to queue
				cell temp;
				temp.x = current_cell.x;
				temp.y = current_cell.y + 1;
				temp.layer = 2;
				q.push(temp);
			}
		}
	}
	
}

void print_arrays(int** A, int** B) {
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (A[i][j] == MAX_INT) {
				cout << " - ";
			}
			else
				cout << A[i][j] << " ";
		}
		cout << endl;
	}
	cout << "-------------------------------------------------------------------------------------------------------" << endl;
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (B[i][j] == MAX_INT) {
				cout << " - ";
			}
			else
				cout << B[i][j] << " ";
		}
		cout << endl;
	}
}