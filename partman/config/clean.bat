 @echo off
 if exist debug0\. rd debug0 /s /q
 if exist debug1\. rd debug1 /s /q
 if exist settings\. rd settings /s /q
 if exist partman*.ffd del partman*.ffd
 if exist *.dep del *.dep
