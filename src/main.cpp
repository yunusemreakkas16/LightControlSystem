#include <Wiegand.h>
#include <SPI.h>
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

WIEGAND wg;
const int redPin = 15;
const int greenPin =2;
const int bluePin = 4;

const int redPin2 =5;
const int greenPin2 =18;
const int bluePin2 =19;

const int redPin3 = 21;
const int greenPin3= 22;
const int bluePin3 = 23;

const int button = 27;

const int SENSOR_PIN = 26;

int hexCode = 0xA54980;

int pirState = LOW;


BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device Connected.");
    BLEDevice::startAdvertising();
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device Disconnected.");
    BLEDevice::startAdvertising();
  }
};

void setup() {
	Serial.begin(9600);

	pinMode(redPin,OUTPUT);
	pinMode(greenPin,OUTPUT);
	pinMode(bluePin,OUTPUT);
	
	pinMode(redPin2,OUTPUT);
	pinMode(greenPin2,OUTPUT);
	pinMode(bluePin2,OUTPUT);

	pinMode(redPin3,OUTPUT);
	pinMode(greenPin3,OUTPUT);
	pinMode(bluePin3,OUTPUT);

	pinMode(button,INPUT); 

	pinMode(SENSOR_PIN,INPUT);
	
	// default Wiegand Pin 2 and Pin 3 see image on README.md
	// for non UNO board, use wg.begin(pinD0, pinD1) where pinD0 and pinD1 
	// are the pins connected to D0 and D1 of wiegand reader respectively.
	wg.begin(34,35);

	BLEUUID customServiceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");	//Random service uuid
	BLEUUID customCharacteristicUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");	// Random characteristic uuid

	BLEDevice::init("MyESP32");
	BLEServer *pServer = BLEDevice::createServer();
  	pServer->setCallbacks(new MyServerCallbacks());

  	BLEService *pService = pServer->createService(customServiceUUID);

  	pCharacteristic = pService->createCharacteristic(
    customCharacteristicUUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );

  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  pServer->getAdvertising()->setMinPreferred(0x06);  // minimum connection time which defined by seconds.
  pServer->getAdvertising()->setMaxPreferred(0x12);  // maximum connection time which defined by seconds.
  pServer->getAdvertising()->start();
  Serial.println("BLE server has started.");
}

void loop() {
	if (digitalRead(button) == 1)
	{
			analogWrite(redPin,255);
			analogWrite(greenPin,0);
			analogWrite(bluePin,0);

			analogWrite(redPin2,0);
			analogWrite(greenPin2,255);
			analogWrite(bluePin2,0);

			analogWrite(redPin3,255);
			analogWrite(greenPin3,0);
			analogWrite(bluePin3,0);

			delay(5000);

			analogWrite(redPin,255);
			analogWrite(greenPin,9);
			analogWrite(bluePin,0);

			analogWrite(redPin2,255);
			analogWrite(greenPin2,0);
			analogWrite(bluePin2,0);

			analogWrite(redPin3,0);
			analogWrite(greenPin3,255);
			analogWrite(bluePin3,0);


	}
	if(wg.available())
	{
		if (hexCode==wg.getCode(),HEX)
		{
			Serial.print("Access Granted!");
			
			 if (deviceConnected) {
   	 			pCharacteristic->setValue("Access Granted!");
    			pCharacteristic->notify();
    			delay(1000);
  			}

			analogWrite(redPin,0);
			analogWrite(greenPin,255);
			analogWrite(bluePin,0);

			analogWrite(redPin2,255);
			analogWrite(greenPin2,0);
			analogWrite(bluePin2,0);

			analogWrite(redPin3,255);
			analogWrite(greenPin3,0);
			analogWrite(bluePin3,0);

			Serial.print("Wiegand HEX = ");
			Serial.print(wg.getCode(),HEX);
			Serial.print(", DECIMAL = ");
			Serial.print(wg.getCode());
			Serial.print(", Type W");
			Serial.println(wg.getWiegandType());

			delay(5000);

			analogWrite(greenPin,0);
			analogWrite(redPin2,0);
			analogWrite(redPin3,0);
			
			analogWrite(redPin,255);
			analogWrite(redPin2,255);
			analogWrite(greenPin3,255);
			
		}

		else
		{
			
			Serial.print("Access Denied!");

			if (deviceConnected) {
   	 			pCharacteristic->setValue("Access Denied!");
    			pCharacteristic->notify();
  			}


			analogWrite(redPin,255);
			analogWrite(greenPin,0);
			analogWrite(bluePin,0);
			delay(5000);

			analogWrite(redPin2,255);
			analogWrite(greenPin2,0);
			analogWrite(bluePin2,0);
			
			analogWrite(redPin3,0);
			analogWrite(greenPin3,255);
			analogWrite(bluePin3,0);

			Serial.print("Wiegand HEX = ");
			Serial.print(wg.getCode(),HEX);
			Serial.print(", DECIMAL = ");
			Serial.print(wg.getCode());
			Serial.print(", Type W");
			Serial.println(wg.getWiegandType());
		}
		
	}

 	int val = digitalRead(SENSOR_PIN);  // read input value

	if (val == HIGH)  // check if the input is HIGH
	{            
  
    	if (pirState == LOW) 
		{
    		Serial.println("Movement detected!"); // print on output change

			if (deviceConnected) {
   	 			pCharacteristic->setValue("Movement detected!");
    			pCharacteristic->notify();
  			}

			pirState = HIGH;

			analogWrite(redPin,255);
			analogWrite(greenPin,0);
			analogWrite(bluePin,0);

			analogWrite(redPin2,255);
			analogWrite(greenPin2,0);
			analogWrite(bluePin2,0);

			analogWrite(redPin3,0);
			analogWrite(greenPin3,255);
			analogWrite(bluePin3,0);

    	}
	} 
  		else 
  		{
  
    		if (pirState == HIGH)
			{
      			Serial.println("There is not any movement!");  // print on output change

					if (deviceConnected) {
   	 				pCharacteristic->setValue("There is not any movement!");
    				pCharacteristic->notify();
  					}

      			pirState = LOW;
    		}
  		}
}
 