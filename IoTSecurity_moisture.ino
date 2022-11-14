#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

// Update these with values suitable for your network.
const char* ssid = "elsayed";
const char* mqtt_pass = "12345778";

//const char* ssid = "Modern 4G Wi-Fi_75AA";
//const char* mqtt_pass = "02732640";

const int soilpin = A0; 

const char* awsEndpoint = "a1zt2nx5nt3gfd-ats.iot.us-east-1.amazonaws.com";

// For the two certificate strings below paste in the text of your AWS
// device certificate and private key:

// xxxxxxxxxx-certificate.pem.crt
static const char certificatePemCrt[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUNdsIpCkVfY/mfIsazw/LLKwRhFcwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIyMTAyMzE0MzUz
NFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOwpcg+kswYmrbse5aG5
gvxQIP/6Jk9pExocS5PcDpBPHvGmFbrmlD5G7ym8pl2VLrzt+p7Jp1M/lj/1C8+L
b9tANoMzIYrMauL/0dCbQlloe/jgBonAfQu1BwZT9WX0m7trv6+G0AHMuIZYr/Ct
gF01aR7gIkwYYhoYXFleQHm5zHruTPFnCqEuVRwhM/Mk/0Hw8Mp0E9CsdxzxvWUg
BbkjEOW/hwQ1F7YuZAi2CgoZ4mZY9zQcJwOURKnMlwf1fcHnBkHVvKGChznDbt77
h57CJxmjH/yyQM1l/xjV9yjr/KGXrqPukcDww4jJUd9w5jTHuCiPYBB2cu4t6ffB
+BkCAwEAAaNgMF4wHwYDVR0jBBgwFoAUhCK/YQHAUzXafTAmVJ5M5fhfg3gwHQYD
VR0OBBYEFFyBOasXs7MU2z2yZtMuSKPDUaxjMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQC0Qzo78vtwAkiEOtyRLHJ3VstD
IQ4bhvfdQmKE0DGnW8EAOXyxjZ8VDpzzN9NEETqTxJTYfsZnF5cuibrlY2kZ6jXv
F/GKL5kEefNYpySmcZgOnad/DVx/ZSEh53nysvntF0R6Kyj9SvepvlTLTMx1p3ul
HQyO9hNuJH4F8e0Q6JxAgjdi7FvEMMf0UjW1XRABRFQ9lgH0DVpYBwfqInKsVxZ+
RJwNGjoWQBGuewRtEVWwK0fGQjoqyaQLoZQXh0q2+O5BSGnEgVC4+YuqCUfXjOrH
E+eaFI9vURquuSyycl45lVEV2Pny5nLSeS7cmH8ameyXGoSeTKTlKitORq6Z
-----END CERTIFICATE-----


)EOF";

