
MAKE=make

all:
	$(MAKE) /f RasDialPro.mak CFG="RasDial95 - Win32 Release"	

	cd Register
	$(MAKE) /f Register.mak CFG="Register - Win32 Release"
	cd ..

	package.cmd