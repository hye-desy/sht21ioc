TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
# Build the IOC application

PROD_IOC = sht21ioc
# sht21ioc.dbd will be created and installed
DBD += sht21ioc.dbd

# sht21ioc.dbd will be made up from these files:
sht21ioc_DBD += base.dbd
sht21ioc_DBD += sht21dev.dbd

# Include dbd files from all support applications:
#sht21ioc_DBD += xxx.dbd

# Add all the support libraries needed by this IOC
#sht21ioc_LIBS += xxx

# sht21ioc_registerRecordDeviceDriver.cpp derives from sht21ioc.dbd
sht21ioc_SRCS += sht21ioc_registerRecordDeviceDriver.cpp
sht21ioc_SRCS += device.cc
sht21ioc_SRCS += gpios.cc
sht21ioc_SRCS += result.cc
sht21ioc_SRCS += i2c.cc
sht21ioc_SRCS += sht21.cc

# Build the main IOC entry point on workstation OSs.
sht21ioc_SRCS_DEFAULT += sht21iocMain.cpp
sht21ioc_SRCS_vxWorks += -nil-

USR_CXXFLAGS += -std=c++11 -O3 -Wall

# Add support from base/src/vxWorks if needed
#sht21ioc_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
sht21ioc_LIBS += $(EPICS_BASE_IOC_LIBS)

sht21ioc_SYS_LIBS += pthread

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

