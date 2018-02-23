#pragma once
#include "stdafx.h"
#include "listener.h"
#include "Schaeffel.h"
#include "Gaze.h"
#include "Graph.h"

// container class for having different pupil tracking methods, where each has its own listener, gaze and graph


class Method : public std::vector <Method>
{
public:
	Method();
	Method(const Method &obj);
	~Method();

	void setActive(CString name);
	CString getActive();
	
	void*										getListener();
	Graph*										getGraph();
	Gaze*										getGaze();
	void refresh();

protected:
	
	CString										name;
	void*										listener;
	Gaze*										gaze;
	Graph*										graph;
	bool										active;

};
