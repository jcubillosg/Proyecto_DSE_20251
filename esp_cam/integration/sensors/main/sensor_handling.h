#include <stdio.h>
#include "driver/i2c_master.h"
#include "driver/i2c_types.h"

/*
 * GPIO AVAIL PORTS FOR SENSOR I2C BUS ARE: 
 * GPIO 41 (SDA)
 * GPIO 40 (SCL)
 */

#define I2C_MASTER_SCL_IO 41
#define I2C_MASTER_SDA_IO 40

/*
 * Constant and static arrays cannot have length determined by variable
 * Correct way of defining length for constant array is to use preprocessor directives
 */

#define DATA_W_LEN 100

const uint16_t MPU_addr = 0x68;
const uint16_t MAX_raw_addr = 0x57; //Datasheet specifies address with R/W bit included
                                    //in the form of R_ADDR and W_ADDR, selecting raw 
                                    //addr as per ESP-IDF requirements
int16_t GyX, GyY, GyZ;

i2c_master_bus_config_t i2c_mst_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = -1,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .glitch_ignore_cnt = 7,
    .flags
    .enable_internal_pullup = true,
};

i2c_master_bus_handle_t bus_handle;

/*
 * Device config for MPU-6050
 */


i2c_device_config_t mpu_6050_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = MPU_addr,
    .scl_speed_hz = 100000,
};

i2c_master_dev_handle_t mpu_6050_handle;

/*
 * Device config for MAX-30102
 */


i2c_device_config_t max_30102_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = MAX_raw_addr,
    .scl_speed_hz = 100000,
};

i2c_master_dev_handle_t max_30102_handle;

void i2c_init(){
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &mpu_6050_cfg, &mpu_6050_handle));
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &max_30102_cfg, &max_30102_handle));
    ESP_ERROR_CHECK(i2c_master_probe(bus_handle, MPU_addr, 10000));
    printf("MPU-6050 Sensor Ackd!\n");
    ESP_ERROR_CHECK(i2c_master_probe(bus_handle, MAX_raw_addr, 10000));
    printf("MAX-30102 Sensor Ackd!\n");
}

/*
 * Code for the MPU-6050
 */

esp_err_t mpu_config(){
    
}


/*
 * Code for the MAX-30102
 */
