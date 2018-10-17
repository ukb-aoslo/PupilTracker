#include "stdafx.h"
#include "Pupil.h"


Pupil::Pupil() :current_diameter(0),
				frozen_diameter(0),
				current_center(coords<double, double> { 0, 0 }),
				frozen_center(coords<double, double> { 0, 0 })
{
}


Pupil::~Pupil()
{
}

void Pupil::reset() {

	current_diameter = 0;
	current_center.x = 0;
	current_center.y = 0;
	frozen_diameter = 0;
	frozen_center.x = 0;
	frozen_center.y = 0;
	center.clear();
	diameter.clear();
	offsetMM.clear();
	offsetPX.clear();

}