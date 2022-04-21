**In this document CS is an acronym for CodeScape

--The uccpapp folder contains a sample UCCP application that can be used to test the loader. 
To check if this is successfully loaded, after the loader has started the thread (Loading complete appears on CS output),
load the debug symbols (select debug symbols only option) of this file from CS add the following variables to CS watch window
	loopcount
	buffer[0]
	jiffies

If the program is running okay, 
-- loopcount should increment continuously 
-- jiffies increments for every MeOS timer tick (1ms) - slower rate than loopcount
-- buffer[0] switches between 0xAAAAAAAA and 0xFFFFFFFF based on loopcount being even or odd. (This is hard to catch from
the watch window, but you should be able to see this change at least on a few occasions)


--The app folder contains the linux loader application (uccpld) and the driver folder contains the linux driver which is used by the 
  application to load the firmware to UCCP.
The app can be used to load any LDR image to the firmware
The app can take upto 2 command line arguments which indicate the LDR file(s) that are to be loaded to the UCCP.

In case a single LDR file is given on the command line, the app will terminate after loading the first LDR.
In case 2 LDR files are given on the command line, the app will terminate after loading both.

For loading the WLAN firmware, there are 2 LDR files, one for the MCP(PHY) image and the other for the MAC image.
These are generated from the firmware build.

To load WLAN firmware, the loader application should be invoked the following way
#uccpld <PHY_LDR_FILE_NAME> <MAC_LDR_FILE_NAME>

(Note: The uccpld application needs to be run with root privileges)

To determine whether the first LDR has completed it's job, the loader checks for a signature "0x5a5a5a5a" at the
start of RPU GRAM (0xB7000000). When it sees this, it concludes that first program has completed its task and then proceeds to load 2nd LDR (Output displayed will be as follows)

If there is only one LDR file given as input, then in the succesful case the ouput displayed is:
	LDR1 (<ldr file name>) is loaded
	
If there are two LDR files given as input, then in the successful case the output displayed is:
	LDR1 (<ldr file name 1>) is loaded
	Waiting for the first program to execute
	First program is executed
	LDR2 (<ldr file name 2>) is loaded
	

--Command line to build uccpapp
	This requires the META Embedded toolkit to be installed on the build PC
	cd uccpapp/mtp/build/smake
	make CONFIG=release TARGET=CORE SOC_VARIANT=MODEL UCCP=420_33 all

--Procedure to build loader application
	
	The following Makefile flags are provided:
	
   1) DRIV_DIR should be set to the path containing driver source code
   
   2) Only for SoC environment, set -DUCCP420_HW=1
	        
   3) To enable the dump of LDR file parsed, set -DFILE_DUMP=1.
   
   To build the application
   cd app
   make
   
   If FILE_DUMP is set, then one or two output files are created as follows:
	1) file1_dump --- if only one LDR input is specified
	2) file1_dump and file2_dump --- if two LDR files are specified as input 
		corresponding to LDR1 and LDR2 respectively.
   
--Procedure to build driver
	The following Makefile variables are provided.
	
   1) To enable debug information output in the driver, set -DIMG_DEBUG=1 in the Makefile
        
   2) Only for SoC environment, set -DUCCP420_HW=1	  
   
    To build the driver
	cd driver
	make
	
   To view the debug output of the driver, type the "dmesg" command, 
   The debug output from driver is prefixed by the string "UCCP420CHAR".
   

--Procedure to test application and driver on Linux PC
   1) Load the UCCP Char Driver kernel module
		insmod uccp_char.ko
   2) Run the uccpld application with the LDR file(s) as input
		./uccpld <ldr_file1> [<ldr_file2>]
		 In the output, verify the status of loading.
   3) Check the dmesg output to see debug information from the driver 
	
--Porting information

The following changes are needed to port the loading system to TSB SoC
1) In the driver, file uccp_drv.c, Function: uccp_init
	Add the code to map the following regions of RPU address map into Host virtual address space
	UCCP Slave port:Start: 0x0203C000, Length to be mapped = 0x4000
	UCCP packed GRAM: Start : 0xB7000000, Length to be mapped = 0x66CC0
	UCCP GRAM: Start: 0xB4000000, Length to be mapped = 0x66CC0

	Store the necessary information (for example base addresses) in the 'priv' structure.

2) In the driver, file uccp_io.c
  All the functions in the file need to be implemented according to the comments given for that functions 




