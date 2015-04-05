adk-driver
=======
Kernel driver to set device into ADK (Accessory Development Kit) mode. 


Support
--------------
Supports nearly 8000+ devices. Check list of supported devices in devices.h file. Support is added for each Manufacturer => Device is supported if the manufacturer is in the list.


Note
---------------
If you are planning on writing another independent driver which works after device enters ADK mode, you should uncomment Google from device table and add each Google devices individually as shown for Nexus 10, as an example. This is because all Android devices use Google's vendor id as vendor id in ADK mode. This will clash with your independent driver's device table.


Author
---------------
Praveen Kumar Pendyala
http://praveen.xyz


License
---------------
Licensed under GPL v2 

