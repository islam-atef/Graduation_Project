/********************************************************************************************/
/* 	AUTHOR  		: islam atef Mohamed 													*/
/* 	VERSION 		:	  V2.0 																*/
/* 	DATE    		:  1/2023 																*/
/*	Description  	:  This is the Program file For the USART Asynchronous Modes Peripheral */
/*											at ARM-CORTEX m4								*/
/********************************************************************************************/
#include "LIB/BIT_MATH.h"
#include "LIB/STD_Types.h"

#include "USART_private.h"
#include "USART_config.h"
#include "USART_interface.h"

#include "LMCAL/01_STK/STK_interface.h"
/********************************************************************************************/
static Uart_Fun_Status UART_Transmit_Handler(USART_Struct *USARTx);
static Uart_Fun_Status UART_Receive_Handler(USART_Struct *USARTx);
static Uart_LOCK_ST    UART_Check_LockState(USART_Struct *USARTx ,COMM_TYPE _CommType_ );
/********************************************************************************************/
static void (* USART1_CallBack) (void) = NULL ;
u8  __USART1__INTERRUPT_TYPE__ ;
static void (* USART2_CallBack) (void) = NULL ;
u8  __USART2__INTERRUPT_TYPE__ ;
static void (* USART6_CallBack) (void) = NULL ;
u8  __USART6__INTERRUPT_TYPE__ ;

USART_Struct *USART1_Struct;
USART_Struct *USART2_Struct;
USART_Struct *USART6_Struct;
/********************************************************************************************/


/// @brief  MCAL_USART_Init_      	: this function performs the Initialization process of The Peripheral .
/// @param  USARTx                	: the Struct of Peripheral's Registers .
/// @param  USART_frame_struct      : the struct of frame characteristics Options .
/// @param  USART_receiving_struct : the struct of Received Data Handling Options .
/// @param	copy_u32BaudRate		: the baud Rate of the Peripheral.
/// @retval	Functions Status.
Uart_Fun_Status	    MCAL_UART_Init_(USART_Struct *USARTx , MUSART_Frame_Config *USART_frame_struct, 
					             MUSART_Receiving_Config *USART_receiving_struct, u32 copy_u32BaudRate )
{
    // Check the USARTx struct.
    if ((USARTx == NULL) || (USARTx -> USART_x == NULL) || ( USARTx -> Time_Limit == NULL ))
    {
        return  Uart_ERROR;
    }
    else
    {
        if      (USARTx -> USART_x == USART1_R){ USART1_Struct = USARTx; }
        else if (USARTx -> USART_x == USART2_R){ USART2_Struct = USARTx; }
        else if (USARTx -> USART_x == USART6_R){ USART6_Struct = USARTx; }
    }
    /* First : define the Frame properties */  
    //  Word Size
    USARTx->USART_x->CR1 |= ( USART_frame_struct -> M_VALUE << CR1_M ) ;

    // Parity Bit 
    switch (USART_frame_struct -> parity_op)
    {
    case Parity_Disable :
        USARTx -> USART_x -> CR1 |= (Disable << CR1_PCE )  ; 
        break;

    case Even_Parity :
        USARTx -> USART_x -> CR1 |= (Enable << CR1_PCE ) | ( (Even_Parity -1)<< CR1_PS ) ;
        break;

    case Odd_Parity :
       USARTx -> USART_x -> CR1 |= (Enable << CR1_PCE ) | ( (Odd_Parity -1)<< CR1_PS ) ;
        break;

    default:   
        break;
    }
    // 3- Stop Bit Size
    USARTx -> USART_x -> CR2 |= (USART_frame_struct -> Stop_Bit_NUM << CR2_STOP0 ) ;
/*--------------------------------------------------------------------------------------------------*/
// Second : define The Receiving data processes
/*--------------------------------------------------------------------------------------------------*/
    // 1- Oversampling_Value type
    USARTx -> USART_x -> CR1 |= (USART_receiving_struct -> Oversampling_type << CR1_OVER8) ;

    // 2- OneBit_Sample method
    USARTx -> USART_x -> CR3 |= (USART_receiving_struct -> OneBit_Sampling_method << CR3_ONEBIT) ;
/*--------------------------------------------------------------------------------------------------*/
// third : define The Operation Mode
/*--------------------------------------------------------------------------------------------------*/
    // 1- Disable all Other Modes (LIN), (Synchronous), (Smartcard) and (IrDA)
    USARTx -> USART_x -> CR2 |= (Disable << CR2_CLKEN) | (Disable << CR2_LINEN) ;
    USARTx -> USART_x -> CR3 |= (Disable << CR3_IREN_) | (Disable << CR3_SCEN) ;
/*--------------------------------------------------------------------------------------------------*/
// Fourth : define the Baud Rate
/*--------------------------------------------------------------------------------------------------*/
    // 1- get the value of the total DIV
    f64 DIV = ((f64)(FCK)/(f64)((copy_u32BaudRate)*8*( 2 - GET_BIT(USARTx -> USART_x -> CR1 , CR1_OVER8) ) ) ) ;

    // 2- get the DIV_mantissa value
    u16 DIV_mantissa = (u16) DIV ;
    // 3- put the DIV_mantissa into the (15-4)bits in the (BRR) register
    USARTx -> USART_x -> BRR |= (DIV_mantissa << 4) ;

    // 4- get the DIV_fraction value
    u8 DIV_fraction = ( (DIV - DIV_mantissa) * 16 ) ;

    // 5- Check if the value of the DIV_fraction is not greater the (15)
    if (DIV_fraction <= 15)
    {
        // 6- if true, then put the DIV_fraction into the first four bits in the (BRR) register
        USARTx -> USART_x -> BRR |= DIV_fraction ;
    }
    else
    {
        // 6- if false, then put zero into the first four bits in the (BRR) register
        USARTx -> USART_x -> BRR &= (0xfff0) ;

        // 7-add (1) to the DIV_mantissa value
        DIV_mantissa++ ;

        // 8- put the new DIV_mantissa into the (15-4)bits in the (BRR) register
        USARTx -> USART_x -> BRR |= (DIV_mantissa << 4) ;
    }
/*--------------------------------------------------------------------------------------------------*/
// Fifth : define the Error code in the USARTx Struct.
/*--------------------------------------------------------------------------------------------------*/
    USARTx -> Error_Code = (u8 *)Error_1;
/*--------------------------------------------------------------------------------------------------*/
    return  Uart_OK;
}




