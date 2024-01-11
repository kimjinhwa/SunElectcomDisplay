#include <Arduino.h>
#include "SerialProtocalParse.h"
#include "BLEDevice.h"
#include "ArduinoJson.h"
#include "src/ui.h"
#include "src/ui_events.h"
#include "main.h"
#include <EEPROM.h>
#include "naradav13.h"

#define USE_SERIAL Serial
#define DEVICE_LCD


#define MODULE_1  1
#define MODULE_2  2

NaradaClient232 naradaClient;
batteryInofo_t batInfo[8];

static const char *TAG ="protocal";
bool isHighVoltage=false;
bool isLowVoltage=false;
bool isHighImpedance=false;

StaticJsonDocument<3072> doc;

static uint8_t nowWindows = MODULE_1;
extern nvsSystemSet ipAddress_struct;
static int ValidData=0;
static int readSerialCount =0;
static uint8_t revData[255];
int readSerial1Data();

int readSerial1Data(){
        if(Serial1.available()){
            revData[readSerialCount] = Serial1.read();
            readSerialCount++;
            if(readSerialCount>254)readSerialCount=0;
            if(readSerialCount>4){
                if((revData[0]= 0x73) && (readSerialCount >= revData[3]+4+2) ) {
                    //LOG_I("\n-----Data count is %d %d\n",readSerialCount ,revData[3]+4+2);
                    //Serial.printf("\n-----Data count is %d %d\n",readSerialCount ,revData[3]+4+2);
                    for(int i=0;i<readSerialCount;i++)
                      Serial.printf(" %02x",revData[i]);
                    ValidData = 1; 
                    return 1;
                }
            }
        };
        return 0;
}
lv_obj_t* ui_packVoltage[8] = {
  ui_lblPack1, ui_lblPack2, ui_lblPack3, ui_lblPack4,
  ui_lblPack5, ui_lblPack6, ui_lblPack7, ui_lblPack8,
};
lv_obj_t* ui_cellVoltage[15] = {
  ui_lblvoltage1, ui_lblvoltage2, ui_lblvoltage3, ui_lblvoltage4, ui_lblvoltage5, 
  ui_lblvoltage1, ui_lblvoltage2, ui_lblvoltage3, ui_lblvoltage4, ui_lblvoltage5, 
};
float avgVoltage =0.0;
//lblOutputVoltage
void displayToLcd(int packNumber)
{
  batteryInofo_t dest;
  naradaClient.copyBatInfoData(packNumber, &dest);
  if (packNumber == 0)
    avgVoltage = dest.totalVoltage;
  else
  {
    avgVoltage += dest.totalVoltage;
    avgVoltage /= 2;
  }
  avgVoltage /= 100.0;
  String strTemp;
  strTemp = "AVG  :" + String(avgVoltage);
  lv_label_set_text(ui_lblOutputVoltage, strTemp.c_str());

  strTemp = "AMP  :" + String((dest.ampere - 30000) / 100.0f);
  lv_label_set_text(ui_lblTotalAmpere, strTemp.c_str());

  strTemp = "TEMP : " + String(
                            (dest.Tempreature[0] - 50 +
                             dest.Tempreature[1] - 50 +
                             dest.Tempreature[2] - 50 +
                             dest.Tempreature[3] - 50) /
                            4);
  lv_label_set_text(ui_lblTotalTemperature, strTemp.c_str());

  float highCellVoltage = dest.voltage[0];
  float lowCellVoltage = dest.voltage[0];

  for (int i = 1; i < dest.voltageNumber; i++)
  {
    highCellVoltage = dest.voltage[i] > highCellVoltage ? dest.voltage[i] : highCellVoltage;
    lowCellVoltage = dest.voltage[i] < lowCellVoltage ? dest.voltage[i] : lowCellVoltage;
  }
  highCellVoltage /= 1000.0f;
  lowCellVoltage /= 1000.0f;
  float differential;
  differential = highCellVoltage - lowCellVoltage;
  strTemp = "HVOL :" + String(highCellVoltage) + "V";
  lv_label_set_text(ui_lblHighVoltage, strTemp.c_str());

  strTemp = "LVOL :" + String(lowCellVoltage) + "V";
  lv_label_set_text(ui_lblLowVoltage, strTemp.c_str());

  strTemp = "DIFF :" + String(differential * 1000) + "mV";
  lv_label_set_text(ui_lblDiff, strTemp.c_str());
  if(dest.Tempreature[0]< 50) dest.Tempreature[0] =0;
  if(dest.Tempreature[1]< 50) dest.Tempreature[1] =0;
  if(dest.Tempreature[2]< 50) dest.Tempreature[2] =0;
  if(dest.Tempreature[3]< 50) dest.Tempreature[3] =0;
  String tTemperature1(dest.Tempreature[0] - 50);
  String tTemperature2(dest.Tempreature[1] - 50);
  String tTemperature3(dest.Tempreature[2] - 50);
  String tTemperature4(dest.Tempreature[3] - 50);

  String cellVoltage = "";
  cellVoltage += String(dest.voltage[0] / 1000.0f);
  cellVoltage += "\n(" + tTemperature1 + ")";
  lv_label_set_text(ui_lblvoltage1, cellVoltage.c_str());
  cellVoltage = "";
  cellVoltage += String(dest.voltage[1] / 1000.0f);
  lv_label_set_text(ui_lblvoltage2, cellVoltage.c_str());
  cellVoltage = "";
  cellVoltage += String(dest.voltage[2] / 1000.0f);
  lv_label_set_text(ui_lblvoltage3, cellVoltage.c_str());
  cellVoltage = "";
  cellVoltage += String(dest.voltage[3] / 1000.0f);
  cellVoltage += "\n(" + tTemperature2 + ")";
  lv_label_set_text(ui_lblvoltage4, cellVoltage.c_str());
  cellVoltage = "";
  cellVoltage += String(dest.voltage[4] / 1000.0f);
  lv_label_set_text(ui_lblvoltage5, cellVoltage.c_str());
  cellVoltage = "";
  cellVoltage += String(dest.voltage[5] / 1000.0f);
  lv_label_set_text(ui_lblvoltage6, cellVoltage.c_str());
  cellVoltage = "";
  cellVoltage += String(dest.voltage[6] / 1000.0f);
  lv_label_set_text(ui_lblvoltage7, cellVoltage.c_str());
  cellVoltage = "";
  cellVoltage += String(dest.voltage[7] / 1000.0f);
  cellVoltage += "\n(" + tTemperature3 + ")";
  lv_label_set_text(ui_lblvoltage8, cellVoltage.c_str());
  cellVoltage = "";
  cellVoltage += String(dest.voltage[8] / 1000.0f);
  lv_label_set_text(ui_lblvoltage9, cellVoltage.c_str());
  cellVoltage = "";
  cellVoltage += String(dest.voltage[9] / 1000.0f);
  lv_label_set_text(ui_lblvoltage10, cellVoltage.c_str());
  cellVoltage = "";
  cellVoltage += String(dest.voltage[10] / 1000.0f);
  lv_label_set_text(ui_lblvoltage11, cellVoltage.c_str());
  cellVoltage = "";
  cellVoltage += String(dest.voltage[11] / 1000.0f);
  cellVoltage += "\n(" + tTemperature4 + ")";
  lv_label_set_text(ui_lblvoltage12, cellVoltage.c_str());
  cellVoltage = "";
  cellVoltage += String(dest.voltage[12] / 1000.0f);
  lv_label_set_text(ui_lblvoltage13, cellVoltage.c_str());
  cellVoltage = "";
  cellVoltage += String(dest.voltage[13] / 1000.0f);
  lv_label_set_text(ui_lblvoltage14, cellVoltage.c_str());
  cellVoltage = "";
  cellVoltage += String(dest.voltage[14] / 1000.0f);
  lv_label_set_text(ui_lblvoltage15, cellVoltage.c_str());

  // lv_label_set_text((lv_obj_t *)ui_packVoltage[packNumber], String( dest.totalVoltage!=0 ? dest.totalVoltage/100.0f:0).c_str());
  String tVoltage = "";
  tVoltage = "#";
  tVoltage += packNumber + 1;
  tVoltage += " ";
  if (dest.totalVoltage != 0)
    tVoltage += String(float(dest.totalVoltage) / 100.0f);
  else
    tVoltage += 0;
  switch (packNumber)
  {
  case 0:
    lv_label_set_text(ui_lblPack1, tVoltage.c_str());
    break;
  case 1:
    lv_label_set_text(ui_lblPack2, tVoltage.c_str());
    break;
  case 2:
    lv_label_set_text(ui_lblPack3, tVoltage.c_str());
    break;
  case 3:
    lv_label_set_text(ui_lblPack4, tVoltage.c_str());
    break;
  case 4:
    lv_label_set_text(ui_lblPack5, tVoltage.c_str());
    break;
  case 5:
    lv_label_set_text(ui_lblPack6, tVoltage.c_str());
    break;
  case 6:
    lv_label_set_text(ui_lblPack7, tVoltage.c_str());
    break;
  case 7:
    lv_label_set_text(ui_lblPack8, tVoltage.c_str());
    break;
  default:
    break;
  }
};
void printPackData(int packNumber){
  batteryInofo_t dest;
  naradaClient.copyBatInfoData(packNumber,&dest);

  Serial.printf("\ndest.voltage %d",dest.voltageNumber );
    for(int j=0;j<15;j++)
      Serial.printf(" %d",dest.voltage[j]);
  Serial.printf("\ndest.ampere %d",dest.ampere  );
  Serial.printf("\ndest.soc%d",dest.soc);
  Serial.printf("\ndest.Capacity  %d",dest.Capacity );
  Serial.printf("\ndest.TempreatureNumber %d",dest.TempreatureNumber);
  Serial.printf("\ndest.Tempreature ");
    for(int j=0;j<4;j++) Serial.printf("dest.Tempreature %d",dest.Tempreature[j]);

  Serial.printf("\ndest.packStatus");
    for(int j=0;j<5;j++)Serial.printf("dest.packStatus%d",dest.packStatus[j]);
  Serial.printf("\ndest.readCycleCount %d",dest.readCycleCount );
  Serial.printf("\ndest.totalVoltage %d",dest.totalVoltage);
  Serial.printf("\ndest.SOH %d",dest.SOH );
  Serial.printf("\ndest.BMS_PROTECT_STATUS %d",dest.BMS_PROTECT_STATUS );
}
void serialProtocalparse()
{
  int packNumber = 0;
  ValidData = readSerial1Data();
  if (ValidData)
  {
    packNumber = revData[1];
    for (int i = 0; i < readSerialCount; i++)
      Serial.write(revData[i]);
    if (naradaClient.readAnswerData(&revData[0]) == 0)
    {
      Serial.printf("\nData Received OK Pack : %d", packNumber);
      displayToLcd(packNumber);
      printPackData(packNumber);
    }

    ValidData = 0;
    readSerialCount = 0;
    memset(revData, 0x00, 255);
  }
}
// while(Serial1.available()){
//   c=Serial1.read();
//   Serial.printf(" %02x",c);
// };
//Serial1.print("hello..");

