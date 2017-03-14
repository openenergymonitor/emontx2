# emonTx V2 Build Guide

# emonTx V2.2 Build Guide

If you run into problems please post on the [forum](https://community.openenergymonitor.org).

The PCB is built in an orderly sequence starting with the resistors and small components that are low on the board, then working up to the bulkier parts like connectors. After soldering each component, you need to cut off the excess component leg from the bottom using some cutters.

It is very easy to cause shorts which will make the board or specific parts of it not work, take your time during the build, be precise, use minimal solder and carefully check each joint after soldering it. If you suspect any short, check it with a multimeter and fix any shorts there and then, before continuing.

If you're new to soldering or looking to improve your technique we highly recommend these videos:

*   [EEVBlog Soldering Tutorial - Part 1: Tools](http://www.eevblog.com/2011/06/19/eevblog-180-soldering-tutorial-part-1-tools)
*   [EEVBlog Soldering Tutorial - Part 2: Soldering](http://www.eevblog.com/2011/07/02/eevblog-183-soldering-tutorial-part-2)
*   [EEVBlog Soldering Tutorial - Part 3: Surface Mount](http://www.eevblog.com/2011/07/18/eevblog-186-soldering-tutorial-part-3-surface-mount)

It's recommended you read through the whole of this guide and familiarise yourself with the steps before you start the actual build.

## Step 1 - Identify Components and Kit Contents

Lay out and identify the components from the kit.



![](https://lh5.googleusercontent.com/-q5G260Gvm4w/T15U7MgaDwI/AAAAAAAAAxY/Od6ju_zGggg/s640/DSC_0003.JPG)

Here are the components required to build your emonTx:

*   The emonTx PCB
*   3x 18 ohm 1% Resistors (brown, grey, black, gold, brown on blue)
*   2x 10k 1% Resistors (brown, black, black, red, brown on blue)
*   1x 100k 1% Resistor (brown, black, black, orange, brown on blue)
*   8x 470k Resistors (yellow, violet, yellow, gold)
*   1x 4k7 Resistor (yellow, violet, red, gold)
*   1x 100 ohm Resistor (brown, black, brown, gold)
*   4x 100nF (0.1uF) Ceramic Capacitors
*   5x 10uF Electrolytic Capacitors
*   2x 22pF Ceramic Capacitors
*   1x Atmel ATMega328 - the main microcontroller
*   1x 28-Pin DIL Socket
*   1x RFM12B Radio Module - available in 433Mhz or 868Mhz versions
*   1x 165mm (433Mhz) or 82mm (868Mhz) Wire for RFM12B Antenna
*   1x 16Mhz Crystal - for the ATMega328.
*   1x Mini B USB Socket
*   1x Microchip MCP1702 3.3v Voltage Regulator
*   1x 3mm Green LED
*   1x 2.1mm Power Socket
*   5x 3.5mm Jack Sockets
*   1x 6pin Header (FTDI port)


![](https://lh6.googleusercontent.com/-GumIcdcX-XU/T15VIxJAWCI/AAAAAAAAA0M/fELblarGrTs/s640/DSC_0042.JPG)


_Note: the photo shows two different types of the 22pF capacitors - brown ceramic disk (labeled 22 and with black tops) and small blue ones (labelled 22) - only one of these pairs are required/included. They are equivalent._

Ok, we're ready to start. Here is the PCB:

![](https://lh6.googleusercontent.com/-nl6i1eFLrfk/T15VA8vs3vI/AAAAAAAAAyk/artDIY1ddZM/s640/DSC_0030.JPG)

![](https://lh6.googleusercontent.com/-osqW8BOtLaY/T15VAX2S3iI/AAAAAAAAA8E/27M24JqcGO0/s640/DSC_0029.JPG)

If not already marked, we recommend you place a mark in the correct box to the rear of the PCB to indicate the operating frequency of the installed RFM12B, as it can be very difficult to determine, later!

This is where everything should go:

![](https://lh6.googleusercontent.com/-9DmuVRodcG8/T15el6ngApI/AAAAAAAAA_g/6BOnT9eEH_M/s640/emonTxV2.2_PCBvalues.png)

_Please note: contrary to the diagram above, and the photo below, we do not fit the 10k resistor under the pulse counting socket at the top of the board, as this can cause problems._

![](http://openenergymonitor.org/emon/sites/default/files/emonTx_V2.0%20overview.png)

_Please note: the above photo is from an older version of the board and therefore is slightly different (eg: some of the resistor values have changed) - it's just here to show the various parts of the board._

## Step 2 - 18 ohm Resistors

Start with the resistors. Form a 90 degree bend in each leg of the resistor right next to the body. It's recommended that you align them all in the same direction.

First we add the 3x 18 ohm resistors - they are blue and their colour code is: Brown, Grey, Black, Gold, Brown.


![](https://lh4.googleusercontent.com/-gRuQEVx-Kg8/T15VLzru_pI/AAAAAAAAA0k/xN13Rgrq4xg/s640/DSC_0049.JPG)


## Step 3 - 10k Resistors

Add the 2x 10k resistors.

Unlike the picture below, these are usually cream coloured and with Brown, Black, Orange stripes.

(If your 10k resistors are blue, their colour code is: Brown, Black, Black, Red, Brown - but these are not usually shipped with the kit)
![](https://lh3.googleusercontent.com/-kkkJdBxpf2M/T15VM-zJnwI/AAAAAAAAA00/RhQi8n1HHWY/s640/DSC_0051.JPG)

_Note: the diagram above shows a 3rd 10k resistor - under the pulse counting input (R8). This is an optional pull down resistor for the pulse counting interrupt input/general digital I/O. More often than not it's not needed so is not included in the kit as if fitted, it will do more harm than good. e.g if a 10K resistor is soldered into R8 an optical sensor plugged into this port to detect LED pulses from pulse output meters won't work._

_R8 is useful only when connecting directly (via wire) to the pulse output from a utility meter, we're not recommending this method since it's not galvanically isolated, and to connect the wires, you have to work in close proximity to high voltage, not ideal. Optical pulse detection is much safer._

[See pulse counting section of building blocks for more information](http://openenergymonitor.org/emon/buildingblocks)

## Step 4 - 100k Resistor

Add the 100k resistor - it's blue, and its colour code is: Brown, Black, Black, Orange, Brown.
![](https://lh4.googleusercontent.com/-RZbvptWdX8I/T15VQA5JJKI/AAAAAAAAA1c/ViJDb8Xxvd0/s640/DSC_0055.JPG)

## Step 5 - 470k Resistors

Add the 8x 470k resistors - their colour code is: Yellow, Violet, Yellow, Gold.
![](https://lh5.googleusercontent.com/-47cV5Lh4hJ4/T15VUWvZ4nI/AAAAAAAAA2E/76IErrpLy9c/s640/DSC_0061.JPG)]

## Step 6 - 4k7 Resistor

Add the 4k7 resistor - its colour code is: Yellow, Violet, Red, Gold.

![](https://lh6.googleusercontent.com/-9-EBvyv3MM4/T15VWnmNFUI/AAAAAAAAA2c/GFyPzWhCvQM/s640/DSC_0064.JPG)]


## Step 7 - 100 ohm Resistor

Add the 100 ohm resistor - its colour code is: Brown, Black, Brown, Gold.


![](https://lh3.googleusercontent.com/-O_CxUeWUZyw/T15VXFBkv8I/AAAAAAAAA2o/EsWBFZQ3qd0/s640/DSC_0065.JPG)
## Step 8 - 22pF Ceramic Capacitors

Add the 2x 22pF ceramic capacitors - they are either blue (as per the photo) or small brown discs with black tips.

![](https://lh5.googleusercontent.com/-9Er30Zoj32w/T15VYhiopAI/AAAAAAAAA28/RF_jmKzqlvM/s640/DSC_0068.JPG)
## Step 9 - 100nF Ceramic Capacitors

Add the 4x 100nF ceramic capacitors - they are labelled 104.

![](https://lh4.googleusercontent.com/-MsQj1lyBUP8/T15Vao3nHPI/AAAAAAAAA3U/Vkrzxi2UPfI/s640/DSC_0071.JPG)]

## Step 10 - 10uF Electrolytic Capacitors

Add the 5x 10uF electrolytic capacitors. These must be inserted into the PCB holes the correct way round. The PCB is marked with a + symbol to indicate the positive lead. The capacitors are marked with a white stripe and a short leg to indicate the negative lead and a longer leg to indicate the positive lead.

![](https://lh6.googleusercontent.com/-A6jdz_PDjeY/T15VcueZr1I/AAAAAAAAA3w/7SyeFfH2Oao/s640/DSC_0075.JPG)

## Step 11 - 16Mhz Crystal

Add the 16Mhz crystal - it can be inserted into the PCB holes in either direction.

![](https://lh3.googleusercontent.com/-JRHDBTGz2vQ/T15VhMn4oyI/AAAAAAAAA4c/28m-SB39ah0/s640/DSC_0080.JPG)

## Step 12 - Green LED

Add the green LED. The PCB is marked with a + symbol to indicate positive. The LED has a short leg (and flat edge) to indicate the negative leg and a longer leg to indicate the positive leg.

![](https://lh6.googleusercontent.com/-tWbCNrN9LyU/T15VizG4OPI/AAAAAAAAA8M/616s8CFvRFQ/s640/DSC_0082.JPG)]

## Step 13 - MCP1702 3.3v Voltage Regulator

Add the MCP1702 3.3v Voltage Regulator - the flat edge goes towards the bottom of the board as per the outline on the PCB.

![](https://lh4.googleusercontent.com/-mHbxkBLkjRo/T15VkquHVjI/AAAAAAAAA5I/V6nBB7tf6Lw/s640/DSC_0085.JPG)]

## Step 14 - Mini USB Socket

Add the mini USB socket.

![](https://lh3.googleusercontent.com/-p7OTog1g8Xw/T15Vl25BKKI/AAAAAAAAA5M/m36g_MtAnIc/s640/DSC_0087.JPG)]


## Step 15 - 3.5mm Sockets

Add the 5x 3.5mm sockets.

![](https://lh6.googleusercontent.com/-mwwH578Qd8g/T15VnfsSuxI/AAAAAAAAA5g/oHkvI-ZNGaM/s640/DSC_0089.JPG)]

## Step 16 - 2.1mm Power Socket

I added the 2.1mm power socket next, but it would actually have been better to **add this after the DIL socket (next step)** as the DIL socket will sit flush on the desk while you solder it.

![](https://lh5.googleusercontent.com/-Be4Uvy5EOHw/T15VpuicpSI/AAAAAAAAA58/wihyKUWryRA/s640/DSC_0092.JPG)]


## Step 17 - DIL Socket

Add the DIL socket. If you are observant, you will notice that I put it the wrong way around on the photos! - the notch on the socket should go to the left of the board, as per the outline on the

PCB.

![](https://lh6.googleusercontent.com/-XdFLRDO8aMw/T15Vquyh5KI/AAAAAAAAA6I/IflMAMPyJlY/s640/DSC_0094.JPG)]

## Step 18 - Programming Header

Add the 6-pin programing header at the bottom of the board.  Solder one pin first so you can adjust the alignment if you don't get it at right angles to the board first time.

![](https://lh6.googleusercontent.com/-h296kTDcPl4/T15VxMwPnwI/AAAAAAAAA64/FYSNoQ0nkmY/s640/DSC_0100.JPG)]

## Step 19 - Voltage Check

We now need to ensure the voltage is correct before adding any of the expensive components. Power the board with either an FTDI programming or a mini USB cable and measure the voltage between the GND and VCC pads on the RFM12B socket. You should get 3.3 V. If you get more than this, you should correct the issue before continuing.

[![](https://lh3.googleusercontent.com/-EdUjbIdRh6g/T15VyX7XAfI/AAAAAAAAA68/K2LKu6QYOh8/s640/DSC_0101.JPG)

## Step 20 - RFM12B

Add the RFM12B module. You can see the shape of the crystal on the PCB. Position the module in place over the pads and gently apply heat and solder the opposite corners to hold it in place so you can solder the remaining pads.


![](https://lh6.googleusercontent.com/-p9d8dQe0BsY/T15V0JTD_2I/AAAAAAAAA7Q/1P4YifflXrs/s640/DSC_0103.JPG)
## Step 21 - Antenna

Add the antenna by soldering the end to the bottom right pad on the RFM12B module.

![](https://lh5.googleusercontent.com/-oMoijk7lrfQ/T15V2kHkJ9I/AAAAAAAAA7s/fxTqxS7hcEI/s640/DSC_0107.JPG)

Finally, check the voltage again (as in step 19). You should still read 3.3 V. If you read significantly less than this, ensure you have soldered the RFM12B module correctly.

## Completed Board

We can now add the ATMega328 chip. You will need to bend the pins in slightly by resting the chip sideways on the desk and rolling it forwards to bend the pins in. Do the same with the other side.

![](https://lh4.googleusercontent.com/-xxlcQzlJRXg/T15WFTHA6BI/AAAAAAAAA-8/s4LLu955HaY/s640/DSC_0130.JPG)

![](https://lh4.googleusercontent.com/-K_LHzGwMAgI/T15V5s8vSqI/AAAAAAAAA8Y/zuv8XjhLm5M/s640/DSC_0111.JPG)

![](https://lh6.googleusercontent.com/-RlacQkkC6x8/T15WGcUaXoI/AAAAAAAAA-4/yR2pLwZloYY/s640/DSC_0132.JPG)]

## Voltage Selection Notes

![](http://openenergymonitor.org/emon/sites/default/files/Pulse%20Vcc.JPG)

On the latest version of the emonTx PCB, two solder jumpers have been added near the IRQ I/O and temperature (DS18B20) ports. They enable the user to decide what voltage to use on these ports. The emonTx runs at 3.3V. More details can be found in [this blog post](http://openenergymonitor.blogspot.com/2011/12/new-emontx-pcb.html). Connection is made by joining the middle pad the either of the outer pads with solder.

If you look carefully, the solder jumper above the temperature port is connected to 5V (small PCB trace). This was an error, but actually work out well. The temperature sensors always use 5v (which is good) and when the emonTx is powered by 2 x AA's the the 5V rail becomes 3.3V anyway. Therefore, you can ignore this solder jumper, it will disappear in the next generation of emonTx PCBs. However the pulse counting voltage selection jumper (pictured) above must be connected to either 3.3V/5V.

See [DS18B20 temperature sensing](http://openenergymonitor.org/emon/buildingblocks/DS18B20-temperature-sensing) section of building blocks for more information on temperature sensing.

## Firmware Upload

The ATmega328 chip is preloaded with the Arduino Optiboot 4.4 bootloader (select Uno in IDE) but no sketch. The correct sketch must be uploaded using the Arduno IDE.

emonTx Arduino firmware can be downloaded from: [https://github.com/openenergymonitor/emonTxFirmware.](https://github.com/openenergymonitor/emonTxFirmware)

The EmonTx v2 firmware examples are in the EmonTx v2 folder. There is a github readme that lists all the examples and the Arduino libraries required here:

[https://github.com/openenergymonitor/emonTxFirmware/tree/master/emonTxV2](https://github.com/openenergymonitor/emonTxFirmware/tree/master/emonTxV2)

The best firmware for power monitoring with a CT sensor and an AC-AC Adapter for voltage measurement is the **emonTx_CT123_Voltage** example.

## Connections

See emonTx [Design Reference](http://openenergymonitor.org/emon/emontx/reference%20) page.

## Testing

Compare your results with the [EmonTX accuracy test](accuracy.md)


