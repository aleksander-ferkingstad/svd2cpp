## svd2cpp
Creates peripheral access c++ code from SVD files. The goal is to create a low cost type safe abstraction for register access on microcontrollers like STM32.

You can use any provided svd files as input for this tool, but it is designed to use the svd files that has been patched by the rust community. stm32 files can be found [here](https://github.com/tinygo-org/stm32-svd). Consider contributing to this project if you need a new SVD file.

### Example code