void btnPackChange(lv_event_t * e)
{
	// Your code here
}

void btnEventPack1(lv_event_t * e)
{
  displayToLcd(0);
}

// void saveButtenEvent(lv_event_t * e)
// {
// 	// Your code here
// }

void btnEventPack2(lv_event_t * e)
{
  displayToLcd(1);
}

void btnEventPack3(lv_event_t * e)
{
  displayToLcd(2);
}

void btnEventPack4(lv_event_t * e)
{
  displayToLcd(3);
}

void btnEventPack5(lv_event_t * e)
{
  displayToLcd(4);
}

void btnEventPack6(lv_event_t * e)
{
  displayToLcd(5);
}

void btnEventPack7(lv_event_t * e)
{
  displayToLcd(6);
}

void btnEventPack8(lv_event_t * e)
{
  displayToLcd(7);
}
void saveButtenEvent(lv_event_t * e)
{
  IPAddress ipaddress(
    String(lv_textarea_get_text(ui_txtIPADDRESS1)).toInt(),
    String(lv_textarea_get_text(ui_txtIPADDRESS2)).toInt(),
    String(lv_textarea_get_text(ui_txtIPADDRESS3)).toInt(),
    String(lv_textarea_get_text(ui_txtIPADDRESS4)).toInt()
  );
  ipAddress_struct.IPADDRESS = (uint32_t)ipaddress;
  IPAddress subnet(
    String(lv_textarea_get_text(ui_txtSUBNET1)).toInt(),
    String(lv_textarea_get_text(ui_txtSUBNET2)).toInt(),
    String(lv_textarea_get_text(ui_txtSUBNET3)).toInt(),
    String(lv_textarea_get_text(ui_txtSUBNET4)).toInt()
  );
  ipAddress_struct.SUBNETMASK= (uint32_t)subnet;
  IPAddress gateway(
    String(lv_textarea_get_text(ui_txtGATEWAY1)).toInt(),
    String(lv_textarea_get_text(ui_txtGATEWAY2)).toInt(),
    String(lv_textarea_get_text(ui_txtGATEWAY3)).toInt(),
    String(lv_textarea_get_text(ui_txtGATEWAY4)).toInt()
  );
  ipAddress_struct.GATEWAY= (uint32_t)gateway;
  String devicName(lv_textarea_get_text(ui_txtDEVICENAME));
  Serial.printf("lv device name is %s",devicName);
  snprintf(ipAddress_struct.deviceName,20,devicName.c_str());
  Serial.printf("device name is %s",ipAddress_struct.deviceName);
  EEPROM.writeBytes(1, (const byte *)&ipAddress_struct, sizeof(nvsSystemSet));
  EEPROM.commit();
  EEPROM.readBytes(1, (byte *)&ipAddress_struct, sizeof(ipAddress_struct));
  Serial.println("\nInitial Memory modified");
  //Serial1.printf("ipaddress -set -i %s -s %s -g %s \r",ipaddress.toString(),gateway.toString(),subnet.toString());
  Serial1.printf("ipaddress -set -i %s -s %s  -g %s \n\r",ipaddress.toString(),subnet.toString(),gateway.toString());
  delay(500);
  Serial1.printf("ipaddress -set -i %s -s %s  -g %s \n\r",ipaddress.toString(),subnet.toString(),gateway.toString());
  delay(500);
  Serial1.printf("ipaddress -set -i %s -s %s  -g %s \n\r",ipaddress.toString(),subnet.toString(),gateway.toString());
  delay(500);
  while (Serial1.available())
  {
    Serial1.read();
  }
  setMemoryDataToLCD();
  Serial1.printf("reboot \r");
   //CompanyLabel3
   // Serial.printf("\ninit data \n%d %d %d %d",initRomData.HighVoltage,initRomData.LowVoltage,initRomData.HighTemp,initRomData.HighImp);
}
void ChangeModuleEvent(lv_event_t * e)
{
  // Your code here
  // ui_Label9;
  // String cmpText(lv_label_get_text(ui_Label9));
  // if (cmpText.compareTo("SET_2") == 0){
  //   lv_label_set_text(ui_Label9, "SET_1"); // 이것은 바꿔야 할 화면 이므로 현재화면이 아니라 넘길 화면이다
  //   nowWindows = MODULE_2;
  //   lv_label_set_text(ui_HeaderTitle,"BMS BAT #2");
  // }
  // else{
  //   lv_label_set_text(ui_Label9, "SET_2");
  //   lv_label_set_text(ui_HeaderTitle,"BMS BAT #1");
  //   nowWindows = MODULE_1;
  // };
  //   setCelldataToDisplay(nowWindows);
}
// void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
// 	const uint8_t* src = (const uint8_t*) mem;
// 	USE_SERIAL.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
// 	for(uint32_t i = 0; i < len; i++) {
// 		if(i % cols == 0) {
// 			USE_SERIAL.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
// 		}
// 		USE_SERIAL.printf("%02X ", *src);
// 		src++;
// 	}
// 	USE_SERIAL.printf("\n");
// }
void setCelldataToDisplay(uint8_t display)
{
  // float TotalVoltage = 0.0;
  // float voltageAverage = 0.0;
  // int16_t maxTemperature = 0;
  // float maxVoltage = 0.0;
  // float minVoltage = 100.0;
  // float maxImpedance = 0.0;
  // float minImpedance = 100.0;
  // float impadanceAverage = 0.0;
  // int i;
  // int start_i,end_i;
  // start_i = (display-1)*20;
  // end_i = display*20;
  // impadanceAverage /= 20;
  // voltageAverage = TotalVoltage / 20;
  // lv_obj_t *ui_vol_imp_Array_1[20][2] =
  //     {
  //         {ui_voltage1, ui_impedance1},
  //         {ui_voltage2, ui_impedance2},
  //         {ui_voltage3, ui_impedance3},
  //         {ui_voltage4, ui_impedance4},
  //         {ui_voltage5, ui_impedance5},
  //         {ui_voltage6, ui_impedance6},
  //         {ui_voltage7, ui_impedance7},
  //         {ui_voltage8, ui_impedance8},
  //         {ui_voltage9, ui_impedance9},
  //         {ui_voltage10, ui_impedance10},
  //         {ui_voltage11, ui_impedance11},
  //         {ui_voltage12, ui_impedance12},
  //         {ui_voltage13, ui_impedance13},
  //         {ui_voltage14, ui_impedance14},
  //         {ui_voltage15, ui_impedance15},
  //         {ui_voltage16, ui_impedance16},
  //         {ui_voltage17, ui_impedance17},
  //         {ui_voltage18, ui_impedance18},
  //         {ui_voltage19, ui_impedance19},
  //         {ui_voltage20, ui_impedance20}};

  // struct tm timeinfo;
  // if (!getLocalTime(&timeinfo))
  // {
  //   Serial.println("Failed to obtain time");
  // }
  // char timeString[30];
  //strftime(timeString, sizeof(timeString), "%Y-%m-%d", &timeinfo);
  // lv_label_set_text(ui_DateLabel, timeString);
  // lv_label_set_text(ui_DateLabel1, timeString);
  // lv_label_set_text(ui_DateLabel, "");
  // lv_label_set_text(ui_DateLabel1, "");
  //strftime(timeString, sizeof(timeString), "%p %I:%M:%S", &timeinfo);
  // lv_label_set_text(ui_TimeLabel, timeString);
  // lv_label_set_text(ui_TimeLabel1, timeString);
  // lv_label_set_text(ui_TimeLabel, "");
  // lv_label_set_text(ui_TimeLabel1, "");

  // lv_label_set_recolor(ui_TotalVoltage, true);
  // lv_label_set_text(ui_TotalVoltage, String(String("Total:") + String(TotalVoltage, 0) + String("V")).c_str());
  // lv_label_set_text(ui_TotalAmpere, String(String("AMP  :") + String(0, 0) + String("A")).c_str());
  // lv_label_set_text(ui_TotalTemperature, String(String("TEMP :") + String(maxTemperature) + String("C")).c_str());
  // // HVOL : 12.4V
  // lv_label_set_text(ui_HighVoltage, String(String("HVOL:") + String(maxVoltage, 2) + String("V")).c_str());
  // lv_label_set_text(ui_LowVoltage, String(String("LVOL:") + String(minVoltage, 2) + String("V")).c_str());
  // lv_label_set_text(ui_AverageVoltage, String(String("LVOL:") + String(voltageAverage, 2) + String("V")).c_str());
  // lv_label_set_text(ui_HighImpedance, String(String("HIMP:") + String(maxImpedance, 2) + String("m")).c_str());
  // lv_label_set_text(ui_LowImpedance, String(String("LIMP:") + String(minImpedance, 2) + String("m")).c_str());
  // lv_label_set_text(ui_AverageImpendance, String(String("AIMP:") + String(impadanceAverage, 2) + String("m")).c_str());
  // lv_refr_now(NULL);
  // int j;
  // for (i=0, j= start_i ; i < 20; i++,j++)
  // {
  //   lv_label_set_recolor(ui_vol_imp_Array_1[i][0], true);
  //   lv_label_set_recolor(ui_vol_imp_Array_1[i][1], true);
  //   lv_label_set_text(ui_vol_imp_Array_1[i][0], makeVoltageString(j, voltageAverage).c_str());
  //   lv_label_set_text(ui_vol_imp_Array_1[i][1], makeImdedanceString(j, impadanceAverage).c_str());
  //   if (i % 5 == 0)
  //     lv_refr_now(NULL);
  // }
  // if (isHighVoltage && isHighImpedance)
  //   lv_label_set_text(ui_CompanyLabel1, "STATUS : Impedance and Voltage High");
  // else if (isHighVoltage)
  //   lv_label_set_text(ui_CompanyLabel1, "STATUS : Voltage High");
  // else if (isHighImpedance)
  //   lv_label_set_text(ui_CompanyLabel1, "STATUS : Impedance High");
  // else
  //   lv_label_set_text(ui_CompanyLabel1, "STATUS : NORMAL");

  // Serial.printf("impedanceAverage=%f\n", impadanceAverage);
  // //Serial.printf("bmsid=%d vol=%d amp=%d cellCount=%d\n", bmsid, vol, amp, totalCellCount);
  // //Serial.printf("cellCount=%d\n", totalCellCount);
  // //Serial.printf("cellString=%d(%d,%d,%d,%d)\n", cell_string_count, cell_string1, cell_string2, cell_string3, cell_string4);
  // Serial.printf("value(0) v: %02.3f,r: %02.3f,c: %d\n", (float)doc["value"][0][0], (float)doc["value"][0][1], (uint16_t)doc["value"][0][2]);
  // Serial.printf("value(1) v: %02.3f,r: %02.3f,c: %d\n", (float)doc["value"][1][0], (float)doc["value"][1][1], (uint16_t)doc["value"][1][2]);
}
// void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

