#pragma once
#include "stdafx.h"
#include "Tracker.h"
#include "PupilTrackerMainFrame.h"

Tracker::Tracker() {

	display = 0;
	frames = 0;

	bufchange = false;

	magnif = MM_PER_PIXEL;	    // magnification in pixel per mm

}

Tracker::~Tracker() {



}


void Tracker::DoFurtherProcessing(smart_ptr<MemBuffer> pBuffer) {

	if (bufchange) {				// buffer change?
		median_pupil.reset();
		bufchange = false;
	}

	// reduce pupil jitter if desired
	if (*buf_size > 2) reduceJitter();

	else {
		median_pupil.current_diameter = pupil.current_diameter;
		median_pupil.current_center.x = pupil.current_center.x;
		median_pupil.current_center.y = pupil.current_center.y;
	}

	// Send purkinje data to AOMControl via netcomm
	if (m_pParent->m_pSock_AOMCONTROL != NULL) {
		m_pParent->m_pSock_AOMCONTROL->coords[0] = purkinje.current_center.x;
		m_pParent->m_pSock_AOMCONTROL->coords[1] = purkinje.current_center.y;
	}

		postOffset();
		postDiameter();

}

void Tracker::postOffset() {

	// Send pupil offset to OffsetTracker Window for painting
	if (freeze) {

		offsetMM.x = (pupil.current_center.x - pupil.frozen_center.x) * magnif;
		offsetMM.y = (pupil.current_center.y - pupil.frozen_center.y) * magnif;

		m_pParent->PostMessage(MESSAGE_OFFSETMM_PROCESSED, (WPARAM)&offsetMM, 0);

		// save that offset too
		pupil.offsetMM.push_back(offsetMM);
		
	}

	else {

			m_pParent->PostMessage(MESSAGE_OFFSETMM_PROCESSED, 0, 0);
			// save that offset too
			pupil.offsetMM.push_back(coords<double, double> {0, 0});
	
	}


}

void Tracker::postDiameter() {

	if (m_pParent != NULL)
		m_pParent->PostMessage(MESSAGE_PUPILDIA_PROCESSED, pupil.current_diameter, 0);

}



