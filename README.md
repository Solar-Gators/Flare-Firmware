# TODO:

1. Implement RTS/CTS

2. Test w/ RX

3. Getfirmware(), do the edge cases 5 and ?

4. Write RT versions


# AT/RT Commands

### RT Commands should be used first before AT

--Use to control local modem(STM32-based)

--Enter using "+++", enterATCommandMode()

## To set registers

1) use ATSn=X, n = register number, X = value
2) use AT&W to save new values
3) use ATX to reboot and augment new changes



# RT Commands
