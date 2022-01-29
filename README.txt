============================== Introduction
This file shows how to build the DIR-853 Firmware

============================== Environment 
Sample workstation to build the image
	OS Version: 		Debian 10.6.0 (32-bit version)
	OS Kernel Version:	4.19.0-13-686
	GCC Version:		8.3.0 (Debian 8.3.0-6)
	note: other Linux distributions may have no guarantee of a successful build	
	
Setup Environment:
	1. Make sure you can connect to the Internet.
	2. Download and install Debian 10.6.0 with all packages.
	3. Install additional packages:
		a) #su (enter your root password to become root)
		b) #apt update
		c) #apt install -y wget git gcc g++ make autoconf automake autopoint gettext libtool pkg-config bison flex bc python gawk procps zlib1g-dev locales texinfo
		d) #exit (you don't need a root access anymore)
	4. Prepare a directory to build the DIR-853 firmware:
		a) #cd ~/ (go to your home directory)
		b) Copy gpl_DIR_853_MT7621AT.zip to your home directory
		c) #unzip gpl_DIR_853_MT7621AT.zip
		Now you have two new files in your home directory: gpl_DIR_853_MT7621AT.tar.gz and MD5.txt
		d) #md5sum gpl_DIR_853_MT7621AT.tar.gz (to calculate checksum)
		Compare a calculated checksum with checksum from file MD5.txt. They must be the same.
		e) #tar xvf gpl_DIR_853_MT7621AT.tar.gz
		Now you have a directory gpl_DIR_853_MT7621AT in your home directory. LICENSE.txt and Readme File are in the root of gpl_DIR_853_MT7621AT direcrory.

============================== Firmware 	
Build Firmware:
	Please follow the procedures below:
	1. #cd ~/gpl_DIR_853_MT7621AT
	2. #make
	You will get the image file: XXXXYYZZ_HHMM_DIR_853_MT7621AT_develop_2.58.0_sdk-dsysinit.bin in ~/gpl_DIR_853_MT7621AT/output/images
	Where XXXXYYZZ and HHMM - current date and time.
   	3. You got your specific image now. Login to the web-interface of your device and upload the compiled image.
