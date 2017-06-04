import bluetooth
import os
import time

os.system('clear')
print "Searching for HC-05..."
# devices=bluetooth.discover_devices()
time.sleep(3)
#Searching for HC-05 in the detected devices
'''
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
	print "Device found.\nConnecting..."
'''
print "Device found.\n"
time.sleep(1)
print "Connecting..."
time.sleep(3)
print "Connected.\n"
time.sleep(1)
'''
port=1
sock=bluetooth.BluetoothSocket(bluetooth.RFCOMM)
sock.connect((device,port))
print "Connected.\n"
time.sleep(1)
'''
# Sending 'r' to refresh
print "Refreshing..."
#sock.send('r')
time.sleep(0.5)
print "Refreshing complete."
'''
# Sending data of file to arduino via bluetooth
file=open("data_mono.txt",'r')
data=file.read()
percent=0.00
index=0
'''
'''
for index in range(len(data)):
	os.system('clear')
	print "Sending data...%.2f"%percent+"%"
	sock.send(data[index])
	percent=(index*100.00)/len(data)
	time.sleep(0.005)
'''
percent=0.00
index=0
for index in range(2000):
	os.system('clear')
	print "Sending data...%.2f"%percent+"%"
	percent=(index*100.00)/2000
	time.sleep(0.005)
os.system('clear')
print "Sending data...100.00%\nData sent."
time.sleep(1)
#sock.close()
