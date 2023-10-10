#include <stdbool.h>
#include <inttypes.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"

#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "esp_lcd_panel_vendor.h"

#include "aircomm.h"

#define I2C_HOST 0

#define LCD_PIXEL_CLOCK_HZ ( 400 * 1000 )
#define GPIO_OLED_SDA      8
#define GPIO_OLED_SCL      9
#define GPIO_OLED_RST      -1
#define I2C_HW_ADDR        0x3C

#define LCD_H_RES 128
#define LCD_V_RES 32

#define LCD_CMD_BITS   8
#define LCD_PARAM_BITS 8

static const char * TAG = "aircomm";

static bool
notify_lvgl_flush_ready( esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t * edata, void * user_ctx ) {
    lv_disp_t * disp = (lv_disp_t *)user_ctx;
    lvgl_port_flush_ready( disp );
    return false;
}

void aircomm_launch( void ) {
    ESP_LOGI( TAG, "Initialize I2C bus" );
    i2c_config_t i2c_conf = {
        .mode             = I2C_MODE_MASTER,
        .sda_io_num       = GPIO_OLED_SDA,
        .scl_io_num       = GPIO_OLED_SCL,
        .sda_pullup_en    = GPIO_PULLUP_ENABLE,
        .scl_pullup_en    = GPIO_PULLUP_ENABLE,
        .master.clk_speed = LCD_PIXEL_CLOCK_HZ,
    };
    ESP_ERROR_CHECK( i2c_param_config( I2C_HOST, &i2c_conf ) );
    ESP_ERROR_CHECK( i2c_driver_install( I2C_HOST, I2C_MODE_MASTER, 0, 0, 0 ) );

    ESP_LOGI( TAG, "Install panel IO" );
    esp_lcd_panel_io_handle_t     io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr            = I2C_HW_ADDR,
        .control_phase_bytes = 1,
        .lcd_cmd_bits        = LCD_CMD_BITS,
        .lcd_param_bits      = LCD_PARAM_BITS,
        .dc_bit_offset       = 6,
    };
    ESP_ERROR_CHECK( esp_lcd_new_panel_io_i2c( (esp_lcd_i2c_bus_handle_t)I2C_HOST, &io_config, &io_handle ) );

    ESP_LOGI( TAG, "Install SSD1306 panel driver" );
    esp_lcd_panel_handle_t     panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = GPIO_OLED_RST,
    };
    ESP_ERROR_CHECK( esp_lcd_new_panel_ssd1306( io_handle, &panel_config, &panel_handle ) );

    ESP_ERROR_CHECK( esp_lcd_panel_reset( panel_handle ) );
    ESP_ERROR_CHECK( esp_lcd_panel_init( panel_handle ) );

    esp_lcd_panel_io_tx_param( io_handle, 0xA8, ( uint8_t[1] ){ 0x1F }, 1 );
    esp_lcd_panel_io_tx_param( io_handle, 0xDA, ( uint8_t[1] ){ 0x02 }, 1 );

    ESP_ERROR_CHECK( esp_lcd_panel_disp_on_off( panel_handle, true ) );

    esp_lcd_panel_io_tx_param( io_handle, 0xC8, NULL, 0 ); // Set COM Output Scan Direction 0xC0 OR 0xC8
    esp_lcd_panel_io_tx_param( io_handle, 0xA1, NULL, 0 );

    ESP_LOGI( TAG, "Initialize LVGL" );
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG( );
    lvgl_port_init( &lvgl_cfg );

    const lvgl_port_display_cfg_t disp_cfg = { .io_handle     = io_handle,
                                               .panel_handle  = panel_handle,
                                               .buffer_size   = LCD_H_RES * LCD_V_RES,
                                               .double_buffer = true,
                                               .hres          = LCD_H_RES,
                                               .vres          = LCD_V_RES,
                                               .monochrome    = true,
                                               .rotation      = {
                                                        .swap_xy  = false,
                                                        .mirror_x = false,
                                                        .mirror_y = false,
                                               } };

    lv_disp_t * disp = lvgl_port_add_disp( &disp_cfg );

    /* Register done callback for IO. */
    const esp_lcd_panel_io_callbacks_t cbs = {
        .on_color_trans_done = notify_lvgl_flush_ready,
    };
    esp_lcd_panel_io_register_event_callbacks( io_handle, &cbs, disp );

    /* Rotation of the screen. */
    // lv_disp_set_rotation( disp, LV_DISP_ROT_NONE );

    ESP_LOGI( TAG, "Display LVGL Scroll Text" );
    lv_obj_t * scr   = lv_disp_get_scr_act( disp );
    lv_obj_t * label = lv_label_create( scr );

    static lv_style_t style;
    lv_style_init( &style );
    lv_style_set_text_font( &style, &lv_font_montserrat_14 );

    lv_label_set_long_mode( label, LV_LABEL_LONG_SCROLL_CIRCULAR );
    lv_label_set_text( label, "MILITARY SOFTWARE DEVELOPMENT - Murat Koes" );
    lv_obj_set_width( label, 128 );
    lv_obj_align( label, LV_ALIGN_TOP_MID, 0, 0 );

    while ( 1 ) {
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }
}
