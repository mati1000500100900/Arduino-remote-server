#include <SPI.h>
#include <Ethernet.h>

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 0, 177);
String ipstr;

EthernetServer server(80);

void setup() {
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  Ethernet.begin(mac, ip);
  server.begin();
  ipstr=ip2string(ip);
}

String ip2string(IPAddress ip) {
  return String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
}

void loop() {
  EthernetClient client = server.available();
  String usrReq;
  if (client) {
    boolean currentLineIsBlank = true;
    usrReq = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        usrReq += c;
        if (c == '\n' && currentLineIsBlank) {
          if (usrReq.indexOf("GET / HTTP") == -1 && usrReq.indexOf("GET /favicon.ico HTTP") == -1) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type:  application/json");
            client.println("Connection: close");
            client.println();
            int pin = usrReq.substring(5, 6).toInt();
            int val = usrReq.substring(7, 8).toInt();
            if (pin >= 4 && pin <= 7) {
              digitalWrite(pin, val);
            }
            client.print("{\"status\":[");
            for (int i = 0; i <= 7; i++) {
              client.print(digitalRead(i));
              if (i != 7) client.print(",");
            }
            client.println("]}");
          }
          else {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            client.println("<!DOCTYPE html>");
            client.println("<meta charset=\"utf-8\" /> ");
            client.println("<html>");
            client.println("<table border=1><tr><td colspan=2>Wejścia</td><td colspan=2>Wyjścia</td></tr><tr><td>Pin 0</td><td id=p0>0</td><td>Pin 4</td><td id=p4><a onclick='set(4,1)'>Wł</a></td></tr><tr><td>Pin 1</td><td id=p1>0</td><td>Pin 5</td><td id=p5><a onclick='set(5,1)'>Wł</a></td></tr><tr><td>Pin 2</td><td id=p2>0</td><td>Pin 6</td><td id=p6><a onclick='set(6,1)'>Wł</a></td></tr><tr><td>Pin 3</td><td id=p3>0</td><td>Pin 7</td><td id=p7><a onclick='set(7,1)'>Wł</a></td></tr></table>");
            client.flush();
            client.print("<script>function a(x){for(i=0;i<4;i++)document.getElementById('p'+i).innerText=x[i];for(i=4;i<8;i++)document.getElementById('p'+i).innerHTML=\"<a onclick='set(\"+i+\",\"+(1-x[i])+\")'>\"+onoff(x[i])+\"</a>\";}setInterval(function(){set(0,0)},1000);function set(pin,val){var r=new XMLHttpRequest();r.onreadystatechange=function(){if(r.readyState==4&&r.status==200){j=eval(\"(\"+r.responseText+\")\");a(j.status)}},r.open(\"GET\",\"http://");
            client.print(ipstr);
            client.println("/\"+pin+\"/\"+val,true);r.send(null)}</script>"); 
            client.println("<script>function onoff(x){return x?'Wył':'Wł'}</script>");
            client.flush();
            client.println("<style>#p4,#p5,#p6,#p7 {cursor: pointer;}</style>");
          }
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
  }
}
