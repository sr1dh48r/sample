UNIT_NAME=UCCPAPP_LOADER
UNIT_TYPE=LOADER
#VERBOSE=1

SMAKE_ROOT= ../../../../../../ensigmaSDK/smake
UCCP_ROOT:=$(shell cd ../../../../../uccToolkit/uccPlatform ; pwd)


UCC_TOOL_KIT=../../../../uccToolkit

GLOBAL_OPTIONS+=MINIM
GLOBAL_OPTIONS+=META_RENAME_TEXT META_RENAME_DATA

ifeq ($(SOC),TZ5000)
export GLOBAL_SOC=CORE
export TARGET_SOC=TZ5000
export GLOBAL_UCCP=420_33
endif

ifeq ($(SOC),VOLTTC)
export GLOBAL_SOC=CORE
export TARGET_SOC=VOLTTC
export GLOBAL_UCCP=420_28
endif

ifeq ($(SOC),DANUBE)
export GLOBAL_SOC=CORE
export TARGET_SOC=DANUBE
export GLOBAL_UCCP=420_37
endif

ifeq ($(SOC),INC)
export GLOBAL_SOC=CORE
export TARGET_SOC=INC
export GLOBAL_UCCP=420_38
endif


include $(UCCP_ROOT)/targetSetup.mk

GLOBAL_PROCESSOR=META

GLOBAL_OPERATING_SYSTEM=MEOS

APPS=$(UNIT_DIR)/..@Makefile

ifeq ($(CLOCKSETUP),TRUE)
UNIT_ABSOLUTE_IMGS+=$(UNIT_DIR)/build/smake/scripts/$(UCCP)/$(TARGET_SOC).img
endif

UNIT_ABSOLUTE_IMGS+=$(UNIT_DIR)/build/smake/scripts/$(UCCP)/UCCPAPP.img


CHECK_LDR=0
include $(SMAKE_ROOT)/smake.mk
