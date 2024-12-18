#pragma once
#include "stdafx.h"
#include "Tracker.h"
#include "PupilTrackerMainFrame.h"

Tracker::Tracker() {

	display = 0;
	frames = 0;

	threshCount = 255;

	bufchange = false;
	beam = false;
	purkinje_assist = false;


}

Tracker::~Tracker() {

}

void Tracker::DoFurtherProcessing(smart_ptr<MemBuffer> pBuffer) {

	// trigger automatic threshold calculation
	if (threshCount < 255) {
		pupil_settings->threshold = threshCount;
		threshCount++;
	}

	if (bufchange) {				// buffer change?
		median_pupil.reset();
		bufchange = false;
	}

	else {
		median_pupil.current_diameter = pupil.current_diameter;
		median_pupil.current_center.x = pupil.current_center.x;
		median_pupil.current_center.y = pupil.current_center.y;
	}

	if (freeze) {
		offset.x = (pupil.frozen_center.x - pupil.current_center.x);
		offset.y = (pupil.frozen_center.y - pupil.current_center.y);
	}

	else offset = (coords<double, double> {0, 0});

	postPupil();
	postPurkinje();

	// save that offset too
	pupil.saveOffset(offset, *mm_per_pix);

	postDiameter();

	// save that diameter too
	//pupil.saveDiameter(diameter);

}



void Tracker::postPupil() {

	// Send pupil data to OffsetTracker Window for painting
	m_pParent->PostMessage(MESSAGE_PUPIL_PROCESSED, (WPARAM)&pupil.current_center, (LPARAM)&pupil.frozen_center);

}

void Tracker::postPurkinje() {

	// Send purkinje data to OffsetTracker Window for painting
	m_pParent->PostMessage(MESSAGE_PURKINJE_PROCESSED, (WPARAM)&purkinje.current_center, (LPARAM)&purkinje.frozen_center);

}

