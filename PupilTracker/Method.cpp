#include "stdafx.h"
#include "Method.h"

Method::Method()
{
	// the Schaeffel method
	name = L"Schaeffel";
	gaze = new Gaze;
	graph = new Graph;
	listener = new Schaeffel(gaze, graph);
	push_back(*this);

	// placeholder for other methods
	// ..

}

Method::Method(const Method &obj) 
{ 
	name = obj.name;
	gaze = obj.gaze;
	graph = obj.graph;
	listener = obj.listener;
}

Method::~Method()
{
	for (auto it = this->begin(); it != this->end(); it++) {
		delete it->gaze;
		delete it->graph;
		if (it->name == "Schaeffel") {
			Schaeffel* s = (Schaeffel*) getListener();
			s->~Schaeffel();
		}
		delete listener;
	}
}

void Method::setActive(CString name) {
	for (auto it = this->begin(); it != this->end(); ++it) {
		if (it->name == name)
			it->active = true;
		else
			it->active = false;
	}
}

void* Method::getListener()
{
	for (auto it = this->begin(); it != this->end(); ++it) {
		if (it->active)
			return it->listener;
	}

	return nullptr;
}

Gaze* Method::getGaze()
{
	for (auto it = this->begin(); it != this->end(); ++it) {
		if (it->active)
			return it->gaze;
	}

	return nullptr;
}

Graph* Method::getGraph()
{
	for (auto it = this->begin(); it != this->end(); ++it) {
		if (it->active)
			return it->graph;
	}
	
	return nullptr;
};