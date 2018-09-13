#include "mtrPosUnwrap.h"
#include "Arduino.h"
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


mtrPosUnwrap::mtrPosUnwrap()
{
}


mtrPosUnwrap::~mtrPosUnwrap()
{
}


void mtrPosUnwrap::init(int countsIn)
{
	setPosCnts(countsIn);
	updateCurSector();
	updatePrev();
}



void mtrPosUnwrap::unwrapAlgo(int countsIn)
{

  
	// Update reading and values
	setPosCnts(countsIn);

  if (filterSet){
    updateCurSector();
    updateMode();
    
    // Run modes
    if (TRACKING)
    	TRACKmode();
    
    if (AQRTRACK)
    	AQRTRACKmode();
    
    if (PREDICT)
    	PREDICTmode();
    
    // Condition outputs
    limitMtrPos();
    updateUnwrapPos();
    updatePrev();
  }
  else{
    wrapCheck();
    posWrapper();
    // Condition outputs
    limitMtrPos();
    updateUnwrapPos();
    updatePrev();
    
  }
    

}



void mtrPosUnwrap::updateCurSector()
{
	curSector = ceil(augMtrPos / sizeSector);
}

void mtrPosUnwrap::updateMode()
{
	int tempdiffCounts = 0;
	tempdiffCounts = diffCounts;

	/* ---------------------------- */
	// Calc PREDICT mode criteria
	/* ---------------------------- */
	// Check if motor pos is at extremes
	predictMode_crit_1 = (augMtrPos == 0);
	predictMode_crit_2 = (augMtrPos == 1023);

	// check if sectors wrapped, because on edge sectors
	wrapCheck();
	predictMode_crit_3 = (abs(curSector - prevSector) > 1) && (wrapChecked == 3);

	// Check if counts > threshold
	posWrapper();
	predictMode_crit_4 = abs(diffCounts) > WRAP_THRESHOLD;

	// Check if difference between new and old diff counts is > threshold
	predictMode_crit_5 = (tempdiffCounts - diffCounts) > WRAP_THRESHOLD;

	predictMode_crit = predictMode_crit_1 || predictMode_crit_2 || predictMode_crit_3 || predictMode_crit_4 || predictMode_crit_5;
	
	/* ---------------------------- */
	// TRACK mode criteria
	/* ---------------------------- */
	TRACKING_crit = (trackingAck) > 2;


	/* ---------------------------- */
	// Update mode values
	/* ---------------------------- */
	if (predictMode_crit) {
		PREDICT = 1;
		TRACKING = 0;
		AQRTRACK = 1;

		//Restore original diffcounts
		diffCounts = tempdiffCounts;

		// Only erase tracking if just entered new mode
		if (predictModePrev == 0) {
			TRACKING_crit = 0;
			trackingAck = 0;
			trackingAckPrev = 0;
		}
	}

	if (TRACKING_crit) {
		PREDICT = 0;
		TRACKING = 1;
		AQRTRACK = 0;
		trackingAck = 0;
		// Use last known good tracking value
		potMtrCntsPrev = trackingAckPrev;
	}

}

void mtrPosUnwrap::TRACKmode()
{
	// Standard unwrap routine, already done in modeCheck
	//posWrapper();

	// Need for smooth transition
	if (predictModePrev) {
		diffCounts = diffCountsPrev;
	}

	// run PREDICT if things go bad, might not be needed anymore
	if (abs(diffCounts) > WRAP_THRESHOLD) {
		PREDICT = 1;
	}

}

void mtrPosUnwrap::PREDICTmode()
{
	// Linear interp,  could change to some quad/poly ....
	augMtrPos = (potMtrCntsPrev + diffCountsPrev) % 1023;
	updateCurSector();
	posWrapper();
}

void mtrPosUnwrap::AQRTRACKmode()
{
	// Check if current and last known measurement is within threshold
	if ((abs(sampledMtrPos - trackingAckPrev) < WRAP_THRESHOLD) && (sampledMtrPos != 0) && (sampledMtrPos != 1023)) {
		trackingAck = trackingAck + 1;
	}
	// start over if not
	else {
		trackingAck = 0;
	}

	trackingAckPrev = sampledMtrPos; //augMtrPos;
}

void mtrPosUnwrap::limitMtrPos()
{
	// Limit diffCounts based on max known RPM
	if (diffCounts > 0) {
		diffCounts = MIN(diffCounts, WRAP_THRESHOLD);
	}
	else if (diffCounts < 0) {
		diffCounts = MAX(diffCounts, -WRAP_THRESHOLD);
	}

}

void mtrPosUnwrap::updateUnwrapPos()
{
	// Update the unwrapped position
	mtrPosCntsUnwrapped = augMtrPos + wrapCnt * 1023;
}

void mtrPosUnwrap::updatePrev()
{
	// Save prev states
	predictModePrev = PREDICT;
	diffCountsPrev = diffCounts;
	potMtrCntsPrev = augMtrPos;
	prevSector = curSector;
}

void mtrPosUnwrap::wrapCheck()
{
	// check if located on boundry sectors and compare to previous
	if (((curSector == 1) || (curSector == 0)) && ((prevSector == (numSectors - 1)) || (prevSector == (numSectors)))) {
		wrapChecked = 1; // positive
	}
	else if (((curSector == (numSectors - 1)) || (curSector == (numSectors))) && ((prevSector == 0) || (prevSector == 1))) {
		wrapChecked = 2; // neg
	}
	// if it didn't wrap, check to see if currently on boundry sector
	else {
		if ((prevSector == 1) || (prevSector == numSectors)) {
			wrapChecked = 3; // edge sectors
		}
		else {
			wrapChecked = 0;
		}
	}
}

void mtrPosUnwrap::posWrapper()
{
	int wrapOffset;

	// Sector max to sector min (positive rotation)
	if (wrapChecked == 1) {
		wrapOffset = 1023 - potMtrCntsPrev;
		diffCounts = (augMtrPos + wrapOffset);
		wrapCnt = wrapCnt + 1;
	}
	// Sector min to sector max (negative rotation)
	else if (wrapChecked == 2) {
		wrapOffset = 1023 - augMtrPos;
		diffCounts = -(wrapOffset + potMtrCntsPrev);
		wrapCnt = wrapCnt - 1;
	}
	// No wrap, regular diff
	else {
		diffCounts = augMtrPos - potMtrCntsPrev;
	}
}


void mtrPosUnwrap::setPosCnts(int positionVal)
{
	// sampledMtrPos = untouched, augMtrPos could get modified
	sampledMtrPos = positionVal;
	augMtrPos = sampledMtrPos;
}


int  mtrPosUnwrap::getDiffCounts()
{
	return diffCounts;

}

void mtrPosUnwrap::setFilter(int filterSelect)
{

  filterSet = filterSelect;

}

void mtrPosUnwrap::setThreshold(int thresSelect)
{

  WRAP_THRESHOLD = thresSelect;

}

int mtrPosUnwrap::getPosCounts()
{
  return augMtrPos;
}





