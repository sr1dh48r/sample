UNIT_TYPE=APP
UNIT_NAME=UCCP_APP

GLOBAL_OPTIONS+=MINIM
UCC_TOOLKIT_DIR=../../../../uccToolkit
ENSIGMASDK_DIR=../../../../../ensigmaSDK

SMAKE_ROOT=$(ENSIGMASDK_DIR)/smake
UCCP_ROOT=$(UCC_TOOLKIT_DIR)/uccPlatform

include $(UCCP_ROOT)/targetSetup.mk

C_SRC = uccpapp.c 

GLOBAL_OPERATING_SYSTEM=MEOS
GLOBAL_DEFINES+=__META_MEOS__

ifeq ($(GLOBAL_UCCP),420_28)
GLOBAL_DEFINES+=__UCCP_420_28__
else ifeq ($(GLOBAL_UCCP),420_33)
GLOBAL_DEFINES+=__UCCP_420_33__
else ifeq ($(GLOBAL_UCCP),420_37)
GLOBAL_DEFINES+=__UCCP_420_37__
else ifeq ($(GLOBAL_UCCP),420_38)
GLOBAL_DEFINES+=__UCCP_420_38__
endif


#Enable MeOS Timer Task to start the timer task and activate Timer ISR of MeOS
#GLOBAL_DEFINES+=__ENABLE_MEOS_TIMERTASK__
# Enable MEM_TEST to run a GRAM memory test
#GLOBAL_DEFINES+= MEM_TEST
# Enable GRAM_SWEEP to sweep entire GRAM, otherwise, only 512 bytes will be tested.
#GLOBAL_DEFINES+=GRAM_SWEEP


include $(SMAKE_ROOT)/smake.mk


