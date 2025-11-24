#include "geometry.h"
#include <cmath>
#include <algorithm>
using namespace std;

double standard_distance(const point &p1, const point &p2) {
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;

    return sqrt((dx*dx) + (dy* dy));
}

static double wraparound_diff(double c1, double c2) {
    double dist = abs(c1 - c2);
    if (dist > 0.5) {
        return 1.0 - dist;
    }
    return dist;
}
double wraparound_distance(const point &p1, const point &p2) {
    double dx = wraparound_diff(p1.x, p2.x);
    double dy = wraparound_diff(p1.y, p2.y);
    return sqrt((dx*dx) + (dy* dy));
}