/// @brief  MCAL_USART_Enable  : the function responses of Enabling the Peripheral, Start The Communication and defining Its Type. 
/// @param  USARTx             : the Struct of Peripheral's Registers .
/// @retval	Functions Status.
Uart_Fun_Status	    MCAL_UART_Enable( USART_Struct *USARTx )
{
    __UART_ENABLE( USARTx -> USART_x );
    return Uart_OK;
}

/// @brief  MCAL_USART_Disable  : the function responses of Disable the Whole Peripheral or Disable part of the Communication process.
/// @param  USARTx              : the Struct of Peripheral's Registers.
/// @retval	Functions Status.
Uart_Fun_Status	    MCAL_UART_Disable( USART_Struct *USARTx )
{
    __UART_DISABLE( USARTx -> USART_x );
    return Uart_OK;
}




/// @brief MCAL_USART_Transmit   : this function Transmit a given data by the synchronous mode " Blocking".
/// @param USARTx                : the Struct of Peripheral's Registers.
/// @param ptData                : pointer of data we want to Transmit.
/// @param Size                  : the size of the data that will be Transmitted.
/// @param Time_Limit            : the maximum time for this function. 
///@retval Functions Status.
Uart_Fun_Status	    MCAL_UART_Transmit(USART_Struct *USARTx , u8 *ptData ,u16 Size, u32 Time_Limit ,u8 Last_element)
{    
    // Check the Given data and the size values.
    if( (ptData == NULL ) || (Size == 0) || (Time_Limit == 0)){ return  Uart_ERROR; }
    
    if (UART_Check_LockState(USARTx ,TX ) == BUSY)
    {
        return Uart_BUSY;
    }
    // the Starting conditions:
    USARTx ->TX_Lock_Flag = BUSY;
    USARTx ->TX_Lock_Counter = 0;

    u8 *Local_buffer;

    // Disable Rx.
    __COMM_DISABLE(USARTx,RX);
    // Enable Tx
    __COMM_ENABLE(USARTx,TX);

    // Define the rest of elements iin the USARTx Struct.
    USARTx -> TX_Buffer_Ptr     = ptData;
    USARTx -> TX_Buffer_Size    = Size;
    USARTx -> TX_Process_Count  = (s16)Size;
    USARTx -> Error_Code        = (u8 *)Error_1;
    USARTx -> TX_Buffer_lastEL  = Last_element;
    // start timer;
    MSTK_voidStartTimer();
    // enter the Transmission process, send {MSB} first.
    while ( (USARTx -> TX_Process_Count) > 0)
    {
        // check the Timer.
        if (MSTK_u32GetElapsedTime() == Time_Limit)
        {
            MSTK_voidStopTimer();
            USARTx ->TX_Lock_Flag = IDLE;
            USARTx ->TX_Lock_Counter = 0;
            // Disable Tx.
            __COMM_DISABLE(USARTx,TX);
            return Uart_TIMEOUT;
        }
        /* Check the (TRANSMIT DATA REGISTER EMPTY) flag "TXE" in SR register if it is {1} or not , 
           and check the local_counter to know if we in the first location or not. */
        if((__UART_GET_FLAG(USARTx -> USART_x,__TXE__) == 1) || ( (USARTx -> TX_Process_Count) == Size) )
        {
            (USARTx -> TX_Process_Count)--;
        	Local_buffer = (u8 *)ptData;
            // load the Transmit word into the (DR) register 
            USARTx -> USART_x -> DR =  (*Local_buffer & (u8)0x00FF);
            ptData += 1U ;
            // Check the (TRANSMISSION COMPLETE) flag "TC" in SR register if it is {0} or not and wait till it is {1}.
            while (__UART_GET_FLAG(USARTx -> USART_x,__TC__) == 0 )
            { 
                // Check the Timer.
                if (MSTK_u32GetElapsedTime() == Time_Limit)
                {
                    // stop the Timer.
                    MSTK_voidStopTimer();
                    USARTx ->TX_Lock_Flag = IDLE;
                    USARTx ->TX_Lock_Counter = 0;
                    // Disable Tx.
                    __COMM_DISABLE(USARTx,TX);
                    return Uart_TIMEOUT;
                }
            }
            // clear the {TC} flag.
            __UART_CLEAR_FLAG(USARTx -> USART_x, __TC__);
            // Check the last element.
            if ( *Local_buffer == Last_element)
            {
                (USARTx -> TX_Buffer_Size) -= (USARTx -> TX_Process_Count+1);
                MSTK_voidStopTimer();
                USARTx ->TX_Lock_Flag = IDLE;
                USARTx ->TX_Lock_Counter = 0;
                // Disable Tx.
                __COMM_DISABLE(USARTx,TX);
                return Uart_UNDERSIZE ;
            }
        }
    }
    if (*Local_buffer != USARTx -> TX_Buffer_lastEL )
    {
        MSTK_voidStopTimer();
        USARTx ->TX_Lock_Flag = IDLE;
        USARTx ->TX_Lock_Counter = 0;
        // Disable Tx.
        __COMM_DISABLE(USARTx,TX);
        return Uart_OVERSIZE ;
    }
    // stop the Timer.
    MSTK_voidStopTimer();
    USARTx -> TX_Process_Count = 0;
    USARTx ->TX_Lock_Flag = IDLE;
    USARTx ->TX_Lock_Counter = 0;
    // Disable Tx.
    __COMM_DISABLE(USARTx,TX);
    return Uart_OK;
}


