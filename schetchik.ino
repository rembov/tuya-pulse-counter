#include <Arduino.h>
#include <TuyaWifi.h>

TuyaWifi my_device;

#define SENSOR1_PIN D1
#define SENSOR2_PIN D2
#define RESET_PIN   D3

#define DP_ID_1 1
#define DP_ID_2 2

volatile unsigned long counter1 = 0;
volatile unsigned long counter2 = 0;
unsigned long last_send = 0;

void ICACHE_RAM_ATTR pulse1() { counter1++; }
void ICACHE_RAM_ATTR pulse2() { counter2++; }

unsigned char pid[] = "xxxxxxxxxxxxxxxx";
unsigned char mcu_ver[] = "1.0.0";

unsigned char dp_array[][2] = {
  {DP_ID_1, DP_TYPE_VALUE},
  {DP_ID_2, DP_TYPE_VALUE}
};

unsigned char dp_process(unsigned char dpid, const unsigned char value[], unsigned short length) {
  return 0;
}

void dp_update_all(void) {
  my_device.mcu_dp_update(DP_ID_1, counter1 * 100, 4);
  my_device.mcu_dp_update(DP_ID_2, counter2 * 100, 4);
}

void setup() {
  pinMode(SENSOR1_PIN, INPUT_PULLUP);
  pinMode(SENSOR2_PIN, INPUT_PULLUP);
  pinMode(RESET_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SENSOR1_PIN), pulse1, FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR2_PIN), pulse2, FALLING);
  Serial.begin(115200);
  my_device.init(pid, mcu_ver);
  my_device.set_dp_cmd_total(dp_array, 2);
  my_device.dp_process_func_register(dp_process);
  my_device.dp_update_all_func_register(dp_update_all);
  last_send = millis();
}

void loop() {
  my_device.uart_service();
  if (digitalRead(RESET_PIN) == LOW) {
    while (digitalRead(RESET_PIN) == LOW);
    my_device.mcu_reset_wifi();
  }
  if (millis() - last_send > 10000) {
    my_device.mcu_dp_update(DP_ID_1, counter1 * 100, 4);
    my_device.mcu_dp_update(DP_ID_2, counter2 * 100, 4);
    last_send = millis();
  }
}

