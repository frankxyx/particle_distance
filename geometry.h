#ifndef GEOMETRY_H
#define GEOMETRY_H

struct point {
    double x, y;
};

double standard_distance(const point &p1, const point &p2);
double wraparound_distance(const point &p1, const point &p2);
#endif
