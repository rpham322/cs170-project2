#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <limits>
#include <set>
#include <iomanip>

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
double leaveOneOutCrossValidation(const vector<vector<double>>& data, const vector<int>& features) {
    int correctlyClassified = 0;
    int totalInstances = data.size();
    for (size_t i = 0; i < totalInstances; ++i) {
        vector<double> objectToClassify;
        for (int feature : features) {
            objectToClassify.push_back(data[i][feature]);
        }
        int labelObjectToClassify = static_cast<int>(data[i][0]);

        double nearestNeighborDistance = numeric_limits<double>::infinity();
        int nearestNeighborLabel = -1;

        for (size_t k = 0; k < totalInstances; ++k) {
            if (k != i) {
                vector<double> competitor;
                for (int feature : features) {
                    competitor.push_back(data[k][feature]);
                }
                double distance = euclideanDistance(objectToClassify, competitor);
                if (distance < nearestNeighborDistance) {
                    nearestNeighborDistance = distance;
                    nearestNeighborLabel = static_cast<int>(data[k][0]);
                }
            }
        }
        if (labelObjectToClassify == nearestNeighborLabel) {
            correctlyClassified++;
        }
    }
    return static_cast<double>(correctlyClassified) / totalInstances * 100; // REMOVE OR ADD 100 if percentage is not working
}


void forwardSelection(const vector<vector<double>>& data) {
    int numFeatures = data[0].size() - 1; // Ignore class column
    int numInstances = data.size();
    vector<int> selectedFeatures;
    set<int> remainingFeatures;
    
    // Initialize remaining features
    for (int i = 1; i <= numFeatures; i++) {
        remainingFeatures.insert(i);
    }

    double bestAccuracy = 0.0;

    cout << "This dataset has " << numFeatures << " features (not including the class attribute), with " <<  numInstances << " instances" << endl; 

    cout << "Running nearest neighbor with all " << numFeatures << " features, using \"leaving-one-out\" evaluation, I get an accuracy of " << fixed << setprecision(1) << leaveOneOutCrossValidation(data, vector<int>(remainingFeatures.begin(), remainingFeatures.end())) << "%\n";
    cout << "Beginning search." << endl;

    while (!remainingFeatures.empty()) {
        int bestFeature = -1;
        double highestAccuracy = 0.0;

        for (int feature : remainingFeatures) {
            vector<int> tempSet(selectedFeatures.begin(), selectedFeatures.end());
            tempSet.push_back(feature);

            double accuracy = leaveOneOutCrossValidation(data, tempSet);
            cout << "Using feature(s) { ";
            for (int f : tempSet) cout << f << " ";
            cout << "} accuracy is " << fixed << setprecision(1) << accuracy << "%" << endl; 

            if (accuracy > highestAccuracy) {
                highestAccuracy = accuracy;
                bestFeature = feature;
            }
        }

        if (highestAccuracy <= bestAccuracy) {
            cout << "(Warning, Accuracy has decreased! Continuing search in case of local maxima)" << endl;
            break;
        }

        bestAccuracy = highestAccuracy;
        selectedFeatures.push_back(bestFeature);
        remainingFeatures.erase(bestFeature);

        cout << "Feature set { ";
        for (int f : selectedFeatures) cout << f << " ";
        cout << "} was best, accuracy is " << bestAccuracy << "%" << endl;
    }

    cout << "Finished search!! The best feature subset is { ";
    for (int f : selectedFeatures) cout << f << " ";
    cout << "}, which has an accuracy of " << bestAccuracy << "%" << endl;
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
    int choice;
    string filename;
    vector<vector<double>> data;

    cout << "Welcome to Randy Phams Feature Selection Algorithm.\n";
    cout << "Type in the name of the file to test: ";
    getline(cin, filename);  // Get the filename from the user

    // Load data from the specified file
    data = loadData(filename);
    if (data.empty()) {
        cerr << "Error! Failed to load data." << endl;
        return 1;  // Exit if no data could be loaded
    }

    cout << "Type the number of the algorithm you want to run." << endl;
    cout << "1) Forward Selection" << endl;
    cout << "2) Backward Elimination" << endl;
    cin >> choice;

    switch(choice) {
        case 1:
            forwardSelection(data);
            break;
        case 2:
            //backwardElimination(data);
            break;
        default:
            cout << "Invalid choice!" << endl;
            break;
    }




    // // Calculate and display the accuracy of the nearest neighbor classification
    // double accuracy = leaveOneOutCrossValidation(data);
    // cout << "The accuracy of the nearest neighbor classifier is: " << accuracy * 100 << "%" << endl;

    return 0;
}