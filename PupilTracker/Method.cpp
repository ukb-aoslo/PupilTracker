#include "stdafx.h"
#include "Method.h"

Method::Method()
{
	// the Schaeffel method

	name = L"Schaeffel";
	gaze = new Gaze;
	graph = new Graph(gaze);
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

}

void Method::setActive(CString name) {
	for (std::vector<Method>::iterator it = this->begin(); it != this->end(); ++it) {
		if (it->name == name)
			it->active = true;
		else
			it->active = false;
	}
}

void* Method::getListener()
{
	for (std::vector<Method>::iterator it = this->begin(); it != this->end(); ++it) {
		if (it->active)
			return it->listener;
		else
			return nullptr;
	}
}

Gaze* Method::getGaze()
{
	for (std::vector<Method>::iterator it = this->begin(); it != this->end(); ++it) {
		if (it->active)
			return it->gaze;
		else
			return nullptr;
	}
}

Graph* Method::getGraph()
{
	for (std::vector<Method>::iterator it = this->begin(); it != this->end(); ++it) {
		if (it->active)
			return it->graph;
		else
			return nullptr;
	}
};