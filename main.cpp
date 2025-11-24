#include <iostream>
#include <vector>
#include <limits>
#include <omp.h>
#include <string>

#include "geometry.h"
#include "data_io.h"

using namespace std;

// takes a function pointer 'dist_func' so we can pass either
// standard_distance or wraparound_distance dynamically.

void analyze_geometry(const vector<point>& points, string label, double (*dist_func)(const point&, const point&)) {
    int n = points.size();

    vector<double> nearest_dists(n);
    vector<double> furthest_dists(n);

    double total_nearest = 0.0;
    double total_furthest = 0.0;

    cout << "start analyzing for: " << label << " geometry..." << endl;
    double start_time = omp_get_wtime();

#pragma omp parallel for default(none) \
        shared(points, nearest_dists, furthest_dists, n, dist_func) \
        reduction(+:total_nearest, total_furthest) \
        schedule(static)
    for (int i = 0; i < n; ++i) {
        double min = numeric_limits<double>::max();
        double max = 0.0;

        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            double d = dist_func(points[i], points[j]);

            if (d < min) min = d;
            if (d > max) max = d;
        }

        nearest_dists[i] = min;
        furthest_dists[i] = max;

        total_nearest += min;
        total_furthest += max;
    }

    double end_time = omp_get_wtime();
    double avg_nearest = total_nearest / n;
    double avg_furthest = total_furthest / n;

    print_summary(avg_nearest, avg_furthest, omp_get_max_threads());
    cout << "time taken: " << (end_time - start_time) << " seconds." << endl;

    save_distances("nearest_" + label + ".txt", nearest_dists);
    save_distances("furthest_" + label + ".txt", furthest_dists);
    cout << "saved output files for " << label << ".\n" << endl;
}

int main(int argc, char * argv[]) {
    /*int N = 10000;
    cout << "generating " << N << "random points..." << endl;
    vector<point> points = generate_random_points(N);
*/

    vector<point> points = read_csv("200000_locations.csv");


    analyze_geometry(points, "standard", standard_distance);
    analyze_geometry(points, "wraparound", wraparound_distance);
    return 0;
}