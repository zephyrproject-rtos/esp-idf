// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __ESP_BT_H__
#define __ESP_BT_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "sdkconfig.h"
#include "esp_task.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Controller config options, depend on config mask.
 *        Config mask indicate which functions enabled, this means
 *        some options or parameters of some functions enabled by config mask.
 */
typedef struct {
    uint16_t controller_task_stack_size;    /*!< Bluetooth controller task stack size */
    uint8_t controller_task_prio;           /*!< Bluetooth controller task priority */
    uint8_t hci_uart_no;                    /*!< If use UART1/2 as HCI IO interface, indicate UART number */
    uint32_t hci_uart_baudrate;             /*!< If use UART1/2 as HCI IO interface, indicate UART baudrate */
    uint8_t scan_duplicate_mode;            /*!< If use UART1/2 as HCI IO interface, indicate UART baudrate */
    uint16_t normal_adv_size;               /*!< Normal adv size for scan duplicate */
    uint16_t mesh_adv_size;                 /*!< Mesh adv size for scan duplicate */
    uint16_t send_adv_reserved_size;        /*!< Controller minimum memory value */
    uint32_t  controller_debug_flag;         /*!< Controller debug log flag */
} esp_bt_controller_config_t;

#ifdef CONFIG_BT_ENABLED
/* While scanning, if the free memory value in controller is less than SCAN_SEND_ADV_RESERVED_SIZE, 
the adv packet will be discarded until the memory is restored. */
#define SCAN_SEND_ADV_RESERVED_SIZE        1000
/* enable controller log debug when adv lost */
#define CONTROLLER_ADV_LOST_DEBUG_BIT      (0<<0)

#ifdef CONFIG_BT_HCI_UART_NO
#define BT_HCI_UART_NO_DEFAULT CONFIG_BT_HCI_UART_NO
#else
#define BT_HCI_UART_NO_DEFAULT 1
#endif /* BT_HCI_UART_NO_DEFAULT */

#ifdef CONFIG_BT_HCI_UART_BAUDRATE
#define BT_HCI_UART_BAUDRATE_DEFAULT CONFIG_BT_HCI_UART_BAUDRATE
#else
#define BT_HCI_UART_BAUDRATE_DEFAULT 921600
#endif /* BT_HCI_UART_BAUDRATE_DEFAULT */

/* normal adv cache size */
#ifdef CONFIG_DUPLICATE_SCAN_CACHE_SIZE
#define NORMAL_SCAN_DUPLICATE_CACHE_SIZE  CONFIG_DUPLICATE_SCAN_CACHE_SIZE
#else
#define NORMAL_SCAN_DUPLICATE_CACHE_SIZE  20
#endif

#ifndef CONFIG_BLE_MESH_SCAN_DUPLICATE_EN
#define CONFIG_BLE_MESH_SCAN_DUPLICATE_EN FALSE
#endif

#define SCAN_DUPLICATE_MODE_NORMAL_ADV_ONLY   0
#define SCAN_DUPLICATE_MODE_NORMAL_ADV_MESH_ADV 1

#if CONFIG_BLE_MESH_SCAN_DUPLICATE_EN
    #define SCAN_DUPLICATE_MODE SCAN_DUPLICATE_MODE_NORMAL_ADV_MESH_ADV
    #ifdef CONFIG_MESH_DUPLICATE_SCAN_CACHE_SIZE
    #define MESH_DUPLICATE_SCAN_CACHE_SIZE  CONFIG_MESH_DUPLICATE_SCAN_CACHE_SIZE
    #else
    #define MESH_DUPLICATE_SCAN_CACHE_SIZE       50
    #endif
#else
    #define SCAN_DUPLICATE_MODE SCAN_DUPLICATE_MODE_NORMAL_ADV_ONLY
    #define MESH_DUPLICATE_SCAN_CACHE_SIZE        0
#endif

#define BT_CONTROLLER_INIT_CONFIG_DEFAULT() {                       \
    .controller_task_stack_size = ESP_TASK_BT_CONTROLLER_STACK,     \
    .controller_task_prio = ESP_TASK_BT_CONTROLLER_PRIO,            \
    .hci_uart_no = BT_HCI_UART_NO_DEFAULT,                          \
    .hci_uart_baudrate = BT_HCI_UART_BAUDRATE_DEFAULT,              \
    .scan_duplicate_mode = SCAN_DUPLICATE_MODE,                     \
    .normal_adv_size = NORMAL_SCAN_DUPLICATE_CACHE_SIZE,            \
    .mesh_adv_size = MESH_DUPLICATE_SCAN_CACHE_SIZE,                \
    .send_adv_reserved_size = SCAN_SEND_ADV_RESERVED_SIZE,          \
    .controller_debug_flag = CONTROLLER_ADV_LOST_DEBUG_BIT,         \
};

