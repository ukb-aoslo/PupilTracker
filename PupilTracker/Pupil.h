#pragma once
#include <vector>
#include <deque>

template <typename T1, typename T2>

struct coords {

		T1 x;
		T2 y;

		coords<T1, T2>& operator - (coords<T1, T2> &b) {
			x -= b.x;
			y -= b.y;
			return *this;
		}

		coords<T1, T2>& operator * (coords<T1, T2> &b) {
			x *= b.x;
			y *= b.y;
			return *this;
		}

};

class Pupil {

public:

	Pupil();
	~Pupil();

	coords <int, int> *pixels;						// pixels supposed to be inside the pupil

	std::vector<coords<double, double>>	center;		// pupil center
	std::vector<double>					diameter;	// pupil diameter in mm
	
	std::vector<coords<double, double>>offsetPX;	// pupil offset in pixels
	std::vector<coords<double, double>>offsetMM;	// pupil offset in mm

	coords<double, double>	current_center;
	double					current_diameter;

	coords<double, double>	frozen_center;			// locked position
	double					frozen_diameter;

	void reset();
	void saveOffset(coords<double, double> &offset);

};