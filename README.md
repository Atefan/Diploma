# Entropy — Hardware True Random Number Generator

ELSYS Sofia diploma project.

## The problem

Computers are deterministic — they can only produce pseudo-random numbers,
which is not good enough for real cryptographic or security-sensitive use
(secure communications, bank transfers, key generation, etc.). This project
builds a **true hardware entropy source** and feeds it directly into the
Linux kernel's entropy pool, so real applications get genuine randomness
instead of PRNG output.

## How it works

1. **Analog front-end** — a custom white-noise generation circuit (charge-pump
   voltage multiplier + PWM stage, op-amp) produces physical noise, sampled
   through the ADC. Schematic and PCB in `Pico_PCB/`.
2. **Firmware** (`Entropy device/`) — runs on a **Raspberry Pi Pico (RP2040,
   dual-core)**. One core samples the ADC and drives the PWM/noise circuit;
   the noise is buffered in a circular queue and streamed to the host over
   USB. Built with the Pico SDK + CMake.
3. **Linux kernel driver** (`driver/`) — a USB driver (`usb_entropy` module)
   that reads the incoming stream and exposes it as `/dev/entropy`, feeding
   it straight into the kernel's entropy pool via `linux/random.h`.
4. **GUI** (`GUI/`) — an SDL2-based Windows app that visualizes the live
   entropy output in real time.
5. **Validation** (`testing/`) — randomness was verified with the NIST Statistical Test Suite (STS 2.1.2, vendored here).
