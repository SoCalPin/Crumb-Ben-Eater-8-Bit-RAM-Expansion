
int FirstTimeThru = 1;       // for debugging to limit run to 1 time
int BarWriteEnablePin = 12;  // Pin which when low indicates that the RAM is in WriteMode
int ReadWritePin = 13;       // data only, flow control Write to RAM = Low, Read from RAM = High
int DataBufferPinA = 10;     // Set high to enable the buffer 3 to 8 decoder
int DataBufferPinB = 11;     // Set high to enable the buffer 3 to 8 decoder
int LatchOutPin = 1;


uint16_t RamAddress = 0;
// address to store or read from RAM
byte RAMArray[1024];
byte RamData = 0;
int lastAddress = -10;
int lastData = -10;
int TestHalt = 0;
int updateFlag = 0;
int lastWriteEnableState = 1;  // 0 = write, 1 = read


void setDataIn() {
  // Set the RAM data input lines (Digital 2 thru 9) to input
  for (int i = 2; i <= 9; i++) {
    // Set the corresponding digital pin (used as digital output) to HIGH (1) or LOW (0)
    pinMode(i, INPUT);
  }
}

void setDataOut() {
  // Set the RAM data out lines (Digital 2 thru 9) to output
  for (int i = 2; i <= 9; i++) {
    // Set the corresponding digital pin (used as digital output) to HIGH (1) or LOW (0)
    pinMode(i, OUTPUT);
  }
}
uint16_t GetAddress() {
  int pin = 0;
  int value = 0;
  int bitValue = 0;
  uint16_t Address = 0;

  //Serial.println("Getting The Address");
  // Read the Lower 8 Bits and then the Upper 8 Bits
  // Data buffer direction is hard wired
  // enable the data buffer
  delay(10);
  // Select the low address 8 bits buffer A-Low B-High
  digitalWrite(DataBufferPinA, 0);
  digitalWrite(DataBufferPinB, 1);

  delay(10);

  // traverse the 8 pins and convert to a value (0-255)
  setDataIn();  // set the 8 digital pins to input to read in the lower byte of the address
                // traverse the 8 pins and convert to a value (0-255)

  for (pin = 2; pin <= 9; pin++) {
    //    Serial.println(pin);

    bitValue = digitalRead(pin);  // Read the state of the pin (HIGH or LOW).
    // If the pin is HIGH (representing a '1' bit), set the corresponding bit in 'value'.
    if (bitValue == 1) {
      value |= (1 << (pin - 2));  // Use bitwise OR and left bit shift to set the bit.
                                  //      if (pin <= 6) {
                                  //        Serial.println("This pin is 1");
                                  //      }
    } else {
      if (pin <= 6) {
        //        Serial.println("This pin is 0");
      }
    }
  }
  //Serial.print("Lo Address =");
  //Serial.println(value, BIN);
  //  Serial.print("Data In =");
  //  Serial.println(value);

  Address = value;
  value = 0;

  // Now go get the upper 8 bits (byte) of the address
  // Read the Lower 8 Bits and then the Upper 8 Bits
  // Data buffer direction is hard wired
  // enable the data buffer
  delay(100);
  // Select the low address 8 bits buffer A-High B-Low
  digitalWrite(DataBufferPinA, 1);
  digitalWrite(DataBufferPinB, 0);

  delay(100);

  // traverse the 8 pins and convert to a value (0-255)

  for (pin = 2; pin <= 9; pin++) {
    //   Serial.println(pin);

    bitValue = digitalRead(pin);  // Read the state of the pin (HIGH or LOW).
    // If the pin is HIGH (representing a '1' bit), set the corresponding bit in 'value'.
    if (bitValue == 1) {
      value |= (1 << (pin - 2));  // Use bitwise OR and left bit shift to set the bit.
      if (pin <= 6) {
        //        Serial.println("This pin is 1");
      }
    } else {
      if (pin <= 6) {
        //        Serial.println("This pin is 0");
      }
    }
  }
  //Serial.print("Hi Address =");
  //Serial.println(value, BIN);
  //  Serial.print("Data In =");
  //  Serial.println(value);
  // put the 2 bytes (16 bits together)
  Address = (value * 256) + Address;
  //Serial.print("RAM Address =");
  //Serial.println(Address, BIN);
  return Address;
}