// 	switch(type) {
// 		case WStype_DISCONNECTED:
// 			USE_SERIAL.printf("[WSc] Disconnected!\n");
// 			break;
// 		case WStype_CONNECTED:
// 			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

// 			// send message to server when Connected
// 			webSocket.sendTXT("Connected");
// 			break;
// 		case WStype_TEXT:
//       if(deserializeJson(doc,payload)){
//         const char *message = doc["message"];
//         const char *content = doc["content"];
//         //USE_SERIAL.printf("[WSc] JSON : %s %s\n", message ,content);
//         lv_label_set_text(ui_CompanyLabel1,content);
//       }
// 			USE_SERIAL.printf("[WSc] get text: %s\n", payload);

// 			// send message to server
// 			 //webSocket.sendTXT("message here");
// 			break;
// 		case WStype_BIN:
// 			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
// 			//hexdump(payload, length);
//       memcpy((uint8_t *)cellvalue,(uint8_t *)payload,length);
//       for(int i=0;i<40;i++){
//          if((i+1)%5==0)
//          Serial.printf("\n%3.2f\t%3.2f\t%2d\t",cellvalue[i].voltage,cellvalue[i].impendance,cellvalue[i].temperature);
//          else
//          Serial.printf("%3.2f\t%3.2f\t%2d\t",cellvalue[i].voltage,cellvalue[i].impendance,cellvalue[i].temperature);
//       }
//       setCelldataToDisplay(nowWindows);
// 			// send data to server
// 			//webSocket.sendBIN(payload, length);
// 			break;
// 		case WStype_ERROR:			
// 		case WStype_FRAGMENT_TEXT_START:
// 		case WStype_FRAGMENT_BIN_START:
// 		case WStype_FRAGMENT:
// 		case WStype_FRAGMENT_FIN:
// 			break;
// 	}

