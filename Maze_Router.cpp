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
void input_data(vector<vector<cell>>& nets);        //Reads the input file
void initialize_arrays(ifstream& in);				//Sets the layers arrays
void record_net(string line, vector<cell>& net);	//Records the pins of the nets
void route_net(vector<cell> pins, vector<cell>& routed_net, vector<cell> avoid_pins);	//Creating a net
void Fill(vector<cell>S, vector<cell>& T, vector<cell>& route, vector<cell> avoid_pins);            //Fill the array 
void back_propagation(int** Copy_M1, int** Copy_M2, cell T, vector<cell>& route); //back propagation function 
void print_arrays(int** A, int** B);    //printing the arrays 
void output_nets(vector<vector<cell>> routed_nets);   //Print the final routed nets 
void heuristic_ordering(vector<vector<cell>>& nets);  //function to re-order the nets to minimize wire lengths
bool compare_net_scores(pair<int, vector<cell>> L1, pair<int, vector<cell>> L2); //bbolean function to help in reordering the nets 

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

	vector<vector<cell>> routed_nets;
	int net_idx = 0;
	heuristic_ordering(nets);
	for (int i = 0; i < nets.size(); i++) {
		routed_nets.resize(net_idx + 1);
		vector<cell> avoid_pins;
		for (int j = i + 1; j < nets.size(); j++) {
			avoid_pins.insert(avoid_pins.end(), nets[j].begin(), nets[j].end());
		}
		route_net(nets[i], routed_nets[net_idx], avoid_pins);

		//Testing routing
		/*for (int i = 0; i < routed_nets[net_idx].size(); i++) {
			cout << routed_nets[net_idx][i].x << " " << routed_nets[net_idx][i].y << " " << routed_nets[net_idx][i].layer << "\n";
		}*/
		cout << endl;
		net_idx++;
	}

	output_nets(routed_nets);
	for (int i = 0; i < routed_nets.size(); i++) {
		cout << routed_nets[i][0].net_name;
		for (int j = 0; j < routed_nets[i].size(); j++) {
			cout << "(" << routed_nets[i][j].layer << "," << routed_nets[i][j].x << "," << routed_nets[i][j].y << ")";
		}
		cout << "\n";
	}

	return 0;
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

void route_net(vector<cell> pins, vector<cell>& routed_net, vector<cell> avoid_pins) {
	vector<cell> T;

	for (int i = 1; i < pins.size(); i++) {
		T.push_back(pins[i]);
	}

	routed_net.push_back(pins[0]);
	while (T.size() > 0) {
		vector<cell> route;
		Fill(routed_net, T, route, avoid_pins);
		if (route.empty()) {
			cout << "Could not route " << T[0].net_name;
			break;
		}
		else {
			for (int i = route.size() - 2; i >= 0; i--) {
				routed_net.push_back(route[i]);
			}
		}

	}

	for (int i = 0; i < routed_net.size(); i++) {
		if (routed_net[i].layer == 1) {
			M1[routed_net[i].x][routed_net[i].y] = -1;
		}
		else {
			M2[routed_net[i].x][routed_net[i].y] = -1;
		}
	}
}

