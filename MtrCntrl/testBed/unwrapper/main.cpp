#include "MtrCntrl.h"
#include "mtrPosUnwrap.h"
#include <iostream>


MotorControl mtrCntrl;
mtrPosUnwrap snsfFilter;


void printMe();

void main() {
	// put your setup code here, to run once:
	//Serial.begin(115200);




	mtrCntrl.Init();
	snsfFilter.init(100);
	//Serial.println(" Init complete! ");

	
	printMe();


	snsfFilter.unwrapAlgo(100);
	printMe();

	int posArray[30] = {500, 501, 500, 502, 503, 505, 507, 508, 511, 300, 302, 305, 520, 525, 530, 535, 800, 200, 202, 400, 550, 552, 553, 555, 557, 558, 560, 570, 580, 590 };

	for (int ii = 0; ii < 30; ii++) {

		printf("%i \n", posArray[ii]);
		// read sensors
		mtrCntrl.readSensor(posArray[ii]);
		// run algo
		snsfFilter.unwrapAlgo(mtrCntrl.getPosCnts());
		printMe();
	}



}

void printMe()
{

	printf("%i   ", snsfFilter.augMtrPos);
	printf("%i   ", snsfFilter.sampledMtrPos);
	printf("%i   ", snsfFilter.predictMode_crit_1);
	printf("%i   ", snsfFilter.predictMode_crit_2);
	printf("%i   ", snsfFilter.predictMode_crit_3);
	printf("%i   ", snsfFilter.predictMode_crit_4);
	printf("%i,   ", snsfFilter.predictMode_crit_5);
	printf("%i,   ", snsfFilter.trackingAck);

	printf(", MODE = %i   ", snsfFilter.PREDICT);
	printf("%i   ", snsfFilter.TRACKING);
	printf("%i,   ", snsfFilter.AQRTRACK);
	printf("%i\n   ", snsfFilter.diffCounts);






}