// }

// This is the Arduino main loop function.

// String makeVoltageString(uint16_t pos,float average)
// {
//     String strVoltage;
//     isHighVoltage=false;
//     isLowVoltage=false;
//     isHighImpedance=false;
//     if(cellvalue[pos].voltage >= initRomData.HighVoltage/100.0){
//       strVoltage = "#ff0000 "; //red
//       strVoltage +=cellvalue[pos].voltage;
//       strVoltage += "# "; 
//       isHighVoltage=true;
//     }
//     else if(cellvalue[pos].voltage < initRomData.LowVoltage/100.0){
//       strVoltage = "#0000ff "; //blue
//       strVoltage +=cellvalue[pos].voltage;
//       strVoltage += "# "; 
//       isLowVoltage=true;
//     } 
//     else
//       strVoltage =cellvalue[pos].voltage;
//     strVoltage +="V";
//     strVoltage +="(";
//     strVoltage += String(cellvalue[pos].temperature) ;
//     strVoltage +=")";
//   return strVoltage ;
// }
// String makeImdedanceString(uint16_t pos,float average)
// {
//   String strImpdance;
//   strImpdance = String(cellvalue[pos].impendance);

//   //if (cellvalue[pos].impendance > (cellvalue[pos].impendance/average))
//   if ( 0.8 > (cellvalue[pos].impendance/average ||  (cellvalue[pos].impendance/average) > 1.2) || cellvalue[pos].impendance > initRomData.HighImp/100.0 )
//   {
//       strImpdance = "#ff0000 "; // red
//       strImpdance += cellvalue[pos].impendance;
//       strImpdance += "# ";
//       isHighImpedance=true;
//   }
//   else
//       strImpdance = cellvalue[pos].impendance;
//   strImpdance += "m";
//   return strImpdance;
// }
// void handleData(ModbusMessage response, uint32_t token) 
// {
//   Serial.printf("Response: serverID=%d, FC=%d, Token=%08X, length=%d:\n", response.getServerID(), response.getFunctionCode(), token, response.size());

