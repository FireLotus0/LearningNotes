#!/bin/bash

if [ $UID -ne 0]; then
	echo Non root user. Pleaserun as root.
else 
	echo Root user.
fi
