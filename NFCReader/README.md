# NFCReader
A test sketch to try out the RFID-RC522 reader and cards.

Logic has written so that three cards are prepared (hardcoded with their serial): READ/WRITE/ERASE
These card set the 3 different states internally to the program. The WRITE card can be used to write a predefined string into any BLANK card - initialize. The READ card is presented so that the Next card presented will be used to match a predefined string and display the matching result on the OLED display. The ERASE card is used to make an Initialized card to become un-initialized by erasing the preset string written to the card.

From the source:
 * MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT.
 * The library file MFRC522.h has a wealth of useful info. Please read it.
 * The functions are documented in MFRC522.cpp.
 *
 * Based on code Dr.Leong   ( WWW.B2CQSHOP.COM )
 * Created by Miguel Balboa (circuitito.com), Jan, 2012.
 * Rewritten by Søren Thing Andersen (access.thing.dk), fall of 2013 (Translation to English, refactored, comments, anti collision, cascade levels.)
 *
 * Released into the public domain.
 *
 * This sample shows how to setup a block on a MIFARE Classic PICC to be in "Value Block" mode.
 * In Value Block mode the operations Increment/Decrement/Restore and Transfer can be used.
