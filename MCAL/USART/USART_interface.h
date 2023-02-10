/********************************************************************************************/
/* 	AUTHOR  		: islam atef Mohamed 													*/
/* 	VERSION 		:	  V2.0 																*/
/* 	DATE    		:  1/2023 																*/
/*	Description  	:  This is the Interface file For the USART Peripheral 					*/
/*											at ARM-CORTEX m4								*/
/********************************************************************************************/
#ifndef		USART_INTERFACE_H
#define		USART_INTERFACE_H

/********************************************************************************************/
/*                   			    The USART Registers                      			    */
/********************************************************************************************/
#define USART1_R  ((MUSART_peri * )USART1_BASE_ADD)
#define USART2_R  ((MUSART_peri * )USART2_BASE_ADD)
#define USART6_R  ((MUSART_peri * )USART6_BASE_ADD)
/********************************************************************************************/

/********************************************************************************************/
/*          		   	The USART Peripheral Lock status type.          	  		        */
/********************************************************************************************/
typedef enum
{	
 	IDLE     = 0x00U,
 	BUSY     = 0x01U,
	ERROR_IN = 0x02U

}Uart_LOCK_ST;
/********************************************************************************************/

/********************************************************************************************/
/*							UART Peripheral information struct								*/
/********************************************************************************************/

typedef struct{

    MUSART_peri     *USART_x ; 					/*	 		UART registers base address        					  */

	u32				 Time_Limit;				/*			UART time limit between each transaction		      */

    u8           	*TX_Buffer_Ptr;      		/*	 		Pointer to UART Tx transfer Buffer 					  */
    u16              TX_Buffer_Size;        	/*	 		UART Tx Transfer Buffer size       					  */
    s16              TX_Process_Count;      	/*	 		UART Tx Transfer process Counter   					  */
	u8				 TX_Buffer_lastEL;			/*	 		UART TX last element should be in its buffer		  */
	Uart_LOCK_ST	 TX_Lock_Flag;				/*   		UART Tx Flag that presents the current state		  */
	u8				 TX_Lock_Counter;			/*	 UART Tx Lock counter that presents the unlock request number */

    u8           	*RX_Buffer_Ptr;      		/*	 		Pointer to UART RX transfer Buffer 					  */
    u16              RX_Buffer_Size;        	/*	 		UART RX Transfer Buffer size       					  */
    s16              RX_Process_Count;      	/*	 		UART RX Transfer process Counter   					  */
	u8				 RX_Buffer_lastEL;			/*	 		UART RX last element should be in its buffer   		  */
	Uart_LOCK_ST	 RX_Lock_Flag;				/*   		UART Tx Flag that presents the current state	  	  */
	u8				 RX_Lock_Counter;			/*	 UART Tx Lock counter that presents the unlock request number */

	u8              *Error_Code;        		/*	 					UART Error code                    		  */

}USART_Struct;

/********************************************************************************************/


/********************************************************************************************/
/*									Error Codes												*/
/********************************************************************************************/
#define Error_1			"UART_ERROR_NONE"        /*		   No error            */
#define Error_2			"UART_ERROR_PE"          /*		   Parity error        */
#define Error_3			"UART_ERROR_NE"          /*		   Noise error         */
#define Error_4			"UART_ERROR_FE"          /*		   Frame error         */
#define Error_5			"UART_ERROR_ORE"         /*		   Overrun error       */
#define Error_6			"UART_ERROR_TIMEOUT"     /*		   Timeout error       */
/********************************************************************************************/

/********************************************************************************************/
/*          	   	The USART Peripheral Functions' status type.            		        */
/********************************************************************************************/
typedef enum
{	
 	Uart_OK        = 0x00U,
 	Uart_ERROR     = 0x01U,
 	Uart_TIMEOUT   = 0x02U,
	Uart_OVERSIZE  = 0x03U,
	Uart_UNDERSIZE = 0x04U,
	Uart_BUSY      = 0x05U

}Uart_Fun_Status;
/********************************************************************************************/


/********************************************************************************************/
/*          		   	The USART Peripheral Interrupt Options.      	      		        */
/********************************************************************************************/
typedef enum
{
	PE_INT			=0,
	FE_Error_INT	=1,
	NF_INT			=2,
	ORE_Error_INT	=3,
	IDLE_INT		=4,
	TC_INT   		=6

}USART_INT_TYPE;
/********************************************************************************************/


/********************************************************************************************/
/*             				The USART Peripheral Configurations           		            */
/********************************************************************************************/
/*------------------------------------------------------------------------------------------*/
//-------------------------------------- Parity Options : -------------
typedef enum{

	Parity_Disable ,
	Even_Parity    ,
	Odd_Parity

}Parity_Op ;
//----------------------------------------- Word Size : ---------------
typedef enum{

	_8_Bit ,
	_9_Bit
}Word_Size ;
//------------------------------------ Stop Bits Options : ------------
typedef enum{

	_1_0_Bit  ,
	_0_5_Bit  ,
	_2_0_Bit  ,
	_1_5_Bit
}Stop_Bit ;
//-------------------------------------------------------------------------------------------
typedef struct{

	volatile	Word_Size 			M_VALUE			;
	volatile 	Stop_Bit 			Stop_Bit_NUM	;
	volatile	Parity_Op			parity_op		;

}MUSART_Frame_Config ;
/*------------------------------------------------------------------------------------------*/
//----------------------------------- Oversampling Options : -----------
typedef enum{

	Sampling_By_16 ,
	Sampling_By_8
}Oversampling_Value ;
//--------------------------------- One sample bit method : -----------
typedef enum{

	Three_Sample ,
	One_Sample
}OneBit_Sample ;
//-------------------------------------------------------------------------------------------
typedef struct{

	volatile	Oversampling_Value		Oversampling_type		;
	volatile	OneBit_Sample			OneBit_Sampling_method	;

}MUSART_Receiving_Config ;
/*------------------------------------------------------------------------------------------*/
/********************************************************************************************/