void StoreData(uint16_t RamAddress, byte RamData) {  // Given the Address and Data, store the data value in the RAMArray
  RAMArray[RamAddress] = RamData;
  //Serial.print("Storing >");
  //Serial.println(RAMArray[RamAddress]);
  //Serial.print("Location >");
  //Serial.println(RamAddress);
}


byte GetDataToStore() {
  int pin = 0;
  int value = 0;
  int bitValue = 0;

  //Serial.println("Save Data to RAM");


  digitalWrite(DataBufferPinA, 0);
  digitalWrite(DataBufferPinB, 0);
  // enable the data buffer
  //delay(100);
  // set the flow of the the Octal Buffer to read from the CPU
  digitalWrite(ReadWritePin, 1);
  //delay(10);

  // traverse the 8 pins and convert to a value (0-255)
  setDataIn();  // set the data pins to input
                // traverse the 8 pins and convert to a value (0-255)

//Serial.print("Pin 2 (0)>>>");
//Serial.println(digitalRead(2));

delay(200);


  for (pin = 2; pin <= 9; pin++) {
    //   Serial.println(pin);

    bitValue = digitalRead(pin);  // Read the state of the pin (HIGH or LOW).
    // If the pin is HIGH (representing a '1' bit), set the corresponding bit in 'value'.
    if (bitValue == 1) {
      value |= (1 << (pin - 2));  // Use bitwise OR and left bit shift to set the bit.
      if (pin <= 6) {
      //         Serial.println("This pin is 1");
      }
    } else {
      if (pin <= 6) {
      //          Serial.println("This pin is 0");
      }
    }
  }
  //Serial.print("Data In =");
  //Serial.println(value, BIN);
//  Serial.print("Data In =");
//  Serial.println(value);


//delay(5000);
// TestHalt = 1;

  return value;
}

void PublishData(byte value) {
  // Set Data Lines to Output, latch values in place
  setDataOut();

//  Serial.print("Publishing >> ");
  Serial.print(value);

  // Iterate through the bits of the byte
  for (int i = 0; i < 8; i++) {
    // Determine the pin number based on the loop counter
    int pin = i + 2;

    // Read the bit and set the corresponding pin
    if (bitRead(value, i)) {
      digitalWrite(pin, HIGH);
    } else {
      digitalWrite(pin, LOW);
    }
  }  // for

//  Serial.println("Latching");

  // the data buffer direction to write to the CPU
  digitalWrite(ReadWritePin, 0);
  // enable the data buffer
  // Select the data buffer A- Low B-Low
  delay(100);
  digitalWrite(DataBufferPinA, 0);
  digitalWrite(DataBufferPinB, 0);
  /*  // the data is now available to be latched
//  delay(10);  // let the 8 bit latch settle
  // set the "clock" on the latch low and back high
  digitalWrite(DataBufferPinA, 1);
  digitalWrite(DataBufferPinB, 1);
//delay(10);
  digitalWrite(DataBufferPinA, 0);
  digitalWrite(DataBufferPinB, 0);
*/

  // latch
  digitalWrite(LatchOutPin, 1);
  delay(20);
  //Serial.println("Latch High");
  //delay(3000);
  digitalWrite(LatchOutPin, 0);
  //Serial.println("Latch LOW");
  delay(200);
  //delay(3000);
  digitalWrite(LatchOutPin, 1);
  //Serial.println("Latch HIGH");
  delay(20);
  //delay(3000);


  // the data is now available to the CPU
//  Serial.println("Done Latching pause");
}


