// #include "BleJsonProtocal.h"
// #include <Arduino.h>
// #include "BLEDevice.h"
// #include <ArduinoJson.h>
// #include "src/ui.h"
// #include "src/ui_events.h"

// StaticJsonDocument<3072> doc;
// static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");

// static BLEUUID    charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

// static boolean doConnect = false;
// static boolean connected = false;
// static boolean doScan = false;
// static BLERemoteCharacteristic* pRemoteCharacteristic;
// static BLEAdvertisedDevice* myDevice;

// String makeImdedanceString(uint16_t pos,float average);
// String makeVoltageString(uint16_t pos,float average);

// void ChangeModuleEvent(lv_event_t * e)
// {
//   // Your code here
//   ui_Label9;
//   String cmpText(lv_label_get_text(ui_Label9));
//   if (cmpText.compareTo("SET_2") == 0)
//     lv_label_set_text(ui_Label9, "SET_1");
//   else
//     lv_label_set_text(ui_Label9, "SET_2");
// }

// static void notifyCallback(
//   BLERemoteCharacteristic* pBLERemoteCharacteristic,
//   uint8_t* pData,
//   size_t length,
//   bool isNotify) {
//     Serial.print("Notify callback for characteristic ");
//     Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
//     Serial.print(" of data length ");
//     Serial.println(length);
//     Serial.print("data: ");
//     Serial.write(pData, length);
//     Serial.println();
// }

// class MyClientCallback : public BLEClientCallbacks {
//   void onConnect(BLEClient* pclient) {
//   }

//   void onDisconnect(BLEClient* pclient) {
//     connected = false;
//     Serial.println("onDisconnect");
//   }
// };
// BLEClient*  pClient ;
// void disconnectFromServer()
// {
//   if (pClient != nullptr && pClient->isConnected())
//   {
//     pClient->disconnect();
//     connected = false;
//     Serial.println("Disconnected from the BLE Server.");
//     doConnect  = true;
//   }
// }
// bool connectToServer() {
//     Serial.print("Forming a connection to ");
//     Serial.println(myDevice->getAddress().toString().c_str());
    
//     pClient  = BLEDevice::createClient();
//     Serial.println(" - Created client");

//     pClient->setClientCallbacks(new MyClientCallback());

//     // Connect to the remove BLE Server.
//     pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
//     Serial.println(" - Connected to server");
//     pClient->setMTU(517); //set client to request maximum MTU from server (default is 23 otherwise)
  
//     // Obtain a reference to the service we are after in the remote BLE server.
//     BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
//     if (pRemoteService == nullptr) {
//       Serial.print("Failed to find our service UUID: ");
//       Serial.println(serviceUUID.toString().c_str());
//       pClient->disconnect();
//       return false;
//     }
//     Serial.println(" - Found our service");


//     // Obtain a reference to the characteristic in the service of the remote BLE server.
//     pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
//     if (pRemoteCharacteristic == nullptr) {
//       Serial.print("Failed to find our characteristic UUID: ");
//       Serial.println(charUUID.toString().c_str());
//       pClient->disconnect();
//       return false;
//     }
//     Serial.println(" - Found our characteristic");

//     // Read the value of the characteristic.
//     if(pRemoteCharacteristic->canRead()) {
//       std::string value = pRemoteCharacteristic->readValue();
//       Serial.print("The characteristic value was: ");
//       Serial.println(value.c_str());
//     }

//     if(pRemoteCharacteristic->canNotify())
//       pRemoteCharacteristic->registerForNotify(notifyCallback);

//     connected = true;
//     return true;
// }
// /**
//  * Scan for BLE servers and find the first one that advertises the service we are looking for.
//  */
// class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
//  /**
//    * Called for each advertising BLE server.
//    */
//   void onResult(BLEAdvertisedDevice advertisedDevice) {
//     Serial.print("BLE Advertised Device found: ");
//     Serial.println(advertisedDevice.toString().c_str());

//     // We have found a device, let us now see if it contains the service we are looking for.
//     if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

//       BLEDevice::getScan()->stop();
//       myDevice = new BLEAdvertisedDevice(advertisedDevice);
//       doConnect = true;
//       doScan = true;

//     } // Found our server
//   } // onResult
// }; // MyAdvertisedDeviceCallbacks


// void bleJsonProtocalsetup() {
//   Serial.begin(115200);
//   Serial.println("Starting Arduino BLE Client application...");
//   BLEDevice::init("");

//   // Retrieve a Scanner and set the callback we want to use to be informed when we
//   // have detected a new device.  Specify that we want active scanning and start the
//   // scan to run for 5 seconds.
//   BLEScan* pBLEScan = BLEDevice::getScan();
//   pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
//   pBLEScan->setInterval(1349);
//   pBLEScan->setWindow(449);
//   pBLEScan->setActiveScan(true);
//   pBLEScan->start(5, false);
// } // End of setup.

// struct _cell_value{
//   float voltage;
//   float impendance;
//   u16_t temperature;
// };
// _cell_value cellvalue[20];

// // This is the Arduino main loop function.

