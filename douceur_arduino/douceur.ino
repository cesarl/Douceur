void setup() {
  // put your setup code here, to run once:
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.begin(115200);
}

void AnalogSend(int port)
{
  Serial.print(analogRead(port));
  Serial.print(' ');
}

void serialEvent()
{
  bool serialAvailable = false;
  while (Serial.available())
  {
    Serial.read();
    serialAvailable = true;
  }
  if (serialAvailable)
  {
    
    for (int i = 0; i < 128; ++i)
    {
        AnalogSend(i % 6);
    }
    Serial.print('@');  
  }
}

void loop()
{

}
