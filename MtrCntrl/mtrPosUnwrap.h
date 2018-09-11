#pragma once
#include "math.h"

class mtrPosUnwrap
{
public:
	mtrPosUnwrap();
	~mtrPosUnwrap();

	// User defined
	int numSectors = 15;
	int WRAP_THRESHOLD = 20;
	int sizeSector     = ceil(1023 / numSectors);  // In counts
	
	// No touch
	float mtrPosCntsUnwrapped = 0.0;

	// User run functions
	void init(int countsIn);
	void unwrapAlgo(int countsIn);
	void setPosCnts(int positionVal);
	int  getDiffCounts();


	
	bool predictMode_crit_1 = false;
	bool predictMode_crit_2 = false;
	bool predictMode_crit_3 = false;
	bool predictMode_crit_4 = false;
	bool predictMode_crit_5 = false;
	bool predictMode_crit   = false;
	bool TRACKING_crit      = false;

	// Variables 
	int curSector       = 0;
	int prevSector      = 0;
	int sampledMtrPos   = 0;
	int augMtrPos       = 0;
	int potMtrCntsPrev  = 0;
	int diffCounts      = 0;
	int diffCountsPrev  = 0;
	int wrapCnt         = 0;
	int wrapChecked     = 0;
	int trackingAck     = 0;
	int trackingAckPrev = 0;
	int predictModePrev = 0;


	// Modes, no change
	int TRACKING = 0;
	int AQRTRACK = 1;
	int PREDICT  = 0;
private:
	// Algo specifics
	void updateCurSector();
	void updateMode();
	void TRACKmode();
	void PREDICTmode();
	void AQRTRACKmode();
	void limitMtrPos();
	void updateUnwrapPos();
	void updatePrev();
	void wrapCheck();
	void posWrapper();

};

