#pragma once
class Settings
{

public:

	BYTE box_size;
	BYTE spot_size;						// minimal size of spot to be accepted
	BYTE threshold;						// detection threshold
	bool blob_detect;					// consider blobs (yes/no)
	
public:
	Settings(
		int threshold,
		int spot_size,
		int box_size,
		bool blob_detect);

	Settings();
	~Settings();

};