/********************************************************************************************/
/*										Type of Communication								*/
/********************************************************************************************/
//---------------------------------- Communication Mode : ----------
typedef enum{

	TX ,
	RX ,
	TX_RX
}COMM_TYPE ;
/********************************************************************************************/



/********************************************************************************************/
/*             		The USART Peripheral Functions Prototypes           		            */
/********************************************************************************************/
/// @brief  MCAL_USART_Init_	  		: this function performs the Initialization process of The Peripheral .
/// @param  USARTx                		: the Struct of Peripheral's Registers .
/// @param  USART_frame_struct       	: the struct of frame characteristics Options .
/// @param  USART_receiving_struct      : the struct of Received Data Handling Options .
/// @param	copy_u32BaudRate			: the baud Rate of the Peripheral.
/// @retval	Functions Status.
Uart_Fun_Status		MCAL_UART_Init_(USART_Struct *USARTx , MUSART_Frame_Config *USART_frame_struct, 
					             	MUSART_Receiving_Config *USART_receiving_struct, u32 copy_u32BaudRate );
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// @brief  MCAL_USART_Enable  : the function responses of Enabling the Peripheral, Start The Communication and defining Its Type. 
/// @param  USARTx             : the Struct of Peripheral's Registers .
/// @retval	Functions Status.
Uart_Fun_Status	    MCAL_UART_Enable( USART_Struct *USARTx );
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// @brief  MCAL_USART_Disable  : the function responses of Disable the Whole Peripheral or Disable part of the Communication process.
/// @param  USARTx              : the Struct of Peripheral's Registers.
/// @retval	Functions Status.
Uart_Fun_Status	    MCAL_UART_Disable( USART_Struct *USARTx );
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// @brief MCAL_USART_Transmit   : this function Transmit a given data by the synchronous mode " Blocking".
/// @param USARTx                : the Struct of Peripheral's Registers.
/// @param ptData                : pointer of data we want to Transmit.
/// @param Size                  : the size of the data that will be Transmitted.
/// @param Time_Limit            : the maximum time for this function. 
///@retval Functions Status.
Uart_Fun_Status	    MCAL_UART_Transmit(USART_Struct *USARTx , u8 *ptData ,u16 Size, u32 Time_Limit, u8 Last_element);
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// @brief MCAL_UART_Receive   : this function Receive an amount of data by the synchronous mode " Blocking".
/// @param USARTx                : the Struct of Peripheral's Registers.
/// @param ptData                : pointer of data we want to Transmit.
/// @param Size                  : the size of the data that will be Transmitted.
/// @param Time_Limit            : the maximum time for this function.
/// @param Last_element          : the last element that should be Received. 
///@retval Functions Status
Uart_Fun_Status 	MCAL_UART_Receive( USART_Struct *USARTx , u8 *ptData ,u16 Size_Limit , u32 Time_Limit, u8 Last_element);
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// @brief MCAL_USART_Transmit_INT  : this function Transmit a given data by the Asynchronous mode "Interrupt".
/// @param USARTx                   : the Struct of Peripheral's Registers.
/// @param ptData                   : pointer of data we want to Transmit.
/// @param Size                     : the size of the data that will be Transmitted.
/// @param Time_Limit               : the maximum time for this function. 
///@retval Functions Status.
Uart_Fun_Status	    MCAL_UART_Transmit_INT(USART_Struct *USARTx , u8 *ptData ,u16 Size ,u8 Last_element);
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// @brief MCAL_USART_Transmit_INT  : this function Receive an amount of data by the Asynchronous mode "Interrupt".
/// @param USARTx                   : the Struct of Peripheral's Registers.
/// @param ptData                   : pointer of data we want to Transmit.
/// @param Size                     : the size of the data that will be Transmitted.
/// @param Time_Limit               : the maximum time for this function. 
///@retval Functions Status.
Uart_Fun_Status	    MCAL_UART_Receive_INT(USART_Struct *USARTx , u8 *ptData ,u16 Size_Limit, u8 Last_element);
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/// @brief  MCAL_UART_INTTCALLBACK : this function is Used to add another function that will be executed at the Handler.
/// @param  USARTx                 : the Struct of Peripheral's Registers.
/// @param  INTT_TYPE              : the type of the Interrupt that should be happened to execute this additional function. 
/// @param  Copy_ptr               : pointer to the additional function that will be executed.
/// @retval Functions Status.
Uart_Fun_Status	    MCAL_UART_INTT_CALLBACK(USART_Struct *USARTx , USART_INT_TYPE INTT_TYPE, void (*Copy_ptr)(void));
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
#endif