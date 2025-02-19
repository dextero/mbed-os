/***************************************************************************//**
* \file cyhal_qspi.h
*
* \brief
* Provides a high level interface for interacting with the Cypress Quad-SPI.
* This interface abstracts out the chip specific details. If any chip specific
* functionality is necessary, or performance is critical the low level functions
* can be used directly.
*
********************************************************************************
* \copyright
* Copyright 2018-2019 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/**
* \addtogroup group_hal_qspi QSPI (Quad Serial Peripheral Interface)
* \ingroup group_hal
* \{
* High level interface for interacting with the Cypress Quad-SPI.
*
* \defgroup group_hal_qspi_macros Macros
* \defgroup group_hal_qspi_functions Functions
* \defgroup group_hal_qspi_data_structures Data Structures
* \defgroup group_hal_qspi_enums Enumerated Types
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "cy_result.h"
#include "cyhal_hw_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**
* \addtogroup group_hal_qspi_enums
* \{
*/

/** QSPI Bus width
 *
 * Some parts of commands provide variable bus width
 */
typedef enum cyhal_qspi_bus_width {
    CYHAL_QSPI_CFG_BUS_SINGLE,
    CYHAL_QSPI_CFG_BUS_DUAL,
    CYHAL_QSPI_CFG_BUS_QUAD,
    CYHAL_QSPI_CFG_BUS_OCTAL
} cyhal_qspi_bus_width_t;

/** Size in bits */
typedef enum cyhal_qspi_size {
    CYHAL_QSPI_CFG_SIZE_8,
    CYHAL_QSPI_CFG_SIZE_16,
    CYHAL_QSPI_CFG_SIZE_24,
    CYHAL_QSPI_CFG_SIZE_32
} cyhal_qspi_size_t;

/** QSPI interrupt triggers */
typedef enum {
    CYHAL_QSPI_IRQ_NONE                             = 0,            /**< Disable all interrupts. >*/
    CYHAL_QSPI_IRQ_TRANSMIT_DONE                    = 1 << 0,       /**< Async transmit done. >*/
    CYHAL_QSPI_IRQ_RECEIVE_DONE                     = 1 << 1,       /**< Async receive done. >*/
} cyhal_qspi_irq_event_t;

/** \} group_hal_qspi_enums */

/**
* \addtogroup group_hal_qspi_macros
* \{
*/

#define CYHAL_QSPI_RSLT_ERR_BUS_WIDTH (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CYHAL_RSLT_MODULE_QSPI, 0)) /**< Bus width Error. >*/
#define CYHAL_QSPI_RSLT_ERR_PIN (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CYHAL_RSLT_MODULE_QSPI, 1)) /**< Pin related Error. >*/
#define CYHAL_QSPI_RSLT_ERR_DATA_SEL (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CYHAL_RSLT_MODULE_QSPI, 2)) /**< Data select Error. >*/
#define CYHAL_QSPI_RSLT_ERR_INSTANCE (CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CYHAL_RSLT_MODULE_QSPI, 3)) /**< QSPI instance related Error. >*/

/** \} group_hal_qspi_macros */


/**
* \addtogroup group_hal_qspi_data_structures
* \{
*/

/** QSPI command settings */
typedef struct cyhal_qspi_command {
    struct {
        cyhal_qspi_bus_width_t bus_width;               /**< Bus width for the instruction >*/
        uint8_t value;                                  /**< Instruction value >*/
        bool disabled;                                  /**< Instruction phase skipped if disabled is set to true >*/
    } instruction;
    struct {
        cyhal_qspi_bus_width_t bus_width;               /**< Bus width for the address >*/
        cyhal_qspi_size_t size;                         /**< Address size >*/
        uint32_t value;                                 /**< Address value >*/
        bool disabled;                                  /**< Address phase skipped if disabled is set to true >*/
    }  address;
    struct {
        cyhal_qspi_bus_width_t bus_width;               /**< Bus width for alternative  >*/
        cyhal_qspi_size_t size;                         /**< Alternative size >*/
        uint32_t value;                                 /**< Alternative value >*/
        bool disabled;                                  /**< Alternative phase skipped if disabled is set to true >*/
    } alt;
    uint8_t dummy_count;                                /**< Dummy cycles count >*/
    struct {
        cyhal_qspi_bus_width_t bus_width;               /**< Bus width for data >*/
    } data;
} cyhal_qspi_command_t;

/** Handler for QSPI interrupts */
typedef void (*cyhal_qspi_irq_handler_t)(void *handler_arg, cyhal_qspi_irq_event_t event);

/** \} group_hal_qspi_data_structures */


/**
* \addtogroup group_hal_qspi_functions
* \{
*/

