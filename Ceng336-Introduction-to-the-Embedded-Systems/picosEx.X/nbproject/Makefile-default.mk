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
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=mkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/picosEx.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/picosEx.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=example/common.c example/int.c example/main.c example/taskdesc.c example/tsk_task0.c example/tsk_task1.c pic_hash_compute.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/example/common.o ${OBJECTDIR}/example/int.o ${OBJECTDIR}/example/main.o ${OBJECTDIR}/example/taskdesc.o ${OBJECTDIR}/example/tsk_task0.o ${OBJECTDIR}/example/tsk_task1.o ${OBJECTDIR}/pic_hash_compute.o
POSSIBLE_DEPFILES=${OBJECTDIR}/example/common.o.d ${OBJECTDIR}/example/int.o.d ${OBJECTDIR}/example/main.o.d ${OBJECTDIR}/example/taskdesc.o.d ${OBJECTDIR}/example/tsk_task0.o.d ${OBJECTDIR}/example/tsk_task1.o.d ${OBJECTDIR}/pic_hash_compute.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/example/common.o ${OBJECTDIR}/example/int.o ${OBJECTDIR}/example/main.o ${OBJECTDIR}/example/taskdesc.o ${OBJECTDIR}/example/tsk_task0.o ${OBJECTDIR}/example/tsk_task1.o ${OBJECTDIR}/pic_hash_compute.o

# Source Files
SOURCEFILES=example/common.c example/int.c example/main.c example/taskdesc.c example/tsk_task0.c example/tsk_task1.c pic_hash_compute.c


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
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/picosEx.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F8722
MP_PROCESSOR_OPTION_LD=18f8722
MP_LINKER_DEBUG_OPTION= -u_DEBUGCODESTART=0x1fd30 -u_DEBUGCODELEN=0x2d0 -u_DEBUGDATASTART=0xef4 -u_DEBUGDATALEN=0xb
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/example/common.o: example/common.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/example" 
	@${RM} ${OBJECTDIR}/example/common.o.d 
	@${RM} ${OBJECTDIR}/example/common.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"PICos18_v2_10/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/example/common.o   example/common.c 
	@${DEP_GEN} -d ${OBJECTDIR}/example/common.o 
	@${FIXDEPS} "${OBJECTDIR}/example/common.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/example/int.o: example/int.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/example" 
	@${RM} ${OBJECTDIR}/example/int.o.d 
	@${RM} ${OBJECTDIR}/example/int.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"PICos18_v2_10/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/example/int.o   example/int.c 
	@${DEP_GEN} -d ${OBJECTDIR}/example/int.o 
	@${FIXDEPS} "${OBJECTDIR}/example/int.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/example/main.o: example/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/example" 
	@${RM} ${OBJECTDIR}/example/main.o.d 
	@${RM} ${OBJECTDIR}/example/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"PICos18_v2_10/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/example/main.o   example/main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/example/main.o 
	@${FIXDEPS} "${OBJECTDIR}/example/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/example/taskdesc.o: example/taskdesc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/example" 
	@${RM} ${OBJECTDIR}/example/taskdesc.o.d 
	@${RM} ${OBJECTDIR}/example/taskdesc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"PICos18_v2_10/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/example/taskdesc.o   example/taskdesc.c 
	@${DEP_GEN} -d ${OBJECTDIR}/example/taskdesc.o 
	@${FIXDEPS} "${OBJECTDIR}/example/taskdesc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/example/tsk_task0.o: example/tsk_task0.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/example" 
	@${RM} ${OBJECTDIR}/example/tsk_task0.o.d 
	@${RM} ${OBJECTDIR}/example/tsk_task0.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"PICos18_v2_10/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/example/tsk_task0.o   example/tsk_task0.c 
	@${DEP_GEN} -d ${OBJECTDIR}/example/tsk_task0.o 
	@${FIXDEPS} "${OBJECTDIR}/example/tsk_task0.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/example/tsk_task1.o: example/tsk_task1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/example" 
	@${RM} ${OBJECTDIR}/example/tsk_task1.o.d 
	@${RM} ${OBJECTDIR}/example/tsk_task1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"PICos18_v2_10/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/example/tsk_task1.o   example/tsk_task1.c 
	@${DEP_GEN} -d ${OBJECTDIR}/example/tsk_task1.o 
	@${FIXDEPS} "${OBJECTDIR}/example/tsk_task1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/pic_hash_compute.o: pic_hash_compute.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/pic_hash_compute.o.d 
	@${RM} ${OBJECTDIR}/pic_hash_compute.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -I"PICos18_v2_10/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/pic_hash_compute.o   pic_hash_compute.c 
	@${DEP_GEN} -d ${OBJECTDIR}/pic_hash_compute.o 
	@${FIXDEPS} "${OBJECTDIR}/pic_hash_compute.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
