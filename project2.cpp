#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <limits>
#include <set>
#include <iomanip>
#include <chrono>


using namespace std;

// Function to compute Euclidean distance
double euclideanDistance(const vector<double>& obj1, const vector<double>& obj2) {
    double sum = 0.0;
    for (size_t i = 0; i < obj1.size(); ++i) {
        sum += pow(obj1[i] - obj2[i], 2);  // Power of 2, summing, and then taking the square root
    }
    return sqrt(sum);
}

// Leave-one-out cross-validation function conversion of professors psuedocode
double leaveOneOutCrossValidation(const vector<vector<double>>& data, const vector<int>& features) {
    int correctlyClassified = 0;
    int totalInstances = data.size();
    for (size_t i = 0; i < totalInstances; ++i) {
        vector<double> objectToClassify;
        for (int feature : features) {
            objectToClassify.push_back(data[i][feature]);
        }
        int labelObjectToClassify = static_cast<int>(data[i][0]);

        double nearestNeighborDistance = numeric_limits<double>::infinity(); // Initializing to large value
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
    return static_cast<double>(correctlyClassified) / totalInstances * 100; 
}



void forwardSelection(const vector<vector<double>>& data) {
    int numFeatures = data[0].size() - 1; // Ignore class column
    int numInstances = data.size(); // Number of instances
    vector<int> selectedFeatures; // Vector for storing selected features for the current subset
    vector<int> bestFeatureSetOverall; // Vector to store the best overall feature set
    set<int> remainingFeatures;
    
    // Initialize remaining features
    for (int i = 1; i <= numFeatures; i++) {
        remainingFeatures.insert(i);
    }

    double bestAccuracyOverall = 0.0;  // Best overall accuracy

    cout << "This dataset has " << numFeatures << " features (not including the class attribute), with " 
         << numInstances << " instances" << endl;
    cout << "Running nearest neighbor with all " << numFeatures 
         << " features, using \"leaving-one-out\" evaluation, I get an accuracy of " 
         << fixed << setprecision(1) << leaveOneOutCrossValidation(data, vector<int>(remainingFeatures.begin(), remainingFeatures.end())) 
         << "%\n";
    cout << "Beginning search." << endl;

    while (!remainingFeatures.empty()) {
        int bestFeature = -1;
        double bestAccuracyInCurrentSubset = 0.0;

        for (int feature : remainingFeatures) {
            vector<int> tempSet(selectedFeatures);
            tempSet.push_back(feature);

            double accuracy = leaveOneOutCrossValidation(data, tempSet);
            cout << "Using feature(s) {";
            for (size_t j = 0; j < tempSet.size(); j++) {
                cout << tempSet[j];
                if (j != tempSet.size() - 1) cout << ",";
            }
            cout << "} accuracy is " << fixed << setprecision(1) << accuracy << "%" << endl;

            if (accuracy > bestAccuracyInCurrentSubset) {
                bestAccuracyInCurrentSubset = accuracy;
                bestFeature = feature;
            }
        }

        if (bestFeature != -1) {  // If a best feature was found
            selectedFeatures.push_back(bestFeature);
            remainingFeatures.erase(bestFeature);
        }

        if (bestAccuracyInCurrentSubset > bestAccuracyOverall) { // Replace best overall accurate subset with current subset that is more accurate
            bestAccuracyOverall = bestAccuracyInCurrentSubset;
            bestFeatureSetOverall = selectedFeatures;  // Update the overall best feature set
        }else{
            if (bestAccuracyInCurrentSubset < bestAccuracyOverall) { // If accuracy decrease display warning message
                cout << "(Warning, Accuracy has decreased! Continuing search in case of local maxima)" << endl; 

            }
        }


        cout << "Feature set {";
        for (size_t j = 0; j < selectedFeatures.size(); j++) {
            cout << selectedFeatures[j];
            if (j != selectedFeatures.size() - 1) cout << ",";
        }
        cout << "} was best, accuracy is " << bestAccuracyInCurrentSubset << "%" << endl;
    }

    // Output the best overall feature set
    cout << "Finished search!! The best feature subset is {";
    for (size_t j = 0; j < bestFeatureSetOverall.size(); j++) {
        cout << bestFeatureSetOverall[j];
        if (j != bestFeatureSetOverall.size() - 1) cout << ",";
    }
    cout << "}, which has an accuracy of " << bestAccuracyOverall << "%" << endl;
}


void backwardElimination(const vector<vector<double>>& data) {
    int numFeatures = data[0].size() - 1;  // Ignore class column
    int numInstances = data.size(); // Number of instances
    vector<int> selectedFeatures; // Vector for storing selected features
    vector<int> bestFeatureSetOverall; // To store the overall best feature set

    // Start with all features selected
    for (int i = 1; i <= numFeatures; i++) {
        selectedFeatures.push_back(i); // Append all features
    }

    // Calculate and display the accuracy for the full set of features before starting the elimination
    double bestAccuracyOverall = leaveOneOutCrossValidation(data, selectedFeatures); 
    cout << "This dataset has " << numFeatures << " features (not including the class attribute), with "
         << numInstances << " instances" << endl;
    cout << "Running nearest neighbor with all " << numFeatures
         << " features, using \"leaving-one-out\" evaluation, I get an accuracy of "
         << fixed << setprecision(1) << bestAccuracyOverall << "%\n";

    // Store the initial full set as the best overall feature set to start
    bestFeatureSetOverall = selectedFeatures;

    cout << "Full feature set accuracy is: " << bestAccuracyOverall << "%" << endl;
    cout << "Beginning search." << endl;

    double lastAccuracy = bestAccuracyOverall; // To track the last accuracy for warning messages

    while (selectedFeatures.size() > 1) {
        int worstFeature = -1;
        double bestAccuracyInCurrentSubset = 0.0;

        // Try removing each feature to see which one results in the highest accuracy
        for (size_t i = 0; i < selectedFeatures.size(); i++) {
            vector<int> tempSet(selectedFeatures);
            tempSet.erase(tempSet.begin() + i);  // Remove the feature at index i

            double accuracy = leaveOneOutCrossValidation(data, tempSet);
            cout << "Using feature(s) {";
            for (size_t j = 0; j < tempSet.size(); j++) {
                cout << tempSet[j];
                if (j != tempSet.size() - 1) cout << ",";
            }
            cout << "} accuracy is " << fixed << setprecision(1) << accuracy << "%" << endl;

            if (accuracy > bestAccuracyInCurrentSubset) {
                bestAccuracyInCurrentSubset = accuracy;
                worstFeature = i;
            }
        }

        if (bestAccuracyInCurrentSubset > bestAccuracyOverall) { // Replace best overall accurate subset with current subset that is more accurate
            bestAccuracyOverall = bestAccuracyInCurrentSubset;
            bestFeatureSetOverall = selectedFeatures;
            bestFeatureSetOverall.erase(bestFeatureSetOverall.begin() + worstFeature);
        }

        if (worstFeature != -1) { // If a worst feature was found, remove it
            selectedFeatures.erase(selectedFeatures.begin() + worstFeature);
        }
        
        if (bestAccuracyInCurrentSubset < lastAccuracy) { // If accuracy decrease display warning message
            cout << "(Warning, Accuracy has decreased! Continuing search in case of local maxima)" << endl;
        }

        lastAccuracy = bestAccuracyInCurrentSubset; // Update last accuracy for next comparison

        cout << "Feature set {";
        for (size_t j = 0; j < selectedFeatures.size(); j++) {
            cout << selectedFeatures[j];
            if (j != selectedFeatures.size() - 1) cout << ",";
        }
        cout << "} was best, accuracy is " << bestAccuracyInCurrentSubset << "%" << endl;
    }

    cout << "Finished search!! The best feature subset is {";
    for (size_t j = 0; j < bestFeatureSetOverall.size(); j++) {
        cout << bestFeatureSetOverall[j];
        if (j != bestFeatureSetOverall.size() - 1) cout << ",";
    }
    cout << "}, which has an accuracy of " << bestAccuracyOverall << "%" << endl;
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

    //Prompting user for input
    cout << "Type the number of the algorithm you want to run." << endl;
    cout << "1) Forward Selection" << endl;
    cout << "2) Backward Elimination" << endl;
    cin >> choice;

    using namespace std::chrono;
    high_resolution_clock::time_point start_time, end_time;  // Declare timing points

    // Cases for selection/elimination algo
    switch(choice) {
        case 1:
            start_time = high_resolution_clock::now();
            forwardSelection(data);
            end_time = high_resolution_clock::now();
            cout << "Forward Selection took " << duration_cast<milliseconds>(end_time - start_time).count() << " milliseconds.\n";
            break;
        case 2:
            start_time = high_resolution_clock::now();
            backwardElimination(data);
            end_time = high_resolution_clock::now();
            cout << "Backward Elimination took " << duration_cast<milliseconds>(end_time - start_time).count() << " milliseconds.\n";
            break;
        default:
            cout << "Invalid choice!" << endl;
            break;
    }



    return 0;
}