/// @brief MCAL_UART_Receive   : this function Receive an amount of data by the synchronous mode " Blocking".
/// @param USARTx                : the Struct of Peripheral's Registers.
/// @param ptData                : pointer of data we want to Transmit.
/// @param Size                  : the size of the data that will be Transmitted.
/// @param Time_Limit            : the maximum time for this function. 
/// @param Last_element          : the last element that should be Received.
///@retval Functions Status
Uart_Fun_Status MCAL_UART_Receive( USART_Struct *USARTx , u8 *ptData ,u16 Size_Limit , u32 Wait_Time, u8 Last_element)
{
    // Check the Given data and the size values.
    if( (ptData == NULL ) || (Size_Limit == 0) || (Wait_Time == 0)){ return  Uart_ERROR; }

    if (UART_Check_LockState(USARTx ,RX ) == BUSY)
    {
        return Uart_BUSY;
    }
    // the Starting conditions:
    USARTx ->RX_Lock_Flag = BUSY;

    u8 *Local_buffer;

    // Disable Tx.
    __COMM_DISABLE(USARTx,TX);

    // Enable Rx.
    __COMM_ENABLE(USARTx,RX);

    // Define the rest of elements iin the USARTx Struct.
    USARTx -> RX_Buffer_Ptr     = ptData;
    USARTx -> RX_Buffer_Size    = Size_Limit;
    USARTx -> RX_Process_Count  = (s16)Size_Limit;
    USARTx -> Error_Code        = (u8 *)Error_1;
    USARTx -> RX_Buffer_lastEL  = Last_element;

    // start timer;
    MSTK_voidStartTimer();

    // wait until Receive first element.
    while ( !( __UART_GET_FLAG(USARTx -> USART_x,__RXNE__) ) )
    {
        if(MSTK_u32GetElapsedTime() >= Wait_Time)
        {
            // stop the Timer.
            MSTK_voidStopTimer();

            USARTx ->RX_Lock_Flag = IDLE;
            USARTx ->RX_Lock_Counter = 0;

            // Disable Rx.
            __COMM_DISABLE(USARTx,RX);

            return Uart_TIMEOUT;
        }
    }

    // enter the Transmission process
    USARTx -> Time_Limit += MSTK_u32GetElapsedTime();
    while ((USARTx -> RX_Process_Count) > 0)
    {
        // Check the (Read DATA REGISTER Not EMPTY) flag "RXNE" in SR register if it is {1} or not.
        if(__UART_GET_FLAG(USARTx -> USART_x,__RXNE__))
        {
            // check the OverWrite Error flag.
            if (__UART_GET_FLAG(USARTx -> USART_x,__ORE__) ==  1)
            {
                USARTx -> Error_Code = (u8 *)Error_5;
                USARTx ->RX_Lock_Flag = IDLE;
                USARTx ->RX_Lock_Counter = 0;

                // stop the Timer.
                MSTK_voidStopTimer();

                // Disable Rx.
                __COMM_DISABLE(USARTx,RX);
                return  Uart_ERROR;
            }

            if ((__UART_GET_FLAG(USARTx -> USART_x,__PE__)||__UART_GET_FLAG(USARTx -> USART_x,__FE__)||__UART_GET_FLAG(USARTx -> USART_x,__NE__)) != 0)
            {
                USARTx ->RX_Lock_Flag = IDLE;
                USARTx ->RX_Lock_Counter = 0;

                // stop the Timer.
                MSTK_voidStopTimer();

                // Disable Rx.
                __COMM_DISABLE(USARTx,RX);
                return  Uart_ERROR;
            }

            (USARTx -> RX_Process_Count)--;
            Local_buffer = (u8 *)ptData;

            // check the parity if Enabled or Disable.
            if (GET_BIT(USARTx->USART_x -> CR1,CR1_PCE) == 0)
            {
                // store the Recevied word into the given pointer location.
                *Local_buffer = (u8)(USARTx -> USART_x -> DR & (u8)0x00FF);
                __UART_CLEAR_FLAG(USARTx -> USART_x,__RXNE__);
            }
            // the parity bit is the 8th-bit.
            else   
            {
                // store the Received word into the given pointer location.
                *Local_buffer = (u8)(USARTx -> USART_x -> DR & (u8)0x007F);
                __UART_CLEAR_FLAG(USARTx -> USART_x,__RXNE__);
            }

            ptData += 1U ;

            // Check the Received element.
            if (*Local_buffer == Last_element)
            {
                USARTx -> RX_Buffer_Size -= (USARTx -> RX_Process_Count +1);
                MSTK_voidStopTimer();
                USARTx ->RX_Lock_Flag = IDLE;
                USARTx ->RX_Lock_Counter = 0;
                // Disable Rx.
                __COMM_DISABLE(USARTx,RX);
                return Uart_UNDERSIZE ;
            }

            // update the Time_Limit value;
            USARTx -> Time_Limit += MSTK_u32GetElapsedTime();
        }

        // check the Timer.
        if (MSTK_u32GetElapsedTime() == USARTx -> Time_Limit)
        {
            // stop the Timer.
            MSTK_voidStopTimer();
            USARTx ->RX_Lock_Flag = IDLE;
            USARTx ->RX_Lock_Counter = 0;
            USARTx ->Error_Code = (u8 *)Error_6;
            // Disable Rx.
            __COMM_DISABLE(USARTx,RX);
            return Uart_TIMEOUT;
        }
    }

    // check if the last element in the buffer after reaching its maximum is the given last element.
    if (*Local_buffer != USARTx -> RX_Buffer_lastEL )
    {
        // stop the Timer.
        MSTK_voidStopTimer();

        USARTx ->RX_Lock_Flag = IDLE;
        USARTx ->RX_Lock_Counter = 0;

        // Disable Rx.
        __COMM_DISABLE(USARTx,RX);
        return Uart_OVERSIZE ;
    }

    // stop the Timer.
    MSTK_voidStopTimer();

    USARTx ->RX_Lock_Flag = IDLE;
    USARTx ->RX_Lock_Counter = 0;

    // Disable Rx.
    __COMM_DISABLE(USARTx,RX);
    return Uart_OK;
}





