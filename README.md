# TODO:

1. Fix sendData to have no ptr version.

2. Implement RTS/CTS

3. Test w/ RX

4. Make it so I don't send all the data in register at the same time

5. Getfirmware(), do the edge cases 5 and ?

# AT/RT Commands

### RT Commands should be used first before AT

--Use to control local modem(STM32-based)

--Enter using "+++", enterATCommandMode()

## To set registers

1) use ATSn=X, n = register number, X = value
2) use AT&W to save new values
3) use ATX to reboot and augment new changes



# RT Commands
