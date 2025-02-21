/*!*********************************************************************************************************************
@file user_app1.c                                                                
@brief User's tasks / applications are written here.  This description
should be replaced by something specific to the task.

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------------------------------------------------------
GLOBALS
- NONE

CONSTANTS
- NONE

TYPES
- NONE

PUBLIC FUNCTIONS
- NONE

PROTECTED FUNCTIONS
- void UserApp1Initialize(void)
- void UserApp1RunActiveState(void)


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_<type>UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                          /*!< @brief Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemTime1ms;                   /*!< @brief From main.c */
extern volatile u32 G_u32SystemTime1s;                    /*!< @brief From main.c */
extern volatile u32 G_u32SystemFlags;                     /*!< @brief From main.c */
extern volatile u32 G_u32ApplicationFlags;                /*!< @brief From main.c */
extern const u8 aau8BlackBox[(u8)14][((u8)14 * (u8)1 / 8 + 1)];
extern const u8 aau8CrossOut[(u8)14][(u8)14];


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/
int board[4][8] = {{1, 0, 0, 0, 0, 0, 0, 0}, {1, 2, 0, 0, 0, 0, 0, 0}, {1, 2, 1, 0, 0, 0, 0, 0}, {2, 2, 1, 0, 0, 0, 0, 0}};
int **ships[4] = {(int*[]) {&board[0][0]}, (int*[]) {&board[1][0], &board[1][1]}, (int*[]) {&board[2][0], &board[2][1], &board[1][2]}, (int*[]) {&board[2][0], &board[2][1], &board[2][2]}};

static fnCode_type UserApp1_pfStateMachine;               /*!< @brief The state machine function pointer */
//static u32 UserApp1_u32Timeout;                           /*!< @brief Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @publicsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------*/
/*! @protectedsection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/

/*!--------------------------------------------------------------------------------------------------------------------
@fn void UserApp1Initialize(void)

@brief
Initializes the State Machine and its variables.

Should only be called once in main init section.

Requires:
- NONE

Promises:
- NONE

*/
void UserApp1Initialize(void)
{
  int x = 0;
  int y = 0;
  int rotation = 0;
  LcdClearScreen();
  PixelAddressType targetPixel; 
  for (int i = 0; i < 128; i++) {
      targetPixel.u16PixelRowAddress = 0;
      targetPixel.u16PixelColumnAddress = i;
      LcdSetPixel(&targetPixel);
  }
  for (int i = 0; i < 64; i++) {
      targetPixel.u16PixelRowAddress = i;
      targetPixel.u16PixelColumnAddress = 0;
      LcdSetPixel(&targetPixel);
  }
  for (int j = 16; j <= 48; j += 16) {
      for (int i = 0; i < 128; i++) {
          targetPixel.u16PixelRowAddress = j - 1;
          targetPixel.u16PixelColumnAddress = i;
          LcdSetPixel(&targetPixel);
          targetPixel.u16PixelRowAddress = j;
          targetPixel.u16PixelColumnAddress = i;
          LcdSetPixel(&targetPixel);
      }
  }
  for (int j = 16; j <= 112; j += 16) {
      for (int i = 0; i < 64; i++) {
          targetPixel.u16PixelRowAddress = i;
          targetPixel.u16PixelColumnAddress = j - 1;
          LcdSetPixel(&targetPixel);
          targetPixel.u16PixelRowAddress = i;
          targetPixel.u16PixelColumnAddress = j;
          LcdSetPixel(&targetPixel);
      }
  }
  for (int i = 0; i < 128; i++) {
      targetPixel.u16PixelRowAddress = 63;
      targetPixel.u16PixelColumnAddress = i;
      LcdSetPixel(&targetPixel);
  }
  for (int i = 0; i < 64; i++) {
      targetPixel.u16PixelRowAddress = i;
      targetPixel.u16PixelColumnAddress = 127;
      LcdSetPixel(&targetPixel);
  }
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_pfStateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*!----------------------------------------------------------------------------------------------------------------------
@fn void UserApp1RunActiveState(void)

@brief Selects and runs one iteration of the current state in the state machine.

All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
- State machine function pointer points at current state

Promises:
- Calls the function to pointed by the state machine function pointer

*/
void UserApp1RunActiveState(void)
{
  UserApp1_pfStateMachine();

} /* end UserApp1RunActiveState */


/*------------------------------------------------------------------------------------------------------------------*/
/*! @privatesection */                                                                                            
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

void displayBoard() {
    PixelBlockType targetBlock;
    targetBlock.u16RowSize = 14;
    targetBlock.u16ColumnSize = 14;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == 1) {
                targetBlock.u16RowStart = i * 16 + 1;
                targetBlock.u16ColumnStart = j * 16 + 1;
                LcdClearPixels(&targetBlock);
                LcdLoadBitmap(&aau8BlackBox[0][0], &targetBlock);
            } else if (board[i][j] == 2) {
                targetBlock.u16RowStart = i * 16 + 1;
                targetBlock.u16ColumnStart = j * 16 + 1;
                LcdClearPixels(&targetBlock);
                LcdLoadBitmap(&aau8CrossOut[0][0], &targetBlock);
                
            }
        }
    }
}

int checkHealth() {
    int ship1 = 1;
    if (*ships[0][0] == 2) {
        LedOn(RED0);
        LedOff(GREEN0);
        LedOff(RED0)
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* What does this state do? */
static void UserApp1SM_Idle(void)
{
    displayBoard();
     
} /* end UserApp1SM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */




/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