/// @brief MCAL_USART_Transmit_INT  : this function Transmit a given data by the Asynchronous mode "Interrupt".
/// @param USARTx                   : the Struct of Peripheral's Registers.
/// @param ptData                   : pointer of data we want to Transmit.
/// @param Size                     : the size of the data that will be Transmitted.
/// @param Time_Limit               : the maximum time for this function. 
///@retval Functions Status.
Uart_Fun_Status	    MCAL_UART_Transmit_INT(USART_Struct *USARTx , u8 *ptData ,u16 Size ,u8 Last_element)
{
    // Check the Given data and the size values.
    if( (ptData == NULL ) || (Size == 0) ){ return  Uart_ERROR; }
    if (UART_Check_LockState(USARTx ,TX ) == BUSY)
    {
        return Uart_BUSY;
    }
    // the Starting conditions:
    USARTx ->TX_Lock_Flag = BUSY;
    USARTx ->TX_Lock_Counter = 0;

    u8 *Local_buffer;
    
   // Disable Rx.
    __COMM_DISABLE(USARTx,RX);
    // Enable Tx
    __COMM_ENABLE(USARTx,TX);

    // Define the rest of elements iin the USARTx Struct.
    USARTx -> TX_Buffer_Ptr     = ptData;
    USARTx -> TX_Buffer_Size    = Size;
    USARTx -> TX_Process_Count  = (s16)Size;
    USARTx -> Error_Code        = (u8 *)Error_1;
    USARTx -> TX_Buffer_lastEL  = Last_element;
    
    // Clear the Transmit complete flag.
    __UART_CLEAR_FLAG(USARTx -> USART_x ,__TC__);
    // Enable the TX register empty interrupt.
    SET_BIT(USARTx -> USART_x ->CR1, CR1_TCIE);

    // Send First element.
    if((__UART_GET_FLAG(USARTx -> USART_x,__TXE__) == 1) || ( (USARTx -> TX_Process_Count) == Size) )
    {
        (USARTx -> TX_Process_Count)--;
        Local_buffer = (u8 *)USARTx -> TX_Buffer_Ptr;
        // load the Transmit word into the (DR) register 
        USARTx -> USART_x -> DR =  (*Local_buffer & (u8)0x00FF);
        USARTx -> TX_Buffer_Ptr ++;
        if(--USARTx->TX_Process_Count < 0)
        {
          /* Disable the UART Transmit Complete Interrupt */
          CLR_BIT(USARTx -> USART_x ->CR1, CR1_TCIE);
          if (*Local_buffer != USARTx -> TX_Buffer_lastEL )
            {
                USARTx ->TX_Lock_Flag = IDLE;
                USARTx ->TX_Lock_Counter = 0;
                // Enable Rx
                __COMM_ENABLE(USARTx,RX);
                return Uart_OVERSIZE ;
            }
        }
        else
        {
            // Check the last Transmitted element.
            if (*Local_buffer == USARTx -> TX_Buffer_lastEL )
            {
                /* Disable the UART Transmit Complete Interrupt */
                CLR_BIT(USARTx -> USART_x ->CR1, CR1_TCIE);
                (USARTx -> TX_Buffer_Size) -= ((USARTx -> TX_Process_Count) +1);
                USARTx ->TX_Lock_Flag = IDLE;
                USARTx ->TX_Lock_Counter = 0;
                // Enable Rx
                __COMM_ENABLE(USARTx,RX);
                return Uart_UNDERSIZE ;
            }
        }
    }
    else
    {
       // Enable Rx
       __COMM_ENABLE(USARTx,RX);
        return Uart_ERROR;
    }
    // Enable Rx
    __COMM_ENABLE(USARTx,RX);
    return Uart_OK;
}


