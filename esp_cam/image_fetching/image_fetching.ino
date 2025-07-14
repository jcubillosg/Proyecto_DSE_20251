#include "config.h"

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(2000000);
  //Serial.setDebugOutput(true);
  delay(1000);

  esp_err_t camera_setup_err;
  esp_err_t sdmmc_setup_err;

  camera_setup_err = camera_setup();
  if(camera_setup_err != ESP_OK){
    Serial.println("Camera setup error!");
    return;
  }

  sdmmc_setup_err = sdmmc_setup();
  if(sdmmc_setup_err != ESP_OK){
    Serial.println("SD_MMC setup error!");
    return;
  }

  delay(1000);

  //FS and SD testing
  /*
  esp_err_t vals[13];
  char test_passed = 0;
  vals[0x00] = list_dir(SD_MMC, "/", 0);
  vals[0x01] = create_dir(SD_MMC, "/mydir");
  vals[0x02] = list_dir(SD_MMC, "/", 0);
  vals[0x03] = remove_dir(SD_MMC, "/mydir");
  vals[0x04] = list_dir(SD_MMC, "/", 2);
  vals[0x05] = write_file(SD_MMC, "/hello.txt", "Hello ");
  vals[0x06] = append_file(SD_MMC, "/hello.txt", "World!\n");
  vals[0x07] = read_file(SD_MMC, "/hello.txt");
  vals[0x08] = rename_file(SD_MMC, "/hello.txt", "/foo.txt");
  vals[0x09] = read_file(SD_MMC, "/foo.txt");
  vals[0x10] = delete_file(SD_MMC, "/foo.txt");
  vals[0x11] = append_file(SD_MMC, "/test.txt", "Testing!\n");
  vals[0x12] = test_file_io(SD_MMC, "/test.txt");

  for(int i=0; i<13; i++){
    if(vals[i] != ESP_OK){
      test_passed++;
    }
  }
  if(!test_passed){
    Serial.println("Passed FS and SD tests!");
  } else{
    Serial.println("Did not pass FS and SD tests.");
  }
  */

  //Camera Testing (writing to SD)
  /*
  char* image_names[] = {
    "image_1",
    "image_2",
    "image_3",
    "image_4",
    "image_5",
    "image_6",
    "image_7",
    "image_8",
    "image_9",
    "image_10",
    "image_11"
  };
  */

  //Serial.printf("#!/usr/bin/bash\n#Starting!\n");
  Serial.printf("Start\n");
  for(unsigned char i = 0; i < 150; i++){
    //camera_capture(image_names[i]);
    camera_capture("test");
    delay(100); //SDMMC timeout
  }
  //Serial.printf("#Finished!\n");
}

void loop(){
    delay(1000);
}
