#ifndef INCLUDES_CAN_GENERALTYPES_H_
#define INCLUDES_CAN_GENERALTYPES_H_

typedef  uint8_t    Can_ControllerStateType;
typedef  uint8_t    Can_StateType;



#define CAN_CS_UNINIT   ((Can_ControllerStateType)0x00)
#define CAN_CS_STARTED 	((Can_ControllerStateType)0x01)
#define CAN_CS_STOPPED	((Can_ControllerStateType)0x02)
#define CAN_CS_SLEEP	((Can_ControllerStateType)0x03)

#define CAN_UNINIT   ((Can_StateType)0x00)
#define CAN_READY	 ((Can_StateType)0x01)



#endif