/// @brief  UART_Transmit_Handler    : it is the function that will be performed inside the USART_IRQHandler in the TCIE interrupt.
/// @param  USARTx                   : the Struct of Peripheral's Registers.
/// @return Functions Status.
static Uart_Fun_Status UART_Transmit_Handler(USART_Struct *USARTx)
{
    // Disable Read register not empty interrupt. 
    CLR_BIT(USARTx -> USART_x ->CR1, CR1_RXNEIE);

    u8 *Local_buffer;
    if (__UART_GET_FLAG(USARTx -> USART_x, __TXE__))
    {
        (USARTx -> TX_Process_Count)--;
        // load the Transmit word into the Local_buffer. 
        Local_buffer = (u8 *)USARTx -> TX_Buffer_Ptr;
        // load the Transmit word into the (DR) register 
        USARTx -> USART_x -> DR =  (*Local_buffer & (u8)0x00FF);
        USARTx -> TX_Buffer_Ptr += 1U;
        USARTx -> TX_Lock_Counter = 0;
        // Check if the buffer reaches its end. 
        if(--USARTx->TX_Process_Count < 0)
        {
          /* Disable the UART Transmit Complete Interrupt */
          CLR_BIT(USARTx -> USART_x ->CR1, CR1_TCIE);
          if (*Local_buffer != USARTx -> TX_Buffer_lastEL )
            {
                USARTx ->TX_Lock_Flag = IDLE;
                USARTx ->TX_Lock_Counter = 0;
                // Enable Read register not empty interrupt. 
                SET_BIT(USARTx -> USART_x ->CR1, CR1_RXNEIE);
                return Uart_OVERSIZE ;
            }
        }
        else
        {
            // Check the last Transmitted element.
            if (*Local_buffer == USARTx -> TX_Buffer_lastEL )
            {
                /* Disable the UART Transmit Complete Interrupt */
                CLR_BIT(USARTx -> USART_x ->CR1, CR1_TCIE);
                (USARTx -> TX_Buffer_Size) -= ((USARTx -> TX_Process_Count) +1);
                USARTx ->TX_Lock_Flag = IDLE;
                USARTx ->TX_Lock_Counter = 0;
                // Enable Read register not empty interrupt. 
                SET_BIT(USARTx -> USART_x ->CR1, CR1_RXNEIE);
                return Uart_UNDERSIZE ;
            }
        }
        // Enable Read register not empty interrupt. 
        SET_BIT(USARTx -> USART_x ->CR1, CR1_RXNEIE);
        return Uart_OK;
    }
    // Enable Read register not empty interrupt. 
    SET_BIT(USARTx -> USART_x ->CR1, CR1_RXNEIE);
    return Uart_BUSY;
}




