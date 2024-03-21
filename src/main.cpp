#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

#include "SparkFunLSM6DS3.h"
#include "fft.h"

#define I2C_MASTER_SCL_IO    22                 /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO    21                 /*!< gpio number for I2C master data */
#define I2C_MASTER_NUM I2C_NUM_0                /*!< I2C port number for master dev */
#define FRAME_SIZE 100

#define I2C_EXAMPLE_MASTER_RX_BUF_DISABLE 0  
#define I2C_EXAMPLE_MASTER_TX_BUF_DISABLE 0  

typedef struct {
  float real;
  float imag;
} complex_t;

typedef struct {
  float x;
  float y;
  float z;
  int available;
} lsm6ds3_accel_data_t;

void fft(complex_t* data, size_t n) {    
}

float abs_square(complex_t value) {
  return (value.real * value.real + value.imag * value.imag);
}

void i2c_master_init() {
  i2c_config_t conf;
  conf.mode          = I2C_MODE_MASTER;
  conf.sda_io_num    = I2C_MASTER_SDA_IO;
  conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
  conf.scl_io_num    = I2C_MASTER_SCL_IO;
  conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
  i2c_param_config(I2C_MASTER_NUM, &conf);
  i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_EXAMPLE_MASTER_RX_BUF_DISABLE, I2C_EXAMPLE_MASTER_TX_BUF_DISABLE, 0);
}

void lsm6ds3_init(int i2c_num) {    
}

lsm6ds3_accel_data_t lsm6ds3_get_accel() {
  lsm6ds3_accel_data_t data;
  data.x         = 0;
  data.y         = 0;
  data.z         = 0;
  data.available = 0; 
  return data; 
}
void setup() {
  i2c_master_init();
  lsm6ds3_init(I2C_MASTER_NUM); 
}

void loop() {
  float frames[FRAME_SIZE] = {0};
  while(1) {
    lsm6ds3_accel_data_t accel_data = lsm6ds3_get_accel();
    for(int i = 0; i < FRAME_SIZE-1; i++) {
        frames[i] = frames[i+1];
    } 
  } 
}

void calculate_normalized_short_time_energy(float* frames, size_t n) {
    complex_t stft_frames[n];
    fft(stft_frames, n);                       // Apply the FFT to the frames using library function.
    float energy = 0.0;
    for(int i = 0; i < n; i++) {
        energy += abs_square(stft_frames[i]);  // Compute the absolute value of the STFT and square it to get the energy.
    }
    float normalized_energy = energy / (n * n);
    printf("Normalized short time energy: %f\n", normalized_energy);
}

void lsm6ds3_task(void* arg)
{
    i2c_master_init();                        // Initiate the I2C connection.
    lsm6ds3_init(I2C_MASTER_NUM);             // Initiate LSM6DS3.
    float frames[FRAME_SIZE] = {0};
    while(1) {
        lsm6ds3_accel_data_t accel_data = lsm6ds3_get_accel();
        for(int i = 0; i < FRAME_SIZE-1; i++) {
            frames[i] = frames[i+1];          // Add new reading to frames and remove old reading.
        }
        frames[FRAME_SIZE-1] = accel_data.x*accel_data.x + accel_data.y*accel_data.y + accel_data.z*accel_data.z;
        if(accel_data.available) {
            calculate_normalized_short_time_energy(frames, FRAME_SIZE);
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void app_main()
{
    xTaskCreate(lsm6ds3_task, "lsm6ds3_task", 1024 * 2, (void*) 0, 10, NULL);
}