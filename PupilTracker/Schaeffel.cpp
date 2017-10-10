#include "stdafx.h"
#include "Schaeffel.h"
#include <mutex>

Schaeffel::Schaeffel(Gaze* gz, Graph* gr)
{
	m_pGaze = gz;
	m_pGraph = gr;
	frozen = false;
	threshold = 50;
	box_size = 130;
	magnif = (float)15.5114;	    // magnification pixel per mm for helmet gaze tracker Oct 26, 2005
									// old magnif was 26.8
	opts = BoxBoundary | PupilPixels;
}


Schaeffel::~Schaeffel()
{
	
}

void Schaeffel::frameReady(Grabber& param, smart_ptr<MemBuffer> pBuffer, DWORD FrameNumber)
{

	DoImageProcessing(pBuffer);
	m_pGaze->RedrawWindow();
	m_pGraph->RedrawWindow();

}

void Schaeffel::DoImageProcessing(smart_ptr<MemBuffer> pBuffer)
{
	//// Get the bitmap info header from the membuffer. It contains the bits per pixel,
	//// width and height.
	smart_ptr<BITMAPINFOHEADER> pInf = pBuffer->getBitmapInfoHeader();

	//// Now retrieve a pointer to the image. For organization of the image data, please
	//// refer to:
	//// http://www.imagingcontrol.com/ic/docs/html/class/Pixelformat.htm

	BYTE* pImageData = pBuffer->getPtr();

	//// Calculate the size of the image.
	int iImageSize = pInf->biWidth * pInf->biHeight * pInf->biBitCount / 8;

	// make negative image for black pupil tracker
	//for (int i = 0; i < iImageSize; i++)
	//{
	//	//pImageData[i] = 255 - pImageData[i];
	//	if (pImageData[i] > 133) pImageData[i] = 255;
	//}
	
	Width = pInf->biWidth; Height = pInf->biHeight; // load video format in global variables

	// now read pixels only in certain places to determine average brightness
	// but ignore specular refractions (> 250)
	// ii = y coordinate, i = x coordnate

	m_ave_bright = 0;
	//int Hirschberg_ratio = m_pGaze->getHirschbergRatio();

	int n = 0;
	int i, ii;
	for (ii = 20; ii < Height - 20; ii = ii + 20)
	{
		for (i = 20; i < Width - 20; i = i + 20)
		{
			if (pImageData[ii * Width + i])
			{
				m_ave_bright = m_ave_bright + pImageData[ii * Width + i];
				if (n <1024) n++;
			}
		}
	}
	m_ave_bright = m_ave_bright / n; // average brightness over max 1024 pixel



	/*****************************************************************************/
	/*    Now starts the image processing for eye detection and gaze tracking    */
	/*****************************************************************************/

	// step 1: find pixels (20 off from frame margin) that are brighter than average * 0.4
	// and store their coordinates in an array of x[n], y[n] declared in Listener.h

	n = 0;
	//int threshold = m_ave_bright * 0.52; // was 0.52;

	for (ii = 20; ii < Height - 20; ii = ii + 5)
	{
		for (i = 20; i < Width - 20; i = i + 5)
		{
			if (pImageData[ii * Width + i] > threshold)
			{
				x[n] = i;
				y[n] = ii;
				if (n < 100000)n++;
			}
		}
	}
	max = n;


	// step 2: identify those pixels marked in step 1 
	// that are surrounded by at least 'size/2' bright pixels
	// in the vertical and the horizontal direction
	// then accept only those where the vertical and horiz diameter are very similar

	int count_up, count_down, count_left, count_right;
	int size = 30; // minimal size of spot to be accepted

	// the box_size condition is necessary to prevent that pixels are later read outside the frame


	n = 0;
	for (i = 0; i < max; i++)
	{
		count_up = 0, count_down = 0, count_left = 0, count_right = 0;

		if (y[i] < Height - box_size &&
			y[i] > box_size && 
			x[i] < Width - box_size &&
			x[i] > box_size)
		{
			for (ii = 0; ii < size; ii++)
			{
				if (pImageData[y[i] * Width + x[i] + ii] > threshold) count_right++;
				if (pImageData[y[i] * Width + x[i] - ii] > threshold) count_left++;
				if (pImageData[(y[i] + ii) * Width + x[i]] > threshold) count_up++;
				if (pImageData[(y[i] - ii) * Width + x[i]] > threshold) count_down++;
			}
		}

		if (count_right > size / 2
			&& count_left > size / 2
			&& count_up   > size / 2
			&& count_down > size / 2
			&& (abs((count_right + count_left) - (count_up + count_down)) < 10)

			)
		{
			xp[n] = x[i];			// xp[i] and yp[i] are now from pixels supposed to be 
			yp[n] = y[i];			// in the pupil - global variables, defined in listener.h
			if (n < 1000)n++;
		}

	}
	xyp = n;						// xyp: number pixels for which the condition is true




	// step 3: find center of pupils roughly 
	// determine center of mass of pixels in left and right half of the frame
	// in those pixels that were found in step 2 
	// global variables, coarse coordinates of the right and left pupil centers

	ave_xp_right = 0, ave_yp_right = 0;
	int n_right = 0; // number of pixels in left and right screen whose positions are averaged

	for (i = 0; i < xyp; i++)
	{

		ave_xp_right = ave_xp_right + (float)xp[i];
		ave_yp_right = ave_yp_right + (float)yp[i];
		n_right++;

	}

	if (n_right > 0)          // only if pixels are found, the average can be calculated
	{
		ave_xp_right = ave_xp_right / n_right; // coarse coordinates 
		ave_yp_right = ave_yp_right / n_right; // of the right pupil centers
	}


	// step 4: find center of pupils with high precision
	// * take center of mass of all bright pixels in an area of box_size
	//   around the pupils centers located in step 3 
	// * determine pupil radius from number of bright pixels
	// * detect first Purkinje image

	ave_x_right = 0, ave_y_right = 0; // high resolution coordinates
	n_right = 0; 
	pupil_right = 0;	// number of pixels whose 
						// positions are averaged (same variables as already used in step 3)


	int bright;        // pixels that are presumed to belong to the pupil

	// right eye
	if (ave_xp_right > 0)
	{
		for (ii = 0; ii < box_size; ii++)
		{
			for (i = 0; i < box_size; i++)
			{
				bright = pImageData[((int)ave_yp_right - box_size / 2 + ii) * Width + (int)ave_xp_right - box_size / 2 + i];

				if (bright > threshold) // locate pixels that are brighter than threshold 
				{
					ave_x_right = ave_x_right + (ave_xp_right - (float)(box_size / 2) + i);
					ave_y_right = ave_y_right + (ave_yp_right - (float)(box_size / 2) + ii);
					n_right++;
				}
			}
		}
	}

	// right eye
	if (n_right > 0) // verify that some pixels were found
	{
		ave_x_right = ave_x_right / ((float)n_right);
		ave_y_right = ave_y_right / ((float)n_right);

		// pupil radius in Pixels determined from n_right: circle area pi*r*r
		pupil_right = (2 * (2 + sqrt(float(n_right) / 3.141592654)));
	}

	else

	{
		ave_x_right = 0;
		ave_y_right = 0;

	}

	// step 5: take x and y coordinate of pupil center as a measure of gaze position
	
	m_pGaze->addPupilCenter(ave_x_right, ave_y_right);
	m_pGaze->addPupilDia(pupil_right / magnif);
	m_pGraph->addPupilDia(pupil_right / magnif);

	// important: if pupil is in center of the video frame (320/240) 
	// this is assumed to be gaze straight 

	// note that gaze is now in degrees - assuming the Hirschberg ratio is about 12
	/*if (x_gaze_r != 0) x_gaze_r = Hirschberg_ratio * (Width / 2 - x_gaze_r) / magnif;
	if (y_gaze_r != 0) y_gaze_r = Hirschberg_ratio * (Height / 2 - y_gaze_r) / magnif;
	m_pGaze->addGazePX(-x_gaze_r, -y_gaze_r);*/

		if (frozen)	m_pGaze->addGazePX(ave_x_right - ave_x_right_fr, (ave_y_right - ave_y_right_fr));

	//if (count < 22500) count++; // advance frame counter

}

