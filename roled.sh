#! /bin/bash
clear
cd /home/yash/Documents/ITSP\ 2k17
echo "               Welcome to the                   "
echo "      RoLED Display Console Interface.          "
echo
echo "We shall start with Monochrome Display section."

# Compiling the C++ program during run time
 g++ imgproc_monochrome.cpp -o mono `pkg-config --libs --cflags opencv` 
#g++ imgproc_rec2circular.cpp -o mono `pkg-config --libs --cflags opencv` 

while true
do
	# Executing the C++ program
	./mono

	clear
	echo "Data transference begins..."
	sleep 2
	
	# # Refreshing the arduino
	# python refresh.py
	
	# Executing the python script
	python data_transfer.py # Includes code for refreshing
	clear
	echo "Want to send new data? (y/n)"
	read ans
	if ["$ans" != "y"]; then
		break
	else
		echo "Ok"
	fi

done

exit
