/********************************************************************************************/
/* 	AUTHOR  		: islam atef Mohamed 													*/
/* 	VERSION 		:	  V2.0 																*/
/* 	DATE    		:  1/2023 																*/
/*	Description  	:  This is the Private file For the USART Peripheral 					*/
/*											at ARM-CORTEX m4								*/
/********************************************************************************************/
#ifndef		USART_PRIVATE_H
#define		USART_PRIVATE_H


/********************************************************************************************/
#define     Disable     0
#define     Enable      1
/********************************************************************************************/

/********************************************************************************************/
/*                   			    The USART Registers                      			    */
/********************************************************************************************/

#define		USART1_BASE_ADD			(u32)(0x40011000)
#define		USART2_BASE_ADD			(u32)(0x40004400)
#define		USART6_BASE_ADD			(u32)(0x40011400)
typedef struct{

    volatile      u32           SR      ;
    volatile      u32           DR      ;
    volatile      u32           BRR     ;
    volatile      u32           CR1     ;
    volatile      u32           CR2     ;
    volatile      u32           CR3     ;
    volatile      u32           GTPR    ;
}MUSART_peri;
/********************************************************************************************/


/**********************************************/
/* 				SR BITS Mapping 			  */
/**********************************************/
/*	Parity error					*/
#define __PE__			0
/*	Framing error					*/
#define __FE__			1
/*	Noise error flag				*/
#define __NE__			2
/*	Overrun error					*/
#define __ORE__	 		3
/*	IDLE line detected				*/
#define __IDLE__ 		4
/*	Read data register not empty	*/
#define __RXNE__		5
/*	Transmission complete			*/
#define __TC__			6
/*	Transmit data register empty	*/
#define __TXE__			7
/*	LIN break detection flag		*/
#define __LBD__			8
/*	CTS flag						*/
#define __CTS__			9

/**********************************************/
/* 				CR1 BITS Mapping 			  */
/**********************************************/
/*	Send break	bit					*/
#define         CR1_SBK				0

/*	Receiver Wakeup	bit				*/
#define         CR1_RWU				1

/*	Receiver Enable bit				*/
#define         CR1_RE				2

/*	Transmitter Enable bit			*/
#define         CR1_TE				3

/*	IDLE interrupt enable bit		*/
#define         CR1_IDLEIE			4

/*	RXNEIE interrupt enable bit		*/
#define         CR1_RXNEIE			5

/*	Transmission complete interrupt enable bit	*/
#define         CR1_TCIE			6

/*	TXE interrupt enable bit		*/
#define         CR1_TXEIE			7

/*	PE interrupt enable bit			*/
#define         CR1_PEIE			8

/*	Parity selection bit			*/
#define         CR1_PS				9

/*	Parity control enable bit		*/

#define         CR1_PCE				10

/*	Wakeup method bit				*/
#define         CR1_WAKE			11

/*	Word length bit					*/
#define         CR1_M				12

/*	USART enable bit				*/
#define         CR1_UE				13

/*	USART Oversampling bit			*/
#define         CR1_OVER8	    	15

/**********************************************/
/* 				CR2 BITS Mapping 			  */
/**********************************************/
/*	Address of the USART node bits			*/
#define         CR2_ADD0		0
#define         CR2_ADD1		1
#define         CR2_ADD2		2
#define         CR2_ADD3		3

/*	lin break detection length bit			*/
#define         CR2_LBDL		5

/* LIN break detection interrupt enable bit */
#define         CR2_LBDIE		6

/*	Last bit clock pulse bit				*/
#define         CR2_LBCL		8

/*	Clock phase bit							*/
#define         CR2_CPHA		9

/*	Clock polarity bit						*/
#define         CR2_CPOL		10

/*	Clock enable bit						*/
#define         CR2_CLKEN		11

/*	STOP bit start							*/
#define         CR2_STOP		12

/*	STOP bits 								*/
#define         CR2_STOP0		12
#define         CR2_STOP1T		13

/*	LIN mode enable bit						*/
#define         CR2_LINEN		14

/**********************************************/
/* 				CR3 BITS Mapping 			  */
/**********************************************/
/*	One sample bit method   	*/
#define CR3_ONEBIT		11
/*	CTS interrupt enable bit	*/
#define CR3_CTSIE		10
/*	CTS enable bit				*/
#define CR3_CTSE		9
/*	RTS enable bit				*/
#define CR3_RTSE		8
/*	DMA enable transmitter bit	*/
#define CR3_DMAT		7
/*	DMA enable receiver bit		*/
#define CR3_DMAR		6
/*	Smartcard mode enable bit	*/
#define CR3_SCEN		5
/*	Smartcard NACK enable bit	*/
#define CR3_NACK		4
/*	Half-duplex selection bit	*/
#define CR3_HDSEL		3
/*	IrDA low-power bit			*/
#define CR3_IRLP		2
/*	IrDA mode enable bit		*/
#define CR3_IREN_		1
/*	Error interrupt enable bit	*/
#define CR3_EIE			0

/**********************************************/