/// @brief MCAL_USART_Transmit_INT  : this function Receive an amount of data by the Asynchronous mode "Interrupt".
/// @param USARTx                   : the Struct of Peripheral's Registers.
/// @param ptData                   : pointer of data we want to Transmit.
/// @param Size                     : the size of the data that will be Transmitted.
/// @param Time_Limit               : the maximum time for this function. 
///@retval Functions Status.
Uart_Fun_Status	    MCAL_UART_Receive_INT(USART_Struct *USARTx , u8 *ptData ,u16 Size_Limit, u8 Last_element)
{
    // Check the Given data and the size values.
    if( (ptData == NULL ) || (Size_Limit == 0)){ return  Uart_ERROR; }

    if (UART_Check_LockState(USARTx ,RX ) == BUSY)
    {
        return Uart_BUSY;
    }

    // Enable Rx
    __COMM_ENABLE(USARTx,RX);

    // the Starting conditions:
    USARTx ->RX_Lock_Flag = BUSY;
    USARTx ->RX_Lock_Counter = 0;

    // Define the rest of elements iin the USARTx Struct.
    USARTx -> RX_Buffer_Ptr     = ptData;
    USARTx -> RX_Buffer_Size    = Size_Limit;
    USARTx -> RX_Process_Count  = (s16)Size_Limit;
    USARTx -> Error_Code        = (u8 *)Error_1;
    USARTx -> RX_Buffer_lastEL  = Last_element;
    
    // clear the DR register.
    (void)USARTx ->USART_x ->DR;
    // Clear the Transmit complete flag.
    __UART_CLEAR_FLAG(USARTx -> USART_x ,__RXNE__);
    // Enable Read register not empty interrupt. 
    SET_BIT(USARTx ->USART_x ->CR1, CR1_RXNEIE);

    return Uart_OK;
}


