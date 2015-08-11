#include <SPI.h>
#include <Ethernet.h>

#include <LiquidCrystal.h>
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

int vent = 3;
int ca=0,c=0,canal=0;
int estvent=0;int settem=2500;int setman=0;
int temp=0;

byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};

IPAddress server(192, 168, 137, 1);

EthernetClient client;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(vent,'OUTPUT');
  
  lcd.setCursor(0, 0);
  lcd.print("Search DHCP");
  lcd.setCursor(0, 1);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    lcd.setCursor(0, 0);
    lcd.print("DHCP Failed");
    // no point in carrying on, so do nothing forevermore:
    for (;;)
      ;
  }
  lcd.setCursor(0, 0);
  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    lcd.print(Ethernet.localIP()[thisByte], DEC);
    lcd.print(".");
  }

  // give the Ethernet shield a second to initialize:
  delay(2000);
  Serial.println("connecting...");
  lcd.clear();

}

void loop() {
  // put your main code here, to run repeatedly:
  temp = ((analogRead(A2)*0.488758553274682)-2)*100;
  Serial.println(temp);
  lcd.setCursor(9,0);
  int deci=temp%100;
  lcd.print(temp/100);
  lcd.print(".");
  lcd.print(deci);
  lcd.setCursor(14,0);
  lcd.print(" C");
  lcd.setCursor(9,1);
  lcd.print("vent=");
  if (digitalRead(vent)==0){
  lcd.print("OF");
  }
  else{
    lcd.print("ON");
  }
  control();
  
  client.stop();
    client.flush();
    Serial.println();
    delay(500);
  if (client.connect(server, 80)) {
    client.print("GET /scada/server/dispositivo.php?id=1&getEstado=");
    client.print(digitalRead(vent));
    client.print("&getTemperatura=");
    client.println(temp);
    client.stop();
    client.flush();
    Serial.println();
    lcd.setCursor(0,0);
    lcd.print("Conected");
    delay(200);
  }
    else{
      lcd.setCursor(0,0);
      lcd.print("#E/Serv");
      loop();
    }

    
    
  if (client.connect(server, 80)) {
    //Serial.println("connected");
    client.println("GET /scada/server/dispositivo.php?id=1"); // the HTTP request
  }
  else {
  // if you didn't get a connection to the server:
  //  Serial.println("connection failed");
      lcd.setCursor(0,0);
      lcd.print("#E/Server");
      loop();
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
  control();
  
  
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

void control(){
  
  lcd.setCursor(0,1);
  lcd.print("set=");
  int deci=settem%100;
  lcd.print(settem/100);
  lcd.print(".");
  lcd.print(deci);
  
  lcd.setCursor(9,1);
  lcd.print("vent=");
  if (digitalRead(vent)==0){
  lcd.print("OF");
  }
  else{
    lcd.print("ON");
  }
  if (setman==0){
    if (temp>settem){
      digitalWrite(vent,1);
    }
    else{
      digitalWrite(vent,0);
    }
  }else if(setman==1){
    if (estvent==1){
      digitalWrite(vent,1);
    }
    else {
      digitalWrite(vent,0);
    }
  }
}

