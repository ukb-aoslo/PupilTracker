// Graph
#pragma once
#include "Gaze.h"

class Graph : public CStatic
{
	DECLARE_DYNAMIC(Graph)

public:
	Graph();
	Graph(Gaze* g);
	virtual ~Graph();

private:

	Gaze* m_pGaze;
	int frame_count;
	void Paint(CDC* dc);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
