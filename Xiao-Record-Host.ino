#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <driver/i2s.h>

#define I2S_WS 3
#define I2S_SD 1
#define I2S_SCK 2

#define SAMPLE_RATE 16000
#define RECORD_SECONDS 10

#define I2S_PORT I2S_NUM_0

const char* ssid = "NxtWave_Te@m";
const char* password = "Nxtwave@KKH2026";

WebServer server(80);

const uint32_t NUM_SAMPLES =
  SAMPLE_RATE * RECORD_SECONDS;

const uint32_t AUDIO_BYTES =
  NUM_SAMPLES * sizeof(int16_t);

int16_t* audioBuffer = nullptr;

int currentSlot = 1;

void writeWavHeader(File& file, uint32_t dataSize) {
  uint32_t fileSize = dataSize + 36;

  file.write((const uint8_t*)"RIFF", 4);
  file.write((uint8_t*)&fileSize, 4);
  file.write((const uint8_t*)"WAVE", 4);

  file.write((const uint8_t*)"fmt ", 4);

  uint32_t subChunk1Size = 16;
  uint16_t audioFormat = 1;
  uint16_t channels = 1;
  uint32_t sampleRate = SAMPLE_RATE;
  uint16_t bitsPerSample = 16;

  uint32_t byteRate =
    sampleRate * channels * bitsPerSample / 8;

  uint16_t blockAlign =
    channels * bitsPerSample / 8;

  file.write((uint8_t*)&subChunk1Size, 4);
  file.write((uint8_t*)&audioFormat, 2);
  file.write((uint8_t*)&channels, 2);
  file.write((uint8_t*)&sampleRate, 4);
  file.write((uint8_t*)&byteRate, 4);
  file.write((uint8_t*)&blockAlign, 2);
  file.write((uint8_t*)&bitsPerSample, 2);

  file.write((const uint8_t*)"data", 4);
  file.write((uint8_t*)&dataSize, 4);
}

void setupI2S() {
  i2s_config_t cfg = {
    .mode =
      (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample =
      I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format =
      I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format =
      I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags =
      ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 256,
    .use_apll = false
  };

  i2s_pin_config_t pins = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_driver_install(
    I2S_PORT,
    &cfg,
    0,
    NULL);

  i2s_set_pin(
    I2S_PORT,
    &pins);
}

String nextFileName() {
  String name =
    "/rec" + String(currentSlot) + ".wav";

  currentSlot++;

  if (currentSlot > 3)
    currentSlot = 1;

  return name;
}

void recordAudio() {
  String fileName = nextFileName();

  Serial.println("Recording...");

  uint32_t sampleIndex = 0;

  while (sampleIndex < NUM_SAMPLES) {
    int32_t sample32;
    size_t bytesRead;

    i2s_read(
      I2S_PORT,
      &sample32,
      sizeof(sample32),
      &bytesRead,
      portMAX_DELAY);

    if (bytesRead) {
      audioBuffer[sampleIndex++] =
        (int16_t)(sample32 >> 14);
    }
  }

  File f =
    LittleFS.open(
      fileName,
      "w");

  writeWavHeader(
    f,
    AUDIO_BYTES);

  f.write(
    (uint8_t*)audioBuffer,
    AUDIO_BYTES);

  f.close();

  Serial.println(
    "Saved: " + fileName);
}

void handleRoot() {
  String html;

  html += "<h1>XIAO Audio Recorder</h1>";

  html +=
    "<a href='/record'>"
    "Record 10 Seconds"
    "</a><br><br>";

  html +=
    "<a href='/rec1.wav'>"
    "Recording 1"
    "</a><br>";

  html +=
    "<a href='/rec2.wav'>"
    "Recording 2"
    "</a><br>";

  html +=
    "<a href='/rec3.wav'>"
    "Recording 3"
    "</a><br>";

  server.send(
    200,
    "text/html",
    html);
}

void handleRecord() {
  recordAudio();

  server.send(
    200,
    "text/plain",
    "Recording completed");
}

void serveFile(String path) {
  if (!LittleFS.exists(path)) {
    server.send(
      404,
      "text/plain",
      "File not found");
    return;
  }

  File file =
    LittleFS.open(
      path,
      "r");

  server.streamFile(
    file,
    "audio/wav");

  file.close();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

//   Serial.print("Cool down: ");
//   for (int i = 4; i >= 0; i--) {
//     Serial.print(i);
//     Serial.print("...");
//     delay(1000);
//   }
//   Serial.println();

Serial.println("1: LittleFS.begin(true)");
LittleFS.begin(true);
delay(1000);

Serial.println("2: audioBuffer allocation");
audioBuffer = (int16_t*)ps_malloc(AUDIO_BYTES);
delay(1000);

Serial.println("3: setupI2S();");
setupI2S();
delay(1000);

Serial.println("4: WiFi.begin");
WiFi.begin(ssid, password);
delay(1000);

Serial.println("5: Initialisation complete......");

//   LittleFS.begin(true);
//   audioBuffer =
//     (int16_t*)
//       ps_malloc(AUDIO_BYTES);

//   setupI2S();

//   WiFi.begin(
//     ssid,
//     password);

  while (
    WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println(
    WiFi.localIP());

  server.on(
    "/",
    handleRoot);

  server.on(
    "/record",
    handleRecord);

  server.on(
    "/rec1.wav",
    []() {
      serveFile("/rec1.wav");
    });

  server.on(
    "/rec2.wav",
    []() {
      serveFile("/rec2.wav");
    });

  server.on(
    "/rec3.wav",
    []() {
      serveFile("/rec3.wav");
    });

  server.begin();
}

void loop() {
  server.handleClient();
}