#else
#define BT_CONTROLLER_INIT_CONFIG_DEFAULT() {0}; _Static_assert(0, "please enable bluetooth in menuconfig to use bt.h");
#endif

/**
 * @brief Bluetooth mode for controller enable/disable
 */
typedef enum {
    ESP_BT_MODE_IDLE       = 0x00,   /*!< Bluetooth is not running */
    ESP_BT_MODE_BLE        = 0x01,   /*!< Run BLE mode */
    ESP_BT_MODE_CLASSIC_BT = 0x02,   /*!< Run Classic BT mode */
    ESP_BT_MODE_BTDM       = 0x03,   /*!< Run dual mode */
} esp_bt_mode_t;

/**
 * @brief Bluetooth controller enable/disable/initialised/de-initialised status
 */
typedef enum {
    ESP_BT_CONTROLLER_STATUS_IDLE = 0,
    ESP_BT_CONTROLLER_STATUS_INITED,
    ESP_BT_CONTROLLER_STATUS_ENABLED,
    ESP_BT_CONTROLLER_STATUS_NUM,
} esp_bt_controller_status_t;


/**
 * @brief BLE tx power type
 *        ESP_BLE_PWR_TYPE_CONN_HDL0-8: for each connection, and only be set after connection completed.
 *                                      when disconnect, the correspond TX power is not effected.
 *        ESP_BLE_PWR_TYPE_ADV : for advertising/scan response.
 *        ESP_BLE_PWR_TYPE_SCAN : for scan.
 *        ESP_BLE_PWR_TYPE_DEFAULT : if each connection's TX power is not set, it will use this default value.
 *                                   if neither in scan mode nor in adv mode, it will use this default value.
 *        If none of power type is set, system will use ESP_PWR_LVL_P1 as default for ADV/SCAN/CONN0-9.
 */
typedef enum {
    ESP_BLE_PWR_TYPE_CONN_HDL0  = 0,            /*!< For connection handle 0 */
    ESP_BLE_PWR_TYPE_CONN_HDL1  = 1,            /*!< For connection handle 1 */
    ESP_BLE_PWR_TYPE_CONN_HDL2  = 2,            /*!< For connection handle 2 */
    ESP_BLE_PWR_TYPE_CONN_HDL3  = 3,            /*!< For connection handle 3 */
    ESP_BLE_PWR_TYPE_CONN_HDL4  = 4,            /*!< For connection handle 4 */
    ESP_BLE_PWR_TYPE_CONN_HDL5  = 5,            /*!< For connection handle 5 */
    ESP_BLE_PWR_TYPE_CONN_HDL6  = 6,            /*!< For connection handle 6 */
    ESP_BLE_PWR_TYPE_CONN_HDL7  = 7,            /*!< For connection handle 7 */
    ESP_BLE_PWR_TYPE_CONN_HDL8  = 8,            /*!< For connection handle 8 */
    ESP_BLE_PWR_TYPE_ADV        = 9,            /*!< For advertising */
    ESP_BLE_PWR_TYPE_SCAN       = 10,           /*!< For scan */
    ESP_BLE_PWR_TYPE_DEFAULT    = 11,           /*!< For default, if not set other, it will use default value */
    ESP_BLE_PWR_TYPE_NUM        = 12,           /*!< TYPE numbers */
} esp_ble_power_type_t;

/**
 * @brief Bluetooth TX power level(index), it's just a index corresponding to power(dbm).
 */
typedef enum {
    ESP_PWR_LVL_N14 = 0,            /*!< Corresponding to -14dbm */
    ESP_PWR_LVL_N11 = 1,            /*!< Corresponding to -11dbm */
    ESP_PWR_LVL_N8  = 2,            /*!< Corresponding to  -8dbm */
    ESP_PWR_LVL_N5  = 3,            /*!< Corresponding to  -5dbm */
    ESP_PWR_LVL_N2  = 4,            /*!< Corresponding to  -2dbm */
    ESP_PWR_LVL_P1  = 5,            /*!< Corresponding to   1dbm */
    ESP_PWR_LVL_P4  = 6,            /*!< Corresponding to   4dbm */
    ESP_PWR_LVL_P7  = 7,            /*!< Corresponding to   7dbm */
} esp_power_level_t;

/**
 * @brief Bluetooth audio data transport path
 */
