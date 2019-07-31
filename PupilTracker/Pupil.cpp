#include "stdafx.h"
#include "Pupil.h"


Pupil::Pupil() :
	current_diameter(0),
	frozen_diameter(0),
	current_center(coords<double, double> { 0, 0 }),
	frozen_center(coords<double, double> { 0, 0 }),
	pxdim(0)
{
}


Pupil::~Pupil()
{
}

void Pupil::saveOffset(coords<double, double> &offset) {

	coords<double, double> conv{ MM_PER_PIXEL, MM_PER_PIXEL };
	offsetMM.push_back(offset * conv);
	offsetPX.push_back(offset);

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