#include "game.h"

void app_main() {

	// display
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
    ssd1306_init(&dev, 128, 64);

	//joystick
	adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(JOYSTICK_X_AXIS, ADC_ATTEN_DB_11);
	
	start();
}