void Tracker::overlayCallback(Grabber& caller, smart_ptr<OverlayBitmap> pBitmap, const tsMediaSampleDesc& MediaSampleDesc)
{

	DShowLib::Error e;
	e = caller.getLastError();
	if (e.isError())
		TRACE("Error: %s\n", e.getString().c_str());

	// clear screen
	pBitmap->fill(RGB(0, 0, 0));
	
	// frames counted
	frames++;

	// for text display
	CString szText;

	if (pBitmap->getEnable() == true) // Draw only, if the overlay bitmap is enabled.
	{

		/*************************************************************************************/
		/*****   this is now the place where pixels are marked and all data are displayed ****/
		/*************************************************************************************/

		if (*opts & FrameCounter)
		{
			szText.Format(TEXT(" Frame#: %d"), frames);
			pBitmap->drawText(RGB(255, 255, 0), 12, 12, LPCTSTR(szText));
		}

		if (*opts & FPS)
		{
			szText.Format(TEXT("FPS: %.2f"), caller.getCurrentActualFPS());
			pBitmap->drawText(RGB(255, 255, 0), 570, 12, LPCTSTR(szText));
		}


		// draw yellow box in life video to show the range in which the pupil should be

		if (*opts & BoxBoundary) {
			pBitmap->drawFrameRect(RGB(0, 150, 0), CRect(pupil_settings->box_size,
				pupil_settings->box_size,
				Width - pupil_settings->box_size,
				Height - pupil_settings->box_size));

			pBitmap->drawFrameRect(RGB(150, 0, 0), CRect(purkinje_settings->box_size,
				purkinje_settings->box_size,
				Width - purkinje_settings->box_size,
				Height - purkinje_settings->box_size));
		}

		// pixels supposed to be in the pupil

		if (*opts & PupilPixels)
				for (size_t i = 0; i < 1000; i++)
					if (pupil.pixels[i].x < Width && pupil.pixels[i].y < Height)
					pBitmap->drawLine(RGB(255, 0, 255), 
						pupil.pixels[i].x - 1,
						Height - pupil.pixels[i].y - 1,
						pupil.pixels[i].x + 1,
						Height - pupil.pixels[i].y + 1
				);
				
		// draw pupil outline

		pBitmap->drawFrameEllipse(RGB(0, 255, 0), CRect((int)median_pupil.current_center.x - (int)median_pupil.current_diameter / 2,
			Height - (int)median_pupil.current_center.y - (int)median_pupil.current_diameter / 2,
			(int)median_pupil.current_center.x + (int)median_pupil.current_diameter / 2,
			Height - (int)median_pupil.current_center.y + (int)median_pupil.current_diameter / 2));

		// draw purkinje outline

		pBitmap->drawFrameEllipse(RGB(255, 0, 0), CRect(
			(int)purkinje.current_center.x - (int)purkinje.current_diameter / 2,
			Height - (int)purkinje.current_center.y - (int)purkinje.current_diameter / 2,
			(int)purkinje.current_center.x + (int)purkinje.current_diameter / 2,
			Height - (int)purkinje.current_center.y + (int)purkinje.current_diameter / 2));

		// draw pupil center

		pBitmap->drawLine(RGB(0, 255, 0),
			(int)median_pupil.current_center.x,
			Height - (int)median_pupil.current_center.y - 1,
			(int)median_pupil.current_center.x,
			Height - (int)median_pupil.current_center.y + 2);

		pBitmap->drawLine(RGB(0, 255, 0),
			(int)median_pupil.current_center.x - 1,
			Height - (int)median_pupil.current_center.y,
			(int)median_pupil.current_center.x + 2,
			Height - (int)median_pupil.current_center.y);

		// draw purkinje center

		pBitmap->drawLine(RGB(255, 0, 0),
			(int)purkinje.current_center.x,
			Height - (int)purkinje.current_center.y - 1,
			(int)purkinje.current_center.x,
			Height - (int)purkinje.current_center.y + 2);

		pBitmap->drawLine(RGB(255, 0, 0),
			(int)purkinje.current_center.x - 1,
			Height - (int)purkinje.current_center.y,
			(int)purkinje.current_center.x + 2,
			Height - (int)purkinje.current_center.y);


		if (freeze) {

			// draw pupil outline

			if (pupil.frozen_diameter > 0)
			{
				pBitmap->drawFrameEllipse(RGB(255, 255, 0), CRect(
					(int)pupil.frozen_center.x - (int)pupil.frozen_diameter / 2,
					Height - (int)pupil.frozen_center.y - (int)pupil.frozen_diameter / 2,
					(int)pupil.frozen_center.x + (int)pupil.frozen_diameter / 2,
					Height - (int)pupil.frozen_center.y + (int)pupil.frozen_diameter / 2));
			}

			// draw pupil center

			if (pupil.frozen_center.x > 0)
			{
				pBitmap->drawLine(RGB(155, 152, 254),
					(int)pupil.frozen_center.x,
					Height - (int)pupil.frozen_center.y - 1,
					(int)pupil.frozen_center.x,
					Height - (int)pupil.frozen_center.y + 2);

				pBitmap->drawLine(RGB(155, 152, 254),
					(int)pupil.frozen_center.x - 1,
					Height - (int)pupil.frozen_center.y,
					(int)pupil.frozen_center.x + 2,
					Height - (int)pupil.frozen_center.y);
			}

		}

	}

	
	if (record == true) {
		if (display < 20) {
			szText = "REC";
			pBitmap->drawText(RGB(254, 152, 0), Width - 40, 37, LPCTSTR(szText));
			pBitmap->drawSolidEllipse(RGB(204, 96, 97), CRect(Width - 55, 40, Width - 44, 51));
			display++;
		}
		if (frames % 40 == 0)
				display = 0;
	}

}

void Tracker::reduceJitter() {
	
	while (median_pupil.center.size() < *buf_size) {
		median_pupil.center.push_back(pupil.current_center);
		median_pupil.diameter.push_back(pupil.current_diameter);
	}

	median_pupil.center.erase(median_pupil.center.begin());
	median_pupil.center.push_back(pupil.current_center);
	median_pupil.diameter.erase(median_pupil.diameter.begin());
	median_pupil.diameter.push_back(pupil.current_diameter);

	median_pupil = getMedian(median_pupil);

}

Pupil Tracker::getMedian(Pupil p)
{
	Pupil result = p;

	size_t size = p.center.size();
	
	// sort coordinates (can't use std::sort here)
	for (auto ita : p.center) {
		for (auto itb : p.center) {
			if (ita.x > itb.x)
				std::swap(ita.x, itb.x);
			if (ita.y > itb.y)
				std::swap(ita.y, itb.y);
		}
	}
		
	std::sort(p.diameter.begin(), p.diameter.end());

	if (size % 2 == 0)
	{
		result.current_center.x = ((p.center[size / 2 - 1].x + p.center[size / 2].x) / 2);
		result.current_center.y = ((p.center[size / 2 - 1].y + p.center[size / 2].y) / 2);
		result.current_diameter = ((p.diameter[size / 2 - 1] + p.diameter[size / 2]) / 2);
	}

	else
	{
		result.current_center.x = p.center[size / 2].x;
		result.current_center.y = p.center[size / 2].y;
		result.current_diameter = p.diameter[size / 2];
	}

	return result;

}