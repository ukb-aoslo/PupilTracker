#include "stdafx.h"
#include "Pupil.h"


Pupil::Pupil() :current_diameter(0),
				frozen_diameter(0)
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
	//pixels.clear();
	free(pixels);
	center.clear();

}