void Schaeffel::freezePupil() {
	
	// toggle between normal and frozen state
	frozen ? frozen = false : frozen = true;
	if (frozen) {
		ave_x_right_fr = ave_x_right;
		ave_y_right_fr = ave_y_right;
		pupil_right_fr = pupil_right;
		m_pGaze->addFrozenPupil(ave_x_right_fr, ave_y_right_fr);
	}
	else
		m_pGaze->stop();

}

void Schaeffel::overlayCallback(Grabber& caller, smart_ptr<OverlayBitmap> pBitmap, const tsMediaSampleDesc& MediaSampleDesc)
{
	// clear screen
	pBitmap->fill(RGB(0, 0, 0));


	// for text display
	char szText[200];
	dim.cy = pBitmap->getHeight();
	dim.cx = pBitmap->getWidth();

	if (pBitmap->getEnable() == true) // Draw only, if the overlay bitmap is enabled.
	{

		/*************************************************************************************/
		/*****   this is now the place where pixels are marked and all data are displayed ****/
		/*************************************************************************************/

		// local variables for all kinds of loops used below
		int i;

		// messages regarding brightness of video image 

		if (m_ave_bright > 240)
		{
			sprintf(szText, " IMAGE TOO BRIGHT - reduce camera aperture ");
			pBitmap->drawText(RGB(255, 0, 0), dim.cx / 2 - 120, dim.cy / 2, szText);
		}
		if (m_ave_bright < 10)
		{
			sprintf(szText, " IMAGE TOO DARK - open camera aperture");
			pBitmap->drawText(RGB(255, 0, 0), dim.cx / 2 - 120, dim.cy / 2, szText);
		}

		//// user help
		//sprintf(szText, "press space bar to interrupt or re-start");
		//pBitmap->drawText(RGB(255, 0, 0), dim.cx / 2 - 120, dim.cy - 15, szText);

		//// plot average image brightness in upper video frame
		//sprintf(szText, "average pixel value = %2.1f", m_ave_bright);
		//pBitmap->drawText(RGB(255, 0, 0), dim.cx - 160, 0, szText);

		//sprintf(szText, "frame number: %04d ", MediaSampleDesc.FrameNumber);
		//pBitmap->drawText(RGB(255, 0, 0), 0, 0, szText);

		//// display frame size
		//sprintf(szText, "video frame height = %2d", dim.cy);
		//pBitmap->drawText(RGB(255, 0, 0), 0, dim.cy - 17, szText);
		//sprintf(szText, "video frame width = %2d", dim.cx);
		//pBitmap->drawText(RGB(255, 0, 0), 0, dim.cy - 32, szText);

		// draw yellow box in life video to show the range in which the pupil should be
		if (opts & BoxBoundary)
			pBitmap->drawFrameRect(RGB(255, 255, 0), CRect(box_size, box_size, dim.cx - box_size, dim.cy - box_size));

		//// draw cross in video frame
		//pBitmap->drawLine(RGB(255, 255, 0), dim.cx / 2, box_size, dim.cx / 2, dim.cy - box_size);
		//pBitmap->drawLine(RGB(255, 255, 0), box_size, dim.cy / 2, dim.cx - box_size, dim.cy / 2);
		//

		// mark pixels yellow that are darker than 0.52 *ave

		/*for(i = 0; i < max; i++)
		pBitmap->drawLine(RGB(255, 255, 0), x[i]-1, Height-y[i]-1, x[i]+1, Height-y[i]+1);*/

		// pixels supposed to be in the pupil

		/*for (i = 0; i < xyp; i++)
			//Ellipse( memDC, x_shift+xp[i]-5, Height-yp[i]-5, x_shift+xp[i]+5, Height-yp[i]+5);
			pBitmap->drawFrameEllipse(RGB(255, 255, 0), CRect(
				xp[i] - 5,
				Height - yp[i] - 5,
				xp[i] + 5,
				Height - yp[i] + 5)
			);*/

		// draw pupil outline
		if (ave_x_right > 0)
		{

			pBitmap->drawFrameEllipse(RGB(0, 255, 0), CRect(
				(int)ave_x_right - (int)pupil_right / 2,
				Height - (int)ave_y_right - (int)pupil_right / 2,
				(int)ave_x_right + (int)pupil_right / 2,
				Height - (int)ave_y_right + (int)pupil_right / 2));

			// draw pupil center

			pBitmap->drawLine(RGB(0, 255, 0),
				(int)ave_x_right,
				Height - (int)ave_y_right - 1,
				(int)ave_x_right,
				Height - (int)ave_y_right + 2);

			pBitmap->drawLine(RGB(0, 255, 0),
				(int)ave_x_right - 1,
				Height - (int)ave_y_right,
				(int)ave_x_right + 2,
				Height - (int)ave_y_right);

		}

		if (frozen) {

			if (ave_x_right_fr > 0)
			{
				pBitmap->drawFrameEllipse(RGB(255, 0, 255), CRect(
					(int)ave_x_right_fr - (int)pupil_right_fr / 2,
					Height - (int)ave_y_right_fr - (int)pupil_right_fr / 2,
					(int)ave_x_right_fr + (int)pupil_right_fr / 2,
					Height - (int)ave_y_right_fr + (int)pupil_right_fr / 2));
			}

			// draw pupil center

			if (ave_x_right_fr > 0)
			{
				pBitmap->drawLine(RGB(255, 0, 255),
					(int)ave_x_right_fr,
					Height - (int)ave_y_right_fr - 1,
					(int)ave_x_right_fr,
					Height - (int)ave_y_right_fr + 2);

				pBitmap->drawLine(RGB(255, 0, 255),
					(int)ave_x_right_fr - 1,
					Height - (int)ave_y_right_fr,
					(int)ave_x_right_fr + 2,
					Height - (int)ave_y_right_fr);
			}

		}

	}

}