/********************************************************************************************/
/*                                    UART Macros                                           */
/********************************************************************************************/
///@brief  Enable UART
///@param  __HANDLE__ specifies the UART Struct.
///@retval None
#define     __UART_ENABLE(__USARTX__)	   ((__USARTX__)->CR1 |=  (1<<CR1_UE))
/******************************************************************************************************************************************/
///@brief  Disable UART
///@param  __HANDLE__ specifies the UART Struct.
///@retval None
#define     __UART_DISABLE(__USARTX__)	   ((__USARTX__)->CR1 &= ~(1<<CR1_UE))
/******************************************************************************************************************************************/
/// @brief  Checks whether the specified UART flag is set or not.
/// @param  __USARTX__ specifies the UART Struct.
/// @param  __FLAG__ specifies the flag to check.
///        This parameter can be one of the following values:
///           @arg __CTS__  :  CTS Change flag 
///           @arg __LBD__  :  LIN Break detection flag
///           @arg __TXE__  :  Transmit data register empty flag
///           @arg __TC__   :   Transmission Complete flag
///           @arg __RXNE__ : Receive data register not empty flag
///           @arg __IDLE__ : Idle Line detection flag
///           @arg __ORE__  :  Overrun Error flag
///           @arg __NE__   :   Noise Error flag
///           @arg __FE__   :   Framing Error flag
///           @arg __PE__   :   Parity Error flag
/// @retval The new state of __FLAG__ (َ 1 or 0 ).
#define     __UART_GET_FLAG(__USARTX__, __FLAG__)       ((((__USARTX__)-> SR) >> (__FLAG__)) & 0x01)
/******************************************************************************************************************************************/
/// @brief  Clears the specified UART pending flag.
/// @param  __USARTX__ specifies the UART Struct.
/// @param  __FLAG__ specifies the flag to check.
///          This parameter can be any combination of the following values:
///            @arg  __CTS__ :  CTS Change flag (not available for UART4 and UART5).
///            @arg  __LBD__ :  LIN Break detection flag.
///            @arg  __TC__  :   Transmission Complete flag.
///            @arg __RXNE__ : Receive data register not empty flag.
/// @note   "PE" (Parity error), "FE" (Framing error), "NE" (Noise error), "ORE" (Overrun 
///          error) and "IDLE" (Idle line detected) flags are cleared by software 
///          sequence: a read operation to USART_SR register followed by a read
///          operation to USART_DR register.
/// @note   RXNE flag can be also cleared by a read to the USART_DR register.
/// @note   "TC" flag can be also cleared by software sequence: a read operation to 
///          USART_SR register followed by a write operation to USART_DR register.
/// @note   "TXE" flag is cleared only by a write to the USART_DR register.
/// @retval None
#define     __UART_CLEAR_FLAG(__USARTX__, __FLAG__)     ((__USARTX__)->SR &= ~(1<<__FLAG__))
/******************************************************************************************************************************************/
///@brief  Lock the Communication of the Peripheral.
///@param  __HANDLE__     specifies the UART Struct.
///@param  __COMM_TYPE__  this parameter could be:  
///            @arg  TX_Lock_Status :  The Tx lock Flag.
///            @arg  RX_Lock_Status :  The Rx lock Flag.
///@retval None
#define     __UART_LOCK(__USARTX__,__COMM_TYPE__)	   ((__USARTX__->__COMMT_LOCK_FLAG__) = BUSY )
/******************************************************************************************************************************************/
///@brief  Unlock the Communication of the Peripheral.
///@param  __HANDLE__ specifies the UART Struct.
///@param  __COMM_TYPE__  this parameter could be:  
///            @arg  TX_Lock_Status :  The Tx lock Flag.
///            @arg  RX_Lock_Status :  The Rx lock Flag.
///@retval None
#define     __UART_UNLOCK(__USARTX__,__COMM_TYPE__)	   ((__USARTX__->__COMMT_LOCK_FLAG__) = IDLE )
/******************************************************************************************************************************************/
///@brief  Enable the Communication of the Peripheral.
///@param  __HANDLE__     specifies the UART Struct.
///@param  __COMM_TYPE__  this parameter could be:  
///            @arg  TX_Lock_Status :  The Tx lock Flag.
///            @arg  RX_Lock_Status :  The Rx lock Flag.
///@retval None
#define     __COMM_ENABLE(__USARTX__,__COMM_TYPE__)	   (__COMM_TYPE__ == TX) ? (SET_BIT((__USARTX__-> USART_x -> CR1), CR1_TE ))  : \
                                                                               (SET_BIT((__USARTX__-> USART_x -> CR1), CR1_RE ))
/******************************************************************************************************************************************/
///@brief  Unlock the Communication of the Peripheral.
///@param  __HANDLE__ specifies the UART Struct.
///@param  __COMM_TYPE__  this parameter could be:  
///            @arg  TX_Lock_Status :  The Tx lock Flag.
///            @arg  RX_Lock_Status :  The Rx lock Flag.
///@retval None
#define     __COMM_DISABLE(__USARTX__,__COMM_TYPE__)	   (__COMM_TYPE__ == TX) ? (CLR_BIT((__USARTX__-> USART_x -> CR1), CR1_TE ))  : \
                                                                                   (CLR_BIT((__USARTX__-> USART_x -> CR1), CR1_RE ))
/******************************************************************************************************************************************/
#endif