/// @brief  UART_Receive_Handler    : it is the function that will be performed inside the USART_IRQHandler in the RXNEIE, PEIE, and EIE interrupts.
/// @param  USARTx 
/// @return Functions Status.
static Uart_Fun_Status UART_Receive_Handler(USART_Struct *USARTx)
{
    /* Disable the UART Transmit Complete Interrupt */
    CLR_BIT(USARTx -> USART_x ->CR1, CR1_TCIE);

    u8 *Local_buffer;
    u8 Error_counter = 0;

    if (__UART_GET_FLAG(USARTx -> USART_x, __ORE__))
    {
        USARTx -> Error_Code = (u8 *)Error_5;
        USARTx ->RX_Lock_Flag = IDLE;
        USARTx ->RX_Lock_Counter = 0;
        /* Enable the UART Transmit Complete Interrupt */
        SET_BIT(USARTx -> USART_x ->CR1, CR1_TCIE);
        return Uart_ERROR; 
    }
    if ((__UART_GET_FLAG(USARTx -> USART_x,__PE__)||__UART_GET_FLAG(USARTx -> USART_x,__FE__)||__UART_GET_FLAG(USARTx -> USART_x,__NE__)) == 0)
    {
        (USARTx -> RX_Process_Count)--;
        Local_buffer = (u8 *)USARTx -> RX_Buffer_Ptr;
        // check the parity if Enabled or Disable.
        if (GET_BIT(USARTx->USART_x -> CR1,CR1_PCE) == 0)
        {
            // store the Recevied word into the given pointer location.
            *Local_buffer = (u8)(USARTx -> USART_x -> DR & (u8)0x00FF);
        }
        // the parity bit is the 8th-bit.
        else   
        {
            // store the Recevied word into the given pointer location.
            *Local_buffer = (u8)(USARTx -> USART_x -> DR & (u8)0x007F);
        }
        USARTx -> RX_Buffer_Ptr += 1U;
        USARTx -> RX_Lock_Counter = 0;
        // Check if the buffer reaches its end. 
        if(--USARTx->TX_Process_Count < 0)
        {
          // Disable the UART Read register Not empty Interrupt. 
          CLR_BIT(USARTx -> USART_x ->CR1, CR1_RXNEIE);
           if (*Local_buffer != USARTx -> RX_Buffer_lastEL )
            {
                USARTx ->RX_Lock_Flag = IDLE;
                USARTx ->RX_Lock_Counter = 0;
                /* Enable the UART Transmit Complete Interrupt */
                SET_BIT(USARTx -> USART_x ->CR1, CR1_TCIE);
                return Uart_OVERSIZE ;
            }
        }
        else
        {
            // Check the Received element.
            if (*Local_buffer == USARTx -> RX_Buffer_lastEL)
            {
                USARTx -> RX_Buffer_Size -= ((USARTx -> RX_Process_Count) +1);
                USARTx -> RX_Lock_Flag = IDLE;
                USARTx -> RX_Lock_Counter = 0;
                /* Enable the UART Transmit Complete Interrupt */
                SET_BIT(USARTx -> USART_x ->CR1, CR1_TCIE);
                return Uart_UNDERSIZE ;
            }
        }
    }
    else
    {
        if (__UART_GET_FLAG(USARTx -> USART_x,__PE__))
        {
            USARTx -> Error_Code = (u8 *)Error_2;
            Error_counter += 2;
        }
        if(__UART_GET_FLAG(USARTx -> USART_x,__NE__))
        {
            USARTx -> Error_Code = (u8 *)Error_3;
            Error_counter += 3;
        }
        if(__UART_GET_FLAG(USARTx -> USART_x,__FE__))
        {
            USARTx -> Error_Code = (u8 *)Error_4;
            Error_counter += 4;
        }
        // two or more errors had happened.
        if (Error_counter > 4)
        {
            switch (Error_counter)
            {
            case 5: USARTx -> Error_Code = (u8 *)"Parity and Noise Errors ";
                break;
            case 7: USARTx -> Error_Code = (u8 *)"Noise and Frame Errors ";
                break;
            case 6: USARTx -> Error_Code = (u8 *)"Parity and Frame Errors ";
                break;
            case 9: USARTx -> Error_Code = (u8 *)"Parity, Frame, and Noise Errors ";
                break;
            }
        }
        USARTx ->RX_Lock_Flag = IDLE;
        USARTx ->RX_Lock_Counter = 0;
        /* Enable the UART Transmit Complete Interrupt */
        SET_BIT(USARTx -> USART_x ->CR1, CR1_TCIE);
        return Uart_ERROR; 
    }
    /* Enable the UART Transmit Complete Interrupt */
    SET_BIT(USARTx -> USART_x ->CR1, CR1_TCIE);
    return Uart_OK;
}





/// @brief  MCAL_UART_INTTCALLBACK : this function is Used to add another function that will be executed at the Handler.
/// @param  USARTx                 : the Struct of Peripheral's Registers.
/// @param  INTT_TYPE              : the type of the Interrupt that should be happened to execute this additional function. 
/// @param  Copy_ptr               : pointer to the additional function that will be executed.
/// @retval Functions Status.
Uart_Fun_Status	    MCAL_UART_INTT_CALLBACK(USART_Struct *USARTx , USART_INT_TYPE INTT_TYPE, void (*Copy_ptr)(void))
{  
    if      (USARTx -> USART_x == USART1_R)
    {
        USART1_CallBack = *Copy_ptr;
        __USART1__INTERRUPT_TYPE__  = INTT_TYPE;
    }
    else if (USARTx -> USART_x == USART2_R)
    {
        USART2_CallBack = *Copy_ptr;
        __USART2__INTERRUPT_TYPE__  = INTT_TYPE;
    }
    else if (USARTx -> USART_x == USART6_R)
    {
        USART6_CallBack = *Copy_ptr;
        __USART6__INTERRUPT_TYPE__  = INTT_TYPE;
    }
    return Uart_OK;
}




