#!../../bin/linux-arm/sht21ioc

## You may have to change sht21ioc to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/sht21ioc.dbd"
sht21ioc_registerRecordDeviceDriver pdbbase

## Load record instances
dbLoadRecords("db/sht21.db","user=PXD")

cd ${TOP}/iocBoot/${IOC}
iocInit

## Start any sequence programs
#seq sncxxx,"user=piHost"
