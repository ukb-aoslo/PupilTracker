#pragma once
#include <vector>

template <typename T1, typename T2>

struct coords {

		T1 x;
		T2 y;

};

class Pupil {

public:

	Pupil();
	~Pupil();

	//std::vector<coords<int, int>>		pixels;		// pixels supposed to be inside the pupil
	coords <int, int> *pixels;

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