```bash
meson setup build --cross-file cross/stm32f767zi.ini 
ninja -C build
arm-none-eabi-objcopy -O ihex build/demo.elf build/demo.hex
st-flash --reset --format=ihex write build/demo.hex
```