//   lv_label_set_text(ui_CompanyLabel1, "Data receive....");
//   for (auto& byte : response) {
//     Serial.printf("%02X ", byte);
//   }
//   Serial.println("");
//   Serial.println("");
//   Serial.println("");
//   Serial.println("");
//   if(response.size()<243)
//   {
//     Serial.println("Data number count error....!");
//     return;
//   }
//   Serial.println("");
//   Serial.println("");
//   Serial.println("");
//   Serial.println("");
//   std::vector<uint8_t>allData(response.begin(),response.end());
//   auto startIndex = allData.begin()+3;
//   std::vector<uint8_t>selectedData(startIndex,startIndex+240);
//   //std::vector<uint16_t>integerArray(selectedData.begin(),selectedData.end());
// // 2바이트씩 묶어서 16비트 정수로 변환
//   std::vector<uint16_t> integerArray;
//   for (size_t i = 0; i < selectedData.size(); i += 2) {
//       uint16_t value = (selectedData[i] << 8) | selectedData[i + 1];
//       integerArray.push_back(value);
//   }
//   for(int i=0;i<120;i++){
//     Serial.printf("%d ",integerArray[i] );
//   }
//   Serial.println("");
//   Serial.println("");
//   Serial.println("");
//   for(int i=0;i<40;i++){
//     cellvalue[i].voltage = static_cast<float>(integerArray[i])/100.0f; 
//     cellvalue[i].temperature= (int16_t)integerArray[i+40]-40; 
//     cellvalue[i].impendance= static_cast<float>(integerArray[i+80])/100.0f; 
//   }
//   Serial.println("");
//   Serial.println("");
//   Serial.println("");
//   for(int i=0;i<40;i++){
//     Serial.printf("\n%f %d %f",cellvalue[i].voltage,cellvalue[i].temperature,cellvalue[i].impendance );
//   }
//   setCelldataToDisplay(nowWindows);
// }
// void handleError(Error error, uint32_t token) 
// {
//   // ModbusError wraps the error code and provides a readable error message for it
//   ModbusError me(error);
//   Serial.printf("Error response: %02X - %s token: %d\n", (int)me, (const char *)me, token);
// }
  //String strD="start";
