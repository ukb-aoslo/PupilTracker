#include "stdafx.h"
#include "Settings.h"


Settings::Settings(int threshold,  int spot_size, int box_size) : threshold(threshold),
spot_size(spot_size), box_size(box_size)
{
}

Settings::Settings() : threshold(0), spot_size(0), box_size(0) {
}

Settings::~Settings()
{
}
