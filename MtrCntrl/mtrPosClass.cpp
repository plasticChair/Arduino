#include "mtrPosClass.h"
//#include <algorithm> 
#include <math.h>
#include "Arduino.h"


mtrUnwrapClass::mtrUnwrapClass()
{
}


mtrUnwrapClass::~mtrUnwrapClass()
{
}

void mtrUnwrapClass::unwrapAlgo()
{
	updateCurSector();
	updateMode();

	if (TRACKING)
		TRACKmode();

	if (PREDICT)
		PREDICTmode();

	if (AQRTRACK)
		AQRTRACKmode();

	limitMtrPos();
	updateUnwrapPos();
	updatePrev();

}



void mtrUnwrapClass::updateCurSector()
{
	curSector = ceil(sampledMtrPos / sizeSector);
}

void mtrUnwrapClass::updateMode()
{
  int temp;
  
	int tempdiffCounts = 0;
	bool predictMode_crit_1;
	bool predictMode_crit_2;
	bool predictMode_crit_3;
	bool predictMode_crit_4;
	bool predictMode_crit_5;
	bool predictMode_crit;

	bool TRACKING_crit;


	tempdiffCounts = diffCounts;

	predictMode_crit_1 = (sampledMtrPos == 0);
	predictMode_crit_2 = (sampledMtrPos == 1023);
  temp = abs(curSector - prevSector);

  wrapCheck();
  
	predictMode_crit_3 = (temp > 1) & ( wrapChecked == 3);
	posWrapper();
	predictMode_crit_4 = abs(diffCounts) > WRAP_THRESHOLD;

	predictMode_crit_5 = (tempdiffCounts - diffCounts) > WRAP_THRESHOLD;
	predictMode_crit = predictMode_crit_1 | predictMode_crit_2 | predictMode_crit_3 | predictMode_crit_4 | predictMode_crit_5;

	TRACKING_crit = (trackingAck) > 2;

	diffCounts = tempdiffCounts;

	if (predictMode_crit) {
		PREDICT = 1;
		trackingAck = 0;
		trackingAckPrev = 0;
		TRACKING = 0;
		AQRTRACK = 1;
	}

	if (TRACKING_crit) {
		PREDICT = 0;
		TRACKING = 1;
		AQRTRACK = 0;
		trackingAck = 0;
		potMtrCntsPrev = trackingAckPrev;
	}

}

void mtrUnwrapClass::TRACKmode()
{
	posWrapper();

	if (predictModePrev){
		diffCounts = diffCountsPrev;
	}


	if (abs(diffCounts) > WRAP_THRESHOLD) {
	PREDICT = 1;
	}

}

void mtrUnwrapClass::PREDICTmode()
{
	sampledMtrPos = (potMtrCntsPrev + diffCountsPrev) % 1023;
	updateCurSector();
	posWrapper();
}

void mtrUnwrapClass::AQRTRACKmode()
{
  int tempVar = sampledMtrPos - trackingAckPrev;
	if (abs(tempVar) < WRAP_THRESHOLD) {
		trackingAck = trackingAck + 1;
	}
	else {
		trackingAck = 0;
		//lostTrackSample = sampledMtrPos;
	}
	
	trackingAckPrev = sampledMtrPos;
}

void mtrUnwrapClass::limitMtrPos()
{
	if (diffCounts > 0) {
		diffCounts = min(diffCounts, WRAP_THRESHOLD);
	}
	else if(diffCounts < 0) {
		diffCounts = max(diffCounts, -WRAP_THRESHOLD);
	}
	
}

void mtrUnwrapClass::updateUnwrapPos()
{
	mtrPosCntsUnwrapped = sampledMtrPos + wrapCnt * 1023;
}

void mtrUnwrapClass::updatePrev()
{
	predictModePrev = PREDICT;
	diffCountsPrev = diffCounts;
	potMtrCntsPrev = sampledMtrPos;
	prevSector = curSector;
}

void mtrUnwrapClass::wrapCheck()
{
	if (((curSector == 1) | (curSector == 0)) & ((prevSector == (numSectors - 1)) | (prevSector == (numSectors)))) {
		wrapChecked = 1; // positive
	}
	else if (((curSector == (numSectors - 1)) | (curSector == (numSectors))) & ((prevSector == 0) | (prevSector == 1))) {
		wrapChecked = 2; // neg
	}
	else {
		if ((prevSector == 1) || (prevSector == numSectors)) {
			wrapChecked = 3; // edge sectors
		}
		else {
			wrapChecked = 0;
		}
	}
}

void mtrUnwrapClass::posWrapper()
{
	int wrapOffset;

	if (wrapChecked == 1) {
		wrapOffset = 1023 - potMtrCntsPrev;
		diffCounts = (sampledMtrPos + wrapOffset);
		wrapCnt = wrapCnt + 1;
		// Wrap check up
	}
	else if (wrapChecked == 2) {
		wrapOffset = 1023 - sampledMtrPos;
		diffCounts = -(wrapOffset + potMtrCntsPrev);
		wrapCnt = wrapCnt - 1;
	}
	else {
		diffCounts = sampledMtrPos - potMtrCntsPrev;
	}



}


void mtrUnwrapClass::setPos(int positionVal)
{
  sampledMtrPos = positionVal;
  
}


int  mtrUnwrapClass::getPos()
{
  return sampledMtrPos;

}