/** Initialize QSPI peripheral.
 *
 * It should initialize QSPI pins (io0-io3, sclk and ssel), set frequency, clock polarity and phase mode.
 *  The clock for the peripheral should be enabled
 *
 * @param[out] obj  QSPI object
 * @param[in]  io0  Data pin 0
 * @param[in]  io1  Data pin 1
 * @param[in]  io2  Data pin 2
 * @param[in]  io3  Data pin 3
 * @param[in]  io4  Data pin 4
 * @param[in]  io5  Data pin 5
 * @param[in]  io6  Data pin 6
 * @param[in]  io7  Data pin 7
 * @param[in]  sclk The clock pin
 * @param[in]  ssel The chip select pin
 * @param[in]  hz   The bus frequency
 * @param[in]  mode Clock polarity and phase mode (0 - 3)
 * @return The status of the init request
 */
cy_rslt_t cyhal_qspi_init(
    cyhal_qspi_t *obj, cyhal_gpio_t io0, cyhal_gpio_t io1, cyhal_gpio_t io2, cyhal_gpio_t io3, cyhal_gpio_t io4, cyhal_gpio_t io5,
    cyhal_gpio_t io6, cyhal_gpio_t io7, cyhal_gpio_t sclk, cyhal_gpio_t ssel, uint32_t hz, uint8_t mode
);

/** Deinitilize QSPI peripheral
 *
 * It should release pins that are associated with the QSPI object, and disable clocks for QSPI peripheral module
 *  that was associated with the object
 *
 * @param[in,out] obj QSPI object
 */
void cyhal_qspi_free(cyhal_qspi_t *obj);

/** Set the QSPI baud rate
 *
 * Actual frequency may differ from the desired frequency due to available dividers and the bus clock
 * Configures the QSPI peripheral's baud rate
 * @param[in] obj The SPI object to configure
 * @param[in] hz  The baud rate in Hz
 * @return The status of the frequency request
 */
cy_rslt_t cyhal_qspi_frequency(cyhal_qspi_t *obj, uint32_t hz);

/** Receive a command and block of data
 *
 * @param[in]     obj QSPI object
 * @param[in]     command QSPI command
 * @param[out]    data RX buffer
 * @param[in,out] length in - RX buffer length in bytes, out - number of bytes read
 * @return The status of the read request
 */
cy_rslt_t cyhal_qspi_read(cyhal_qspi_t *obj, const cyhal_qspi_command_t *command, void *data, size_t *length);

/** Receive a command and block of data in asynchronous mode. Require __enable_irq() call in order to work.
 *
 * @param[in]     obj QSPI object
 * @param[in]     command QSPI command
 * @param[out]    data RX buffer
 * @param[in,out] length in - RX buffer length in bytes, out - number of bytes read
 * @return The status of the read request
 */
cy_rslt_t cyhal_qspi_read_async(cyhal_qspi_t *obj, const cyhal_qspi_command_t *command, void *data, size_t *length);

/** Send a command and block of data
 *
 * @param[in]     obj     QSPI object
 * @param[in]     command QSPI command
 * @param[in]     data    TX buffer
 * @param[in,out] length  in - TX buffer length in bytes, out - number of bytes written
 * @return The status of the write request
 */
cy_rslt_t cyhal_qspi_write(cyhal_qspi_t *obj, const cyhal_qspi_command_t *command, const void *data, size_t *length);

/** Send a command and block of data in asynchronous mode. Require __enable_irq() call in order to work.
 *
 * @param[in]     obj     QSPI object
 * @param[in]     command QSPI command
 * @param[in]     data    TX buffer
 * @param[in,out] length  in - TX buffer length in bytes, out - number of bytes written
 * @return The status of the write request
 */
cy_rslt_t cyhal_qspi_write_async(cyhal_qspi_t *obj, const cyhal_qspi_command_t *command, const void *data, size_t *length);

/** Send a command (and optionally data) and get the response. Can be used to send/receive device specific commands
 *
 * @param[in]     obj     QSPI object
 * @param[in]     command QSPI command
 * @param[in]     tx_data TX buffer
 * @param[in,out] tx_size in - TX buffer length in bytes, out - bytes actually written
 * @param[out]    rx_data RX buffer
 * @param[in,out] rx_size in - RX buffer length in bytes, out - bytes actually read
 * @return The status of the transfer request
 */
cy_rslt_t cyhal_qspi_transfer(
    cyhal_qspi_t *obj, const cyhal_qspi_command_t *command, const void *tx_data, size_t tx_size, void *rx_data,
    size_t rx_size
);

/** The QSPI interrupt handler registration
 *
 * @param[in] obj         The QSPI object
 * @param[in] handler     The callback handler which will be invoked when the interrupt fires
 * @param[in] handler_arg Generic argument that will be provided to the handler when called
 */
void cyhal_qspi_register_irq(cyhal_qspi_t *obj, cyhal_qspi_irq_handler_t handler, void *handler_arg);

/** Configure QSPI interrupt enablement.
 *
 * @param[in] obj      The QSPI object
 * @param[in] event    The QSPI IRQ type
 * @param[in] enable   True to turn on interrupts, False to turn off
 */
void cyhal_qspi_irq_enable(cyhal_qspi_t *obj, cyhal_qspi_irq_event_t event, bool enable);

/** \} group_hal_qspi_functions */

#if defined(__cplusplus)
}
#endif

/** \} group_hal_qspi */
