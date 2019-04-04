#include "stdafx.h"
#include "Settings.h"


Settings::Settings(int threshold, int spot_size, int box_size, bool blob_detect) :
	threshold(threshold),
	spot_size(spot_size),
	box_size(box_size),
	blob_detect(blob_detect)
{
}

Settings::Settings() :
	threshold(0),
	spot_size(0),
	box_size(0),
	blob_detect(false)
{
}

Settings::~Settings()
{
}
