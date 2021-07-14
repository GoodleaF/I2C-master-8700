/******************************************************************************
* File Name: main.h
*
* Version 1.30
*
* Description: 
*  This file provides constants for the I2C Master project.
*
* Related Document: 
*  CE222306_PSoC4_I2C_SCB.pdf
*
* Hardware Dependency: 
*  See CE222306_PSoC4_I2C_SCB.pdf
*
*******************************************************************************
* Copyright (2018), Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress’s integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, 
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress 
* reserves the right to make changes to the Software without notice. Cypress 
* does not assume any liability arising out of the application or use of the 
* Software or any product or circuit described in the Software. Cypress does 
* not authorize its products for use in any products where a malfunction or 
* failure of the Cypress product may reasonably be expected to result in 
* significant property damage, injury or death ("High Risk Product"). By 
* including Cypress’s product in a High Risk Product, the manufacturer of such 
* system or application assumes all risk of such use and in doing so agrees to 
* indemnify Cypress against all liability. 
*******************************************************************************/

#if !defined(MAIN_H)
#define MAIN_H

#include <project.h>


/***************************************
*            Constants
****************************************/

/* LED constants */
#define LED_SUCCESS_ON        (0u)
#define LED_SUCCESS_OFF       (1u)    

/* LED1 on the CY8CKIT-149 kit has non-inverted polarity */
#if (CY_PSOC4_4100MS)
    #define LED_ERROR_ON  (1u)
    #define LED_ERROR_OFF (0u) 
#else
    #define LED_ERROR_ON  (0u)
    #define LED_ERROR_OFF (1u)
#endif
    
/* I2C Slave address to communicate with */
#define I2C_SLAVE_ADDR      (0x74u)

/* Buffer and packet size */
#define BUFFER_SIZE         (4u)
#define PACKET_SIZE         (BUFFER_SIZE)

/* Packet positions */
#define PACKET_SOP_POS      (0u)
#define PACKET_CMD_POS      (1u)
#define PACKET_STS_POS      (PACKET_CMD_POS)
#define PACKET_EOP_POS      (2u)

/* Start and end of packet markers */
#define PACKET_SOP          (0x01u)
#define PACKET_EOP          (0x17u)

/* Command statuses */
#define STS_CMD_DONE        (0x00u)
#define STS_CMD_FAIL        (0xFFu)

/* Transfer statuses */
#define TRANSFER_CMPLT      (0x00u)
#define TRANSFER_ERROR      (0xFFu)

/* List of supported commands */
#define CMD_SET_OFF         (0u)
#define CMD_SET_RED         (1u)
#define CMD_SET_GREEN       (2u)
#define CMD_SET_BLUE        (3u)

/* Delay between commands in milliseconds */
#define CMD_TO_CMD_DELAY    (500u)

#endif /* (MAIN_H) */


/* [] END OF FILE */
