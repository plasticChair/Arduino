#include <ESP8266WiFi.h>

// const char* host = "utcnist2.colorado.edu";
const char* host = "128.138.141.172";
String TimeDate = "";

void setup()
{
  Particle.begin();
  Particle.println("time test sketch");
}

void loop()
{
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 13;

  if (!client.connect(host, httpPort)) {
    Particle.println("connection failed");
    return;
  }

  // This will send the request to the server
  client.print("HEAD / HTTP/1.1\r\nAccept: */*\r\nUser-Agent: Mozilla/4.0 (compatible; ESP8266 Arduino Oak;)\r\n\r\n");

  delay(100);

  // Read all the lines of the reply from server and print them to Serial
  // expected line is like : Date: Thu, 01 Jan 2015 22:00:14 GMT
  char buffer[12];
  String dateTime = "";

  while (client.available())
  {
    String line = client.readStringUntil('\r');

    if (line.indexOf("Date") != -1)
    {
      Particle.print("=====>");
    }
    else
    {
      //Particle.print(line);

      TimeDate = line.substring(7);
      //Particle.println(TimeDate);

      // date starts at pos 7
      TimeDate = line.substring(7, 15);
      TimeDate.toCharArray(buffer, 10);
      Particle.print("UTC Date/Time: ");
      Particle.print(buffer);

      // time starts at pos 14
      TimeDate = line.substring(16, 24);
      TimeDate.toCharArray(buffer, 10);
      Particle.print(" ");
      Particle.println(buffer);
    }
  }

  delay(10000);

}
