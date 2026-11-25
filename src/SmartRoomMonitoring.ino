#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_BME280 bme;

const int IR_ENTRY_PIN = 14;
const int LDR_PIN = 34;
const int ROOM_LED_PIN = 26;

volatile int occupancyCount = 0;
volatile float globalTemp = 0.0;
volatile float globalHumid = 0.0;
volatile int globalLight = 0;

SemaphoreHandle_t entrySemaphore;
SemaphoreHandle_t i2cMutex;
bool displayConnected = false;
bool bmeConnected = false;

void IRAM_ATTR irEntryISR() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(entrySemaphore, &xHigherPriorityTaskWoken);
  if (xHigherPriorityTaskWoken) {
    portYIELD_FROM_ISR();
  }
}

void TaskIR(void *pvParameters) {
  (void)pvParameters;
  uint32_t lastEntryTime = 0;

  for (;;) {
    if (xSemaphoreTake(entrySemaphore, portMAX_DELAY) == pdTRUE) {
      uint32_t currentTime = millis();
      if (currentTime - lastEntryTime > 400) {
        if (digitalRead(IR_ENTRY_PIN) == LOW) {
          occupancyCount++;
          Serial.print("[SENSOR EVENT] Object Counted! Total: ");
          Serial.println(occupancyCount);
          lastEntryTime = currentTime;
        }
      }
    }
  }
}

void TaskDisplay(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    if (displayConnected) {
      if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.print("STABLE MONITOR SYSTEM");
        display.drawFastHLine(0, 10, 128, SSD1306_WHITE);
        display.setCursor(0, 18);
        display.print("Total Count : ");
        display.print(occupancyCount);
        display.setCursor(0, 30);
        display.print("Temp        : ");
        display.print(globalTemp, 1);
        display.print(" C");
        display.setCursor(0, 42);
        display.print("Humidity    : ");
        display.print(globalHumid, 1);
        display.print(" %");
        display.setCursor(0, 54);
        display.print("Light Lvl    : ");
        display.print(globalLight);
        display.display();
        xSemaphoreGive(i2cMutex);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(400));
  }
}

void TaskLDR(void *pvParameters) {
  (void)pvParameters;
  static bool lastLedState = false;

  for (;;) {
    int currentReading = analogRead(LDR_PIN);
    if (abs(currentReading - globalLight) > 30) {
      Serial.print("[LDR TASK] Light Level Shift Detected: ");
      Serial.println(currentReading);
    }
    globalLight = currentReading;

    if (globalLight < 2000) {
      digitalWrite(ROOM_LED_PIN, LOW);
      if (lastLedState == true) {
        Serial.println("[LIGHT CONTROL] Room is BRIGHT. Turning room light OFF.");
        lastLedState = false;
      }
    } else {
      digitalWrite(ROOM_LED_PIN, HIGH);
      if (lastLedState == false) {
        Serial.println("[LIGHT CONTROL] Room is DARK. Turning room light ON.");
        lastLedState = true;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void TaskBME(void *pvParameters) {
  (void)pvParameters;

  for (;;) {
    if (bmeConnected) {
      if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        globalTemp = bme.readTemperature();
        globalHumid = bme.readHumidity();
        xSemaphoreGive(i2cMutex);

        static int heartbeatCount = 0;
        if (++heartbeatCount >= 5) {
          Serial.print("[BME TASK HEARTBEAT] Temp: ");
          Serial.print(globalTemp, 1);
          Serial.print("C | Humid: ");
          Serial.print(globalHumid, 1);
          Serial.println("%");
          heartbeatCount = 0;
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(F("\n=== SYSTEM BOOT: HARDENED 1-SENSOR DEBOUNCE ==="));

  entrySemaphore = xSemaphoreCreateBinary();
  i2cMutex = xSemaphoreCreateMutex();

  Wire.begin(21, 22);
  Wire.setClock(400000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("[OLED] Driver Setup Failed."));
    displayConnected = false;
  } else {
    Serial.println(F("[OLED] Driver Connected Successfully."));
    displayConnected = true;
    display.clearDisplay();
    display.display();
  }

  if (!bme.begin(0x76)) {
    Serial.println(F("[BME280] Sensor Setup Failed."));
    bmeConnected = false;
  } else {
    Serial.println(F("[BME280] Sensor Connected Successfully."));
    bmeConnected = true;
  }

  pinMode(IR_ENTRY_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(IR_ENTRY_PIN), irEntryISR, FALLING);
  pinMode(ROOM_LED_PIN, OUTPUT);
  digitalWrite(ROOM_LED_PIN, LOW);

  xTaskCreatePinnedToCore(TaskIR, "IR_Worker", 2560, NULL, 3, NULL, 1);
  xTaskCreatePinnedToCore(TaskDisplay, "OLED_Display", 2560, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskLDR, "LDR_Sampler", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskBME, "BME_Atmosphere", 2048, NULL, 1, NULL, 1);

  Serial.println(F("=== HARDWARE SETUP PROTOCOL COMPLETE ==="));
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
