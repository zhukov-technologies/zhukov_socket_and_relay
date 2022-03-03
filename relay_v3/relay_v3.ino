#include <espnow.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <TimerMs.h>
#include "EncButton.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

EncButton<EB_TICK, 4> butt(INPUT_PULLUP);

TimerMs tmr_sync_channel(60000, 1, 0);
TimerMs tmr_wi_fi(10 * 60000, 1, 0);

uint8_t broadcastAddress[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

const char* WIFI_SSID = "MYEWWT";

struct {
  byte ID;
  byte value;
  char WIFI_ROUTER_SSID[35] = "MYEWWT";
  char WIFI_ROUTER_PASS[65] = "iloveyou";
} from_MYEWWT;

struct {
  byte ID;
  byte value;
} to_MYEWWT;

void setup() {
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);

  Serial.begin(115200);
  Serial.println();
  EEPROM.begin(512);

  set_channel();

  if (esp_now_init() != 0) {
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_recv_cb(OnDataRecv);


  EEPROM.get(300, from_MYEWWT.WIFI_ROUTER_PASS);
  EEPROM.get(400, from_MYEWWT.WIFI_ROUTER_SSID);
  WiFi.begin(from_MYEWWT.WIFI_ROUTER_SSID, from_MYEWWT.WIFI_ROUTER_PASS);

  butt.setHoldTimeout(7000);
}
void loop() {

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("Пробуем обновиться");

    WiFiClient client;

    ESPhttpUpdate.onStart(update_started);
    ESPhttpUpdate.onEnd(update_finished);
    ESPhttpUpdate.onProgress(update_progress);
    ESPhttpUpdate.onError(update_error);

    t_httpUpdate_return ret = ESPhttpUpdate.update(client, "http://www.cg10019.tmweb.ru/relay_v2.bin");

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        WiFi.disconnect();
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        WiFi.disconnect();
        break;

      case HTTP_UPDATE_OK:
        Serial.println("UPDATE_OK");
        break;
    }
  }


  butt.tick(); //обработка кнопки

  if (tmr_sync_channel.tick()) { //если сработал таймер
    Serial.println("Сработал таймер, настраиваем канал");
    set_channel(); //настраиваем wi-fi канал
  }

  if (tmr_wi_fi.tick()) {
    Serial.println("сработал таймер, включаем wi-fi");
    EEPROM.get(300, from_MYEWWT.WIFI_ROUTER_PASS);
    EEPROM.get(400, from_MYEWWT.WIFI_ROUTER_SSID);
    WiFi.begin(from_MYEWWT.WIFI_ROUTER_SSID, from_MYEWWT.WIFI_ROUTER_PASS);
  }



  if (butt.click()) { //если кликнули на кнопку
    Serial.println("клик");
    from_MYEWWT.ID == 0;
    set_relay(); //функция включения/выключения реле
  }

  if (butt.held()) { //если кнопка была зажата
    Serial.println("кнопка зажата");
    for (int i = 0; i < 6; i++) {
      broadcastAddress[i] = 0xFF;
    }
    to_MYEWWT.ID = 0; //запрос на добавление к MYEWWT
    Serial.println("Отправляем пакет");
    esp_now_send(broadcastAddress, (uint8_t *) &to_MYEWWT, sizeof(to_MYEWWT));
  }
}

void set_relay() {

  if (to_MYEWWT.value == 1)
  {
    Serial.println("Выключено");
    to_MYEWWT.value = 0;
    digitalWrite(12, LOW);
  }

  else if (to_MYEWWT.value == 0)
  {
    Serial.println("Включено");
    to_MYEWWT.value = 1;
    digitalWrite(12, HIGH);
  }

  if (from_MYEWWT.ID == 1) //если запрос от MYEWWT и требуется ответ
  {
    Serial.println("Отправляем ответ к MYEWWT");
    to_MYEWWT.ID = 1;
    esp_now_send(broadcastAddress, (uint8_t *) &to_MYEWWT, sizeof(to_MYEWWT));
  }
}

void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
  memcpy(&from_MYEWWT, incomingData, sizeof(from_MYEWWT));

  for (int i = 0; i < 6; i++) {
    broadcastAddress[i] = mac_addr[i];
  }

  if (from_MYEWWT.ID == 1)
  {
    Serial.println("Запрос от MYEWWT");
    Serial.println("Записываем ssid&pass в EEPROM");
    EEPROM.put(300, from_MYEWWT.WIFI_ROUTER_PASS);
    EEPROM.put(400, from_MYEWWT.WIFI_ROUTER_SSID); //если значение не изменено, перезапись не выполняется
    EEPROM.commit();
    if (from_MYEWWT.value == 2) //запрос на включение/выключение
    {
      Serial.println("Запрос на включение/выключение");
      set_relay();
    }

    else if (from_MYEWWT.value == 3) //запрос состояния прибора
    {
      Serial.println("Запрос состояния, отправляем пакет.");
      esp_now_send(broadcastAddress, (uint8_t *) &to_MYEWWT, sizeof(to_MYEWWT));
    }
  }

  if (from_MYEWWT.ID == 4)
  {
    Serial.println("Запрос от выключателя");
    if (from_MYEWWT.value == 2) //запрос на включение/выключение
    {
      Serial.println("Запрос на включение/выключение");
      set_relay();
    }
  }



}


int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
    for (uint8_t i = 0; i < n; i++) {
      if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
        return WiFi.channel(i);
      }
    }
  }
  return 0;
}


void set_channel() {

  int32_t channel;
  channel = getWiFiChannel(WIFI_SSID);

  if (channel >= 1) {
    Serial.print("Настраиваем основной канал: ");
    Serial.println(channel);
    wifi_promiscuous_enable(1);
    wifi_set_channel(channel);
    wifi_promiscuous_enable(0);
  }

  else {

    EEPROM.begin(512);
    EEPROM.get(400, from_MYEWWT.WIFI_ROUTER_SSID);
    channel = getWiFiChannel(from_MYEWWT.WIFI_ROUTER_SSID);
    if (channel >= 1) {
      Serial.print("Настраиваем второстепенный канал: ");
      Serial.println(channel);
      wifi_promiscuous_enable(1);
      wifi_set_channel(channel);
      wifi_promiscuous_enable(0);
    }
  }
}



void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
  WiFi.disconnect();
}