else
${OBJECTDIR}/example/common.o: example/common.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/example" 
	@${RM} ${OBJECTDIR}/example/common.o.d 
	@${RM} ${OBJECTDIR}/example/common.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"PICos18_v2_10/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/example/common.o   example/common.c 
	@${DEP_GEN} -d ${OBJECTDIR}/example/common.o 
	@${FIXDEPS} "${OBJECTDIR}/example/common.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/example/int.o: example/int.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/example" 
	@${RM} ${OBJECTDIR}/example/int.o.d 
	@${RM} ${OBJECTDIR}/example/int.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"PICos18_v2_10/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/example/int.o   example/int.c 
	@${DEP_GEN} -d ${OBJECTDIR}/example/int.o 
	@${FIXDEPS} "${OBJECTDIR}/example/int.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/example/main.o: example/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/example" 
	@${RM} ${OBJECTDIR}/example/main.o.d 
	@${RM} ${OBJECTDIR}/example/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"PICos18_v2_10/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/example/main.o   example/main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/example/main.o 
	@${FIXDEPS} "${OBJECTDIR}/example/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/example/taskdesc.o: example/taskdesc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/example" 
	@${RM} ${OBJECTDIR}/example/taskdesc.o.d 
	@${RM} ${OBJECTDIR}/example/taskdesc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"PICos18_v2_10/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/example/taskdesc.o   example/taskdesc.c 
	@${DEP_GEN} -d ${OBJECTDIR}/example/taskdesc.o 
	@${FIXDEPS} "${OBJECTDIR}/example/taskdesc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/example/tsk_task0.o: example/tsk_task0.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/example" 
	@${RM} ${OBJECTDIR}/example/tsk_task0.o.d 
	@${RM} ${OBJECTDIR}/example/tsk_task0.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"PICos18_v2_10/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/example/tsk_task0.o   example/tsk_task0.c 
	@${DEP_GEN} -d ${OBJECTDIR}/example/tsk_task0.o 
	@${FIXDEPS} "${OBJECTDIR}/example/tsk_task0.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/example/tsk_task1.o: example/tsk_task1.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/example" 
	@${RM} ${OBJECTDIR}/example/tsk_task1.o.d 
	@${RM} ${OBJECTDIR}/example/tsk_task1.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"PICos18_v2_10/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/example/tsk_task1.o   example/tsk_task1.c 
	@${DEP_GEN} -d ${OBJECTDIR}/example/tsk_task1.o 
	@${FIXDEPS} "${OBJECTDIR}/example/tsk_task1.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/pic_hash_compute.o: pic_hash_compute.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/pic_hash_compute.o.d 
	@${RM} ${OBJECTDIR}/pic_hash_compute.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"PICos18_v2_10/Include" -ms -oa-  -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/pic_hash_compute.o   pic_hash_compute.c 
	@${DEP_GEN} -d ${OBJECTDIR}/pic_hash_compute.o 
	@${FIXDEPS} "${OBJECTDIR}/pic_hash_compute.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/picosEx.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  PICos18_v2_10/Kernel/picos18.lib  PICos18_v2_10/Linker/18f8722.lkr
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) "PICos18_v2_10/Linker/18f8722.lkr"  -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG -m"${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map" -l"PICos18_v2_10/Kernel"  -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_PK3=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}/../lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/picosEx.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}  PICos18_v2_10/Kernel/picos18.lib 
else
dist/${CND_CONF}/${IMAGE_TYPE}/picosEx.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  PICos18_v2_10/Kernel/picos18.lib PICos18_v2_10/Linker/18f8722.lkr
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) "PICos18_v2_10/Linker/18f8722.lkr"  -p$(MP_PROCESSOR_OPTION_LD)  -w  -m"${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map" -l"PICos18_v2_10/Kernel"  -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}/../lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/picosEx.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}  PICos18_v2_10/Kernel/picos18.lib 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell "${PATH_TO_IDE_BIN}"mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
