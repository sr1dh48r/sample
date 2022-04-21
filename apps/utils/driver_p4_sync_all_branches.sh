#!/bin/bash
#*************************************************************************************************************************************************
if [ $# -lt 1 ] ; then
	echo ""
	echo "Usage : sh $0 Mandatorry: PROJECTNAME1 PROJECTNAME2 ...DONTSUBMIT"
	echo ""
	exit 1
fi
#*************************************************************************************************************************************************

# Change these variables suitable values
WORKSPACE_NAME=rpu_driver_checkout
DONTSUBMIT=0
#****
#E-MAIL CONTACTS
CC="Sachin.Kulkarni@imgtec.com"
#exit 1
SCRIPT_ROOT=`pwd`
####LOGGING
SCRIPT_LOG=p4_auto_sync_logging_`date "+%Y-%m-%d:%H-%M-%S"`.log
exec > >(tee -i $SCRIPT_LOG)
exec 2>&1

init_client_spec() {
	rm client_specs.txt

	echo "Initializing p4 client spec...\n"

	export P4CLIENT=$WORKSPACE_NAME
	p4 set P4CLIENT=$WORKSPACE_NAME

	echo "Client:	"$WORKSPACE_NAME"" >> client_specs.txt
	echo "Owner:	"$P4USER"" >> client_specs.txt
	echo "Host:	    "$HOSTNAME"" >> client_specs.txt
	echo "Description:" >> client_specs.txt
	echo "	Created by "$P4USER"." >> client_specs.txt
	echo "Root:	"$PWD"" >> client_specs.txt
	echo "Options:	allwrite noclobber compress unlocked nomodtime rmdir" >> client_specs.txt
	echo "SubmitOptions:	revertunchanged" >> client_specs.txt
	echo "LineEnd:	local" >> client_specs.txt
	echo "View:" >> client_specs.txt

}

prepare_client_spec() {
	BRANCH_HOST_PATH=$1
	SOFTMAC_PATH=$2

	init_client_spec

	if [ $? -ne 0 ] ; then
		echo "init_client_spec failed: $?"
		return 1
	fi

	echo "    $BRANCH_HOST_PATH/$SOFTMAC_PATH/... //$WORKSPACE_NAME/..." >> client_specs.txt

	echo "Loading p4 client spec...\n`cat client_specs.txt`\n"
	p4 client -i < client_specs.txt  

	if [ $? -ne 0 ] ; then
		return 2
	fi

	return 0
}

MAIN_BRANCH_HOST_PATH="//ensigma/sw/connectivity/wlan/MAIN/ip/software/host"
SOFTMAC_PATH=drivers/linux/softmac

MAIN_BRANCH_DIR="/home/user/p4_automation/MAIN_P4_DIR"
IN_TREE_TO_OOT_RENAME=0

rm -rf /home/user/p4_automation
mkdir -p $MAIN_BRANCH_DIR
cd $MAIN_BRANCH_DIR
prepare_client_spec $MAIN_BRANCH_HOST_PATH $SOFTMAC_PATH

##ERR CHECK
ret=$?
if [ $ret -ne 0 ] ; then
	echo "prepare_client_spec failed: ret: $ret\n"
	p4 client -d $WORKSPACE_NAME
	exit 1
fi

echo "P4 Syncing MAIN branch from depot...\n"
p4 sync 
p4 client -d $WORKSPACE_NAME


### PROJECT SPECIFIC 
###
### 
for PROJ in "$@"
do
	echo "Processing $PROJ...."
	if [ "$PROJ" = "EUROPA" ] ; then
		PROJ_BRANCH_HOST_DIR="//ensigma/sw/connectivity/wlan/DEV/EUROPA/host"
		SOFTMAC_PATH="drivers/linux/softmac"
		KERNEL_VER="4\.1.*"
		KERNEL_ROOT="cross"
		COMPILE_CMD="europa_driver_compile"
		TO="AjayKumar.Parida@imgtec.com"
	elif [ "$PROJ" = "LPW" ] ; then
		PROJ_BRANCH_HOST_DIR="//ensigma/sw/connectivity/wlan/DEV/LPW_MIPS/ip/software/host"
		SOFTMAC_PATH="drivers/softmac"
		KERNEL_VER="4\.1.*"
		KERNEL_ROOT="local"
		TO="Chaitanya.Tata@imgtec.com"
	elif [ "$PROJ" = "DANUBE" ] ; then
		PROJ_BRANCH_HOST_DIR="//ensigma/sw/connectivity/wlan/DEV/DANUBE/ip/software/host"
		SOFTMAC_PATH="drivers/softmac"
		#KERNEL="4\.4.*"
		COMPILE_CMD="danube_driver_compile"
		KERNEL_ROOT="cross"
		TO="AjayKumar.Parida@imgtec.com"
	elif [ "$PROJ" = "AMD" ] ; then
		PROJ_BRANCH_HOST_DIR="//ensigma/sw/connectivity/wlan/DEV/AMD/canova/software/host"
		SOFTMAC_PATH="drivers/linux/4.4/drivers/net/wireless/img/softmac"
		KERNEL_ROOT="local"
		KERNEL_VER="4\.4.*"
		IN_TREE_TO_OOT_RENAME=1
		TO="chaitanya.tata@imgtec.com"
	fi

	### GET THE KERNEL VERSION on the PC
	if [ "$KERNEL_ROOT" = "local" ]; then	
		KERNELS=`ls  /lib/modules/ | awk '{print $NF}'`
		MATCH_FOUND=0
		for KRN in $KERNELS
		do
			echo "Checking $KRN now..$KERNEL_VER\n"
			if [[ $KRN =~ $KERNEL_VER ]]; then
				KERNEL_ROOT=/lib/modules/$KRN/build
				if [ -d $KERNEL_ROOT ]; then 
					MATCH_FOUND=1
					COMPILE_CMD="make KROOT=$KERNEL_ROOT"
					break
				fi
			fi
		done

		if [ $MATCH_FOUND -eq 0 ]; then
			echo "Skipping $PROJ as there is no matching kernel found\n need: $KERNEL_VER, got: $KERNELS"
			continue
		fi
	fi

	PROJ_BRANCH_DIR="/home/user/p4_automation/$PROJ"
	mkdir -p $PROJ_BRANCH_DIR
	cd $PROJ_BRANCH_DIR
	prepare_client_spec $PROJ_BRANCH_HOST_DIR $SOFTMAC_PATH

	##ERR CHECK
	ret=$?
	if [ $ret -ne 0 ] ; then
		echo "prepare_client_spec failed: ret: $ret\n"
		p4 client -d $WORKSPACE_NAME
		exit 1
	fi

	echo "P4 Syncing $PROJ from depot...\n"
	p4 sync 
	PROJ_S=`echo $PROJ | tr A-Z a-z`
	cd $MAIN_BRANCH_DIR
	echo "Running script on MAIN to generate $PROJ specific code\n"
	sh scripts/generate/generate_driver_$PROJ_S.sh
	cd $PROJ
	echo "Local Syncing $PROJ from MAIN spinoff code\n"
	rsync -avh --stats --progress . $PROJ_BRANCH_DIR


	#COMPILE DRIVER
	cd $PROJ_BRANCH_DIR

	##swap makefiles in and out of tree
	if [ $IN_TREE_TO_OOT_RENAME -eq 1 ] ; then
		mv Makefile Makefile_in_tree
		mv Makefile_out_of_tree Makefile
	fi


	echo "Compiling the Driver for $PROJ : $COMPILE_CMD"
	eval $COMPILE_CMD
	##ERR CHECK
	ret=$?
	if [ $ret -ne 0 ] ; then
		echo "Compilation failed: ret: $ret\n"
		SUBJECT="Automated Compilation Status: $PROJ "
		echo "TO: $TO , CC: $CC"
		## STOP LOGGING
		exec >&2 
		echo "Done"
		p4 client -d $WORKSPACE_NAME
		cat "$SCRIPT_ROOT/$SCRIPT_LOG" | mail -s "$SUBJECT" $TO $CC
		exit 1
	fi
	
	#P4 SUBMIT
	if [ $DONTSUBMIT -eq 0 ] ; then
		p4 reconcile -e -c default
		p4 submit -f revertunchanged -s -d "SYNCing $PROJ with MAIN. Note: This is an automated p4 submission."
		p4 client -d $WORKSPACE_NAME
	else
		echo "Skipping the p4 submit, please do it manually"
	fi
done
