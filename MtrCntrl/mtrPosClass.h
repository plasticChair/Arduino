#pragma once

class mtrUnwrapClass
{
public:
	mtrUnwrapClass();
	virtual ~mtrUnwrapClass();


	int curSector;
	int prevSector;
	int sizeSector;
	int numSectors;

	int sampledMtrPos;
	int potMtrCnts;
	int potMtrCntsPrev;

	int diffCounts;
	int diffCountsPrev;
	int wrapCnt;
	int wrapChecked;

	int WRAP_THRESHOLD;
	float mtrPosCntsUnwrapped;

	int TRACKING;
	int AQRTRACK;
	int PREDICT;
	int predictModePrev;

	int trackingAck;
	int trackingAckPrev;

	void unwrapAlgo();
	void updateCurSector();
	void updateMode();
	void TRACKmode();
	void PREDICTmode();
	void AQRTRACKmode();
	void limitMtrPos();
	void updateUnwrapPos();
	void updatePrev();

  void setPos(int positionVal);
  int  getPos();

	void wrapCheck();
	void posWrapper();

};

