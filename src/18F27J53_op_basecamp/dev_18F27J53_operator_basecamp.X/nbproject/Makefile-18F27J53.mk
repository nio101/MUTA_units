#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-18F27J53.mk)" "nbproject/Makefile-local-18F27J53.mk"
include nbproject/Makefile-local-18F27J53.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=18F27J53
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/dev_18F27J53_operator_basecamp.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/dev_18F27J53_operator_basecamp.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=src/18F27J53/console.c src/18F27J53/delay.c src/18F27J53/spi.c src/18F27J53/symbol.c src/18F27J53/system.c src/miwi/drv_mrf_miwi_89xa.c src/miwi/drv_mrf_miwi_crc.c src/miwi/drv_mrf_miwi_security.c src/miwi/miwi_mesh.c src/usb/usb_device.c src/usb/usb_device_hid.c src/app_led_usb_status.c src/main.c src/usb_descriptors.c ../../muta/muta_messages.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/src/18F27J53/console.p1 ${OBJECTDIR}/src/18F27J53/delay.p1 ${OBJECTDIR}/src/18F27J53/spi.p1 ${OBJECTDIR}/src/18F27J53/symbol.p1 ${OBJECTDIR}/src/18F27J53/system.p1 ${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.p1 ${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.p1 ${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.p1 ${OBJECTDIR}/src/miwi/miwi_mesh.p1 ${OBJECTDIR}/src/usb/usb_device.p1 ${OBJECTDIR}/src/usb/usb_device_hid.p1 ${OBJECTDIR}/src/app_led_usb_status.p1 ${OBJECTDIR}/src/main.p1 ${OBJECTDIR}/src/usb_descriptors.p1 ${OBJECTDIR}/_ext/1853667253/muta_messages.p1
POSSIBLE_DEPFILES=${OBJECTDIR}/src/18F27J53/console.p1.d ${OBJECTDIR}/src/18F27J53/delay.p1.d ${OBJECTDIR}/src/18F27J53/spi.p1.d ${OBJECTDIR}/src/18F27J53/symbol.p1.d ${OBJECTDIR}/src/18F27J53/system.p1.d ${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.p1.d ${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.p1.d ${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.p1.d ${OBJECTDIR}/src/miwi/miwi_mesh.p1.d ${OBJECTDIR}/src/usb/usb_device.p1.d ${OBJECTDIR}/src/usb/usb_device_hid.p1.d ${OBJECTDIR}/src/app_led_usb_status.p1.d ${OBJECTDIR}/src/main.p1.d ${OBJECTDIR}/src/usb_descriptors.p1.d ${OBJECTDIR}/_ext/1853667253/muta_messages.p1.d

# Object Files
OBJECTFILES=${OBJECTDIR}/src/18F27J53/console.p1 ${OBJECTDIR}/src/18F27J53/delay.p1 ${OBJECTDIR}/src/18F27J53/spi.p1 ${OBJECTDIR}/src/18F27J53/symbol.p1 ${OBJECTDIR}/src/18F27J53/system.p1 ${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.p1 ${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.p1 ${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.p1 ${OBJECTDIR}/src/miwi/miwi_mesh.p1 ${OBJECTDIR}/src/usb/usb_device.p1 ${OBJECTDIR}/src/usb/usb_device_hid.p1 ${OBJECTDIR}/src/app_led_usb_status.p1 ${OBJECTDIR}/src/main.p1 ${OBJECTDIR}/src/usb_descriptors.p1 ${OBJECTDIR}/_ext/1853667253/muta_messages.p1

# Source Files
SOURCEFILES=src/18F27J53/console.c src/18F27J53/delay.c src/18F27J53/spi.c src/18F27J53/symbol.c src/18F27J53/system.c src/miwi/drv_mrf_miwi_89xa.c src/miwi/drv_mrf_miwi_crc.c src/miwi/drv_mrf_miwi_security.c src/miwi/miwi_mesh.c src/usb/usb_device.c src/usb/usb_device_hid.c src/app_led_usb_status.c src/main.c src/usb_descriptors.c ../../muta/muta_messages.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-18F27J53.mk dist/${CND_CONF}/${IMAGE_TYPE}/dev_18F27J53_operator_basecamp.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F27J53
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/src/18F27J53/console.p1: src/18F27J53/console.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/18F27J53" 
	@${RM} ${OBJECTDIR}/src/18F27J53/console.p1.d 
	@${RM} ${OBJECTDIR}/src/18F27J53/console.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/18F27J53/console.p1  src/18F27J53/console.c 
	@-${MV} ${OBJECTDIR}/src/18F27J53/console.d ${OBJECTDIR}/src/18F27J53/console.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/18F27J53/console.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/18F27J53/delay.p1: src/18F27J53/delay.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/18F27J53" 
	@${RM} ${OBJECTDIR}/src/18F27J53/delay.p1.d 
	@${RM} ${OBJECTDIR}/src/18F27J53/delay.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/18F27J53/delay.p1  src/18F27J53/delay.c 
	@-${MV} ${OBJECTDIR}/src/18F27J53/delay.d ${OBJECTDIR}/src/18F27J53/delay.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/18F27J53/delay.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/18F27J53/spi.p1: src/18F27J53/spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/18F27J53" 
	@${RM} ${OBJECTDIR}/src/18F27J53/spi.p1.d 
	@${RM} ${OBJECTDIR}/src/18F27J53/spi.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/18F27J53/spi.p1  src/18F27J53/spi.c 
	@-${MV} ${OBJECTDIR}/src/18F27J53/spi.d ${OBJECTDIR}/src/18F27J53/spi.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/18F27J53/spi.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/18F27J53/symbol.p1: src/18F27J53/symbol.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/18F27J53" 
	@${RM} ${OBJECTDIR}/src/18F27J53/symbol.p1.d 
	@${RM} ${OBJECTDIR}/src/18F27J53/symbol.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/18F27J53/symbol.p1  src/18F27J53/symbol.c 
	@-${MV} ${OBJECTDIR}/src/18F27J53/symbol.d ${OBJECTDIR}/src/18F27J53/symbol.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/18F27J53/symbol.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/18F27J53/system.p1: src/18F27J53/system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/18F27J53" 
	@${RM} ${OBJECTDIR}/src/18F27J53/system.p1.d 
	@${RM} ${OBJECTDIR}/src/18F27J53/system.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/18F27J53/system.p1  src/18F27J53/system.c 
	@-${MV} ${OBJECTDIR}/src/18F27J53/system.d ${OBJECTDIR}/src/18F27J53/system.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/18F27J53/system.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.p1: src/miwi/drv_mrf_miwi_89xa.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/miwi" 
	@${RM} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.p1.d 
	@${RM} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.p1  src/miwi/drv_mrf_miwi_89xa.c 
	@-${MV} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.d ${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.p1: src/miwi/drv_mrf_miwi_crc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/miwi" 
	@${RM} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.p1.d 
	@${RM} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.p1  src/miwi/drv_mrf_miwi_crc.c 
	@-${MV} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.d ${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.p1: src/miwi/drv_mrf_miwi_security.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/miwi" 
	@${RM} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.p1.d 
	@${RM} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.p1  src/miwi/drv_mrf_miwi_security.c 
	@-${MV} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.d ${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/miwi/miwi_mesh.p1: src/miwi/miwi_mesh.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/miwi" 
	@${RM} ${OBJECTDIR}/src/miwi/miwi_mesh.p1.d 
	@${RM} ${OBJECTDIR}/src/miwi/miwi_mesh.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/miwi/miwi_mesh.p1  src/miwi/miwi_mesh.c 
	@-${MV} ${OBJECTDIR}/src/miwi/miwi_mesh.d ${OBJECTDIR}/src/miwi/miwi_mesh.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/miwi/miwi_mesh.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/usb/usb_device.p1: src/usb/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/usb" 
	@${RM} ${OBJECTDIR}/src/usb/usb_device.p1.d 
	@${RM} ${OBJECTDIR}/src/usb/usb_device.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/usb/usb_device.p1  src/usb/usb_device.c 
	@-${MV} ${OBJECTDIR}/src/usb/usb_device.d ${OBJECTDIR}/src/usb/usb_device.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/usb/usb_device.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/usb/usb_device_hid.p1: src/usb/usb_device_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/usb" 
	@${RM} ${OBJECTDIR}/src/usb/usb_device_hid.p1.d 
	@${RM} ${OBJECTDIR}/src/usb/usb_device_hid.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/usb/usb_device_hid.p1  src/usb/usb_device_hid.c 
	@-${MV} ${OBJECTDIR}/src/usb/usb_device_hid.d ${OBJECTDIR}/src/usb/usb_device_hid.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/usb/usb_device_hid.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/app_led_usb_status.p1: src/app_led_usb_status.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/app_led_usb_status.p1.d 
	@${RM} ${OBJECTDIR}/src/app_led_usb_status.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/app_led_usb_status.p1  src/app_led_usb_status.c 
	@-${MV} ${OBJECTDIR}/src/app_led_usb_status.d ${OBJECTDIR}/src/app_led_usb_status.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/app_led_usb_status.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/main.p1: src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/main.p1.d 
	@${RM} ${OBJECTDIR}/src/main.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/main.p1  src/main.c 
	@-${MV} ${OBJECTDIR}/src/main.d ${OBJECTDIR}/src/main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/usb_descriptors.p1: src/usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/usb_descriptors.p1.d 
	@${RM} ${OBJECTDIR}/src/usb_descriptors.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/usb_descriptors.p1  src/usb_descriptors.c 
	@-${MV} ${OBJECTDIR}/src/usb_descriptors.d ${OBJECTDIR}/src/usb_descriptors.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/usb_descriptors.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1853667253/muta_messages.p1: ../../muta/muta_messages.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1853667253" 
	@${RM} ${OBJECTDIR}/_ext/1853667253/muta_messages.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1853667253/muta_messages.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1853667253/muta_messages.p1  ../../muta/muta_messages.c 
	@-${MV} ${OBJECTDIR}/_ext/1853667253/muta_messages.d ${OBJECTDIR}/_ext/1853667253/muta_messages.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1853667253/muta_messages.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/src/18F27J53/console.p1: src/18F27J53/console.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/18F27J53" 
	@${RM} ${OBJECTDIR}/src/18F27J53/console.p1.d 
	@${RM} ${OBJECTDIR}/src/18F27J53/console.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/18F27J53/console.p1  src/18F27J53/console.c 
	@-${MV} ${OBJECTDIR}/src/18F27J53/console.d ${OBJECTDIR}/src/18F27J53/console.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/18F27J53/console.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/18F27J53/delay.p1: src/18F27J53/delay.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/18F27J53" 
	@${RM} ${OBJECTDIR}/src/18F27J53/delay.p1.d 
	@${RM} ${OBJECTDIR}/src/18F27J53/delay.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/18F27J53/delay.p1  src/18F27J53/delay.c 
	@-${MV} ${OBJECTDIR}/src/18F27J53/delay.d ${OBJECTDIR}/src/18F27J53/delay.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/18F27J53/delay.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/18F27J53/spi.p1: src/18F27J53/spi.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/18F27J53" 
	@${RM} ${OBJECTDIR}/src/18F27J53/spi.p1.d 
	@${RM} ${OBJECTDIR}/src/18F27J53/spi.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/18F27J53/spi.p1  src/18F27J53/spi.c 
	@-${MV} ${OBJECTDIR}/src/18F27J53/spi.d ${OBJECTDIR}/src/18F27J53/spi.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/18F27J53/spi.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/18F27J53/symbol.p1: src/18F27J53/symbol.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/18F27J53" 
	@${RM} ${OBJECTDIR}/src/18F27J53/symbol.p1.d 
	@${RM} ${OBJECTDIR}/src/18F27J53/symbol.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/18F27J53/symbol.p1  src/18F27J53/symbol.c 
	@-${MV} ${OBJECTDIR}/src/18F27J53/symbol.d ${OBJECTDIR}/src/18F27J53/symbol.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/18F27J53/symbol.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/18F27J53/system.p1: src/18F27J53/system.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/18F27J53" 
	@${RM} ${OBJECTDIR}/src/18F27J53/system.p1.d 
	@${RM} ${OBJECTDIR}/src/18F27J53/system.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/18F27J53/system.p1  src/18F27J53/system.c 
	@-${MV} ${OBJECTDIR}/src/18F27J53/system.d ${OBJECTDIR}/src/18F27J53/system.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/18F27J53/system.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.p1: src/miwi/drv_mrf_miwi_89xa.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/miwi" 
	@${RM} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.p1.d 
	@${RM} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.p1  src/miwi/drv_mrf_miwi_89xa.c 
	@-${MV} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.d ${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_89xa.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.p1: src/miwi/drv_mrf_miwi_crc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/miwi" 
	@${RM} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.p1.d 
	@${RM} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.p1  src/miwi/drv_mrf_miwi_crc.c 
	@-${MV} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.d ${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_crc.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.p1: src/miwi/drv_mrf_miwi_security.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/miwi" 
	@${RM} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.p1.d 
	@${RM} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.p1  src/miwi/drv_mrf_miwi_security.c 
	@-${MV} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.d ${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/miwi/drv_mrf_miwi_security.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/miwi/miwi_mesh.p1: src/miwi/miwi_mesh.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/miwi" 
	@${RM} ${OBJECTDIR}/src/miwi/miwi_mesh.p1.d 
	@${RM} ${OBJECTDIR}/src/miwi/miwi_mesh.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/miwi/miwi_mesh.p1  src/miwi/miwi_mesh.c 
	@-${MV} ${OBJECTDIR}/src/miwi/miwi_mesh.d ${OBJECTDIR}/src/miwi/miwi_mesh.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/miwi/miwi_mesh.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/usb/usb_device.p1: src/usb/usb_device.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/usb" 
	@${RM} ${OBJECTDIR}/src/usb/usb_device.p1.d 
	@${RM} ${OBJECTDIR}/src/usb/usb_device.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/usb/usb_device.p1  src/usb/usb_device.c 
	@-${MV} ${OBJECTDIR}/src/usb/usb_device.d ${OBJECTDIR}/src/usb/usb_device.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/usb/usb_device.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/usb/usb_device_hid.p1: src/usb/usb_device_hid.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src/usb" 
	@${RM} ${OBJECTDIR}/src/usb/usb_device_hid.p1.d 
	@${RM} ${OBJECTDIR}/src/usb/usb_device_hid.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/usb/usb_device_hid.p1  src/usb/usb_device_hid.c 
	@-${MV} ${OBJECTDIR}/src/usb/usb_device_hid.d ${OBJECTDIR}/src/usb/usb_device_hid.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/usb/usb_device_hid.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/app_led_usb_status.p1: src/app_led_usb_status.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/app_led_usb_status.p1.d 
	@${RM} ${OBJECTDIR}/src/app_led_usb_status.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/app_led_usb_status.p1  src/app_led_usb_status.c 
	@-${MV} ${OBJECTDIR}/src/app_led_usb_status.d ${OBJECTDIR}/src/app_led_usb_status.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/app_led_usb_status.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/main.p1: src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/main.p1.d 
	@${RM} ${OBJECTDIR}/src/main.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/main.p1  src/main.c 
	@-${MV} ${OBJECTDIR}/src/main.d ${OBJECTDIR}/src/main.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/main.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/src/usb_descriptors.p1: src/usb_descriptors.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/src" 
	@${RM} ${OBJECTDIR}/src/usb_descriptors.p1.d 
	@${RM} ${OBJECTDIR}/src/usb_descriptors.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/src/usb_descriptors.p1  src/usb_descriptors.c 
	@-${MV} ${OBJECTDIR}/src/usb_descriptors.d ${OBJECTDIR}/src/usb_descriptors.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/src/usb_descriptors.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1853667253/muta_messages.p1: ../../muta/muta_messages.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1853667253" 
	@${RM} ${OBJECTDIR}/_ext/1853667253/muta_messages.p1.d 
	@${RM} ${OBJECTDIR}/_ext/1853667253/muta_messages.p1 
	${MP_CC} --pass1 $(MP_EXTRA_CC_PRE) --chip=$(MP_PROCESSOR_OPTION) -Q -G  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib $(COMPARISON_BUILD)  --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    -o${OBJECTDIR}/_ext/1853667253/muta_messages.p1  ../../muta/muta_messages.c 
	@-${MV} ${OBJECTDIR}/_ext/1853667253/muta_messages.d ${OBJECTDIR}/_ext/1853667253/muta_messages.p1.d 
	@${FIXDEPS} ${OBJECTDIR}/_ext/1853667253/muta_messages.p1.d $(SILENT) -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/dev_18F27J53_operator_basecamp.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) --chip=$(MP_PROCESSOR_OPTION) -G -mdist/${CND_CONF}/${IMAGE_TYPE}/dev_18F27J53_operator_basecamp.X.${IMAGE_TYPE}.map  -D__DEBUG=1 --debugger=pickit3  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"        $(COMPARISON_BUILD) --memorysummary dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml -odist/${CND_CONF}/${IMAGE_TYPE}/dev_18F27J53_operator_basecamp.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	@${RM} dist/${CND_CONF}/${IMAGE_TYPE}/dev_18F27J53_operator_basecamp.X.${IMAGE_TYPE}.hex 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/dev_18F27J53_operator_basecamp.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE) --chip=$(MP_PROCESSOR_OPTION) -G -mdist/${CND_CONF}/${IMAGE_TYPE}/dev_18F27J53_operator_basecamp.X.${IMAGE_TYPE}.map  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 -I"src" -I"src/18F27J53" -I"../../../xc8/v1.35/include" -I"src/usb" -I"src/miwi" --warn=0 --asmlist -DXPRJ_18F27J53=$(CND_CONF)  --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,+plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"     $(COMPARISON_BUILD) --memorysummary dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml -odist/${CND_CONF}/${IMAGE_TYPE}/dev_18F27J53_operator_basecamp.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}     
	
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/18F27J53
	${RM} -r dist/18F27J53

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