typedef enum {
    ESP_SCO_DATA_PATH_HCI = 0,            /*!< data over HCI transport */
    ESP_SCO_DATA_PATH_PCM = 1,            /*!< data over PCM interface */
} esp_sco_data_path_t;

/**
 * @brief  Set BLE TX power
 *         Connection Tx power should only be set after connection created.
 * @param  power_type : The type of which tx power, could set Advertising/Connection/Default and etc
 * @param  power_level: Power level(index) corresponding to absolute value(dbm)
 * @return              ESP_OK - success, other - failed
 */
esp_err_t esp_ble_tx_power_set(esp_ble_power_type_t power_type, esp_power_level_t power_level);

/**
 * @brief  Get BLE TX power
 *         Connection Tx power should only be get after connection created.
 * @param  power_type : The type of which tx power, could set Advertising/Connection/Default and etc
 * @return             >= 0 - Power level, < 0 - Invalid
 */
esp_power_level_t esp_ble_tx_power_get(esp_ble_power_type_t power_type);

/**
 * @brief  Set BR/EDR TX power
 *         BR/EDR power control will use the power in range of minimum value and maximum value.
 *         The power level will effect the global BR/EDR TX power, such inquire, page, connection and so on.
 *         Please call the function after esp_bt_controller_enable and before any function which cause RF do TX.
 *         So you can call the function can before do discover, beofre profile init and so on.
 *         For example, if you want BR/EDR use the new TX power to do inquire, you should call
 *         this function before inquire. Another word, If call this function when BR/EDR is in inquire(ING),
 *         please do inquire again after call this function.
 *         Default minimum power level is ESP_PWR_LVL_N2, and maximum power level is ESP_PWR_LVL_P1.
 * @param  min_power_level: The minimum power level
 * @param  max_power_level: The maximum power level
 * @return              ESP_OK - success, other - failed
 */
esp_err_t esp_bredr_tx_power_set(esp_power_level_t min_power_level, esp_power_level_t max_power_level);

/**
 * @brief  Get BR/EDR TX power
 *         If the argument is not NULL, then store the corresponding value.
 * @param  min_power_level: The minimum power level
 * @param  max_power_level: The maximum power level
 * @return              ESP_OK - success, other - failed
 */
esp_err_t esp_bredr_tx_power_get(esp_power_level_t *min_power_level, esp_power_level_t *max_power_level);

/**
 * @brief  set default SCO data path
 *         Should be called after controller is enabled, and before (e)SCO link is established
 * @param  data_path: SCO data path
 * @return              ESP_OK - success, other - failed
 */
esp_err_t esp_bredr_sco_datapath_set(esp_sco_data_path_t data_path);

/**
 * @brief  Initialize BT controller to allocate task and other resource.
 * @param  cfg: Initial configuration of BT controller.
 * This function should be called only once, before any other BT functions are called.
 * @return       ESP_OK - success, other - failed
 */
esp_err_t esp_bt_controller_init(esp_bt_controller_config_t *cfg);

/**
 * @brief  De-initialize BT controller to free resource and delete task.
 *
 * This function should be called only once, after any other BT functions are called.
 * This function is not whole completed, esp_bt_controller_init cannot called after this function.
 * @return  ESP_OK - success, other - failed
 */
esp_err_t esp_bt_controller_deinit(void);

/**
 * @brief Enable BT controller.
 *               Due to a known issue, you cannot call esp_bt_controller_enable() a second time
 *               to change the controller mode dynamically. To change controller mode, call
 *               esp_bt_controller_disable() and then call esp_bt_controller_enable() with the new mode.
 * @param mode : the mode(BLE/BT/BTDM) to enable.
 * @return       ESP_OK - success, other - failed
 */
esp_err_t esp_bt_controller_enable(esp_bt_mode_t mode);

/**
 * @brief  Disable BT controller
 * @return       ESP_OK - success, other - failed
 */
esp_err_t esp_bt_controller_disable(void);

/**
 * @brief  Get BT controller is initialised/de-initialised/enabled/disabled
 * @return status value
 */
esp_bt_controller_status_t esp_bt_controller_get_status(void);

/** @brief esp_vhci_host_callback
 *  used for vhci call host function to notify what host need to do
 */
typedef struct esp_vhci_host_callback {
    void (*notify_host_send_available)(void);               /*!< callback used to notify that the host can send packet to controller */
    int (*notify_host_recv)(uint8_t *data, uint16_t len);   /*!< callback used to notify that the controller has a packet to send to the host*/
} esp_vhci_host_callback_t;

/** @brief esp_vhci_host_check_send_available
 *  used for check actively if the host can send packet to controller or not.
 *  @return true for ready to send, false means cannot send packet
 */