void Tracker::postDiameter() {
	
	// Send pupil diameter data to OffsetTracker Window for painting
	m_pParent->PostMessage(MESSAGE_PUPILDIA_PROCESSED, (WPARAM)&pupil.current_diameter, 0);

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

	/*************************************************************************************/
	/*****   this is now the place where pixels are marked and all data are displayed ****/
	/*************************************************************************************/

	if (pBitmap->getEnable() == true) // Draw only, if the overlay bitmap is enabled.
	{

		// paint supposed AOSLO beam if desired

		if (beam) {
		
			for (int i = 0; i < 3; i++)
				pBitmap->drawFrameEllipse(RGB(220, 20, 180), CRect(
					((int)AOSLO_beam.current_center.x - (int)AOSLO_beam.current_diameter / 2) - i,
					((int)AOSLO_beam.current_center.y - (int)AOSLO_beam.current_diameter / 2) - i,
					((int)AOSLO_beam.current_center.x + (int)AOSLO_beam.current_diameter / 2) + i,
					((int)AOSLO_beam.current_center.y + (int)AOSLO_beam.current_diameter / 2) + i));
		}

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
			pBitmap->drawFrameRect(RGB(0, 150, 0), CRect(
				pupil_settings->box_size,
				pupil_settings->box_size,
				Width - pupil_settings->box_size,
				Height - pupil_settings->box_size)
			);

			pBitmap->drawFrameRect(RGB(250, 0, 0), CRect(
				pupil.current_center.x - sqrt(purkinje.pxdim) / 2,
				pupil.current_center.y - sqrt(purkinje.pxdim) / 2,
				pupil.current_center.x + sqrt(purkinje.pxdim) / 2,
				pupil.current_center.y + sqrt(purkinje.pxdim) / 2)
			);
		}

		// pixels supposed to be in the pupil

		if (*opts & PupilPixels)
				for (size_t i = 0; i < 1000; i++)
					if (pupil.pixels[i].x < Width && pupil.pixels[i].y < Height)
						pBitmap->drawSolidEllipse(RGB(255, 0, 255), CRect(
							pupil.pixels[i].x - 2,
							Height - pupil.pixels[i].y - 2,
							pupil.pixels[i].x + 2,
							Height - pupil.pixels[i].y + 2
						)
				);

		// pixels supposed to be in the purkinje
		
		if (*opts & PupilPixels)
			for (size_t i = 0; i < purkinje.pxdim; i++)
				if (purkinje.pixels[i].x < Width && purkinje.pixels[i].y < Height)
					pBitmap->drawSolidEllipse(RGB(155, 0, 155), CRect(
						purkinje.pixels[i].x - 2,
						Height - purkinje.pixels[i].y - 2,
						purkinje.pixels[i].x + 2,
						Height - purkinje.pixels[i].y + 2
					)
				);

		// draw pupil outline

		pBitmap->drawFrameEllipse(RGB(0, 255, 0), CRect(
			median_pupil.current_center.x - median_pupil.current_diameter / 2,
			median_pupil.current_center.y - median_pupil.current_diameter / 2,
			median_pupil.current_center.x + median_pupil.current_diameter / 2,
			median_pupil.current_center.y + median_pupil.current_diameter / 2));

		// draw purkinje

		pBitmap->drawSolidEllipse(RGB(255, 0, 0), CRect(
			purkinje.current_center.x - purkinje.current_diameter / 2,
			purkinje.current_center.y - purkinje.current_diameter / 2,
			purkinje.current_center.x + purkinje.current_diameter / 2,
			purkinje.current_center.y + purkinje.current_diameter / 2));

		// draw purkinje assist

		if (purkinje_assist) {
			
			SIZE sz{ purkinje.diameter.back(), purkinje.diameter.back() };
			for (size_t i = 0; i < 4; i++)
				pBitmap->drawFrameEllipse(RGB(255, 0, 0), CRect(purkinjePoints[i], sz));

		}

		// draw pupil center

		pBitmap->drawLine(RGB(0, 255, 0),
			median_pupil.current_center.x,
			median_pupil.current_center.y - 1,
			median_pupil.current_center.x,
			median_pupil.current_center.y + 2);

		pBitmap->drawLine(RGB(0, 255, 0),
			median_pupil.current_center.x - 1,
			median_pupil.current_center.y,
			median_pupil.current_center.x + 2,
			median_pupil.current_center.y);


		if (freeze) {

			// draw pupil outline

			if (pupil.frozen_diameter > 0)
			{
				pBitmap->drawFrameEllipse(RGB(10, 10, 10), CRect(
					(int)pupil.frozen_center.x - (int)pupil.frozen_diameter / 2 - 1,
					(int)pupil.frozen_center.y - (int)pupil.frozen_diameter / 2 - 1,
					(int)pupil.frozen_center.x + (int)pupil.frozen_diameter / 2 + 1,
					(int)pupil.frozen_center.y + (int)pupil.frozen_diameter / 2 + 1));

				pBitmap->drawFrameEllipse(RGB(255, 255, 0), CRect(
					(int)pupil.frozen_center.x - (int)pupil.frozen_diameter / 2,
					(int)pupil.frozen_center.y - (int)pupil.frozen_diameter / 2,
					(int)pupil.frozen_center.x + (int)pupil.frozen_diameter / 2,
					(int)pupil.frozen_center.y + (int)pupil.frozen_diameter / 2));

				pBitmap->drawFrameEllipse(RGB(10, 10, 10), CRect(
					(int)pupil.frozen_center.x - (int)pupil.frozen_diameter / 2 + 1,
					(int)pupil.frozen_center.y - (int)pupil.frozen_diameter / 2 + 1,
					(int)pupil.frozen_center.x + (int)pupil.frozen_diameter / 2 - 1,
					(int)pupil.frozen_center.y + (int)pupil.frozen_diameter / 2 - 1));
			}

			// draw pupil center

			if (pupil.frozen_center.x > 0)
			{
				pBitmap->drawLine(RGB(155, 152, 254),
					(int)pupil.frozen_center.x,
					(int)pupil.frozen_center.y - 1,
					(int)pupil.frozen_center.x,
					(int)pupil.frozen_center.y + 2);

				pBitmap->drawLine(RGB(155, 152, 254),
					(int)pupil.frozen_center.x - 1,
					(int)pupil.frozen_center.y,
					(int)pupil.frozen_center.x + 2,
					(int)pupil.frozen_center.y);
			}

		}


	}

	
	if (record) {
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
