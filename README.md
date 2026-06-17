# uhd-signal-fun

Flipper Zero signals and libuhd C++ demod examples

Goal: learn libuhd and gain an understanding of practical demodulation of common modulation types

## Hardware Reqs


- uhd (USRP) compadible SDR
    - USRP B200 (tested against this)
    - Chinese B200 knock off (TZT B200-mini-i) (untested)
    
## Software Reqs

- `gcc`
- `libuhd`

## Flipper Zero SubGHZ Signals

- Signal 1: [signal1.sub](./flipper/signal1.sub)
    - Center Freq: 443.92 MHz
    - Modulation: OOK

## Compile and Run SW

1. `make rx-signalX` - where X is the signal num.
2. `./rx-signalX`
