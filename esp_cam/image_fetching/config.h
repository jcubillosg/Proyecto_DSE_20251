#include <string.h>
#include "esp_camera.h"
#include "SD_MMC.h"
#include "FS.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#define MAX_BUFF_SIZE 256

//#define CAMERA_MODEL_ESP32S3_EYE
#define CAMERA_MODEL_AI_THINKER

#if defined(CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM  32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  0
#define SIOD_GPIO_NUM  26
#define SIOC_GPIO_NUM  27

#define Y9_GPIO_NUM    35
#define Y8_GPIO_NUM    34
#define Y7_GPIO_NUM    39
#define Y6_GPIO_NUM    36
#define Y5_GPIO_NUM    21
#define Y4_GPIO_NUM    19
#define Y3_GPIO_NUM    18
#define Y2_GPIO_NUM    5

#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM  23
#define PCLK_GPIO_NUM  22

#define LED_GPIO_NUM   4

#elif defined(CAMERA_MODEL_ESP32S3_EYE)
#define PWDN_GPIO_NUM  -1
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  15
#define SIOD_GPIO_NUM  4
#define SIOC_GPIO_NUM  5

#define Y2_GPIO_NUM 11
#define Y3_GPIO_NUM 9
#define Y4_GPIO_NUM 8
#define Y5_GPIO_NUM 10
#define Y6_GPIO_NUM 12
#define Y7_GPIO_NUM 18
#define Y8_GPIO_NUM 17
#define Y9_GPIO_NUM 16

#define VSYNC_GPIO_NUM 6
#define HREF_GPIO_NUM  7
#define PCLK_GPIO_NUM  13
#endif

esp_err_t camera_setup(void){
  /*
   * FRAMESIZE_UXGA (1600 x 1200)
   * FRAMESIZE_QVGA (320 x 240)
   * FRAMESIZE_CIF (352 x 288)
   * FRAMESIZE_VGA (640 x 480)
   * FRAMESIZE_SVGA (800 x 600)
   * FRAMESIZE_XGA (1024 x 768)
   * FRAMESIZE_SXGA (1280 x 1024)
   */

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_SVGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  if (psramFound()) {
    config.jpeg_quality = 10;
    config.fb_count = 2;
    config.grab_mode = CAMERA_GRAB_LATEST;
  } else {
    // Limit the frame size when PSRAM is not available
    config.frame_size = FRAMESIZE_SVGA;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return err;
  }

  sensor_t *s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the brightness just a bit
    s->set_saturation(s, -2);  // lower the saturation
  }

/*
  // drop down frame size for higher initial frame rate
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }
*/
  return ESP_OK;
}


esp_err_t sdmmc_setup(){
  if(!SD_MMC.begin()){
    return ESP_FAIL;        
  }
  uint8_t card_type = SD_MMC.cardType();
  if(card_type == CARD_NONE){
    return ESP_FAIL;        
  }
  //Serial.printf("SD Card size: %llu bytes.\n", SD_MMC.cardSize());
  return ESP_OK;
}

esp_err_t list_dir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return ESP_FAIL;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return ESP_FAIL;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        list_dir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
  return ESP_OK;
}

esp_err_t create_dir(fs::FS &fs, const char *path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
  return ESP_OK;
}

esp_err_t remove_dir(fs::FS &fs, const char *path) {
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path)) {
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
  return ESP_OK;
}

esp_err_t read_file(fs::FS &fs, const char *path) {
  Serial.printf("Reading file: %s\n", path);
  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return ESP_FAIL;
  }
  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  return ESP_OK;
}

esp_err_t write_file(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return ESP_FAIL;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  return ESP_OK;
}

esp_err_t append_file(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return ESP_FAIL;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  return ESP_OK;
}

esp_err_t rename_file(fs::FS &fs, const char *path1, const char *path2) {
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
  return ESP_OK;
}

esp_err_t delete_file(fs::FS &fs, const char *path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
  return ESP_OK;
}

esp_err_t test_file_io(fs::FS &fs, const char *path) {
  Serial.printf("Starting IO testing on file: %s\n", path);
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file) {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %lu ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }

  file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return ESP_FAIL;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++) {
    file.write(buf, 512);
  }
  end = millis() - start;
  //Serial.printf("%u bytes written for %lu ms\n", 2048 * 512, end);
  file.close();
  return ESP_OK;
}

esp_err_t camera_capture(char* filename){
  //acquire a frame
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
      ESP_LOGE(TAG, "Camera Capture Failed");
      return ESP_FAIL;
  }
  //Serial.printf("printf \'");
  for(int i = 0; i<(fb->len); i++){
    Serial.printf("%c", *(fb->buf + i));
  }
  Serial.printf("\n**********\n");
  //Serial.printf("\' > %s.jpeg\n", filename);
/*
  char path[MAX_BUFF_SIZE];
  strcpy(path, "/images/");
  //FILENAME MUST NOT EXCEED 242 CHARACTERS [256 - 8 -5 - 1] 
  strcat(path, filename);
  strcat(path, ".jpeg");

  fs::FS &fs = SD_MMC; 
  if(fs.exists(path)){
    fs.remove(path);
  }
  File file = fs.open(path,FILE_WRITE);
  if(!file){
    Serial.printf("Failed to open file in writing mode");
    return ESP_FAIL;
  } else{
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Image added to path: %s. Image size: %d\n", path, fb->len);
  }
  file.close();
*/
  //releases memory allocated by frame buffer
  esp_camera_fb_return(fb);
  return ESP_OK;
}
