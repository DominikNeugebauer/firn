# FIRN

Firmware for **firn**, a digital synthesizer running on an STM32F411 (STM32F411E-DISCO).
The STM32 generates the audio and streams it via I2S and DMA to the on-board CS43L22 DAC (48 kHz, 16-bit stereo).

## Current state
- Waveforms: sine, square, triangle, sawtooth
- Potentiometer 1 (PA1): cutoff of the resonant low-pass filter (50 ... 8000 Hz)
- Potentiometer 2 (PA2): frequency (55 ... 880 Hz)
- User button (PA0): gate on
- Amplitude is fixed at full volume
- Signal chain: oscillator -> SVF low-pass -> one-pole high-pass -> envelope -> DAC

## Structure
- `Core/Src/main.c`     – setup, main loop, audio callbacks
- `Core/Src/synth.c`    – oscillator and waveforms
- `Core/Src/cs43l22.c`  – audio DAC driver
- `Core/Src/envelope.c` – ADSR Envelope
- `Core/Src/onepole.c`  – one-pole high-pass filter (6 dB/oct)
- `Core/Src/svf.c`      – Chamberlin state variable filter (12 dB/oct, resonance)
- `firn.ioc`            – STM32CubeMX configuration

Switching between waveforms is not possible in this version!

## SWD Connection

```
SWCLK -> PA14
SWDIO -> PA13
GND   -> GND
5V    -> 5V
```

## Commands

Check SMT32
```
st-info --probe
```

Build

```
rm -rf build
cmake --preset Debug
cmake --build --preset Debug
```

Flash
```
openocd -f interface/stlink.cfg -c "transport select swd" -f target/stm32f4x.cfg \
        -c "program build/Debug/firn.elf verify reset exit"
```

Infofile for vscode
```
! Check if compile_commands.json is accessible !
ls build/Debug/compile_commands.json

mkdir -p .vscode && cat > .vscode/c_cpp_properties.json << 'EOF'
{
  "configurations": [
    {
      "name": "STM32",
      "compilerPath": "/usr/bin/arm-none-eabi-gcc",
      "compileCommands": "${workspaceFolder}/build/Debug/compile_commands.json",
      "intelliSenseMode": "gcc-arm",
      "cStandard": "c11"
    }
  ],
  "version": 4
}
EOF
```

## Project info

The debugging interface can be configured in the MX-Project under Pinout & Configuration -> SYS -> Debug "Serial Wire". Settings about toolchain compiler etc can be found under Project Manager. The clock settings of the chip are configured at Clock Configuration.