// bool isHighVoltage=false;
// bool isLowVoltage=false;
// bool isHighImpedance=false;
// String makeVoltageString(uint16_t pos,float average)
// {
//     String strVoltage;
//     isHighVoltage=false;
//     isLowVoltage=false;
//     isHighImpedance=false;
//     if(cellvalue[pos].voltage > 13.45){
//       strVoltage = "#ff0000 "; //red
//       strVoltage +=cellvalue[pos].voltage;
//       strVoltage += "# "; 
//       isHighVoltage=true;
//     }
//     else if(cellvalue[pos].voltage < 10.8){
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
//   if ( 0.8 > (cellvalue[pos].impendance/average ||  (cellvalue[pos].impendance/average) > 1.2) )
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
// void bleJsonProtocalLoop()
// {

//   // If the flag "doConnect" is true then we have scanned for and found the desired
//   // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
//   // connected we set the connected flag to be true.
//   if (doConnect == true)
//   {
//     if (connectToServer())
//     {
//       Serial.println("We are now connected to the BLE Server.");
//     }
//     else
//     {
//       Serial.println("We have failed to connect to the server; there is nothin more we will do.");
//     }
//     doConnect = false;
//   }

//   // If we are connected to a peer BLE Server, update the characteristic each time we are reached
//   // with the current time since boot.
//   if (connected)
//   {
//     String newValue = "Time since boot: " + String(millis() / 1000);
//     Serial.println("Setting new characteristic value to \"" + newValue + "\"");

//     // Set the characteristic's value to be the array of bytes that is actually a string.
//     pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());

//     std::string readValue = pRemoteCharacteristic->readValue();
//     Serial.print("Read characteristic value : ");

//     Serial.println(readValue.c_str());
//     DeserializationError error = deserializeJson(doc, String(readValue.c_str()));

//     // JSON 파싱에 성공했는지 확인
//     if (error)
//     {
//       Serial.print("deserializeJson() 실패: ");
//       Serial.println(error.c_str());
//       disconnectFromServer();
//       return;
//     };
//     //
//     // lv_label_set_text_static
//     uint16_t bmsid = doc["bmsid"];
//     uint16_t vol = doc["vol"];
//     uint16_t amp = doc["amp"];
//     uint16_t totalCellCount = doc["len"];
//     uint16_t cell_string_count = doc["cell"][0];
//     uint16_t cell_string1 = doc["cell"][1][0];
//     uint16_t cell_string2 = doc["cell"][1][1];
//     uint16_t cell_string3 = doc["cell"][1][2];
//     uint16_t cell_string4 = doc["cell"][1][3];
//     float TotalVoltage=0.0;
//     float voltageAverage =0.0;
//     uint16_t maxTemperature=0;
//     float maxVoltage=0.0;
//     float minVoltage=100.0;
//     float maxImpedance=0.0;
//     float minImpedance=100.0;
//     float impadanceAverage =0.0;

//     float value1 = doc["value"][0][0];

//     for (int i = 0; i < 20; i++)
//     {
//       cellvalue[i].voltage = doc["value"][i][0];
//       if(maxVoltage <= cellvalue[i].voltage )maxVoltage = cellvalue[i].voltage ;
//       if(minVoltage >= cellvalue[i].voltage )minVoltage = cellvalue[i].voltage ;

//       cellvalue[i].impendance = doc["value"][i][1];
//       if(maxImpedance<= cellvalue[i].voltage )maxImpedance= cellvalue[i].impendance;
//       if(minImpedance>= cellvalue[i].voltage )minImpedance= cellvalue[i].impendance;

//       cellvalue[i].temperature = doc["value"][i][2];
//       if(maxTemperature <= cellvalue[i].voltage )maxTemperature = cellvalue[i].temperature;

//       impadanceAverage +=cellvalue[i].impendance ;
//       TotalVoltage+=cellvalue[i].voltage; 
//     };
//     impadanceAverage /=20;
//     voltageAverage  = TotalVoltage/20;
//     //Total    : 225.5V

//     lv_obj_t *ui_vol_imp_Array_1[20][2] =
//         {
//             {ui_voltage1, ui_impedance1},
//             {ui_voltage2, ui_impedance2},
//             {ui_voltage3, ui_impedance3},
//             {ui_voltage4, ui_impedance4},
//             {ui_voltage5, ui_impedance5},
//             {ui_voltage6, ui_impedance6},
//             {ui_voltage7, ui_impedance7},
//             {ui_voltage8, ui_impedance8},
//             {ui_voltage9, ui_impedance9},
//             {ui_voltage10, ui_impedance10},
//             {ui_voltage11, ui_impedance11},
//             {ui_voltage12, ui_impedance12},
//             {ui_voltage13, ui_impedance13},
//             {ui_voltage14, ui_impedance14},
//             {ui_voltage15, ui_impedance15},
//             {ui_voltage16, ui_impedance16},
//             {ui_voltage17, ui_impedance17},
//             {ui_voltage18, ui_impedance18},
//             {ui_voltage19, ui_impedance19},
//             {ui_voltage20, ui_impedance20}};

