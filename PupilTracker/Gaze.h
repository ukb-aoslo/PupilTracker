#pragma once
// Gaze

class Gaze : public CStatic
{
	DECLARE_DYNAMIC(Gaze)

private:

	struct coords {
		double x;
		double y;
	};

	double magnif;

	std::vector<coords>Pupil;
	std::vector<coords>GazePX;
	std::vector<coords>GazeMM;
	std::vector<coords>FrozenPupil;
	std::vector<SYSTEMTIME>TimeStamps;
	std::vector<double>PupilDia;

	float conv;
	bool ready;
	
	coords current, offset, locked;

	LRESULT Gaze::OnUpdateControl(WPARAM wParam, LPARAM lParam);

public:

	Gaze();
	~Gaze();
	
	bool record;
	int recIndex;

	void addPupilCenter(double x, double y);
	void addPupilDia(double);
	void addGaze(double x, double y);
	void addFrozenPupil(double x, double y);
	void addTimeStamp(SYSTEMTIME t);
	void setMagnif(double m);
	void stop();
	void Save();
	void Paint(CDC* dc);

	int getGazePxSize() {
		return GazePX.size();
	};

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


