#ifndef INCLUDES_CAN_H_
#define INCLUDES_CAN_H_

#include "ComStack_Types.h"
#include "Can_GeneralTypes.h"
#include "Can_Cfg.h"

#define CAN_DRIVER_ID	 		 0x80U
#define CAN_SET_CONTROLLER_MODE	 0x03U
#define CAN_E_UNINIT			 0x05U
#define CAN_E_PARAM_CONTROLLER   0x04U
#define CAN_E_TRANSITION		 0x06U

#define CAN_INT_ERROR           0x00000008

//
//! This flag is used to allow a CAN controller to generate status
//! interrupts.
//
#define CAN_INT_STATUS          0x00000004

//
//! This flag is used to allow a CAN controller to generate any CAN
//! interrupts.  If this is not set, then no interrupts are generated
//! by the CAN controller.
//
#define CAN_INT_MASTER          0x00000002


#endif
