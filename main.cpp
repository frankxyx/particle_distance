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

void analyze_geometry_serial(const vector<Point>& points, const string& label, double (*dist_func)(const Point&, const Point&)) {
    int n = static_cast<int>(points.size());

    vector<double> nearest_dists(n);
    vector<double> furthest_dists(n);

    double total_nearest = 0.0;
    double total_furthest = 0.0;

    cout << "start serial (single threaded) analyzing: " << label << "geometry..." << endl;
    double start_time = omp_get_wtime();
    int actual_threads = omp_get_num_threads();

    for (int i = 0; i < n; ++i) {
        double min_d = numeric_limits<double>::max();
        double max_d = 0.0;

        for (int j = 0; j < n; ++j) {
            if (i == j) continue;

            double d = dist_func(points[i], points[j]);

            if (d < min_d) min_d = d;
            if (d > max_d) max_d = d;
        }
        nearest_dists[i] = min_d;
        furthest_dists[i] = max_d;

        // shouldn't be any race condition here since only one thread should exist
        total_nearest += min_d;
        total_furthest += max_d;
    }

    double end_time = omp_get_wtime();
    double avg_nearest = total_nearest / n;
    double avg_furthest = total_furthest / n;

    print_summary(avg_nearest, avg_furthest, actual_threads);
    cout << "time taken: " << (end_time - start_time) << " seconds" << endl;

    save_distances("data", "nearest_" + label + "_serial.txt", nearest_dists);
    save_distances("data", "furthest_" + label + "_serial.txt", furthest_dists);
}

void analyze_geometry_standard(const vector<Point>& points, const string& label, double (*dist_func)(const Point&, const Point&)) {
    int n = static_cast<int>(points.size());

    vector<double> nearest_dists(n);
    vector<double> furthest_dists(n);

    double total_nearest = 0.0;
    double total_furthest = 0.0;

    cout << "start standard paralleled analyzing: " << label << "geometry..." << endl;
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

    save_distances("data", "nearest_" + label + ".txt", nearest_dists);
    save_distances("data", "furthest_" + label + ".txt", furthest_dists);
}

void analyze_geometry_symmetric(const vector<Point>& points, const string& label, double (*dist_func)(const Point&, const Point&)) {
    int n = static_cast<int>(points.size());

    vector<double> nearest_dists(n, numeric_limits<double>::max());
    vector<double> furthest_dists(n, -1.0);

    vector<omp_lock_t> locks(n);
    for (int i = 0; i < n; ++i) {
        omp_init_lock(&locks[i]);
    }

    cout << "start symmetric parallelled analyzing for " << label << "geometry..." << endl;
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

    save_distances("data", "nearest_" + label + "_sym.txt", nearest_dists);
    save_distances("data", "furthest_" + label + "_sym.txt", furthest_dists);
}

void analyze_geometry_optimal(const vector<Point>& points, const string& label, double (*dist_func)(const Point&, const Point&)) {
    int n = static_cast<int>(points.size());

    vector<double> nearest_dists(n, numeric_limits<double>::max());
    vector<double> furthest_dists(n, -1.0);

    vector<omp_lock_t> locks(n);
    for (int i = 0; i < n; ++i) {
        omp_init_lock(&locks[i]);
    }

    cout << "start symmetric (smart skipping) analyzing " << label << "geometry..." << endl;
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

    save_distances("data", "nearest_" + label + "_opt.txt", nearest_dists);
    save_distances("data", "furthest_" + label + "_opt.txt", furthest_dists);
}

int main(int argc, char * argv[]) {
    // code for generating random points
    /*int N = 10000;
    cout << "generating " << N << "random points..." << endl;
    vector<Point> points = generate_random_points(N);
*/

    //this takes a long time to run, use a smaller number of points for debugging purposes
    vector<Point> points = read_csv("200000_locations.csv");

    analyze_geometry_serial(points, "standard", standard_distance);
    analyze_geometry_serial(points, "wraparound", wraparound_distance);

    analyze_geometry_standard(points, "standard", standard_distance);
    analyze_geometry_standard(points, "wraparound", wraparound_distance);

    analyze_geometry_symmetric(points, "standard", standard_distance);
    analyze_geometry_symmetric(points, "wraparound", wraparound_distance);

    analyze_geometry_optimal(points, "standard", standard_distance);
    analyze_geometry_optimal(points, "wraparound", wraparound_distance);
    return 0;
}