void Fill(vector<cell>S, vector<cell>& T, vector<cell>& route, vector<cell> avoid_pins) {
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

	for (int i = 0; i < avoid_pins.size(); i++) {
		if (avoid_pins[i].layer == 1) {
			Copy_M1[avoid_pins[i].x][avoid_pins[i].y] = -1;
		}
		else {
			Copy_M2[avoid_pins[i].x][avoid_pins[i].y] = -1;
		}
	}

	cell target;
	while (!found) {
		cell current_cell = q.front();
		q.pop();

		//Checking if we reached one of the targets
		for (int i = 0; i < T.size(); i++) {
			if (T[i].x == current_cell.x && T[i].y == current_cell.y && T[i].layer == current_cell.layer) {
				target = T[i];
				T.erase(T.begin() + i);	//Removing the target as it will be routed
				found = true;
				//cout << "Found target\n";
				back_propagation(Copy_M1, Copy_M2, target, route);
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

void back_propagation(int** Copy_M1, int** Copy_M2, cell T, vector<cell>& route) {
	//print_arrays(Copy_M1, Copy_M2);
	//cout << T.x << " " << T.y << " " << T.layer << endl;
	int curr_val;
	cell current_cell = T;
	route.push_back(current_cell);
	if (current_cell.layer == 1) {
		curr_val = Copy_M1[current_cell.x][current_cell.y];
	}
	else {
		curr_val = Copy_M2[current_cell.x][current_cell.y];
	}
	do {
		if (current_cell.layer == 1) {
			//In case the path is in layer one and wants to move to the left 
			if (current_cell.y > 0 && Copy_M1[current_cell.x][current_cell.y] == (Copy_M1[current_cell.x][current_cell.y - 1] + 1)) {
				current_cell.y--;
				curr_val--;
			}
			//In case the path is in layer one and wants to move to the right 
			else if (current_cell.y < (n - 1) && Copy_M1[current_cell.x][current_cell.y] == (Copy_M1[current_cell.x][current_cell.y + 1] + 1)) {
				current_cell.y++;
				curr_val--;
			}
			//In case the path is in layer one and wants to move to the other layer using via   
			else if (Copy_M1[current_cell.x][current_cell.y] == (Copy_M2[current_cell.x][current_cell.y] + via_cost)) {
				current_cell.layer = 2;
				curr_val -= via_cost;
			}
			//In case the path is in layer one and wants to move vertically   
			else if (current_cell.x < (m - 1) && Copy_M1[current_cell.x][current_cell.y] == (Copy_M1[current_cell.x + 1][current_cell.y] + wrong_dir)) {
				current_cell.x++;
				curr_val -= wrong_dir;
			}
			//In case the path is in layer one and wants to move vertically   
			else if (current_cell.x > 0 && Copy_M1[current_cell.x][current_cell.y] == (Copy_M1[current_cell.x - 1][current_cell.y] + wrong_dir)) {
				current_cell.x--;
				curr_val -= wrong_dir;
			}
			//in case there is no path whether in the wrong direction, right direction nor using via 
			else {
				int trans_val = MAX_INT;
				cell temp_cell = current_cell;
				if (Copy_M2[current_cell.x][current_cell.y] != -1) {
					trans_val = Copy_M2[current_cell.x][current_cell.y] + via_cost;
					temp_cell = current_cell;
					temp_cell.layer = 2;
					curr_val = Copy_M2[current_cell.x][current_cell.y];
				}
				if (current_cell.x > 0 && Copy_M1[current_cell.x - 1][current_cell.y] != -1 && trans_val > Copy_M1[current_cell.x - 1][current_cell.y] + wrong_dir) {
					trans_val = Copy_M1[current_cell.x - 1][current_cell.y] + wrong_dir;
					temp_cell.layer = 1;
					temp_cell.x = current_cell.x - 1;
					curr_val = Copy_M1[current_cell.x - 1][current_cell.y];
				}
				if (current_cell.x < (m - 1) && Copy_M1[current_cell.x + 1][current_cell.y] != -1 && trans_val > Copy_M1[current_cell.x + 1][current_cell.y] + wrong_dir) {
					temp_cell.layer = 1;
					temp_cell.x = current_cell.x + 1;
					curr_val = Copy_M1[current_cell.x + 1][current_cell.y];
				}
				current_cell = temp_cell;
			}
		}
		else {
			//In case the path is in layer two and wants to move to the down  
			if (current_cell.x < (m - 1) && Copy_M2[current_cell.x][current_cell.y] == (Copy_M2[current_cell.x + 1][current_cell.y] + 1)) {
				current_cell.x++;
				curr_val -= 1;
			}
			//In case the path is in layer two and wants to move to the up  
			else if (current_cell.x > 0 && Copy_M2[current_cell.x][current_cell.y] == (Copy_M2[current_cell.x - 1][current_cell.y] + 1)) {
				current_cell.x--;
				curr_val -= 1;
			}
			//In case the path is in layer two and wants to move to the other layer through via 
			else if (Copy_M2[current_cell.x][current_cell.y] == (Copy_M1[current_cell.x][current_cell.y] + via_cost)) {
				current_cell.layer = 1;
				curr_val -= via_cost;
			}
			//In case the path is in layer two and wants to move to the left 
			else if (current_cell.y > 0 && Copy_M2[current_cell.x][current_cell.y] == (Copy_M2[current_cell.x][current_cell.y - 1] + wrong_dir)) {
				current_cell.y--;
				curr_val -= wrong_dir;
			}
			//In case the path is in layer two and wants to move to the right 
			else if (current_cell.y < (n - 1) && Copy_M2[current_cell.x][current_cell.y] == (Copy_M2[current_cell.x][current_cell.y + 1] + wrong_dir)) {
				current_cell.y++;
				curr_val -= wrong_dir;
			}
			//in case there is no path whether in the wrong direction, right direction nor using via 
			else {
				cell temp_cell = current_cell;
				int trans_val = MAX_INT;
				if (Copy_M1[current_cell.x][current_cell.y] != -1) {
					trans_val = Copy_M1[current_cell.x][current_cell.y] + via_cost;
					temp_cell = current_cell;
					temp_cell.layer = 1;
					curr_val = Copy_M1[current_cell.x][current_cell.y];
				}
				if (current_cell.y > 0 && Copy_M2[current_cell.x][current_cell.y - 1] != -1 && trans_val > Copy_M2[current_cell.x][current_cell.y - 1] + wrong_dir) {
					trans_val = Copy_M2[current_cell.x][current_cell.y - 1] + wrong_dir;
					temp_cell.layer = 2;
					temp_cell.y = current_cell.y - 1;
					curr_val = Copy_M2[current_cell.x][current_cell.y - 1];
				}
				if (current_cell.x < (m - 1) && Copy_M2[current_cell.x][current_cell.y + 1] != -1 && trans_val > Copy_M2[current_cell.x][current_cell.y + 1] + wrong_dir) {
					temp_cell.layer = 2;
					temp_cell.y = current_cell.y + 1;
					curr_val = Copy_M2[current_cell.x][current_cell.y + 1];
				}
				current_cell = temp_cell;
			}
		}
		//pushing the correct path from one pin to another in this vector 
		route.push_back(current_cell);
		//cout << curr_val << endl;
	} while (curr_val != 0);

}

void output_nets(vector<vector<cell>> routed_nets) {
	//file to save the final output 
	ofstream out;
	out.open("Routed Nets.txt");
	if (out.fail()) {
		cout << "Error creating output file!";
		exit(1);
	}
	//printing the final nets in the output file 
	for (int i = 0; i < routed_nets.size(); i++) {
		out << routed_nets[i][0].net_name;
		for (int j = 0; j < routed_nets[i].size(); j++) {
			out << "(" << routed_nets[i][j].layer << "," << routed_nets[i][j].x << "," << routed_nets[i][j].y << ")";
		}
		out << "\n";
	}
}

void heuristic_ordering(vector<vector<cell>> &nets) {
	vector<pair<int,vector<cell>>> score;
	score.resize(nets.size());

	for (int i = 0; i < nets.size(); i++) {
		int min_x = MAX_INT;
		int min_y = MAX_INT;
		int max_x = -1;
		int max_y = -1;
		score[i].first = 0;
		score[i].second = nets[i];
		for (int j = 0; j < nets[i].size(); j++) {
			//if statements to find the minimum,maximum x values and the minimum,maximum y values 
			if (nets[i][j].x <= min_x)
				min_x = nets[i][j].x;
			if (nets[i][j].y <= min_y)
				min_y = nets[i][j].y;
			if (nets[i][j].x >= max_x)
				max_x = nets[i][j].x;
			if (nets[i][j].y >= max_y)
				max_y = nets[i][j].y;
		}
		for (int k = 0; k < nets.size(); k++) {
			if (i != k) {
				for (int c = 0; c < nets[k].size(); c++) {
					//giving a score to each net 
					if (nets[k][c].x < max_x && nets[k][c].x > min_x && nets[k][c].y < max_y && nets[k][c].y > min_y) {
						score[i].first++;
					}
			    }
			}	
		}
	}

	sort(score.begin(), score.end(), compare_net_scores);
	for (int i = 0; i < nets.size(); i++) {
		nets[i] = score[i].second;
	}
	//Testing the function 
	/*for (int i = 0; i < nets.size(); i++)
		cout << nets[i][0].net_name << endl;*/
	
}

bool compare_net_scores(pair<int, vector<cell>> L1, pair<int, vector<cell>> L2) {
	return (L1.first < L2.first);
}