//void serialProtocalparse() {
  // if(Serial.available()){
  //     strD =Serial.readString();
  // }
  //     lv_label_set_text(ui_CompanyLabel1,strD.c_str());

  // Serial.begin(115200);
  // Serial.println("");
  // while(WiFi.status() != WL_CONNECTED){
  //   delay(500);
  //   Serial.print(".");
  // }
  // Serial.println("");
  // Serial.println("WiFi connected");
  // Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
	// webSocket.begin("192.168.10.2", 81, "/");
	// webSocket.onEvent(webSocketEvent);
	// webSocket.setReconnectInterval(5000);
  // MB.onDataHandler(&handleData);
  // MB.onErrorHandler(&handleError);
  // MB.setTimeout(1000);
  // MB.setIdleTimeout(1200);

//} // End of setup.
// template <typename T> 
// T generateRandomNumber(T min, T max) {
//     float scale = rand() / (float )RAND_MAX; // 0에서 1 사이의 값으로 스케일 조정
//     T result = min + scale * (max - min);
//     return (T )round(result * 1000) / 1000.0; // 소수점 세 자리까지 반올림
// } ;
// void testSetDocWifiData()
// {
//     for (int i = 0; i < 40; i++)
//     {
//       cellvalue[i].voltage = generateRandomNumber(13.100, 13.450);
//       cellvalue[i].impendance = generateRandomNumber(7.000, 9.000);
//       cellvalue[i].temperature = generateRandomNumber(28,30);
//     }