bool esp_vhci_host_check_send_available(void);

/** @brief esp_vhci_host_send_packet
 * host send packet to controller
 * @param data the packet point
 *,@param len the packet length
 */
void esp_vhci_host_send_packet(uint8_t *data, uint16_t len);

/** @brief esp_vhci_host_register_callback
 * register the vhci referece callback, the call back
 * struct defined by vhci_host_callback structure.
 * @param callback esp_vhci_host_callback type variable
 */
void esp_vhci_host_register_callback(const esp_vhci_host_callback_t *callback);

/** @brief esp_bt_controller_mem_release
 * release the memory by mode, if never use the bluetooth mode
 * it can release the .bbs, .data and other section to heap.
 * The total size is about 70k bytes.
 *
 * esp_bt_controller_mem_release(mode) should be called only before esp_bt_controller_init()
 * or after esp_bt_controller_deinit().
 *
 * Note that once BT controller memory is released, the process cannot be reversed. It means you can not use the bluetooth
 * mode which you have released by this function.
 *
 * If your firmware will later upgrade the Bluetooth controller mode (BLE -> BT Classic or disabled -> enabled)
 * then do not call this function.
 *
 * If the app calls esp_bt_controller_enable(ESP_BT_MODE_BLE) to use BLE only then it is safe to call
 * esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT) at initialisation time to free unused BT Classic memory.
 *
 * If user never use bluetooth controller, could call esp_bt_controller_mem_release(ESP_BT_MODE_BTDM)
 * before esp_bt_controller_init or after esp_bt_controller_deinit.
 *
 * For example, user only use bluetooth to config SSID and PASSWORD of WIFI, after config, will never use bluetooth.
 * Then, could call esp_bt_controller_mem_release(ESP_BT_MODE_BTDM) after esp_bt_controller_deinit.
 *
 * @param mode : the mode want to release memory
 * @return ESP_OK - success, other - failed
 */
esp_err_t esp_bt_controller_mem_release(esp_bt_mode_t mode);

/**
 * @brief enable bluetooth to enter modem sleep
 *
 * Note that this function shall not be invoked before esp_bt_controller_enable()
 *
 * There are currently two options for bluetooth modem sleep, one is ORIG mode, and another is EVED Mode. EVED Mode is intended for BLE only.
 *
 * For ORIG mode:
 * Bluetooth modem sleep is enabled in controller start up by default if CONFIG_BTDM_CONTROLLER_MODEM_SLEEP is set and "ORIG mode" is selected. In ORIG modem sleep mode, bluetooth controller will switch off some components and pause to work every now and then, if there is no event to process; and wakeup according to the scheduled interval and resume the work. It can also wakeup earlier upon external request using function "esp_bt_controller_wakeup_request".
 * Note that currently there is problem in the combination use of bluetooth modem sleep and Dynamic Frequency Scaling(DFS). So do not enable DFS if bluetooth modem sleep is in use.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_bt_sleep_enable(void);


/**
 * @brief disable bluetooth modem sleep
 *
 * Note that this function shall not be invoked before esp_bt_controller_enable()
 *
 * If esp_bt_sleep_disable() is called, bluetooth controller will not be allowed to enter modem sleep;
 *
 * If ORIG modem sleep mode is in use, if this function is called, bluetooth controller may not immediately wake up if it is dormant then.
 * In this case, esp_bt_controller_wakeup_request() can be used to shorten the time for wakeup.
 *
 * @return
 *                  - ESP_OK : success
 *                  - other  : failed
 */
esp_err_t esp_bt_sleep_disable(void);

/**
 * @brief to check whether bluetooth controller is sleeping at the instant, if modem sleep is enabled
 *
 * Note that this function shall not be invoked before esp_bt_controller_enable()
 * This function is supposed to be used ORIG mode of modem sleep
 *
 * @return  true if in modem sleep state, false otherwise
 */
bool esp_bt_controller_is_sleeping(void);

/**
 * @brief request controller to wakeup from sleeping state during sleep mode
 *
 * Note that this function shall not be invoked before esp_bt_controller_enable()
 * Note that this function is supposed to be used ORIG mode of modem sleep
 * Note that after this request, bluetooth controller may again enter sleep as long as the modem sleep is enabled
 *
 * Profiling shows that it takes several milliseconds to wakeup from modem sleep after this request.
 * Generally it takes longer if 32kHz XTAL is used than the main XTAL, due to the lower frequncy of the former as the bluetooth low power clock source.
 */
void esp_bt_controller_wakeup_request(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_BT_H__ */
