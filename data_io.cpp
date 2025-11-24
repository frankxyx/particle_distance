#include "data_io.h"
#include <iostream>
#include <fstream>
#include <random>
#include <sstream>
#include <iomanip>
#include <cerrno>
#include <cstring>

#include "geometry.h"

using namespace std;

vector<point> generate_random_points(int n) {
    vector<point> points;
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

vector<point> read_csv(const string& filename) {
    vector<point> points;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not open the file " << filename << endl;
        exit(1);
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string val;
        point p;

        if (getline(ss, val, ',')) p.x = stod(val);
        if (getline(ss, val, ',')) p.y = stod(val);

        points.push_back(p);
    }

    file.close();
    cout << "Loaded " << points.size() << " points from " << filename << endl;
    return points;
}

void save_distances(const string& filename, const vector<double>& distances) {
    ofstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not create output file " << filename << endl;
        return;
    }

    file << fixed << setprecision(6);

    for (double d : distances) {
        file << d << "\n";
    }
    file.close();
}

void print_summary(double avg_nearest, double avg_furthest, int num_threads) {
    cout << "---------------------------------------------------------------" << endl;
    cout << "processing complete using " << num_threads << "threads" << endl;
    cout << fixed << setprecision(6);
    cout << "average distance to nearest neighbour: " << avg_nearest << endl;
    cout << "average distance to furtherest neighbour: " << avg_furthest << endl;
    cout << "----------------------------------------------------------------" << endl;
}