//     doc.clear();
//     doc["bmsid"] = 1;
//     doc["vol"] = 386;
//     doc["amp"] = 15;
//     doc["len"] = 40;
//     doc["cell"][0] = 3;
//     doc["cell"][1][0] = 7;
//     doc["cell"][1][1] = 7;
//     doc["cell"][1][2] = 6;
//     doc["cell"][1][3] = 0;
//     for (int i = 0; i < 40; i++)
//     {
//       doc["value"][i][0] = String(cellvalue[i].voltage,2);
//       doc["value"][i][1] = String(cellvalue[i].impendance,2);
//       doc["value"][i][2] = cellvalue[i].temperature;
//     }
// }

static unsigned long previousMillis_1 = 0;  
const long interval_1s = 1000;  
static unsigned long previousMillis_5 = 0;  
const long interval_5s = 5000;  
void WebSocketJsonProtocalLoop(void *parameters)
{
  unsigned long currentMillis;
  //for (;;)
  {
    //webSocket.loop();
    currentMillis = millis();
    //
    if (currentMillis - previousMillis_5 >= interval_5s)
    {
      //Serial.println("GET_BMS_DATA Loop");
      // testSetDocWifiData();
      // webSocket.sendTXT("GET_BMS_DATA",sizeof("GET_BMS_DATA"));
      //Error err;
      // lv_label_set_text(ui_CompanyLabel1, "Data rendring....");
      // setCelldataToDisplay(nowWindows);
      // lv_label_set_text(ui_CompanyLabel1, "reading....");
      //err = MB.addRequest((uint32_t)currentMillis, 1, READ_HOLD_REGISTER, 0, 120);
      // if (err != SUCCESS)
      // {
      //   ModbusError e(err);
      //   Serial.printf("Error creating request: %02X - %s\n", (int)e, (const char *)e);
      // }
      previousMillis_5 = currentMillis;
    }
    vTaskDelay(10);
  };
}

