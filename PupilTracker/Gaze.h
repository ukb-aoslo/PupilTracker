#pragma once
#include "concurrent_vector.h"
// Gaze

class Gaze : public CStatic
{
	DECLARE_DYNAMIC(Gaze)

private:

	struct coords {
		float x;
		float y;
	};

	std::vector<coords>Pupil;
	std::vector<coords>GazePX;
	std::vector<coords>FrozenPupil;
	std::vector<float>PupilDia;

	float conv;

public:
	Gaze();
	~Gaze();
	void addPupilCenter(float x, float y);
	void addPupilDia(float);
	void addGazePX(float x, float y);
	void addFrozenPupil(float x, float y);
	void stop();
	void Save();
	void Paint(CDC* dc);

	bool record;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


