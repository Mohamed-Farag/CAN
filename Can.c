#include "Can.h"
#include "Det.h"
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "Timer0A.h"
#include "inc/tm4c123gh6pm.h"

#if (DEVLOPEMENT_ERROR)
/*Description :  variable to contain the CAN Mode State with (UNINIT) Value*/
static Can_StateType CanUnitState = CAN_UNINIT ;
#endif

/*Description :  variable to contain the CAN Controller Mode (UNINIT,STARTED,STOPPED,SLEEP)*/
static Can_ControllerStateType Can_ControllerMode [NUM_OF_CAN_CONTROLLERS];
/*Description :  variable for interrupt Enable in start mode */
static uint8_t uint8_uint8_InterruptEnableInStartMode[NUM_OF_CAN_CONTROLLERS];
/*Description :  variable for interrupt Disable in stop mode */
static uint8_t uint8_InterruptDisableInStoptMode[NUM_OF_CAN_CONTROLLERS];
/*Description :  variable for interrupt Disable counter */
static uint8_t uint8_InterruptDisableCounter[NUM_OF_CAN_CONTROLLERS];
/* This is a mapping between interrupt number (for the peripheral interrupts
   only) and the register that contains the interrupt enable for that interrupt.*/
static const uint32_t g_pui32EnRegs[] =
{
		NVIC_EN0, NVIC_EN1, NVIC_EN2, NVIC_EN3, NVIC_EN4
};

/*[SWS_Can_00230]         */
/*Service name   : Can_SetControllerMode
Syntax           : Std_ReturnType Can_SetControllerMode(uint8 Controller,Can_ControllerStateType Transition)
Service ID[hex]  : 0x03
Sync/Async       : Asynchronous
Reentrancy       : Non Reentrant
Parameters (in)  : Controller CAN controller for which the status shall be changed
                   Transition Transition value to request new CAN controller state
Parameters(inout):None
Parameters (out) : None
Return value:Std_ReturnType E_OK    : request accepted
                            E_NOT_OK: request not accepted, a development error occurred
Description: This function performs software triggered state transitions of the CAN controller State machine.*/

