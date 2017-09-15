// Graph
#pragma once
#include "Gaze.h"

class Graph : public CStatic
{
	DECLARE_DYNAMIC(Graph)

public:
	Graph();
	Graph(Gaze* g);
	void freeze();
	virtual ~Graph();

private:

	Gaze* m_pGaze;
	int frame_count;
	int turn;
	int frozen_pupil;
	bool frozen;

	std::vector<int>pupilsize;
	void Paint(CDC* dc);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
