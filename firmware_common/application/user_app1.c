/*!*********************************************************************************************************************
@file user_app1.c                                                                
----------------------------------------------------------------------------------------------------------------------
Authors:
Django Schmidt
&
Chase Mackenzie


----------------------------------------------------------------------------------------------------------------------
This is Quick-Match-Battleship, a 4x8 board where ships are placed, shot-at, and destroyed. 
The object of the game is to sink all of your opponents battleships but be careful, if
your ship gets shot at, there are some real world consequences...

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

/* Bitmaps */
extern const u8 aau8BlackBox[(u8)14][((u8)14 * (u8)1 / 8 + 1)];
extern const u8 aau8CrossOut[(u8)14][(u8)14];
extern const u8 aau8Target[(u8)14][(u8)14];

/* ant_api */
extern u32 G_u32AntApiCurrentMessageTimeStamp;                            // From ant_api.c
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;            // From ant_api.c
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  // From ant_api.c
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;                // From ant_api.c


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_<type>" and be declared as static.
***********************************************************************************************************************/

int isANTMaster = 0; /* Sets one devboard as ANT Master to avoid confusion (1 = TRUE) Master always goes first */

int board[4][8] = {{5, 5, 5, 5, 5, 5, 5, 5}, {5, 5, 5, 5, 5, 5, 5, 5}, {5, 5, 5, 5, 5, 5, 5, 5}, {5, 5, 5, 5, 5, 5, 5, 5}};
int x = 0;
int currentBoardState = 0;
int xPrev = 0;
int yPrev = 0;
int y = 0;
int rotation = 0;
int ship = 0;
bool placementState = TRUE;
static fnCode_type UserApp1_pfStateMachine;                 /*!< @brief The state machine function pointer */
static u32 UserApp1_u32Timeout;                             /*!< @brief Timeout counter used across states */
static u32 UserApp1_u32DataMsgCount = 0;                    /* ANT_DATA packet counter */
static u32 UserApp1_u32TickMsgCount = 0;                    /* ANT TICK packet counter */
static u8 au8AntMessage[] =  {0, 0, 0, 0, 0xA5, 0, 0, 0};  /* ANT Master Test Message. First 4 bits allocated for data */