//           struct tm timeinfo;
//           if (!getLocalTime(&timeinfo))
//           {
//             Serial.println("Failed to obtain time");
//           }
//    // 시간 문자열 생성
//     char timeString[30];
//     strftime(timeString, sizeof(timeString), "%Y-%m-%d", &timeinfo);
//     lv_label_set_text(ui_DateLabel, timeString);
//     lv_label_set_text(ui_DateLabel1, timeString);
//     strftime(timeString, sizeof(timeString), "%p %I:%M:%S", &timeinfo);
//     lv_label_set_text(ui_TimeLabel, timeString);
//     lv_label_set_text(ui_TimeLabel1, timeString);

//     lv_label_set_recolor(ui_TotalVoltage,true);
//     lv_label_set_text(ui_TotalVoltage,     String(String("Total:") + String(TotalVoltage,0)+String("V")).c_str() );
//     lv_label_set_text(ui_TotalAmpere,      String(String("AMP  :") + String(0,0)+String("A")).c_str() );
//     lv_label_set_text(ui_TotalTemperature, String(String("TEMP :") + String(maxTemperature)+String("C")).c_str() );
//     //HVOL : 12.4V
//     lv_label_set_text(ui_HighVoltage, String(String("HVOL:") + String(maxVoltage,2)+String("V")).c_str() );
//     lv_label_set_text(ui_LowVoltage,  String(String("LVOL:") + String(minVoltage,2)+String("V")).c_str() );
//     lv_label_set_text(ui_AverageVoltage,  String(String("LVOL:") + String(voltageAverage,2)+String("V")).c_str() );
//     lv_label_set_text(ui_HighImpedance,  String(String("HIMP:") + String(maxImpedance,2)+String("m")).c_str() );
//     lv_label_set_text(ui_LowImpedance,  String(String("LIMP:") + String(minImpedance,2)+String("m")).c_str() );
//     lv_label_set_text(ui_AverageImpendance,  String(String("AIMP:") + String(impadanceAverage,2)+String("m")).c_str() );
//     lv_refr_now(NULL); 

//     for (int i = 0; i < 20; i++)
//     {
//       lv_label_set_recolor(ui_vol_imp_Array_1[i][0],true);
//       lv_label_set_recolor(ui_vol_imp_Array_1[i][1],true);
//       lv_label_set_text(ui_vol_imp_Array_1[i][0], makeVoltageString(i,voltageAverage  ).c_str());
//       lv_label_set_text(ui_vol_imp_Array_1[i][1], makeImdedanceString(i,impadanceAverage).c_str());
//       // lv_label_set_text(ui_vol_imp_Array_1[i][0], "AAA");
//       // lv_label_set_text(ui_vol_imp_Array_1[i][1], "BBB");
//       if(i%5 == 0) lv_refr_now(NULL); 
//     }
//     if(isHighVoltage && isHighImpedance)
//       lv_label_set_text(ui_CompanyLabel1,"STATUS : Impedance and Voltage High");
//     else if(isHighVoltage)
//       lv_label_set_text(ui_CompanyLabel1,"STATUS : Voltage High");
//     else if(isHighImpedance)
//       lv_label_set_text(ui_CompanyLabel1,"STATUS : Impedance High");
//     else 
//       lv_label_set_text(ui_CompanyLabel1,"STATUS : NORMAL");

//     Serial.printf("impedanceAverage=%f\n",impadanceAverage);
//     Serial.printf("bmsid=%d vol=%d amp=%d cellCount=%d\n", bmsid, vol, amp, totalCellCount);
//     Serial.printf("cellCount=%d\n", totalCellCount);
//     Serial.printf("cellString=%d(%d,%d,%d,%d)\n", cell_string_count, cell_string1, cell_string2, cell_string3, cell_string4);
//     Serial.printf("value(0) v: %02.3f,r: %02.3f,c: %d\n", (float)doc["value"][0][0], (float)doc["value"][0][1], (uint16_t)doc["value"][0][2]);
//     Serial.printf("value(1) v: %02.3f,r: %02.3f,c: %d\n", (float)doc["value"][1][0], (float)doc["value"][1][1], (uint16_t)doc["value"][1][2]);
//   }
//   else if (doScan)
//   {
//     Serial.println("Scanning continue");
//     BLEDevice::getScan()->start(0); // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
//   }
//   delay(10); // Delay a second between loops.
//   disconnectFromServer();
//   delay(900); // Delay a second between loops.
// } // End of loop

//     //lv_area_align(ui_vol_imp_Array_1[0][0],
//     //lv_obj_align()
//   //   lv_group_t* myGroup= lv_group_create();
//   // // 그룹에 위젯 추가
//   // for (int i = 0; i < 20; i++) {
//   //     lv_group_add_obj(myGroup, ui_vol_imp_Array_1[i][0]);
//   //     lv_group_add_obj(myGroup, ui_vol_imp_Array_1[i][1]);
//   // }
//   // lv_group_focus_obj(ui_vol_imp_Array_1[0][0]);
//   // lv_group_send_data(myGroup);