// xxxxxxxxxx-private.pem.key
static const char privatePemKey[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEA7ClyD6SzBiatux7lobmC/FAg//omT2kTGhxLk9wOkE8e8aYV
uuaUPkbvKbymXZUuvO36nsmnUz+WP/ULz4tv20A2gzMhisxq4v/R0JtCWWh7+OAG
icB9C7UHBlP1ZfSbu2u/r4bQAcy4hliv8K2AXTVpHuAiTBhiGhhcWV5AebnMeu5M
8WcKoS5VHCEz8yT/QfDwynQT0Kx3HPG9ZSAFuSMQ5b+HBDUXti5kCLYKChniZlj3
NBwnA5REqcyXB/V9wecGQdW8oYKHOcNu3vuHnsInGaMf/LJAzWX/GNX3KOv8oZeu
o+6RwPDDiMlR33DmNMe4KI9gEHZy7i3p98H4GQIDAQABAoIBAFjEs2UB6XD0dPUT
BgmmK4AftmfBZUkQG+gCnr3zR14i/2ShoyVfqnVGQHuVt4sRmkHVyhCJh+WYzyc3
OrtGlEL7fgIO5BrB/6NXuUwKMARfa8n+mzaILfiKkRtlz6YFLcxjEegual1i0DFf
gLKc5NUkUGEUqo+j0qwwX/YPoXifDyymBsmVvGScjlQVLnb0fD/xgmHqEYxxbvBi
Pm4pcgtOZyw0Qh6Ssm8rz/B2ir66b60PKcs17PfcDqIwvNC7sW5cSRcjLMgyD9CJ
yTYfsrEjvRLB1GZBi5rJDJFbPrfIGranrO40mpMHHOErvI29aBhxTAnqCGbPXofF
/T5Lp8kCgYEA/2cjG/iAE44o0OXqJJmhhL47UF10MimmPQKyXCppIFimIz6997du
18VjYmmMyvQ2LhHIpSVLsvkOUwqof59L41IsSiddJOTGDGVMhA9PYMcs/Aro+0pI
oG1EyjqlfixzBVlSV1tIwF7QjnPkoe3/wJhlglPzqrES7i+gmdKUVD8CgYEA7LbK
2AUDldeBnBDEUzV8C8rLUnL7EKlCQiXf3hjbrpo+LPR5mz4CU5/rNd7cJRzcV7M0
5wPSglJ3vjgz9c5gIQ5BDGt8Rbqg1iut9OR9JmWeNQdhgdSVISF1U8UayzgNcZcE
uhfvKFPpoPhJqQGyGXlzmgorKIeRMD6Ol1QUPacCgYAf+rwMrorq5+v4bDFrL8ln
o6Itp+X6ufhUv1ex7qWHJVJ4K00CDXfVkKOnGOxGJVn7XsKu2wKlgEXo4/bBNCL+
bbWZ+1/QVMzvlGeGSZx407ewP3V4sY5yd/wbDkjWiZ5FVy/WPFnZrIWUsLI6DGDv
rqVXMkfaYZ2rMU98sz9bZQKBgQDobxT+yO2cVOCPvmcIzZvgeGDttLmSwqCtE0Of
cglEiXBD9Er0hQ8DziPH4e2JA63Y778imD48eX3a6pUa6Gq8CqMU7vY4AemsY3eM
4E+DRIT0zSqRsYw92/y5JPIKWh/ZSdD6akYL81mVFjg+G8wl93vr5dA7G+MU3Fhn
f8fgQQKBgEnOIIh7JC1TC4HPJCzSeUPEL5eBkfo9OyxqMYov6w+qnoU3F3e43ZE7
tbejiMaJWNMzlDW1oFR8PTRHRaw7ALdQtvr3wk9pzCUl4R/tAtmJkesYEaoBxgNm
hveOcvE8oryjDYpp04pLdj+Vai7S01beCHtCY+NY599xj6nUgFZJ
-----END RSA PRIVATE KEY-----


)EOF";

// This is the AWS IoT CA Certificate from: 
// https://docs.aws.amazon.com/iot/latest/developerguide/managing-device-certs.html#server-authentication
// This one in here is the 'RSA 2048 bit key: Amazon Root CA 1' which is valid 
// until January 16, 2038 so unless it gets revoked you can leave this as is:
static const char caPemCrt[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----

)EOF";

BearSSL::X509List client_crt(certificatePemCrt);
BearSSL::PrivateKey client_key(privatePemKey);
BearSSL::X509List rootCert(caPemCrt);


WiFiClientSecure wiFiClient;
PubSubClient pubsubclient(awsEndpoint, 8883,wiFiClient);

void setCurrentTime() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: "); Serial.print(asctime(&timeinfo));
}


void setup_wifi() {
  // Connecting to a WiFi network
  WiFi.begin(ssid, mqtt_pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  setCurrentTime();
  wiFiClient.setClientRSACert(&client_crt, &client_key);
  wiFiClient.setTrustAnchors(&rootCert);
}



void reconnect() {
  // Loop until we're reconnected
  Serial.println("In reconnect...");
  while (!pubsubclient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (pubsubclient.connect(mqtt_pass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(pubsubclient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();
  
 // client.setServer(mqtt_server, 1883);
}

void pubSubCheckConnect() {
  if ( ! pubsubclient.connected()) {
    Serial.print("pubsubclient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubsubclient.connected()) {
      Serial.print(".");
      pubsubclient.connect("esp-iot");
    }
    Serial.println(" connected");
    pubsubclient.subscribe("inTopic");
  }
  pubsubclient.loop();
}

void loop() {

  pubSubCheckConnect();
  char msg[8];
  if (!pubsubclient.connected()) {
    reconnect();
  }

  sprintf(msg,"%i",analogRead(soilpin));
  pubsubclient.publish("iot/sensors/soil", msg);
  Serial.print("Soil moisture value:");
  Serial.println(msg);

  delay(5000);
}
