--The hostapp is the firmware loading program which is to be run on the host.

--The uccpapp is a sample UCCP application that can be used to test the loader. 
To check if this is successfully loaded, after the loader has started the thread (Loading complete appears on CS output),
load the symbols (symbols only) of this file from CS add the following variables to CS watch window
	loopcount
	buffer[0]
	jiffies

If the program is running okay, 
--loopcount should increment continuously 
--jiffies increments for every MeOS timer tick (1ms) - slower rate than loopcount
-- buffer[0] switches between 0xAAAAAAAA and 0xFFFFFFFF based on loopcount being even or odd. (This is hard to catch from
the watch window, but you should be able to see this change at least one a few occasions)


--The hostapp has the following global variables to specify the LDR file(s) to be loaded.
	LDRFile1
	LDRFile2
  By default, LDRFile1 and LDRfile2 are set to UCCPAPP_LOADER.ldr.
  While loading MAC and PHY LDRs, LDRFile1 must be set to name of PHY LDR and LDRFile2 must be set to the name of MAC LDR
  

In all cases, the file server root directory of codescape should set to the location where the LDR files (to be loaded)are present.
Before starting the hostapp (and after loading the hostapp), LDRFile1 and LDRFile2 should be set from the CS watch window

To determine whether the first LDR has completed it's job, the loader checks for a signature "0x5a5a5a5a" at the
start of GRAM (0xB7000000). When it sees this, it concludes that first LDR has completed its task and proceeds to load 2nd LDR

In the case of UCCPAPP, you have write the completion signature manually from CS memory window for the loader to start
loading the same application 2nd time.

As each section is loaded, the hostapp outputs short pnenumonics like LM (Load Mem), ZM (Zero Mem) etc.
After loading is complete the hostapp writes 'Loading complete' to the CodeScape log window.

The hostapp can tested on simulator.Everything excluding the actual loading can be tested on simulator. The writes to 
UCCP are skipped. For this, the line GLOBAL_DEFINES+=DA_SIM must be un-commented in the Makefile

--Command line to build hostapp
	make CONFIG=test UCCP=420_33 all
The loader build doesn't really need the UCCP option. However, it is being set to make use of the "ldrcheck" utlity in 
the UCCRunTime to check if it's size has exceeded the available core memory.

The loader makes use of HEAP segment and it starts at an offset of 15MB from start of Ext RAM

--Command line to build uccpapp
	make CONFIG=release TARGET=CORE SOC_VARIANT=MODEL UCCP=420_33 all


