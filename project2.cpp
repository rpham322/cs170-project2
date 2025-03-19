#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <limits>

using namespace std;




// Function to read in data from file
vector<vector<double>> loadData(const string& filename) {
    vector<vector<double>> data;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        vector<double> row;
        double value;
        while (ss >> value) {
            row.push_back(value);
        }
        if (!row.empty()) {
            data.push_back(row);
        }
    }

    file.close();
    return data;
}

int main() {
    string filename = "/Users/randypham/Downloads/CS170_Small_Data__97.txt"; 
    vector<vector<double>> data = loadData(filename);


    return 0;
}
