/*
 * UserParams.cpp
 *
 * Created: 3/31/2019 11:54:14 AM
 *  Author: mosta
 */ 
#define WXSTATION_ID 1 // For sending to Server

#define MEAS_INTERVAL 30 //seconds
#define ServerTx_INTERVAL 15 //Minutes
#define ANE_INTERVAL 2 // Seconds
#define LOWPOWER_LIMIT 3.45 // Volts


#define SLEEP_HOUR 23
#define WAKE_HOUR 1

//#define SERVER_NAME "scottn.ddns.net"
//#define SERVER_IP "068.230.107.205"
//#define PATH "/ardu2SQL.php?"
#define PORT_IP 5211
#define PORT_SERVER 80


#define wxPackageSize 16
#define PULSEDT_BUFFLENGTH 32


#define LED_ON 0
#define LED_OFF 1

#define txWindSF 8.9737
#define txGustSF 1
#define txTempSF 10.7684
#define txPressSF 5.115
#define txHumidSF 10.23
#define txBattVSF 212.5
#define txBattPSF 2.55
#define txTempOff -30
#define txPressOff 900
#define txWBattOff 3
#define sec2MPH 60/50.69

#define FRAM_RESET_CNT_Addr  0
#define FRAM_MEM_POINTER_Addr  3
#define FRAM_NUM_P0INTS_Addr  1
#define FRAM_MEAS_INT_Addr 7
#define FRAM_LP_MODE_Addr  9
#define FRAM_DATA_OFFSET 0x20
#define FRAM_CNTRL_OFFSET 0x10