void setup() {
  // put your setup code here, to run once:
  // Set the RAM Address and Data Lines to Input

  // detfault Set "ram" for output mode
  // Set Data Lines to Output
  setDataOut();

  // Set Buffer Selection - Binary lines to Output
  pinMode(DataBufferPinA, OUTPUT);
  pinMode(DataBufferPinB, OUTPUT);

  // Set Control Lines
  pinMode(BarWriteEnablePin, INPUT);  // BAR - Right Enable (Active Low) - when CPU wants to write to memory

  pinMode(ReadWritePin, OUTPUT);  // Flow control for data

  pinMode(LatchOutPin, OUTPUT);

  digitalWrite(LatchOutPin, 1);



  Serial.begin(9600);  // Initialize serial communication for debugging

  // testing  - progam below adds by three
  RAMArray[0] = 83;
  RAMArray[1] = 79;
  RAMArray[2] = 43;
  RAMArray[3] = 47;
  RAMArray[4] = 224;
  RAMArray[5] = 99;
  Serial.println("Exiting Setup");
}

void loop() {

  if (TestHalt == 0) {  // stop executing just loop
//    Serial.println("Not Halted");
    //  if (FirstTimeThru == 1) {                     // Execute Code
    FirstTimeThru = 0;                          // so we don't repeat this code
    if (digitalRead(BarWriteEnablePin) == 0) {  // Then the CPU is trying to write to the RAM

      // only when the write enable transistion from read to write (1 to 0) do we pay attention to the write...

      if (lastWriteEnableState == 1) {  // we just trasitioned from Read to Write (1 to 0) so we will write the data value to the "RAM"
        lastWriteEnableState = 0;       // so next loop we ignore
        // let things settle on the ttl side
      
//        Serial.println("Running Store code");
        RamAddress = GetAddress();

        Serial.print("Store Addr is >>");
        Serial.println(RamAddress, BIN);

        RamData = GetDataToStore();
        Serial.print("Data is >>");
        Serial.println(RamData, BIN); 

        if (RamData != lastData) {
          updateFlag = 1;
          lastData = RamData;
        }
        if (RamData == 0) {
          TestHalt = 1;
          Serial.println("Test Halt Activated");
        }


        if (RamAddress != lastAddress) {  // publish its not the same
          updateFlag = 1;                 // there is a change so publish
          lastAddress = RamAddress;
        }


        if (updateFlag == 1) {  // then the data or the address has changed
          updateFlag = 0;
          lastData = RamData;
          lastAddress = RamAddress;

          Serial.print("Value to store in RAM> ");
          Serial.println(RamData, BIN);
          StoreData(RamAddress, RamData);  // given the passed in address, get the data from the data bus and store it in the RAM at the address location
          // once we transition from Write to Read (0 to 1) then publish the value out to the CPU.
        }
//        Serial.println("back from Store code");
      }
    }

    else {
      // The CPU is trying to read from the RAM
      // read from RAM at the address provided and send the value out on the data lines
      if (lastWriteEnableState == 0) {  // we just transition from Write to Read. Publish the value  vaalue written to RAM then look around
        lastWriteEnableState = 1;
        PublishData(RamData);  // Send the retrieved value out to the CPU so it shows up on the lLEDs
                               //Serial.print("Value stored @ location = ");
                               //Serial.println(RAMArray[RamAddress], BIN);
      } else {
//        Serial.println("running the Read code");
        lastWriteEnableState = 1;  // Reset to show that we are now in read mode
        RamAddress = GetAddress();

        if (lastAddress != RamAddress) {  // address changed so go get value from RAM and present to the CPU

        Serial.print("Address is >>");
        Serial.println(RamAddress, BIN);

          lastAddress = RamAddress;
          // get the value from the array
          RamData = RAMArray[RamAddress];

/*
          if (RamData == 0) {
            //  TestHalt = 1;
            //  Serial.println("Test HALT Tripped!");
          }
*/
          Serial.print("Value to the CPU> ");
          Serial.println(RamData, BIN);

          PublishData(RamData);  // Send the retrieved value out to the CPU
//          Serial.println("back from the Read code");
        }
      }
    }
  }
}
