#ifndef GEOMETRY_H
#define GEOMETRY_H

struct Point {
    double x, y;
};

double standard_distance(const Point &p1, const Point &p2);
double wraparound_distance(const Point &p1, const Point &p2);
#endif
