#include "data_io.h"
#include <iostream>
#include <fstream>
#include <random>
#include <sstream>
#include <iomanip>
#include <cerrno>
#include <cstring>
#include <filesystem>

#include "geometry.h"

using namespace std;
namespace fs = std::filesystem;

vector<Point> generate_random_points(int n) {
    vector<Point> points;
    points.reserve(n); // preallocate memory to avoid resizing

    // initialize Mersenne Twister random number generator
    random_device rd;
    mt19937 gen(rd());

    uniform_real_distribution<> dis(0, 1);

    for (int i = 0; i < n; ++i) {
        points.push_back({dis(gen), dis(gen)});
    }

    return points;
}

vector<Point> read_csv(const string& filename) {
    vector<Point> points;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not open the file " << filename << endl;
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string val;
        Point p;

        if (getline(ss, val, ',')) p.x = stod(val);
        if (getline(ss, val, ',')) p.y = stod(val);

        points.push_back(p);
    }

    file.close();
    cout << "Loaded " << points.size() << " points from " << filename << endl;
    return points;
}

void save_distances(const string& directory, const string& filename, const vector<double>& distances) {
    try {
        if (!fs::exists(directory)) {
            fs::create_directory(directory);
        }
    }
    catch (const fs::filesystem_error& e) {
        cerr << "error creating directory: " << e.what() << endl;
        return;
    }

    fs::path full_path = fs::path(directory) / filename;

    ofstream file(full_path);

    if (!file.is_open()) {
        cerr << "error: could not create output file " << full_path << endl;
        cerr << "reason: " << strerror(errno) << endl;
        return;
    }

    file << fixed << setprecision(6);

    for (double d : distances) {
        file << d << "\n";
    }

    file.close();
}

void print_summary(double avg_nearest, double avg_furthest, int num_threads) {
    cout << "\n---------------------------------------------------------------" << endl;
    cout << "processing complete using " << num_threads << "threads" << endl;
    cout << fixed << setprecision(6);
    cout << "average distance to nearest neighbour: " << avg_nearest << endl;
    cout << "average distance to furtherest neighbour: " << avg_furthest << endl;
    cout << "----------------------------------------------------------------" << endl;
}