#include "MPU9250.h"
#define DEBUG_PORT Serial5
// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire, 0x68);
int status;

void setup() {
	// serial to display data
	DEBUG_PORT.begin(115200);
	while (!DEBUG_PORT) {}

	// start communication with IMU 
	status = IMU.begin();
	DEBUG_PORT.print("\n\n------------=-------------\n");

	IMU.setAccelRange(IMU.ACCEL_RANGE_4G);
	IMU.setDlpfBandwidth(IMU.DLPF_BANDWIDTH_5HZ);

	

	if (status < 0) {
		DEBUG_PORT.println("IMU initialization unsuccessful");
		DEBUG_PORT.println("Check IMU wiring or try cycling power");
		DEBUG_PORT.print("Status: ");
		DEBUG_PORT.println(status);
		while (1) {}
	}

	DEBUG_PORT.println("cal mode entered");
	IMU.calibrateMag();


	DEBUG_PORT.println(IMU.getMagScaleFactorX());
}

void loop() {
	float heading, pitch, roll, magXcomp, magYcomp, accXnorm, accYnorm;
	float accRawX, accRawY, accRawZ, magRawX, magRawY, magRawZ;

	float alpha = 0.1;

	// read the sensor
	IMU.readSensor();


#if 0
	// display the data
	DEBUG_PORT.print(IMU.getAccelX_mss(), 6);
	DEBUG_PORT.print("\t");
	DEBUG_PORT.print(IMU.getAccelY_mss(), 6);
	DEBUG_PORT.print("\t");
	DEBUG_PORT.print(IMU.getAccelZ_mss(), 6);
	DEBUG_PORT.print("\n");

	DEBUG_PORT.print(IMU.getGyroX_rads(), 6);
	DEBUG_PORT.print("\t");
	DEBUG_PORT.print(IMU.getGyroY_rads(), 6);
	DEBUG_PORT.print("\t");
	DEBUG_PORT.print(IMU.getGyroZ_rads(), 6);
	DEBUG_PORT.print("\n");
	DEBUG_PORT.print(IMU.getMagX_uT(), 6);
	DEBUG_PORT.print("\t");
	DEBUG_PORT.print(IMU.getMagY_uT(), 6);
	DEBUG_PORT.print("\t");
	DEBUG_PORT.print(IMU.getMagZ_uT(), 6);
	DEBUG_PORT.print("\n\n\n");
	DEBUG_PORT.println(IMU.getTemperature_C(), 6);

#endif // 1
	DEBUG_PORT.print("\n\n-------------\n");
	accRawX = IMU.getAccelX_mss();
	accRawY = IMU.getAccelY_mss();
	accRawZ = IMU.getAccelZ_mss();

	//magRawY = IMU.getMagX_uT();
	//magRawX = IMU.getMagY_uT();
	//magRawZ = -IMU.getMagZ_uT();

	magRawX = magRawX + alpha * (IMU.getMagY_uT() - magRawX);
	magRawY = magRawY + alpha * (IMU.getMagX_uT() - magRawY);
	magRawZ = magRawZ + alpha * (-IMU.getMagZ_uT() - magRawZ);

	accXnorm = accRawX / sqrt(accRawX * accRawX + accRawY * accRawY + accRawZ * accRawZ);
	accYnorm = accRawY / sqrt(accRawX * accRawX + accRawY * accRawY + accRawZ * accRawZ);

	pitch = asin(accXnorm);
	roll = -asin(accYnorm / cos(pitch));

	magXcomp = magRawX *cos(pitch) + accRawZ *sin(pitch);
	magYcomp = magRawX *sin(roll)*sin(pitch) + accRawY *cos(roll) - accRawZ *sin(roll)*cos(pitch);

	//magXcomp = magRawX * cos(pitch) + magRawY * sin(roll)*sin(pitch) + magRawZ * cos(roll)* sin(pitch);
	// Tilt compensated magnetic field Y component:
	//magYcomp = magRawY * cos(roll) - magRawZ * sin(roll);
	// magnetic heading
	//heading = atan2(-magYcomp, -magXcomp);

	heading = 180 * atan2(magYcomp, magXcomp) / M_PI;

//	heading = 180 * atan2(magRawY, magRawX) / M_PI;
	if (heading < 0)
		heading += 360;

	//if (heading > M_PI)    // Angle normalization (-180 deg, 180 deg)
		//heading -= (2.0 * M_PI);
//	else if (heading < -M_PI)
	//	heading += (2.0 * M_PI);

	//DEBUG_PORT.print(magRawX);
	//DEBUG_PORT.print(",");
	//DEBUG_PORT.println(magRawY);

	 DEBUG_PORT.println(heading);


#if 0
	DEBUG_PORT.print(accRawX);
	DEBUG_PORT.print(", ");
	DEBUG_PORT.print(accRawY);
	DEBUG_PORT.print(", ");
	DEBUG_PORT.print(accRawZ);
	DEBUG_PORT.println();
#endif // 0




	delay(50);
}