#pragma once
#include <vector>

template <typename T1, typename T2>

struct coords {

		T1 x;
		T2 y;

};

template<typename T1, typename T2>
coords<double, double> operator - (T1 a, const T2& b) {
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

class Pupil {

public:

	Pupil();
	~Pupil();

	coords <int, int> *pixels;						// pixels supposed to be inside the pupil

	std::vector<coords<double, double>>	center;		// pupil center
	std::vector<double>					diameter;	// pupil diameter


	std::vector<coords<double, double>>offsetPX;	// pupil offset in pixels
	std::vector<coords<double, double>>offsetMM;	// pupil offset in mm

	coords<double, double>	current_center;
	double					current_diameter;

	coords<double, double>	frozen_center;			// locked position
	double					frozen_diameter;

	void reset();

};