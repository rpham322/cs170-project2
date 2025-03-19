#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <limits>

using namespace std;

// Function to compute Euclidean distance
double euclideanDistance(const vector<double>& obj1, const vector<double>& obj2) {
    double sum = 0.0;
    for (size_t i = 0; i < obj1.size(); ++i) {
        sum += pow(obj1[i] - obj2[i], 2);  // Power of 2, summing, and then taking the square root
    }
    return sqrt(sum);
}

// Leave-one-out cross-validation function
double leaveOneOutCrossValidation(const vector<vector<double>>& data) {
    int correctlyClassified = 0;    // Number of correctly classified instances, initialized to zero
    int totalInstances = data.size(); // Total number of instances

    // Loop through each instance in the dataset
    for (size_t i = 0; i < totalInstances; ++i) {
        vector<double> objectToClassify(data[i].begin() + 1, data[i].end()); // Extract feature values
        int labelObjectToClassify = static_cast<int>(data[i][0]); // Extract class label

        double nearestNeighborDistance = numeric_limits<double>::infinity();
        int nearestNeighborLocation = -1;
        int nearestNeighborLabel = -1;

        // Loop through all other instances to find the nearest neighbor
        for (size_t k = 0; k < totalInstances; ++k) {
            if (k != i) {  // Exclude the current instance
                double distance = euclideanDistance(objectToClassify, vector<double>(data[k].begin() + 1, data[k].end()));

                if (distance < nearestNeighborDistance) { // Update nearest neighbor
                    nearestNeighborDistance = distance;
                    nearestNeighborLocation = k;
                    nearestNeighborLabel = static_cast<int>(data[k][0]);
                }
            }
        }

        // Check if classification is correct before incrementing counter
        if (labelObjectToClassify == nearestNeighborLabel) {
            correctlyClassified++; // Increment counter of correctly classified instances
        }
    }
    // Compute accuracy
    return static_cast<double>(correctlyClassified) / totalInstances;
}


// Function to read in data from file
vector<vector<double>> loadData(const string& filename) {
    vector<vector<double>> data;
    ifstream file(filename);  // Open the file
    if (!file) {
        cerr << "Cannot open the file!" << endl;
        return {};  // Return an empty vector if file cannot be opened
    }
    string line;

    while (getline(file, line)) {  // Read lines from the file
        stringstream ss(line);
        vector<double> row;
        double value;
        while (ss >> value) {  // Extract values to populate the row
            row.push_back(value);
        }
        if (!row.empty()) {
            data.push_back(row);  // Add the row to the main data vector
        }
    }

    file.close();  // Close the file
    return data;  // Return the loaded data
}


int main() {
    string filename;
    vector<vector<double>> data;

    cout << "Welcome to the Nearest Neighbor Testing Program.\n";
    cout << "Type in the name of the file to test: ";
    getline(cin, filename);  // Get the filename from the user

    // Load data from the specified file
    data = loadData(filename);
    if (data.empty()) {
        cerr << "Error: Failed to load data!" << endl;
        return 1;  // Exit if no data could be loaded
    }

    // Calculate and display the accuracy of the nearest neighbor classification
    double accuracy = leaveOneOutCrossValidation(data);
    cout << "The accuracy of the nearest neighbor classifier is: " << accuracy * 100 << "%" << endl;

    return 0;
}