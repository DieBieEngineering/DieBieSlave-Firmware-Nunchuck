#include "middleSOES.h"

#define WD_RESET           1000
#define DEFAULTTXPDOMAP    0x1a00
#define DEFAULTRXPDOMAP    0x1600
#define DEFAULTTXPDOITEMS  1
#define DEFAULTRXPDOITEMS  1

extern driverSWNunChuckDataStruct nunChuckData;

uint32_t            encoder_scale;
uint32_t            encoder_scale_mirror;

volatile _ESCvar  ESCvar;
_MBX              MBX[MBXBUFFERS];
_MBXcontrol       MBXcontrol[MBXBUFFERS];
uint8_t           MBXrun=0;
uint16_t          SM2_sml,SM3_sml;
_Rbuffer          Rb;
_Wbuffer          Wb;
_Cbuffer          Cb;
_App              App;
uint16_t          TXPDOsize,RXPDOsize;
int               wd_cnt = WD_RESET;
volatile uint8_t  digoutput;
volatile uint8_t  diginput;
uint16_t          txpdomap = DEFAULTTXPDOMAP;
uint16_t          rxpdomap = DEFAULTRXPDOMAP;
uint8_t           txpdoitems = DEFAULTTXPDOITEMS;
uint8_t           rxpdoitems = DEFAULTTXPDOITEMS;

/** Mandatory: Hook called from the slave stack SDO Download handler to act on
 * user specified Index and Sub-index.
 *
 * @param[in] index      = index of SDO download request to handle
 * @param[in] sub-index  = sub-index of SDO download request to handle
 */
void ESC_objecthandler(uint16_t index, uint8_t subindex) {
   switch (index) {
      case 0x1c12: {
         if (rxpdoitems > 1) {
            rxpdoitems = 1;
         }
				 
         if ((rxpdomap != 0x1600) && (rxpdomap != 0x1601) && (rxpdomap != 0x0000)) {
            rxpdomap = 0x1600;
         }
				 
         RXPDOsize = SM2_sml = sizeRXPDO();
         break;
      }
      case 0x1c13: {
         if (txpdoitems > 1)
         {
            txpdoitems = 1;
         }
         if ((txpdomap != 0x1A00) && (txpdomap != 0x1A01) && (rxpdomap != 0x0000)) {
            txpdomap = 0x1A00;
         }
         TXPDOsize = SM3_sml = sizeTXPDO();
         break;
      }
      case 0x7100: {
         switch (subindex) {
            case 0x01: {
               encoder_scale_mirror = encoder_scale;
               break;
            }
         }
         break;
      }
      case 0x8001: {
         switch (subindex) {
            case 0x01: {
               Cb.reset_counter = 0; // When something is written to this variable
               break;
            }
         }
         break;
      }
   }
}

/** Mandatory: Hook called from the slave stack ESC_stopoutputs to act on state changes
 * forcing us to stop outputs. Here we can set them to a safe state.
 * set
 */
void APP_safeoutput(void) {
   DPRINT("APP_safeoutput called\n");
   Wb.LED = 0;
}
/** Mandatory: Write local process data to Sync Manager 3, Master Inputs.
 */
void TXPDO_update(void) {
	ESC_write(SM3_sma, &Rb.button, TXPDOsize);
}
/** Mandatory: Read Sync Manager 2 to local process data, Master Outputs.
 */
void RXPDO_update(void) {
   ESC_read(SM2_sma, &Wb.LED, RXPDOsize);
}

/** Mandatory: Function to update local I/O, call read ethercat outputs, call
 * write ethercat inputs. Implement watch-dog counter to count-out if we have
 * made state change affecting the App.state.
 */
void DIG_process(void) {
   if (wd_cnt){
      wd_cnt--;
   }
   if (App.state & APPSTATE_OUTPUT){
      /* SM2 trigger ? */
      if (ESCvar.ALevent & ESCREG_ALEVENT_SM2) {
         ESCvar.ALevent &= ~ESCREG_ALEVENT_SM2;
         RXPDO_update();
         wd_cnt = WD_RESET;
         /* dummy output point */
         if (Wb.LED) {
            modEffectChangeState(STAT_LED_DEBUG,STAT_SET);
         } else {
            modEffectChangeState(STAT_LED_DEBUG,STAT_RESET);
         }
      }
      if (!wd_cnt) {
         //DPRINT("DIG_process watchdog tripped\n");
         ESC_stopoutput();
         /* watchdog, invalid outputs */
         ESC_ALerror(ALERR_WATCHDOG);
         /* goto safe-op with error bit set */
         ESC_ALstatus(ESCsafeop | ESCerror);
      }
   }else{
      wd_cnt = WD_RESET;
   }
   if (App.state){
			//Rb.button = nunChuckData.joystickX; //
			Rb.button = nunChuckData.buttonC ? 255 : 0;		 
			Cb.reset_counter++;
			Rb.encoder =  (nunChuckData.accelerometerX << 22);
			TXPDO_update();
   }
}

/** SOES main loop. Start by initializing the stack software followed by
 * the application loop for cyclic read the EtherCAT state and staus, update
 * of I/O.
 */
void middleSOESInit(void) {
	DPRINT ("SOES (Simple Open EtherCAT Slave)\n");

	ESC_reset();
	ESC_init();

	TXPDOsize = SM3_sml = sizeTXPDO();
	RXPDOsize = SM2_sml = sizeRXPDO();
	
	//Read BYTE-ORDER register 0x64.
	uint32_t data = 0;
	do {
		data = PDIReadLAN9252DirectReg(LAN9252_BYTE_ORDER_REG);     // If this works lots of other things work.
	}while(0x87654321 != data);

	/*  wait until ESC is started up */
	do {
		ESC_read(ESCREG_DLSTATUS, (void *)&ESCvar.DLstatus,sizeof(ESCvar.DLstatus));
		ESCvar.DLstatus = etohs(ESCvar.DLstatus);
	} while ((ESCvar.DLstatus & 0x0001) == 0);

	/* reset ESC to init state */
	ESC_ALstatus(ESCinit);
	ESC_ALerror(ALERR_NONE);
	ESC_stopmbx();
	ESC_stopinput();
	ESC_stopoutput();
}

void middleSOESTask(void) {
   /* On init restore PDO mappings to default size */
   if((ESCvar.ALstatus & 0x0f) == ESCinit) {
      txpdomap = DEFAULTTXPDOMAP;
      rxpdomap = DEFAULTRXPDOMAP;
      txpdoitems = DEFAULTTXPDOITEMS;
      rxpdoitems = DEFAULTTXPDOITEMS;
   }
   /* Read local time from ESC*/
   ESC_read(ESCREG_LOCALTIME, (void *) &ESCvar.Time, sizeof (ESCvar.Time));
   ESCvar.Time = etohl(ESCvar.Time);
	 
	 ESC_ReadAlEvent();

   /* Check the state machine */
   ESC_state();

   /* If else to two separate execution paths
    * If we're running BOOSTRAP
    *  - MailBox
    *   - FoE
    * Else we're running normal execution
    *  - MailBox
    *   - CoE
    */
    if (ESC_mbxprocess()) {
       ESC_coeprocess();
       ESC_xoeprocess();
    }
    DIG_process();
}