// void WebSocketJsonProtocalLoopFunc()
// {
//   //  // 시간 문자열 생성
//   //   char timeString[30];
//   //   strftime(timeString, sizeof(timeString), "%Y-%m-%d", &timeinfo);
//   //   strftime(timeString, sizeof(timeString), "%p %I:%M:%S", &timeinfo);

//   //   //HVOL : 12.4V

//   //   Serial.printf("impedanceAverage=%f\n",impadanceAverage);
//   //   Serial.printf("bmsid=%d vol=%d amp=%d cellCount=%d\n", bmsid, vol, amp, totalCellCount);
//   //   Serial.printf("cellCount=%d\n", totalCellCount);
//   //   Serial.printf("cellString=%d(%d,%d,%d,%d)\n", cell_string_count, cell_string1, cell_string2, cell_string3, cell_string4);
//   //   Serial.printf("value(0) v: %02.3f,r: %02.3f,c: %d\n", (float)doc["value"][0][0], (float)doc["value"][0][1], (uint16_t)doc["value"][0][2]);
//   //   Serial.printf("value(1) v: %02.3f,r: %02.3f,c: %d\n", (float)doc["value"][1][0], (float)doc["value"][1][1], (uint16_t)doc["value"][1][2]);
//   // }
//   delay(10); // Delay a second between loops.
// } // End of loop