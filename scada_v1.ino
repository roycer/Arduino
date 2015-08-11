#include <SPI.h>
#include <Ethernet.h>

int ca=0,c=0,canal=0;
int estvent=-1;int settem=0;int setman=-1;
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};

IPAddress server(192, 168, 137, 1);

EthernetClient client;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(8,'OUTPUT');
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for (;;)
      ;
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");
  }

  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

}

void loop() {
  // put your main code here, to run repeatedly:
  int temp = ((analogRead(A2)*0.488758553274682)-2)*100;
  Serial.println(temp);
  client.stop();
    client.flush();
    Serial.println();
    delay(500);
  if (client.connect(server, 80)) {
    client.print("GET /scada/server/dispositivo.php?id=1&getEstado=");
    client.print(digitalRead(8));
    client.print("&getTemperatura=");
    client.println(temp);
    client.stop();
    client.flush();
    Serial.println();
    delay(200);
  }
    
    
  if (client.connect(server, 80)) {
    //Serial.println("connected");
    client.println("GET /scada/server/dispositivo.php?id=1"); // the HTTP request
  }
  else {
  // if you didn't get a connection to the server:
  //  Serial.println("connection failed");
  }
  
  delay(200);
  
  if (client.connected()) {
     while (client.available()) {
      c=client.read();
      //Serial.println(c);
        if (c==10){
          estvent=client.read()-48;
        }
        if (c==44){
          settem= (client.read()-48)*1000;
          settem=settem+(client.read()-48)*100;
          settem=settem+(client.read()-48)*10;
          settem=settem+(client.read()-48);
        }
        if (c==46){
          setman=(client.read()-48);
        }
    }
    Serial.println(settem);
    Serial.println(estvent);
    Serial.println(setman);
    
  Serial.println();
}
 else {
    client.stop();
    client.flush();
    Serial.println();
    
 }
 
  if (setman==0){
    if (temp>settem){
      digitalWrite(8,1);
    }
    else{
      digitalWrite(8,0);
    }
  }else if(setman==1){
    if (estvent==1){
      digitalWrite(8,1);
    }
    else {
      digitalWrite(8,0);
    }
  }
  
  /*
 
  if (temp>settem || (setman==1 && estvent==1)){
    digitalWrite(8,1);
  }
  else if (setman==1 && estvent==0){
    digitalWrite(8,0);
  }
  else{
    digitalWrite(8,0);
  }
  */
  
  delay(10);
}
