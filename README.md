# Record Player

This is a miniature record player made using an Arduino ESP32 and various electronic components. The following is a dev log of my progress through this project, from the initial idea all the way to the final product (which has yet to be made).

<img width="2048" height="1536" alt="IMG_0939" src="https://github.com/user-attachments/assets/7f1136ac-f2fc-46e5-81c3-a5859a36b5ea" /> <h6> The first prototype of the record player </h6>

# Ideation 
The initial idea came from [this](https://www.youtube.com/watch?v=fBjv4E7mpA4) video, in which AKZ Dev created a mini record player that connected to Spotify. He used an existing record coaster set and designed a case in CAD software to hold the electronics below it. I saw this and thought it would be a fun project, but I wanted to make it entirely local. Instead of having it play through Spotify, I wanted it to be able to play songs off of a mini-SD card through a speaker mounted in the front. This is where the project began.

Note: I didn't start taking photos of my work until the initial prototype was done, so there is going to be a lot of text at the start of this.

# Research & Parts

I started by researching all of the parts needed, most of which were provided by AKZ Dev. He also provided a CAD file for the case, which was free of charge. Huge thanks to him; this project would have been significantly more difficult without his resources. However, there was one change that I made to the core design. He used a Raspberry Pi Zero as the central microprocessor. I wanted to do the same, as it would allow me to use his code and have an easy starting place from which I could make modifications. 

I ran into an issue, however: I couldn't find one. Anywhere. I looked on every single reseller in the United States and on forums like Reddit, and everyone was saying that it was nearly impossible to get one now. So I pivoted and started looking for other solutions. 

I searched through the comments of the YouTube video to see if anyone else had this problem, and I saw that one user had suggested using an ESP32 in place of a Raspberry Pi Zero, as it would perform the same. I started researching this possibility and found what I ultimately decided on using: an Arduino ESP32 Breakout Board. It is more expensive than a traditional ESP32, but the advantage is that it allows me to use the Arduino ecosystem, which I was already familiar with. I also looked into several other parts I would need, and setteled on the following Bill of Materials.

# Bill of Materials

| Part Name | Amazon Link |
| --------- | ----------- |
| Record Coaster Set | https://www.amazon.com/dp/B092S8WYXS |
| RC522 RFID Reader	| https://www.amazon.com/dp/B07KGBJ9VG |
| Stepper Motor & Driver | https://www.amazon.com/dp/B01CP18J4A |
| A3144 Hall Effect Sensor | https://www.amazon.com/dp/B0FB8P22H4 |
| MAX98357 Amplifier | https://www.amazon.com/dp/B0B4J93M9N |
| 4Ohm 40mm Speaker | https://www.amazon.com/dp/B01LN8ONG4 |
| MP1584EN 5V Buck Converter | https://www.amazon.com/dp/B0B779ZYN1 |
| ADA254 SD Card Reader | https://www.amazon.com/dp/B00NAY2NAI |
| Breadboards | https://www.amazon.com/dp/B0CYPVMK9J |
| Dupont Wires | https://www.amazon.com/dp/B01EV70C78 |
| NFC Stickers | https://www.amazon.com/dp/B07GFHLZD1 |

# Inital Prototype

Once I had all the parts, I got to work making the initial version. I started by making it all on a breadboard, as it made tracing and debugging connections significantly easier. I went component-by-component, making sure each part worked the way I wanted before adding another and connecting them togtether. For the most part, this went smoothly. 

I started with the NFC reader, providing it power and making sure it was correctly identifiying the NFC stickers. It used the I2C communication protocol, which I will admit I don't fully understand. However, I read the spec sheet on the reader and looked into I2C for the ESP32, and learned that I had to connect it to specific pins in order for it to communicate properly.

The next component was the stepper motor. This one was a bit more complicated, as the motor I chose (and the one he used in the original video) was an inductive stepper motor. This meant it worked by powering and unpowering the coils inside the motor in rapid succession. I had to install a specific library in the arduino IDE in order to interface with it. I also had issues with it heating up, and so I had to add code to turn off the motor completely whenever it wasn't in use. Otherwise it would keep the coils energized and the motor would slowly heat up.

The hall effect sensor was the simplest of them all. It wanted three wires: 3.3V, GND, and signal, which went to a GPIO pin on the ESP32. It had an onboard LED whenver it detected a magnetic field, which made testing it very easy. I realized, however, that it only recognized the south pole of the magnet. I made a note of that for when I would eventually put the case together.

The next component was the MAX Amplifier. This component was the bane of my existence. It takes the cake by far for being the most tempermental component I have ever used. I thought it would be simple at first, as I found an example online of how to send audio data from the SD card reader directly to the AMP. And that part was simple, but getting it to sound good was extremely difficult. It would constantly crackle, pop, and generally sound like it was being played out of a flip phone that was underwater. I spent days trying to figure out why this was, and did extensive research online and using ChatGPT and Claude to try to fix it. 

Ultimetely, the issue came from two sources. First, the connections of the breadboard wires was not the most electrically sound, which made sense because they were less than $10 on Amazon. And I had no issues with them up until this point. But the AMP had an issue with them, a very large and personal issue. It would sound good, and then I would move the wires slightly, and then it would crackle again. Sometimes, it would only sound good if I was holding the AMP in my hand. As you can imagine this drove me insane.

It only got worse once I added both the AMP and the stepper motor at the same time. The motor introduced electrical noise into the power line, and the MAX wanted a clean power signal, otherwise the sound quality would be impacted. I tried a lot of different things to fix this, including adding an external power supply. I started with a 9V battery, but I realized it doesn't have enough current output to support the amplifier and the motor at the same time. I then switched to 4xAA batteries, which produced 6V. I used a buck converter to convert both of these voltages
