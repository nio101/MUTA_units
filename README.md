# MUTA_units
An open source home automation solution for hobbyists and makers

## customizing the firmwares before flashing

In the **MPLAB IPE**, activate *Setting/advanced mode*, open the firmware/.hex file you want to customize, and then select *View/show memory*.
On the *Operate* tab, you'll have a memory view window. Skip to the end of the memory and you fill view the constants you can edit inline before flashing:
* static_encryption_key @ 0x01FFEC (8 bytes)
* make/model @ 0x01FFF4 (2 bytes)
* unit_uid @ 0x01FFF6 (2 bytes)
You can then use the *view/fill memory* dialog to modify those values and check the modifications on the memory view afterwards.
*important: due to Endianness, the first and second bytes are swapped for each memory value, so you should input for example 0x0201@0x01FFF4 to get a 01 model and a 02 make*
*remark: This manual process could be done in a much easier way with the hexmate util, but my first tries weren't successfull and I couldn't spend more time on it.*