/**********************************************************************************************************************
Board State Dictionary
**********************************************************************************************************************/
/*
0 = Hit
1 = Ship 1
2 = Ship 2
3 = Ship 3
4 = Ship 4
5 = Empty
6 = Target




*/ 



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
   /* Start ANT Initialize */
  AntAssignChannelInfoType sChannelInfo;

  if(AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_UNCONFIGURED)
  {
    if (isANTMaster == 1) { // Uses following settings if board deemed master
      sChannelInfo.AntChannel = U8_ANT_CHANNEL_PERIOD_HI_USERAPP;
      sChannelInfo.AntChannelType = CHANNEL_TYPE_MASTER;
    } else { // Uses following settings if board deemed slave
      sChannelInfo.AntChannel = U8_ANT_CHANNEL_USERAPP;
      sChannelInfo.AntChannelType = CHANNEL_TYPE_SLAVE;
    }
    sChannelInfo.AntChannelPeriodHi = U8_ANT_CHANNEL_PERIOD_HI_USERAPP;
    sChannelInfo.AntChannelPeriodLo = U8_ANT_CHANNEL_PERIOD_LO_USERAPP;
    
    sChannelInfo.AntDeviceIdHi = U8_ANT_DEVICE_HI_USERAPP;
    sChannelInfo.AntDeviceIdLo = U8_ANT_DEVICE_LO_USERAPP;
    sChannelInfo.AntDeviceType = U8_ANT_DEVICE_TYPE_USERAPP;
    sChannelInfo.AntTransmissionType = U8_ANT_TRANSMISSION_TYPE_USERAPP;
      
    sChannelInfo.AntFrequency = U8_ANT_FREQUENCY_USERAPP;
    sChannelInfo.AntTxPower = U8_ANT_TX_POWER_USERAPP;
    
    sChannelInfo.AntNetwork = ANT_NETWORK_DEFAULT;
    for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
    {
      sChannelInfo.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
    }
    
    AntAssignChannel(&sChannelInfo);
  } 

  /* Start Battleship Initialize */
  LedOn(GREEN0);
  LedOff(RED0);
  LedOff(BLUE0);
  LedOn(GREEN1);
  LedOff(RED1);
  LedOff(BLUE1);
  LedOn(GREEN2);
  LedOff(RED2);
  LedOff(BLUE2);
  LedOn(GREEN3);
  LedOff(RED3);
  LedOff(BLUE3);
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
  /* If good initialization, set state to WaitAntReady */
  if( 1 )
  {
    UserApp1_pfStateMachine = UserApp1SM_WaitAntReady;
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

/*-------------------------------------------------------------------------------------------------------------------*/
/* Loops to display ships on the board */
void displayBoard() {
    PixelBlockType targetBlock;
    targetBlock.u16RowSize = 14;
    targetBlock.u16ColumnSize = 14;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            targetBlock.u16RowStart = i * 16 + 1;
                targetBlock.u16ColumnStart = j * 16 + 1;
            if (board[i][j] > 0 && board[i][j] < 5) {
                LcdClearPixels(&targetBlock);
                LcdLoadBitmap(&aau8BlackBox[0][0], &targetBlock);
            } else if (board[i][j] == 0) {
                LcdClearPixels(&targetBlock);
                LcdLoadBitmap(&aau8CrossOut[0][0], &targetBlock);
            } else if (board[i][j] == 6){
                LcdClearPixels(&targetBlock);
                LcdLoadBitmap(&aau8Target[0][0], &targetBlock);
            } else {
                LcdClearPixels(&targetBlock);
            }
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Checks the Health of the ships and changes LED states */
void checkHealth() {
    int ship1 = 0;
    int ship2 = 0;
    int ship3 = 0;
    int ship4 = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] == 1) {
                ship1++;
            } else if(board[i][j] == 2) {
                ship2++;
            } else if(board[i][j] == 3) {
                ship3++;
            } else if(board[i][j] == 4) {
                ship4++;
            }
        }
    }
    if (ship1 == 0) {
        LedOff(RED0);
        LedOff(GREEN0);
        LedOff(BLUE0);
    }
    if (ship2 == 1) {
        LedOn(RED1);
        LedOff(GREEN1);
        LedOff(BLUE1);
    } else if (ship2 == 0) {
        LedOff(RED1);
        LedOff(GREEN1);
        LedOff(BLUE1);
    }
    if (ship3 == 2) {
        LedOn(RED2);
        LedOn(GREEN2);
        LedOff(BLUE2);
    } else if (ship3 == 1) {
        LedOn(RED2);
        LedOff(GREEN2);
        LedOff(BLUE2);
    } else if (ship3 == 0) {
        LedOff(RED2);
        LedOff(GREEN2);
        LedOff(BLUE2);
    }
    if (ship4 == 2) {
        LedOn(RED3);
        LedOn(GREEN3);
        LedOff(BLUE3);
    } else if (ship4 == 1) {
        LedOn(RED3);
        LedOff(GREEN3);
        LedOff(BLUE3);
    } else if (ship4 == 0) {
        LedOff(RED3);
        LedOff(GREEN3);
        LedOff(BLUE3);
    }
    if (ship1 == 0 && ship2 == 0 && ship3 == 0 && ship4 == 0) {
      UserApp1_pfStateMachine = endGame; // End Game
    } else {
      UserApp1_pfStateMachine = shoot; // Continue State Machine
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Placement of ships at the beginning of the game */
void placement() {
    if (ship == 0) {
        board[y][x] = 1; 
        if (x != xPrev || y != yPrev) {
            board[yPrev][xPrev] = 5;
            xPrev = x;
            yPrev = y;
        }
    } else if (ship == 1 && board[y][x] > 1 && ((x > 6 && board[y][x - 1] > 1) || (x <= 6 && board[y][x + 1] > 1))) {
        board[y][x] = 2;
        if (x > 6) {
            board[y][x - 1] = 2;
            if ((x != xPrev || y != yPrev) && board[yPrev][xPrev] > 1 && board[yPrev][xPrev - 1] > 1) {
                board[yPrev][xPrev] = 5;
                board[yPrev][xPrev - 1] = 5;
                xPrev = x;
                yPrev = y;
            }
        } else {
            board[y][x + 1] = 2;
            if (x != xPrev || y != yPrev) {
                board[yPrev][xPrev] = 5;
                board[yPrev][xPrev + 1] = 5;
                xPrev = x;
                yPrev = y;
            } 
        }
    } else if (ship == 2 && board[y][x] > 2 && ((x > 5 && board[y][x - 1] > 2 && board[y][x - 2] > 2) || (x <= 5 && board[y][x + 1] > 2 && board[y][x + 2] > 2))) {
        board[y][x] = 3;
        if (x > 5) {
            board[y][x - 1] = 3;
            board[y][x - 2] = 3;
            if ((x != xPrev || y != yPrev) && board[yPrev][xPrev] > 2 && board[yPrev][xPrev - 1] > 2 && board[yPrev][xPrev - 2] > 2) {
                board[yPrev][xPrev] = 5;
                board[yPrev][xPrev - 1] = 5;
                board[yPrev][xPrev - 2] = 5;
                xPrev = x;
                yPrev = y;
            }
        } else {
            board[y][x + 1] = 3;
            board[y][x + 2] = 3;
            if ((x != xPrev || y != yPrev) && board[yPrev][xPrev] > 2 && board[yPrev][xPrev + 1] > 2 && board[yPrev][xPrev + 2] > 2) {
                board[yPrev][xPrev] = 5;
                board[yPrev][xPrev + 1] = 5;
                board[yPrev][xPrev + 2] = 5;
                xPrev = x;
                yPrev = y;
            }
        }
    } else if (ship == 3 && board[y][x] > 3  && ((x > 5 && board[y][x - 1] > 3 && board[y][x - 2] > 3) || (x <= 5 && board[y][x + 1] > 3 && board[y][x + 2] > 3))) {
        board[y][x] = 4;
        if (x > 5) {
            board[y][x - 1] = 4;
            board[y][x - 2] = 4;
            if ((x != xPrev || y != yPrev) && board[yPrev][xPrev] > 3 && board[yPrev][xPrev - 1] > 3 && board[yPrev][xPrev - 2] > 3) {
                board[yPrev][xPrev] = 5;
                board[yPrev][xPrev - 1] = 5;
                board[yPrev][xPrev - 2] = 5;
                xPrev = x;
                yPrev = y;
            }
        } else {
            board[y][x + 1] = 1;
            board[y][x + 2] = 1;
            if ((x != xPrev || y != yPrev) && board[yPrev][xPrev] > 3 && board[yPrev][xPrev + 1] > 3 && board[yPrev][xPrev + 2] > 3) {
                board[yPrev][xPrev] = 5;
                board[yPrev][xPrev + 1] = 5;
                board[yPrev][xPrev + 2] = 5;
                xPrev = x;
                yPrev = y;
            }
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------*/
/* Allows user to select a space to shoot enemy when it's their turn */
void shoot() {
    if (x != xPrev || y != yPrev) { // Deletes previous sprite
        board[yPrev][xPrev] = currentBoardState; // Uses previous board state
        xPrev = x;
        yPrev = y;
    }
    if(WasButtonPressed(BUTTON0) && WasButtonPressed(BUTTON1)) { // Checks if both button are pressed at the same time
        ButtonAcknowledge(BUTTON0);
        ButtonAcknowledge(BUTTON1);
        board[y][x] = 0; // Board code 0 is an X
        displayBoard(); // Update board to reflect this
        UserApp1_pfStateMachine = sendShot(x, y); // Continue state machine
    }
    if(IsButtonHeld(BUTTON0, 50)) { // Checks if button is held for 50ms to give both button hit priority
        if(WasButtonPressed(BUTTON0)) { // If the button is held this function does not loop preventing executing movement too many times
            ButtonAcknowledge(BUTTON0);
            x++;
            if (x > 7) { // Reset to left of board
                x = 0;
            }
            currentBoardState = board[y][x]; // Saves board type to not overwrite other things
            board[y][x] = 6; // board code 6 is a target
        }
    }
    if(IsButtonHeld(BUTTON1, 50)) { // Checks if button is held for 50ms to give both button hit priority
        if(WasButtonPressed(BUTTON1)) { // If the button is held this function does not loop preventing executing movement too many times
            ButtonAcknowledge(BUTTON1);
            y++;
            if (y > 3) { // Reset to top of board
                y = 0;
            }
            currentBoardState = board[y][x]; // Saves board type to not overwrite other things
            board[y][x] = 6; // board code 6 is a target
        }
    }
  displayBoard();
} /* end shoot() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Using ANT: Sends shot location, recieves confirmation of hit */
int sendShot(int x, int y) {
  au8AntMessage[3] = y * 8 + x; // Converts coordinates to decimal
  if(AntReadAppMessageBuffer()) {
    if(G_eAntApiCurrentMessageClass == ANT_DATA) {
      /* We got returning fire check the data in checkHit */
      checkHit(G_au8AntApiCurrentMessageBytes[3]); // Continue state machine in further function
    } else if (G_eAntApiCurrentMessageClass == ANT_TICK) {
      /* A channel period has gone by, send shot coordinates until we get return fire */
      AntQueueBroadcastMessage(U8_ANT_CHANNEL_USERAPP, au8AntMessage);

      au8AntMessage[7]++; // Increment message counter for debug
      if(au8AntMessage[7] == 0) {
        au8AntMessage[6]++;
        if(au8AntMessage[6] == 0) 
          au8AntMessage[5]++;
      }
    } /* End ANT_TICK */
  } /* End ReadAppMessage Buffer */
} /* end sendShot() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Checks if shot from other board hit boat */
void checkHit(u8 shot) {
  // Convert hexadecimal to decimal coordinates
  int x = shot/8;
  int y = (shot % 8);

  // Check if the spot was occupied ie) not == 5
  if (board[x][y] != 5 ) 
    board[x][y] = 0; // Mark as hit
  displayBoard(); // Update board
  checkHealth(); // Update ship health and continue state machine in further function
} /* end checkHit() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ANT Channel to be configured*/
static void UserApp1SM_WaitAntReady(void) {
  if(AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_CONFIGURED) {
    if(AntOpenChannelNumber(U8_ANT_CHANNEL_USERAPP)) {
      UserApp1_pfStateMachine = UserApp1SM_WaitChannelOpen;
    } else {
      UserApp1_pfStateMachine = UserApp1SM_Error;
    }
  }
} /* end UserApp1SM_WaitAntReady() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Hold here until ANT Confirms channel is open*/
static void UserApp1SM_WaitChannelOpen(void) {
  if(AntRadioStatusChannel(U8_ANT_CHANNEL_USERAPP) == ANT_OPEN)
    UserApp1_pfStateMachine = UserApp1SM_Idle;
} /* end UserApp1SM_WaitChannelOpen() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* When all ships have no health alert other board of defeat */
void endGame() {

} /* End endGame() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* THIS STATE IS USELESS AND MUST BE REMOVED */
static void UserApp1SM_Idle(void) {
    if (placementState) {
        if (ship > 3) {
            placementState = FALSE;
        } else {
            placement();
            if (WasButtonPressed(BUTTON0) && WasButtonPressed(BUTTON1)) {
                ButtonAcknowledge(BUTTON0);
                ButtonAcknowledge(BUTTON1);
                ship++;
            }
        }
    }
    if(WasButtonPressed(BUTTON0)) {
        ButtonAcknowledge(BUTTON0);
        x++;
        if (x > 7) {
            x = 0;
        }
    }
    if(WasButtonPressed(BUTTON1)) {
        ButtonAcknowledge(BUTTON1);
        y++;
        if (y > 3) {
            y = 0;
        }
    }
    
    displayBoard();

} /* end UserApp1SM_Idle() */
     
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void) {
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
