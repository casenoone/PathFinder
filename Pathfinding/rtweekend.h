#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <cstdlib>
#include <limits>
#include <memory>


// Usings

using std::shared_ptr; //为啥要用std::这种写法？
using std::make_shared;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180;
}

inline double random_double() {
	// Returns a random real in [0,1).
	return rand() / (RAND_MAX + 1.0);
	
}

inline int random_int(double min, double max) {
	return (min + (max - min)*random_double());
}

inline double random_double(double min, double max) {
	// Returns a random real in [min,max).
	return min + (max - min)*random_double();
}

inline double clamp(double x, double min, double max) {
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

inline double rmin(const double &x, const double &y) {
	double temp;
	if (x <= y) {
		return x;
	}
	return y;
}


inline double rmax(const double &x, const double &y) {
	double temp;
	if (x > y) {
		return x;
	}
	return y;
}

// Common Headers

#include "ray.h"
#include "vector3.h"
#endif
