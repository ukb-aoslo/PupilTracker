// Graph
#pragma once
#include "Gaze.h"

class Graph : public CStatic
{
	DECLARE_DYNAMIC(Graph)

public:
	Graph();
	void freeze();
	void addPupilDia(double x);
	virtual ~Graph();

private:
	int frame_count;
	int turn;
	double frozen_pupil;
	bool frozen;
	
	LRESULT OnUpdateControl(WPARAM wParam, LPARAM lParam);
	std::vector<double>PupilDia;

	void Paint(CDC* dc);

public:

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
