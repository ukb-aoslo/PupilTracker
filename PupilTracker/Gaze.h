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

	std::vector<coords>Pupil;
	std::vector<coords>GazePX;
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
	void addPupilCenter(double x, double y);
	void addPupilDia(double);
	void addGazePX(double x, double y);
	void addFrozenPupil(double x, double y);
	void addTimeStamp(SYSTEMTIME t);
	void stop();
	void Save();
	void Paint(CDC* dc);
	bool record;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


