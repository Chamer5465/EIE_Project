/*!*********************************************************************************************************************
@file user_app1.h                                                                
@brief Header file for user_app1

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
1. Follow the instructions at the top of user_app1.c
2. Use ctrl-h to find and replace all instances of "user_app1" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

**********************************************************************************************************************/

#ifndef __USER_APP1_H
#define __USER_APP1_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/


/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

void displayBoard(int gameboard[4][8]);
void displayGrid(void);
void ledsOff(void);
void checkHealth();
void checkHit(u8 shot);
void displayMessage(u8 message[], int line);


/*------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp1Initialize(void);
void UserApp1RunActiveState(void);


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/  
void placement(void);
void shoot(void);
void sendShot(void);

static void UserApp1SM_WaitAntReady(void);
static void UserApp1SM_WaitChannelOpen(void);   

void endGame(void);
void shockingFunction(void);

static void UserApp1SM_Error(void);


/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
/* Ant Definitions */
#define U8_ANT_CHANNEL_USERAPP (u8)ANT_CHANNEL_0 /* Channel 0 – 7 */
#define U8_ANT_DEVICE_LO_USERAPP (u8)0x0 /* Low byte of two-byte Device # */
#define U8_ANT_DEVICE_HI_USERAPP (u8)0x0 /* High byte of two-byte Device # */
#define U8_ANT_DEVICE_TYPE_USERAPP (u8)0 /* 1 – 255 */
#define U8_ANT_TRANSMISSION_TYPE_USERAPP (u8)0 /* 1-127 (MSB is pairing bit) */
#define U8_ANT_CHANNEL_PERIOD_LO_USERAPP (u8)0x00 /* Low byte of two-byte channel period */
#define U8_ANT_CHANNEL_PERIOD_HI_USERAPP (u8)0x20 /* High byte of two-byte channel period */
#define U8_ANT_FREQUENCY_USERAPP (u8)50 /* 2400MHz + this number 0 – 99 */
#define U8_ANT_TX_POWER_USERAPP RADIO_TX_POWER_4DBM /* RADIO_TX_POWER_xxx */

#endif /* __USER_APP1_H */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
