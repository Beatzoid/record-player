# Record Player

This is a miniature record player made using an Arduino ESP32 and various electronic components. The following is a dev log of my progress through this project, from the initial idea all the way to the final product (which has yet to be made).

![First Prototype](https://github.com/user-attachments/assets/7f1136ac-f2fc-46e5-81c3-a5859a36b5ea) <h6> The first prototype of the record player </h6>

# Ideation 
The initial idea came from [this](https://www.youtube.com/watch?v=fBjv4E7mpA4) video, in which AKZ Dev created a mini record player that connected to Spotify. He used an existing record coaster set and designed a case in CAD software to hold the electronics below it. I saw this and thought it would be a fun project, but I wanted to make it entirely local. Instead of having it play through Spotify, I wanted it to be able to play songs off of a mini-SD card through a speaker mounted in the front. This is where the project began.

Note: I didn't start taking photos of my work until the initial prototype was done, so there is going to be a lot of text at the start of this.

# Research & Parts

I started by researching all of the parts needed, most of which were provided by AKZ Dev. He also provided a CAD file for the case, which was free of charge. Huge thanks to him; this project would have been significantly more difficult without his resources. However, there was one change that I made to the core design. He used a Raspberry Pi Zero as the central microprocessor. I wanted to do the same, as it would allow me to use his code and have an easy starting place from which I could make modifications. 

I ran into an issue, however: I couldn't find one. Anywhere. I looked on every single reseller in the United States and on forums like Reddit, and everyone was saying that it was nearly impossible to get one now. So I pivoted and started looking for other solutions. 

I searched through the comments of the YouTube video to see if anyone else had this problem, and I saw that one user had suggested using an ESP32 in place of a Raspberry Pi Zero, as it would perform the same. I started researching this possibility and found what I ultimately decided on using: an Arduino ESP32 Breakout Board. It is more expensive than a traditional ESP32, but the advantage is that it allows me to use the Arduino ecosystem, which I was already familiar with. I also looked into several other parts I would need and settled on the following Bill of Materials.

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

# Initial Prototype

Once I had all the parts, I got to work making the initial version. I started by making it all on a breadboard, as it made tracing and debugging connections significantly easier. I went component-by-component, making sure each part worked the way I wanted before adding another and connecting them together. For the most part, this went smoothly. 

I started with the NFC reader, providing it power and making sure it was correctly identifying the NFC stickers. It used the I2C communication protocol, which I will admit I don't fully understand. However, I read the spec sheet on the reader and looked into I2C for the ESP32, and learned that I had to connect it to specific pins in order for it to communicate properly.

The next component was the stepper motor. This one was a bit more complicated, as the motor I chose (and the one he used in the original video) was an inductive stepper motor. This meant it worked by powering and unpowering the coils inside the motor in rapid succession. I had to install a specific library in the Arduino IDE in order to interface with it. I also had issues with it heating up, and so I had to add code to turn off the motor completely whenever it wasn't in use. Otherwise, it would keep the coils energized, and the motor would slowly heat up.

The hall effect sensor was the simplest of them all. It wanted three wires: 3.3V, GND, and signal, which went to a GPIO pin on the ESP32. It had an onboard LED whenever it detected a magnetic field, which made testing it very easy. I realized, however, that it only recognized the south pole of the magnet. I made a note of that for when I would eventually put the case together.

The next component was the MAX Amplifier. This component was the bane of my existence. It takes the cake by far for being the most temperamental component I have ever used. I thought it would be simple at first, as I found an example online of how to send audio data from the SD card reader directly to the AMP. And that part was simple, but getting it to sound good was extremely difficult. It would constantly crackle, pop, and generally sound like it was being played out of a flip phone that was underwater. I spent days trying to figure out why this was, and did extensive research online and using ChatGPT and Claude to try to fix it. 

Ultimately, the issue came from two sources. First, the connections of the breadboard wires were not the most electrically sound, which made sense because they were less than $10 on Amazon. And I had no issues with them up until this point. But the AMP had an issue with them, a very large and personal issue. It would sound good, and then I would move the wires slightly, and then it would crackle again. Sometimes, it would only sound good if I was holding the AMP in my hand. As you can imagine, this drove me insane.

It only got worse once I added both the AMP and the stepper motor at the same time. The motor introduced electrical noise into the power line, and the MAX wanted a clean power signal; otherwise, the sound quality would be impacted. I tried a lot of different things to fix this, including adding an external power supply. I started with a 9V battery, but I realized it doesn't have enough current output to support the amplifier and the motor at the same time. I then switched to 4xAA batteries, which produced 6V. I used a buck converter to convert both of these voltages.

Finally, after almost 3 weeks of work, I had my first prototype made. 

# The First Prototype

As you may imagine, it was messy, cumbersome, and only worked when I looked at it a certain way. Here are some initial pictures:

![](https://github.com/user-attachments/assets/58fc0fb1-1f9e-4a69-8076-6ace780e09e0)
![](https://github.com/user-attachments/assets/77e079da-e8e4-4b73-bf23-5e809caf5f7a)

The problems with it are as follows:
1) The power supply has a lot of issues. Like, a significant number. Every time the amp drew power to play music, the motor would slow down. And on certain portions of the song, the motor would slow down by different amounts. It almost followed the beat of the song in a way. Unintended, but still neat.
2) The power management system was atrocious. I ripped the power rail off either side of a breadboard and shoved it into the way-too-small box in order to distribute the power. That, combined with the very flimsy DuPont wire connections, led to a lot of issues when trying to trace wire connections.
3) It was, in general, very fragile. I had to be very careful with how I was moving it around

I wanted to fix all of these issues, but it reached the end of the summer, and so I had to go to school. I didn't want to risk breaking it, and so I left the project behind. I still had the code and a spreadsheet with the pin connections, however. I was not going to give up on this project.

# Version Two

I put the project on hold for the first few weeks of school. But then, I joined a club on campus that gave me access to Altium, the industry-standard PCB design software. I immediately thought of designing a custom PCB for my project, and from there I was off to the races.

![](https://github.com/user-attachments/assets/c6c72f83-7671-4b0f-ae1d-00bb89338c25)

<h6> The first prototype of my PCB</h6>

This is the first prototype of my PCB that I made. It had a good foundational approach, but the details were shaky. The first issue was the ESP32 mount on the left side that I used. Instead of using a standard Arduino ESP32 footprint, I decided to use two separate 15-pin female headers that I would then stick the Arduino ESP32 into. In theory, this would work, but in practice it was very difficult to achieve. The spacing of the headers needs to be very precise; otherwise, I would spend time and money manufacturing the PCB and then have to redo the entire process. 

I went back to the drawing board and started redesigning it. The first thing I did was find an ESP32 footprint on [componentsearchengine.com](componentsearchengine.com) and imported it into Altium using their [library loader](https://componentsearchengine.com/library/altium). This gave me confidence that the ESP32 would fit onto the PCB without issue. Here is what the footprint looked like:

As you can see, the main difference is that it is a through-hole connection, instead of having headers. This does require more work on the soldering side, but the trade-off of knowing it will fit the first try convinced me this was the right path to take. Here is what it looked like after configuring the ESP32 mount in Altium:

![](https://github.com/user-attachments/assets/983196d4-6882-4d48-90eb-8b2f9412169f)

And here is my first prototype of my entire PCB:

![](https://github.com/user-attachments/assets/9d705bb9-801f-4433-badc-bf1f19c1b677)

There are some other differences with this mount as well, namely for the 3.3V and GND connections on the ESP32. I changed those specific ports to use Net Labels instead of Ports. Doing this allowed me to create two new layers in my PCB, one for 3.3V and one for GND. Here is the layer stack I configured in Altium:

![](https://github.com/user-attachments/assets/200e17a7-56a1-4912-8a7a-1332339e467f)

I configured it in this manner in order to organize the traces on the PCB. By doing this, I am able to connect the through-hole connections of the components directly to the respective layer for each pin. This allows me to only have to connect the IO pins to the ESP32, significantly easing the layout of the PCB wiring. In addition, it solved an issue I was having where Altium wanted me to wire the GND and 3.3V pins of the components to each other in addition to the ESP32. 

I ran into an issue with this, however, which is that the polygon pours I used to connect the pins on the respective layers together were not connecting to the pins I connected to those layers. I discovered that I had to configure the pins as "Full Stack" rather than "Simple" in this menu:

![](https://github.com/user-attachments/assets/776a2509-0396-48f2-9cb8-93621904ae65)

Once I did that, I was able to connect the pours to the correct pins on each layer. Here is what they looked like:

![](https://github.com/user-attachments/assets/56b506ab-68c7-4333-a528-0c81a15b8b11)

![](https://github.com/user-attachments/assets/6eeb67bf-fbaa-4255-bc4e-8b17c3967fbe)

It's difficult to see in the images, but there are lines connecting from the outside to the vias on the headers.

As I worked through developing my prototype, I came across both some ideas I had that I wanted to add and some issues that I wanted to solve. The first thing you may notice, however, that I have not talked about is the 5V buck converter in the top right corner of the PCB.

The 5V buck converter serves one purpose: to power the stepper motor that turns the record. That's it. The output from the battery gets fed directly into the ESP32, which powers it, and all the other components draw from the 3.3V pin on the ESP32. Separating out the power supply for the motor from all the other components ensures that the supply lines stay clean of any interference generated by the inductive motor load.

The other components in that area of the PCB are an electrolytic capacitor, which is used when the motor tries to draw too much power from the 5V buck converter; a smaller SMD capacitor that is used to filter out noise; and a Zener diode for protecting the upstream power supply and other components. 

From that first prototype, I progressed, adding more headers for each component I had in my record player. Eventually, once I had all of the headers mapped out, I started relocating them to optimize the trace lines on the PCB to minimize crossings as much as possible. 

I also added a couple of other components to improve the user experience. One of those was a power switch to make it easier to turn the record player on and off. Another was a rotary encoder to act as a volume switch. And the last was a power LED, wired directly to the 3.3V line of the ESP32 to indicate whether or not the record player is on. Here are all of those components in schematic view.

![](https://github.com/user-attachments/assets/825a4a21-9dbc-465f-9874-d5723c802e89)
![](https://github.com/user-attachments/assets/8cbf0a98-c874-45ec-9967-f4aa00398a0b)
![](https://github.com/user-attachments/assets/ee2da100-4ca2-4f88-aa68-96e290bc3e7e)

Now, after all this talking and describing the iteration process, here is the current version of the PCB I have come up with.

![](https://github.com/user-attachments/assets/21062fff-5ae7-4419-8f66-77cb6487b0aa)
![](https://github.com/user-attachments/assets/5fae6988-08ac-401f-930b-931c7d4b3529)
![](https://github.com/user-attachments/assets/2630efe3-b7cb-4de5-8f71-deaef7b25efb)

There are still some crossings in the PCB wiring, but those are unavoidable. To fix those, I will route the traces on the top and bottom of the PCB.

# The Case

One of the most important parts of the whole project, as without it there is nothing to put the electronics into. Here is what the original creators' design looked like:

![](https://github.com/user-attachments/assets/39c0ad49-38a0-45bc-a3ed-31d2478ca8ab)

It has mounts for the Raspberry PI (which is what the original project used), along with all of the necessary components. It also had holes in the back for airflow and connections, and a grill in the front to mimic a traditional record player. Overall, a very nice design, and impressive given he said it was his first 3D design. 

I modified the design in a few different ways. One was to extend it up to a height of 2". This was done to ensure the speaker I used fit, with the lid still fitting snugly on the case. The other modification I made was to cut out a hole in the grill to allow the sound from the speaker to be heard with full clarity.

This is the final design I settled on:

INSERT PICTURE

For future modifications of the case, I would want to design screw holes to secure the custom PCB. I put mounting holes on all four corners of the PCB for that exact reason. I would also want to ensure that all the components have a place to sit, either on the PCB or in the case. I don't want anything floating unsupported. To achieve this new design, I will utilize Fusion360, as it is industry-standard software. I have no experience with the program, and the few times I've opened it have been overwhelming, but over the next few weeks I plan to teach myself through online courses. I then plan to 3D print the new case using the resources available at my school. 

# PCB Manufacturing

To manufacture the PCB, I will use JLCPCB. That is what the wind power club I am a part of uses, and it seems to be the most in-depth, robust, and high-quality company on the market. However, I ran into issues when trying to use their PCBA service. Mainly, the parts I used weren't available in their PCBA parts library. I had to search for parts in their library, cross-referencing with both DigiKey, in case I wanted to order parts manually, and componentsearchengine.com to ensure I had the part footprint in Altium. This process took several hours, and there was probably a better way to do it, but it's done now. I had to add a property to every single component in my project to connect it to one in the JCL PCBA library, like this:

![](https://github.com/user-attachments/assets/98899e9a-32da-4334-83ed-dcf791d91858)

I then created an output job file so I could save the specific export settings JCL wanted and export everything with one click.

![](https://github.com/user-attachments/assets/182f53c8-2a7d-4195-ab5d-892fbce54472)

This allowed me to export the Gerber and NC Drill File for PCB production, as well as the BOM and pick-and-place documents for the PCBA process. Ultimately, however, I think I am going to order the parts manually through DigiKey and solder them myself. Not only is this about $50 cheaper, but it will also give me experience soldering at this scale, which is a very useful skill to have.

# The Code

Originally, I was going to use the original project's code as a base and build off of that, but as I couldn't obtain a Raspberry Pi Zero (still convinced they don't exist), I had to start from scratch. Luckily, the Arduino ESP32 makes it very easy, as all the normal Arduino functions I was used to were available to me. I worked through each component piece by piece, ensuring that the APIs I was using were communicating with it correctly. I also had to manage an SPI bus for the SD card reader and RFID reader, and an I2S bus for the audio output. I will admit I don't know how they work technically, only that they are communication protocols. I am planning to research them, along with I2C, in the future to gain a deeper understanding of how they all work individually and together.

An interesting piece of code I had to write, following a debugging process, was this one:

![](https://github.com/user-attachments/assets/a396f1c4-9393-45cf-9dfc-0a040b8890db)

I noticed that when the record player was idle, the motor would either retain heat or increase its heat slowly over time. I also noticed the lights on the driver board I was using, which lit up when the corresponding coil was energized or de-energized. After some testing, I realized the coils were still being energized even when the motor wasn't moving. I then added the above code in order to prevent the motor from overheating. 

I also added a very simple web server to the project, with the following HTML rendered:

![](https://github.com/user-attachments/assets/0f251a1a-9b3d-4b07-9ba1-52ecf36138b3)

As I said, simple. No fancy React, Bootstrap, or even CSS. Just pure HTML, as our ancestors intended. It is served over a local access point generated by the ESP32. The process is as follows: you connect to the AP, load up the website, place a record on the record player, and upload a song via the web portal. It then takes the NFC tag UID of the current record, renames the music file to that ID (which is how it maps the songs to the tags), and saves it locally to the SD card. By that logic, you can create new records and assign songs to them without ever touching the code.
