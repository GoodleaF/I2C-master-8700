/******************************************************************************
* File Name: main.c
*
* Version 1.30
*
* Description: 
*  This code example demonstrates basic usage of PSoC 4 Serial Communication 
*  Block (SCB) Component in I2C Master mode. This project implements the 
*  I2C Master device, which sends commands to I2C Slave device and reads 
*  status of the command execution: success or error. Result or command 
*  execution shown using RGB LED: green for success and red for error.
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

#include "main.h"


/*******************************************************************************
* Function Name: WriteCommandPacket
********************************************************************************
* 
* Summary:
*  Master initiates the transfer to write command packet into the slave.
*  The status of the transfer is returned.
*
* Parameters:
*  cmd: command to execute. Supported commands:
*   - CMD_SET_RED:   set red color of the LED.
*   - CMD_SET_GREEN: set green color of the LED.
*   - CMD_SET_BLUE:  set blue color of the LED.
*   - CMD_SET_OFF:   turn off the LED.
*
* Return:
*  Status of the data transfer. There are two statuses
*  - TRANSFER_CMPLT: transfer completed successfully.
*  - TRANSFER_ERROR: the error occurred while transfer.
*
*******************************************************************************/
uint32 WriteCommandPacket(uint8 cmd)
{
    uint8  buffer[BUFFER_SIZE];
    uint32 status = TRANSFER_ERROR;

    /* Initialize buffer with packet */
    buffer[PACKET_SOP_POS] = PACKET_SOP;
    buffer[PACKET_CMD_POS] = cmd;
    buffer[PACKET_EOP_POS] = PACKET_EOP;

    (void) I2CM_I2CMasterClearStatus();
    
    /* Start I2C write and check status*/
    if(I2CM_I2C_MSTR_NO_ERROR == I2CM_I2CMasterWriteBuf(I2C_SLAVE_ADDR,
                                    buffer, PACKET_SIZE,
                                    I2CM_I2C_MODE_COMPLETE_XFER))
    {
        /*If I2C write started without errors, 
        / wait until I2C Master completes write transfer 
        */
        while (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_WR_CMPLT))
        {
            /* Wait */
        }
        
        /* Display transfer status */
        if (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_ERR_XFER))
        {
            /* Check if all bytes was written */
            if (I2CM_I2CMasterGetWriteBufSize() == BUFFER_SIZE)
            {
                status = TRANSFER_CMPLT;
            }
        }
    }

    return (status);
}


/*******************************************************************************
* Function Name: ReadStatusPacket
********************************************************************************
* 
* Summary:
*  Master initiates the transfer to read status packet from the Slave.
*  The status of the transfer is returned.
*
* Parameters:
*  None
*
* Return:
*  Status of the transfer. There are two statuses
*  - TRANSFER_CMPLT: transfer completed successfully.
*  - TRANSFER_ERROR: the error occurred while transfer.
*
*******************************************************************************/
# if 0
uint32 ReadStatusPacket(void)
{
    uint8  buffer[BUFFER_SIZE];
    uint32 status = TRANSFER_ERROR;
    
    (void) I2CM_I2CMasterClearStatus();
    
    if(I2CM_I2C_MSTR_NO_ERROR ==  I2CM_I2CMasterReadBuf(I2C_SLAVE_ADDR,
                                    buffer, PACKET_SIZE,
                                    I2CM_I2C_MODE_COMPLETE_XFER))
    {
        /* If I2C read started without errors, 
        / wait until master complete read transfer */
        while (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_RD_CMPLT))
        {
            /* Wait */
        }
        
        /* Display transfer status */
        if (0u == (I2CM_I2C_MSTAT_ERR_XFER & I2CM_I2CMasterStatus()))
        {
            /* Check packet structure */
            if ((I2CM_I2CMasterGetReadBufSize() == BUFFER_SIZE) &&
                (PACKET_SOP == buffer[PACKET_SOP_POS]) &&
                (PACKET_EOP == buffer[PACKET_EOP_POS]))
            {
                /* Check packet status */
                if (STS_CMD_DONE == buffer[PACKET_STS_POS])
                {
                    status = TRANSFER_CMPLT;
                }
            }
        }
    }

    return (status);
}
#endif


