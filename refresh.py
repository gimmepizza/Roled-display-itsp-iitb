import bluetooth
import os
import time

devices=bluetooth.discover_devices()

#Searching for HC-05 in the detected devices
hc05_found=False

for device in devices:
	device_name=bluetooth.lookup_name(device)
	if device_name=="HC-05\n":
		hc05_found=True
		break

if not hc05_found:
	print "Required device not found.\nPlease check your connections and try again."
	exit()
else:
	print "Device found.\nRefreshing..."

port=1
sock=bluetooth.BluetoothSocket(bluetooth.RFCOMM)
sock.connect((device,port))
sock.send('r')
print "Refreshing complete."
sock.close()