Std_ReturnType Can_SetControllerMode( uint8_t Controller, Can_ControllerStateType Transition )
{
	/*initialize local variable to return (E_OK,E_NOT_OK) */ 
	 Std_ReturnType ret = E_NOT_OK;

#if (DEVLOPEMENT_ERROR)

/*[SWS_Can_00198] If development error detection for the Can module is enabled:
                  if the module is not yet initialized, the function Can_SetControllerMode shall raise
                  development error CAN_E_UNINIT and return E_NOT_OK.*/
	if ( CanUnitState == CAN_UNINIT )
	{
		Det_ReportError( CAN_DRIVER_ID ,0 ,CAN_SET_CONTROLLER_MODE ,CAN_E_UNINIT );

		ret = E_NOT_OK;
	}

/* [SWS_Can_00199] If development error detection for the Can module is enabled:
                   if the parameter Controller is out of range, the function Can_SetControllerMode
                   shall raise development error CAN_E_PARAM_CONTROLLER and return
                   E_NOT_OK.*/
	else if (Controller >= NUM_OF_CAN_CONTROLLERS)
	{
		Det_ReportError( CAN_DRIVER_ID ,0 ,CAN_SET_CONTROLLER_MODE ,CAN_E_PARAM_CONTROLLER );
		ret = E_NOT_OK;
	}

/* [SWS_Can_00200] If development error detection for the Can module is enabled:
                   if an invalid transition has been requested, the function Can_SetControllerMode shall
                   raise the error CAN_E_TRANSITION and return E_NOT_OK.*/
	else if (((Transition == CAN_CS_STARTED) && ( Can_ControllerMode [Controller]!=CAN_CS_STOPPED))||
			((Transition == CAN_CS_SLEEP) &&  (Can_ControllerMode   [Controller]!= CAN_CS_STOPPED)))
	{
		Det_ReportError( CAN_DRIVER_ID ,0 ,CAN_SET_CONTROLLER_MODE ,CAN_E_TRANSITION );
		ret = E_NOT_OK;
	}

	else

#endif
	{
		switch(Transition)
		{
		 case CAN_CS_STARTED:
/*[SWS_Can_00261] The function Can_SetControllerMode(CAN_CS_STARTED)
                  shall set the hardware registers in a way that makes the CAN controller participating
                  on the network.*/
			if(Controller==0U)
			{
				uint8_InterruptDisableInStoptMode[Controller]=0U;
/* [SWS_Can_00384] Each time the CAN controller state machine is triggered with the state
                   transition value CAN_CS_STARTED, the function Can_SetControllerMode shall re-initialise
                   the CAN controller with the same controller configuration set previously used by functions
                   Can_SetBaudrate or Can_Init.*/
				if(uint8_InterruptDisableCounter[0U]==0U)
				{
				/* Enables individual CAN controller interrupt sources */
					HWREG(CAN0_BASE + CAN_O_CTL) |= CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS;
				/* Enable the general interrupt.*/
					HWREG(g_pui32EnRegs[(INT_CAN0 - 16) / 32]) =(1 << ((INT_CAN0 - 16) & 31));
				/* Enables the CAN controller for message processing.  Once enabled, the
                  controller automatically transmits any pending frames, and processes any
				  received frames.*/
					HWREG(CAN0_BASE + CAN_O_CTL) &= ~CAN_CTL_INIT;
				/*set the interrupt Enable in start mode flag      */ 	
					uint8_uint8_InterruptEnableInStartMode[Controller]=1U;
				}

				else
				{
				/* Enables the CAN controller for message processing.  Once enabled, the
                  controller automatically transmits any pending frames, and processes any
				  received frames.*/				
					HWREG(CAN0_BASE + CAN_O_CTL) &= ~CAN_CTL_INIT;
				}


			}

			else if(Controller==1U)
			{
 /*SWS_Can_00425] Enabling of CAN interrupts shall not be executed,
                  when CAN interrupts have been disabled by function Can_DisableControllerInterrupts.*/
				uint8_InterruptDisableInStoptMode[Controller]=0U;
				
				if(uint8_InterruptDisableCounter[1U]==0U)
				{
 /* [SWS_Can_00261]  The function Can_SetControllerMode(CAN_CS_STARTED) shall set
                     the hardware registers in a way that makes the CAN controller participating
                      on the network.*/					
				/* Enables individual CAN controller interrupt sources */	
					HWREG(CAN1_BASE + CAN_O_CTL) |= CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS;
				/* Enable the general interrupt.*/	
					HWREG(g_pui32EnRegs[(INT_CAN1 - 16) / 32]) =1 << ((INT_CAN1 - 16) & 31);
				/* Enables the CAN controller for message processing.  Once enabled, the
                  controller automatically transmits any pending frames, and processes any
				  received frames.*/
					HWREG(CAN1_BASE + CAN_O_CTL) &= ~CAN_CTL_INIT;
				/*set the interrupt Enable in start mode flag      */	
					uint8_uint8_InterruptEnableInStartMode[Controller]=1U;
				}

				else
				{
				/* Enables the CAN controller for message processing.  Once enabled, the
                  controller automatically transmits any pending frames, and processes any
				  received frames.*/
					HWREG(CAN1_BASE + CAN_O_CTL) &= ~CAN_CTL_INIT;
				}


			}
			else
			{

			}
			/* setting the new mode to STARTED */
			Can_ControllerMode[Controller] = CAN_CS_STARTED;
			break;

		    case CAN_CS_STOPPED:
/* [SWS_Can_00197]  The function Can_SetControllerMode shall disable interrupts that are not
                    allowed in the new state. */
				   if(Controller == 0U)
                    {
                  /*Disabling of CAN interrupts shall not be executed, when CAN interrupts have been disabled
                     by function Can_disableControllerInterrupts.*/						
						 if(uint8_InterruptDisableCounter[0U]>=1U)
                           {
								HWREG(CAN0_BASE + CAN_O_CTL) |= CAN_CTL_INIT;
                                 

                           }
						   else
                            {
/* [SWS_Can_00263] The function Can_SetControllerMode(CAN_CS_STOPPED) shall set the
                   bits inside the CAN hardware such that the CAN controller stops participating on the
                   network.*/
				   
				     /*Disabling of CAN interrupts shall not be executed, when CAN interrupts have been disabled
                     by function Can_disableControllerInterrupts.*/	
								HWREG(CAN0_BASE + CAN_O_CTL) |= CAN_CTL_INIT;  
                     /* disable the CAN controller Interrupt     */
								HWREG(CAN0_BASE + CAN_O_CTL) &= ~(CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
					 /* set interrupt disable in stop mode */			
                               uint8_InterruptDisableInStoptMode[Controller]=1U;

                            }
//							for()
//							{
//								????????????
//								/*[SWS_Can_00282] ⌈The function Can_SetControllerMode(CAN_CS_STOPPED)
//                                 shall cancel pending messages.*/
//
//							}
                    }
						
					else if(Controller == 1U)
					{
						    if(uint8_InterruptDisableCounter[1U]>=1U)
                           {
				     /*Disabling of CAN interrupts shall not be executed, when CAN interrupts have been disabled
                     by function Can_disableControllerInterrupts.*/								   
								HWREG(CAN1_BASE + CAN_O_CTL) |= CAN_CTL_INIT;
                                 

                           }
						   else
                            {
				     /*Disabling of CAN interrupts shall not be executed, when CAN interrupts have been disabled
                     by function Can_disableControllerInterrupts.*/									
								HWREG(CAN1_BASE + CAN_O_CTL) |= CAN_CTL_INIT;  
                               
					 /* disable the CAN controller Interrupt     */		   
								HWREG(CAN1_BASE + CAN_O_CTL) &= ~(CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
					 /* set interrupt disable in stop mode */			
                               uint8_InterruptDisableInStoptMode[Controller]=1U;

                            }
					}
//							for(  )
//							{
//																????????????
//								/*[SWS_Can_00282] ⌈The function Can_SetControllerMode(CAN_CS_STOPPED)
//                                 shall cancel pending messages.*/
//
//							}
						

					/* setting the new mode to STOPPED */
					Can_ControllerMode[Controller] = CAN_CS_STOPPED;
			break;
			case CAN_CS_SLEEP:
			         /*[SWS_Can_00258] When the CAN hardware does not support sleep mode and is triggered to
                     transition into SLEEP state, the Can module shall emulate a logical SLEEP state from which it
                     returns only, when it is triggered by software to transition into STOPPED state.*/
                     /*[SWS_Can_00404] The CAN hardware shall remain in state STOPPED, while the logical SLEEP state is active.
                      [SWS_Can_00290] If the CAN HW does not support a sleep mode, the function
                     Can_SetControllerMode(CAN_CS_SLEEP) shall set the CAN controller to the logical sleep mode.*/
                     /*[SWS_Can_00197] The function Can_SetControllerMode shall disable interrupts that are not allowed in the new state. */
			        				   if(Controller == 0U)
                    {
						 if(uint8_InterruptDisableCounter[0U]>=1U)
                           {
                        /*Disabling of CAN interrupts shall not be executed, when CAN interrupts have been disabled
                          by function Can_disableControllerInterrupts.*/							   
								HWREG(CAN0_BASE + CAN_O_CTL) |= CAN_CTL_INIT;
                                

                           }
						   else
                            {
						/*Disabling of CAN interrupts shall not be executed, when CAN interrupts have been disabled
                          by function Can_disableControllerInterrupts.*/		
								HWREG(CAN0_BASE + CAN_O_CTL) |= CAN_CTL_INIT;  
							   
                               /* disable the CAN controller Interrupt     */	
								HWREG(CAN0_BASE + CAN_O_CTL) &= ~(CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
							/* set interrupt disable in stop mode */	
                               uint8_InterruptDisableInStoptMode[Controller]=1U;

                            }
//							for()
//							{
//																????????????
//								/*[SWS_Can_00282] ⌈The function Can_SetControllerMode(CAN_CS_STOPPED)
//                                 shall cancel pending messages.*/
//
//							}
							   
						
					}
					else if(Controller == 1U)
					{
						    if(uint8_InterruptDisableCounter[1U]>=1U)
                           {
						 /*Disabling of CAN interrupts shall not be executed, when CAN interrupts have been disabled
                          by function Can_disableControllerInterrupts.*/
								HWREG(CAN1_BASE + CAN_O_CTL) |= CAN_CTL_INIT;
                                 

                           }
						   else
                            {
						  /*Disabling of CAN interrupts shall not be executed, when CAN interrupts have been disabled
                          by function Can_disableControllerInterrupts.*/
								HWREG(CAN1_BASE + CAN_O_CTL) |= CAN_CTL_INIT;  
                               /* disable the CAN controller Interrupt     */	
								HWREG(CAN1_BASE + CAN_O_CTL) &= ~(CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
							/* set interrupt disable in stop mode */	
                               uint8_InterruptDisableInStoptMode[Controller]=1U;

                            }
					}
//							for(  )
//							{
//																????????????
//								/*[SWS_Can_00282] ⌈The function Can_SetControllerMode(CAN_CS_STOPPED)
//                                 shall cancel pending messages.*/
//
//							}
						

					/* setting the new mode to SLEEP */
					Can_ControllerMode[Controller] = CAN_CS_SLEEP;
			break;

     	}
	}


/*[SWS_Can_00262] The function Can_SetControllerMode(CAN_CS_STARTED) shall wait for
                  limited time until the CAN controller is fully operational. Compare to SWS_Can_00398.*/

/*[SWS_Can_00264] The function Can_SetControllerMode(CAN_CS_STOPPED) shall wait for
                  a limited time until the CAN controller is really switched off. Compare to SWS_Can_00398.*/
           Timer0A_Init();
			while((TIMER0_RIS_R & 0x1) == 0)
			{
				
			}
	/*clear TIMER0A timeout flag   */		
			TIMER0_ICR_R = 0x01;
    /* return the value E_OK   
                        E_NOT_OK*/
	return ret;
}