/*******************************************************************************
* Function Name: main
********************************************************************************
* 
* Summary:
*  This function:
*   1. Turns off the RGB LED.
*   2. Starts the I2C Master Component.
*   3. Enables global interrupts.
*   4. Sends command and reads status packets from the Slave every 500mS.
*
*******************************************************************************/
uint8  readbuffer[BUFFER_SIZE];
uint8  writebuffer1[2u];
uint8  writebuffer2[2u];
uint8  writebuffer3[2u];//1~3 칩을 켜는 커맨드
uint8  writebuffer4[4u];//[0] addr 주소값 
uint8  writebuffer5[4u];//[1],[2] == displaymode 전부 켬(1)
uint8  writebuffer6[4u];//[3]==[1],[2],[3] 3개를 세로로 놨을 때 1의 갯수가 짝수가 되도록 0과 1조정한 값 
uint8  writebuffer7[4u];
uint8  writebuffer8[4u];
uint8  writebuffer9[4u];
uint8  writebuffer10[4u];
uint8  writebuffer11[4u];
uint8  writebuffer12[4u];
uint8 totalwbuffer[32u];
int main()
{
    //uint8 command = CMD_SET_RED;
    //uint32 status = TRANSFER_ERROR;

    /* Disable LEDs 
    //LED_SUCCESS_Write(LED_SUCCESS_OFF);
    //LED_ERROR_Write(LED_ERROR_OFF);
    
     Start the I2C Master */
   
    //int d =0;
    //칩 켜는 부분
    writebuffer1[0] = 0x11;//디스플레이 on/ off
    writebuffer1[1] = 0x11;
    writebuffer2[0] = 0x24;//표시될 디스플레이 범위
    writebuffer2[1] = 0x24;
    writebuffer3[0] = 0x37;//디스플레이 밝기
    writebuffer3[1] = 0x37;    
 
    
    
    CyGlobalIntEnable;
    I2CM_Start();

    for(;;)
    {
        
    writebuffer4[0] = 0x50;//윗변 + 윗 물방울//위만 led on
    writebuffer4[1] = 0xC0;//0xC5 0x0F
    writebuffer4[2] = 0xC0;//0x58 0x0C
    writebuffer4[3] = 0x50;//0xCD 0x53     parity data
    
    writebuffer5[0] = 0x51;//오른쪽 변+ 위'C
    writebuffer5[1] = 0xC0;//0xC5 0x0F
    writebuffer5[2] = 0xC0;//0x58 0x0C
    writebuffer5[3] = 0x51;//0xCC 0x52
    
    writebuffer6[0] = 0x52;//오른쪽 아랫변
    writebuffer6[1] = 0xC0;//0xC5 0x0F
    writebuffer6[2] = 0xC0;//0x58 0x0C
    writebuffer6[3] = 0x52;//0xCF 0x51
    
    writebuffer7[0] = 0x53;//아랫변 + 아래'C
    writebuffer7[1] = 0xC0;//0xC5 0x0F
    writebuffer7[2] = 0xC0;//0x58 0x0C
    writebuffer7[3] = 0x53;//0xCE 0x50
    
    writebuffer8[0] = 0x54;//왼쪽 아랫변+아래 눈꽃
    writebuffer8[1] = 0xC0;//0xC5 0x0F
    writebuffer8[2] = 0xC0;//0x58 0x0C
    writebuffer8[3] = 0x54;//0xC9 0x57
    
    writebuffer9[0] = 0x55;//왼쪽 윗변 + 아래 자물쇠
    writebuffer9[1] = 0xC0;//0xC5 0x0F
    writebuffer9[2] = 0xC0;//0x58 0x0C
    writebuffer9[3] = 0x55;//0xC8 0x56
       
    writebuffer10[0] = 0x56;//가운데 + 아래 와이파이
    writebuffer10[1] = 0xC0;//0xC5 0x0F
    writebuffer10[2] = 0xC0;//0x58 0x0C
    writebuffer10[3] = 0x56;//0xCB 0x55
      
    writebuffer11[0] = 0x57;//마이너스
    writebuffer11[1] = 0xC0;//0xC5 0x0F
    writebuffer11[2] = 0xC0;//0x58 0x0C
    writebuffer11[3] = 0x57;//0xCA 0x54
    
    
    int d=0;
    
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer4[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer5[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer6[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer7[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer8[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer9[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer10[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer11[i];
    }
        
        I2CM_I2CMasterWriteBuf(I2C_SLAVE_ADDR,
                                    writebuffer1, 2u,
                                    I2CM_I2C_MODE_COMPLETE_XFER);
        CyDelay(10);
        /*while (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_WR_CMPLT))
        {
            // Wait 
        }*/
       
        I2CM_I2CMasterWriteBuf(I2C_SLAVE_ADDR,
                                    writebuffer2, 2u,
                                    I2CM_I2C_MODE_COMPLETE_XFER);
        CyDelay(10);
        /*while (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_WR_CMPLT))
        {
            // Wait 
        }*/
       
        I2CM_I2CMasterWriteBuf(I2C_SLAVE_ADDR,
                                    writebuffer3, 2u,
                                    I2CM_I2C_MODE_COMPLETE_XFER);
        CyDelay(10);
        
        
        for(int i=0;i<8;i++){
        I2CM_I2CMasterWriteBuf(I2C_SLAVE_ADDR,
                                    &totalwbuffer[4*i], 4u,
                                    I2CM_I2C_MODE_COMPLETE_XFER);
        CyDelay(1);
        }
        
        CyDelay(1000);
        
    writebuffer4[0] = 0x50;//윗변 + 윗 물방울//아래만 led on
    writebuffer4[1] = 0x30;
    writebuffer4[2] = 0xF0;
    writebuffer4[3] = 0x90;//parity data
    
    writebuffer5[0] = 0x51;//오른쪽 변+ 위'C
    writebuffer5[1] = 0x30;
    writebuffer5[2] = 0xF0;
    writebuffer5[3] = 0x91;
    
    writebuffer6[0] = 0x52;//오른쪽 아랫변
    writebuffer6[1] = 0x30;
    writebuffer6[2] = 0xF0;
    writebuffer6[3] = 0x92;
    
    writebuffer7[0] = 0x53;//아랫변 + 아래'C
    writebuffer7[1] = 0x30;
    writebuffer7[2] = 0xF0;
    writebuffer7[3] = 0x93;
    
    writebuffer8[0] = 0x54;//왼쪽 아랫변+아래 눈꽃
    writebuffer8[1] = 0x30;
    writebuffer8[2] = 0xF0;
    writebuffer8[3] = 0x94;
    
    writebuffer9[0] = 0x55;//왼쪽 윗변 + 아래 자물쇠
    writebuffer9[1] = 0x30;
    writebuffer9[2] = 0xF0;
    writebuffer9[3] = 0x95;
       
    writebuffer10[0] = 0x56;//가운데 + 아래 와이파이
    writebuffer10[1] = 0x30;
    writebuffer10[2] = 0xF0;
    writebuffer10[3] = 0x96;
      
    writebuffer11[0] = 0x57;//마이너스
    writebuffer11[1] = 0x30;
    writebuffer11[2] = 0xF0;
    writebuffer11[3] = 0x97;
    
    
    d=0;
    
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer4[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer5[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer6[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer7[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer8[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer9[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer10[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer11[i];
    }
        
        
        
        for(int i=0;i<8;i++){
        I2CM_I2CMasterWriteBuf(I2C_SLAVE_ADDR,
                                    &totalwbuffer[4*i], 4u,
                                    I2CM_I2C_MODE_COMPLETE_XFER);
        CyDelay(1);
        }
        
        CyDelay(1000);
        
        
    writebuffer4[0] = 0x50;//윗변 + 윗 물방울//위만 led on
    writebuffer4[1] = 0x0F;//0xC5 0x0F
    writebuffer4[2] = 0x0C;//0x58 0x0C
    writebuffer4[3] = 0x53;//0xCD 0x53     parity data
    
    writebuffer5[0] = 0x51;//오른쪽 변+ 위'C
    writebuffer5[1] = 0x0F;//0xC5 0x0F
    writebuffer5[2] = 0x0C;//0x58 0x0C
    writebuffer5[3] = 0x52;//0xCC 0x52
    
    writebuffer6[0] = 0x52;//오른쪽 아랫변
    writebuffer6[1] = 0x0F;//0xC5 0x0F
    writebuffer6[2] = 0x0C;//0x58 0x0C
    writebuffer6[3] = 0x51;//0xCF 0x51
    
    writebuffer7[0] = 0x53;//아랫변 + 아래'C
    writebuffer7[1] = 0x0F;//0xC5 0x0F
    writebuffer7[2] = 0x0C;//0x58 0x0C
    writebuffer7[3] = 0x50;//0xCE 0x50
    
    writebuffer8[0] = 0x54;//왼쪽 아랫변+아래 눈꽃
    writebuffer8[1] = 0x0F;//0xC5 0x0F
    writebuffer8[2] = 0x0C;//0x58 0x0C
    writebuffer8[3] = 0x57;//0xC9 0x57
    
    writebuffer9[0] = 0x55;//왼쪽 윗변 + 아래 자물쇠
    writebuffer9[1] = 0x0F;//0xC5 0x0F
    writebuffer9[2] = 0x0C;//0x58 0x0C
    writebuffer9[3] = 0x56;//0xC8 0x56
       
    writebuffer10[0] = 0x56;//가운데 + 아래 와이파이
    writebuffer10[1] = 0x0F;//0xC5 0x0F
    writebuffer10[2] = 0x0C;//0x58 0x0C
    writebuffer10[3] = 0x55;//0xCB 0x55
      
    writebuffer11[0] = 0x57;//마이너스
    writebuffer11[1] = 0x0F;//0xC5 0x0F
    writebuffer11[2] = 0x0C;//0x58 0x0C
    writebuffer11[3] = 0x54;//0xCA 0x54
        
        d=0;
    
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer4[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer5[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer6[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer7[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer8[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer9[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer10[i];
    }
    
    d++;
    for(int i=0; i < 4; i++){
        totalwbuffer[4*d+i] = writebuffer11[i];
    }
        
        
        
        for(int i=0;i<8;i++){
        I2CM_I2CMasterWriteBuf(I2C_SLAVE_ADDR,
                                    &totalwbuffer[4*i], 4u,
                                    I2CM_I2C_MODE_COMPLETE_XFER);
        CyDelay(1);
        }
        
        CyDelay(1000);
        
        
        #if 0
        
        /*
        while (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_WR_CMPLT))
        {
            // Wait 
        }
        */
        
        I2CM_I2CMasterWriteBuf(I2C_SLAVE_ADDR,
                                    writebuffer4, 4u,
                                    I2CM_I2C_MODE_COMPLETE_XFER);
        CyDelay(10);
        /*
        while (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_WR_CMPLT))
        {
            // Wait 
        }
        */    
        
        I2CM_I2CMasterWriteBuf(I2C_SLAVE_ADDR,
                                    writebuffer5, 4u,
                                    I2CM_I2C_MODE_COMPLETE_XFER);
        CyDelay(10);
        /*
        while (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_WR_CMPLT))
        {
            // Wait 
        }
        */
        
        I2CM_I2CMasterWriteBuf(I2C_SLAVE_ADDR,
                                    writebuffer6, 4u,
                                    I2CM_I2C_MODE_COMPLETE_XFER);
        CyDelay(10);
        /*
        while (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_WR_CMPLT))
        {
            // Wait 
        }
        */
        
        I2CM_I2CMasterWriteBuf(I2C_SLAVE_ADDR,
                                    writebuffer7, 4u,
                                    I2CM_I2C_MODE_COMPLETE_XFER);
        CyDelay(10);
        /*
        while (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_WR_CMPLT))
        {
            // Wait 
        }
        */
        
        I2CM_I2CMasterWriteBuf(I2C_SLAVE_ADDR,
                                    writebuffer8, 4u,
                                    I2CM_I2C_MODE_COMPLETE_XFER);
        CyDelay(10);
        /*
        while (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_WR_CMPLT))
        {
            // Wait 
        }
        */
        
        I2CM_I2CMasterWriteBuf(I2C_SLAVE_ADDR,
                                    writebuffer9, 4u,
                                    I2CM_I2C_MODE_COMPLETE_XFER);
        CyDelay(10);
        /*
        while (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_WR_CMPLT))
        {
            // Wait 
        }
        */
        
        I2CM_I2CMasterWriteBuf(I2C_SLAVE_ADDR,
                                    writebuffer10, 4u,
                                    I2CM_I2C_MODE_COMPLETE_XFER);
        CyDelay(10);
        /*
        while (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_WR_CMPLT))
        {
            // Wait 
        }
        */
        
        I2CM_I2CMasterWriteBuf(I2C_SLAVE_ADDR,
                                    writebuffer11, 4u,
                                    I2CM_I2C_MODE_COMPLETE_XFER);
        CyDelay(10);
        /*
        while (0u == (I2CM_I2CMasterStatus() & I2CM_I2C_MSTAT_WR_CMPLT))
        {
            // Wait 
        }
        */
        #endif
        #if 0
        //status = TRANSFER_ERROR;
        
        /* Send packet with command to the slave */
        if (TRANSFER_CMPLT == WriteCommandPacket(command))
        {
            /* Read response packet from the slave */
            if (TRANSFER_CMPLT == ReadStatusPacket())
            {
                status = TRANSFER_CMPLT;
                
                /* Next command to be written */
                command++;
                if (command > CMD_SET_BLUE)
                {
                    command = CMD_SET_OFF;
                }
            }
        }

        if(status == TRANSFER_CMPLT)
        {
            LED_SUCCESS_Write(LED_SUCCESS_ON);
            LED_ERROR_Write(LED_ERROR_OFF);
        }
        else
        {
            LED_SUCCESS_Write(LED_SUCCESS_OFF);
            LED_ERROR_Write(LED_ERROR_ON);
        }
        
        /* Delay between the commands */
        CyDelay(CMD_TO_CMD_DELAY); 
        #endif
    }
    
}


/* [] END OF FILE */
