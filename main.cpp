#include <iostream>
#include <vector>
#include <limits>
#include <omp.h>
#include <string>
#include <iomanip>

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

    cout << "start analyzing for: " << label << "geometry..." << endl;
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
    cout << "saved output files for " << label << "\n" << endl;
}

void analyze_geometry_symmetric(const vector<point>& points, string label, double (*dist_func)(const point&, const point&)) {
    int n = points.size();

    vector<double> nearest_dists(n, numeric_limits<double>::max());
    vector<double> furthest_dists(n, -1.0);

    vector<omp_lock_t> locks(n);
    for (int i = 0; i < n; ++i) {
        omp_init_lock(&locks[i]);
    }

    cout << "start symmetric analyzing for " << label << "geometry..." << endl;
    double start_time = omp_get_wtime();

    // scheduling dynamically is often better in this case since the inner loop gets shorter as i increase
    #pragma omp parallel for default(none) \
            shared(points, nearest_dists, furthest_dists, n, dist_func, locks, cout) \
            schedule(dynamic)
    for (int i = 0; i < n; ++i) {
        // loop from i+1 to N (the upper triangle)

        for (int j = i + 1; j < n; ++j) {
            double d = dist_func(points[i], points[j]);

            if (d < nearest_dists[i]) nearest_dists[i] = d;
            if (d > furthest_dists[i]) furthest_dists[i] = d;

            omp_set_lock(&locks[j]);
            if (d < nearest_dists[j]) nearest_dists[j] = d;
            if (d > furthest_dists[j]) furthest_dists[j] = d;
            omp_unset_lock(&locks[j]);
        }
    }
    double end_time = omp_get_wtime();

    //clean up locks
    for (int i = 0; i < n; ++i) {
        omp_destroy_lock(&locks[i]);
    }

    double total_nearest = 0.0;
    double total_furthest = 0.0;

    for (int i = 0; i < n; ++i) {
        total_nearest += nearest_dists[i];
        total_furthest += furthest_dists[i];
    }

    double avg_nearest = total_nearest / n;
    double avg_furthest = total_furthest / n;

    print_summary(avg_nearest, avg_furthest, omp_get_max_threads());
    cout << "time taken: " << (end_time - start_time) << endl;

    save_distances("nearest_" + label + "_sym.txt", nearest_dists);
    save_distances("furthest_" + label + "_sym.txt", furthest_dists);
    cout << "saved output files for " << label << "_sym" << "\n" << endl;
}

void analyze_geometry_optimal(const vector<point>& points, string label, double (*dist_func)(const point&, const point&)) {
    int n = points.size();

    vector<double> nearest_dists(n, numeric_limits<double>::max());
    vector<double> furthest_dists(n, -1.0);

    vector<omp_lock_t> locks(n);
    for (int i = 0; i < n; ++i) {
        omp_init_lock(&locks[i]);
    }

    cout << "start optimal analyzing for " << label << "geometry..." << endl;
    double start_time = omp_get_wtime();

    // scheduling dynamically is often better in this case since the inner loop gets shorter as i increase
    #pragma omp parallel for default(none) \
            shared(points, nearest_dists, furthest_dists, n, dist_func, locks, cout) \
            schedule(dynamic)
    for (int i = 0; i < n; ++i) {
        // loop from i+1 to N (the upper triangle)

        for (int j = i + 1; j < n; ++j) {
            double d = dist_func(points[i], points[j]);

            if (d < nearest_dists[i]) nearest_dists[i] = d;
            if (d > furthest_dists[i]) furthest_dists[i] = d;

            if (d < nearest_dists[j] || d > furthest_dists[j]) {
                omp_set_lock(&locks[j]);
                if (d < nearest_dists[j]) nearest_dists[j] = d;
                if (d > furthest_dists[j]) furthest_dists[j] = d;
                omp_unset_lock(&locks[j]);
            }
        }
    }
    double end_time = omp_get_wtime();

    //clean up locks
    for (int i = 0; i < n; ++i) {
        omp_destroy_lock(&locks[i]);
    }

    double total_nearest = 0.0;
    double total_furthest = 0.0;

    for (int i = 0; i < n; ++i) {
        total_nearest += nearest_dists[i];
        total_furthest += furthest_dists[i];
    }

    double avg_nearest = total_nearest / n;
    double avg_furthest = total_furthest / n;

    print_summary(avg_nearest, avg_furthest, omp_get_max_threads());
    cout << "time taken: " << (end_time - start_time) << endl;

    save_distances("nearest_" + label + "_opt.txt", nearest_dists);
    save_distances("furthest_" + label + "_opt.txt", furthest_dists);
    cout << "saved output files for " << label << "_opt" << "\n" << endl;
}

int main(int argc, char * argv[]) {
    /*int N = 10000;
    cout << "generating " << N << "random points..." << endl;
    vector<point> points = generate_random_points(N);
*/

    vector<point> points = read_csv("200000_locations.csv");


    analyze_geometry(points, "standard", standard_distance);
    analyze_geometry(points, "wraparound", wraparound_distance);

    analyze_geometry_symmetric(points, "standard", standard_distance);
    analyze_geometry_symmetric(points, "wraparound", wraparound_distance);

    analyze_geometry_optimal(points, "standard", standard_distance);
    analyze_geometry_optimal(points, "wraparound", wraparound_distance);
    return 0;
}