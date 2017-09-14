#pragma once
// Gaze

class Gaze : public CStatic
{
	DECLARE_DYNAMIC(Gaze)

private:

	struct coords {
		float x;
		float y;
	};

	std::vector<coords>Pupil;
	std::vector<coords>GazePX;
	std::vector<float>PupilDia;

	int box_size;						// box in which pixels are searched that belong to the pupil in step 4
	float Hirschberg_ratio;
	float magnif;						// magnification of the video image (pixel/mm)

public:
	Gaze();
	~Gaze();
	void addPupilCenter(float x, float y);
	void addPupilDia(float);
	void addGazePX(float x, float y);
	void Save(FILE* pFile);
	void Paint(CDC* dc);

	int getBoxSize();
	int getHirschbergRatio();
	float getPupilDia();
	float getMagnif() { return magnif; };
	coords getGazePX();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


