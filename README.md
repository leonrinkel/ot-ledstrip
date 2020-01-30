# ot-ledstrip

## What Is This?

This is a custom application for Nordic Semiconductors OpenThread capable ICs, like the nRF52840, with the purpose of wirelessly controlling RGB LED strips.

It works by joining or forming a specified OpenThread network and then exposing a COAP server to the network. At the moment only one COAP resource for turning the strip on or off is implemented. Resources like `/brightness`, `/rgb` or `/hue` will follow soon. Other Thread nodes on the same network can then write to these resources to control the LED strip.

It includes a simple PWM driver for SK6812 or compatible LEDs (also known as Adafruit NeoPixels).

I’m using this in a smart home setup similar to Phillips Hue or IKEA Tradfri. A Raspberry Pi acts as an OpenThread to IP bridge and also exposes an Apple HomeKit IP accessory for each strip to my home network. I can then easily control the strips using the Home app or Siri on all my Apple devices. (The bridge is not part of this repo.)

## Getting Started

Building this project requires Arms GCC compiler for Cortex-M based microcontrollers, as well as Nordics SDK for Thread. Executing `./script/bootstrap` from the projects root directory should automatically download and extract these dependencies.

This repo contains the necessary configuration to be opened in Visual Studio Code. There are several tasks like `Build` or `Flash` and C/C++ configurations for the different supported targets. (I also have a very convenient GDB debugging configuration in the works which will follow soon.)

You can change the number of LEDs and the PWM output pin in `src/ledstrip.h`.

The Thread network credentials can be set in the `sdk_config.h` files or using a CLI over USB like in the following.

```sh
$ screen /dev/*somedevice* 115200
> thread stop
> dataset init new
> dataset panid *somepanid*
> dataset channel *somechannel*
> dataset masterkey *somemasterkey*
> dataset commit active
> thread start
```

The whole thing is work in progress so don't expect it to work right away. It's more like an inspiration how you could build your own smart home. Feel free to contribute or open issues.
