# GPS_PositionTrack_GoogleEarth
This program demonstrates the working of uBlox Neo-6M GPS module. The microcontroller used for this project is STM32F103C8 Bluepill Development Board.</br>
-------------------------------------------------------------------
The UART1 peripheral is initialized and GPS module such as whatever data the GPS module sends to the microcontroller over UART1 TX is processed by </br>
the microcontroller and is forwarded to the Computer thru the TTL serial debugger. The Microcontroller collects the data from the GPS module and formats it into</br>
the standard KML format which [Google Earth](https://www.google.com/intl/en_in/earth/) software can understand.
You can read more about GPS NMEA format and how to convert into KML format in this fantastic blog written by Mr.Paul McWhorter. [Link](https://toptechboy.com/lesson-25-display-your-gps-data-as-track-on-google-earth/)

This Software Code uses Direct Register Level Programming to keep the code size less.  </br>

You can read more about the concept of Ring Buffer [here](https://en.wikipedia.org/wiki/Circular_buffer).

-------------------------------------------------------------------
UART Specifications :</br>
UART1_TX -> PA9</br>
UART1-RX -> PA10</br>
UART_Baud Rate -> 115200</br>
System Clock -> Default Clock setting of STM32F103C8</br>

-------------------------------------------------------------------
This program is written in KEil µVision5.17.00 IDE
