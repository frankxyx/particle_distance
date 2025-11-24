#ifndef DATA_IO_H
#define DATA_IO_H

#include <vector>
#include <string>
#include "geometry.h"

using namespace std;

vector<point> generate_random_points(int n);
vector<point> read_csv(const string& filename);

void save_distances(const string& filename, const vector<double> & distances);
void print_summary(double avg_nearest, double avg_furthest, int num_threads);

#endif
