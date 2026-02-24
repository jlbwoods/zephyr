# MFD LED Test Sample

This sample demonstrates the CYW43439 MFD driver with LED control
on Raspberry Pi Pico W and Pico 2 W boards.

## Building

For Pico W:
```
west build -b rpi_pico/rp2040/w/mfd samples/drivers/led/mfd_led_test
```

For Pico 2 W:
```
west build -b rpi_pico2/rp2350a/m33/w/mfd samples/drivers/led/mfd_led_test
```

## Expected Output

```
=== MFD LED Test for Pico W/2W ===
LED device: cyw43439_led
Starting blink test...

LED: ON
LED: OFF
LED: ON
LED: OFF
...
```

The onboard LED should blink at 1 Hz.
