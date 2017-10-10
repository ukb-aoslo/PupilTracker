// Graph
#pragma once
#include "Gaze.h"

class Graph : public CStatic
{
	DECLARE_DYNAMIC(Graph)

public:
	Graph();
	void freeze();
	void addPupilDia(float x);
	virtual ~Graph();

private:
	int frame_count;
	int turn;
	float frozen_pupil;
	bool frozen;

	std::vector<float>PupilDia;
	void Paint(CDC* dc);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