/// @brief  USART1_IRQHandler   : the HANDLER Function of The USART1_IRQHandler interrupt.
/// @param  takes No parameters.
/// @retval return Nothing.
void USART1_IRQHandler(void)
{
    // UART in mode Transmitter. 
	if(GET_BIT(USART1_Struct -> USART_x -> CR1,CR1_TCIE) && __UART_GET_FLAG(USART1_Struct -> USART_x ,__TC__))
	{
	    UART_Transmit_Handler(USART1_Struct);
        if (GET_BIT(USART1_Struct -> USART_x ->SR ,__USART1__INTERRUPT_TYPE__))
        {
            USART1_CallBack();
        }
        __UART_CLEAR_FLAG(USART1_Struct -> USART_x ,__TC__);
	}
    // UART in mode Receiver.
	if(GET_BIT(USART1_Struct -> USART_x -> CR1,CR1_RXNEIE) && __UART_GET_FLAG(USART1_Struct -> USART_x ,__RXNE__))
	{
	    UART_Receive_Handler(USART1_Struct);
        if (GET_BIT(USART1_Struct -> USART_x ->SR ,__USART1__INTERRUPT_TYPE__))
        {
        USART1_CallBack();
        }
        __UART_CLEAR_FLAG(USART1_Struct -> USART_x ,__RXNE__);
	}
}

/// @brief  USART2_IRQHandler   : the HANDLER Function of The USART2_IRQHandler interrupt.
/// @param  takes No parameters.
/// @retval return Nothing.
void USART2_IRQHandler(void)
{
    // UART in mode Transmitter. 
	if(GET_BIT(USART2_Struct -> USART_x -> CR1,CR1_TCIE) && __UART_GET_FLAG(USART2_Struct -> USART_x ,__TC__))
	{
	    UART_Transmit_Handler(USART2_Struct);
	}
    // UART in mode Receiver.
	if(GET_BIT(USART2_Struct -> USART_x -> CR1,CR1_RXNEIE) && __UART_GET_FLAG(USART2_Struct -> USART_x ,__RXNE__))
	{
	    UART_Receive_Handler(USART2_Struct);
	}
}

/// @brief  USART6_IRQHandler   : the HANDLER Function of The USART6_IRQHandler interrupt.
/// @param  takes No parameters.
/// @retval return Nothing.
void USART6_IRQHandler(void)
{
    // UART in mode Transmitter. 
	if(GET_BIT(USART6_Struct -> USART_x -> CR1,CR1_TCIE) && __UART_GET_FLAG(USART6_Struct -> USART_x ,__TC__))
	{
	    UART_Transmit_Handler(USART6_Struct);
	}
    // UART in mode Receiver.
	if(GET_BIT(USART6_Struct -> USART_x -> CR1,CR1_RXNEIE) && __UART_GET_FLAG(USART6_Struct -> USART_x ,__RXNE__))
	{
	    UART_Receive_Handler(USART6_Struct);
	}
}





/// @brief UART_Check_LockState : this function check and control the Lock flag of the {TX} or {RX}.
/// @param USARTx               : the Struct of Peripheral's Registers.
/// @param _CommType_           : the Type of Communication:
///        @arg  TX_Lock_Status :  The Tx lock Flag.
///        @arg  RX_Lock_Status :  The Rx lock Flag.    
/// @return     the Flag state.                    
static Uart_LOCK_ST    UART_Check_LockState(USART_Struct *USARTx ,COMM_TYPE _CommType_ )
{
    switch (_CommType_)
    {
    case TX:
        if (USARTx -> TX_Lock_Flag == BUSY)
        {
            if ((USARTx -> TX_Lock_Counter) >= LOCK_TIME_LIMIT)
            {
                // perform the Unlock process.
                USARTx -> TX_Lock_Flag = IDLE ;
                USARTx -> TX_Lock_Counter = 0 ;
                return IDLE;
            }
            else
            { 
                USARTx -> TX_Lock_Counter ++;
                return BUSY;
            } 
        }
        else
        {
            return IDLE ;
        } 
        break;
    
    case RX:
        if (USARTx -> RX_Lock_Flag == BUSY)
        {
            if ((USARTx -> RX_Lock_Counter) >= LOCK_TIME_LIMIT)
            {
                // perform the Unlock process.
                USARTx -> RX_Lock_Flag = IDLE ;
                USARTx -> RX_Lock_Counter = 0 ;
                return IDLE;
            }
            else
            { 
                USARTx -> RX_Lock_Counter ++;
                return BUSY;
            } 
        }
        else
        {
            return IDLE ;
        } 
        break;
    case TX_RX:
        break;
    }
    return ERROR_IN;
}