#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=clang
CCC=clang++
CXX=clang++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=Clang-Linux
CND_DLIB_EXT=so
CND_CONF=clang_Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/aes.o \
	${OBJECTDIR}/context.o \
	${OBJECTDIR}/ebno.o \
	${OBJECTDIR}/image.o \
	${OBJECTDIR}/intrlv.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/modmap.o \
	${OBJECTDIR}/trellis.o \
	${OBJECTDIR}/ttcm.o \
	${OBJECTDIR}/utils.o \
	${OBJECTDIR}/uttcm.o


# C Compiler Flags
CFLAGS=-m64 -fstack-protector-all

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/latest

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/latest: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	clang -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/latest ${OBJECTFILES} ${LDLIBSOPTIONS} -lm -lopenjp2

${OBJECTDIR}/aes.o: aes.c nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Werror -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/aes.o aes.c

${OBJECTDIR}/context.o: context.c nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Werror -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/context.o context.c

${OBJECTDIR}/ebno.o: ebno.c nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Werror -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ebno.o ebno.c

${OBJECTDIR}/image.o: image.c nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Werror -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/image.o image.c

${OBJECTDIR}/intrlv.o: intrlv.c nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Werror -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/intrlv.o intrlv.c

${OBJECTDIR}/main.o: main.c nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Werror -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/modmap.o: modmap.c nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Werror -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/modmap.o modmap.c

${OBJECTDIR}/trellis.o: trellis.c nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Werror -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/trellis.o trellis.c

${OBJECTDIR}/ttcm.o: ttcm.c nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Werror -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ttcm.o ttcm.c

${OBJECTDIR}/utils.o: utils.c nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Werror -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils.o utils.c

${OBJECTDIR}/uttcm.o: uttcm.c nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -Werror -std=c11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/uttcm.o uttcm.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
