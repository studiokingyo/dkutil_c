ECHO / dKingyoUtility3 distribute processing

dacekeadgar_copy -copy -nosw Release\*.lib  dKingyoUtility3\Distribute
dacekeadgar_copy -copy -nosw ReleaseMT\*.lib dKingyoUtility3\Distribute 
dacekeadgar_copy -copy -nosw Release\*.dll  dKingyoUtility3\Distribute
dacekeadgar_copy -copy -nosw ReleaseMT\*.dll dKingyoUtility3\Distribute 

dacekeadgar_copy -copy -nosw Release\*.dll  C:\WINNT
dacekeadgar_copy -copy -nosw ReleaseMT\*.dll C:\WINNT

dacekeadgar_copy -copy -nosw ..\*.h dKingyoUtility3\include


dacekeadgar_copy -copy -nosw .\dKingyoUtility3.h dKingyoUtility3\include


dacekeadgar_copy -copy -nosw Release\*.map  dKingyoUtility3\Data
dacekeadgar_copy -copy -nosw ReleaseMT\*.map dKingyoUtility3\Data 

dacekeadgar_copy -copy -nosw .\*.def dKingyoUtility3\Data 

dacekeadgar_copy -copy -nosw ..\dkutil_cstd\*.h dKingyoUtility3\include\dkutil_cstd\


