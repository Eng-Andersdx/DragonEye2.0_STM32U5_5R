###############################################################################
# DragonEyeU5_5R - target Makefile for TouchGFX Designer "Run Target"
#
# Location : <project root>/gcc/Makefile
# Invoked by TouchGFX Designer from the TouchGFX/ folder:
#     make -f ../gcc/Makefile -j8        (compile  - "Run Target" step 1)
#     make -f ../gcc/Makefile flash      (program  - "Run Target" step 2)
#
# Paths are resolved relative to this file, so it also works from any folder:
#     make -f gcc/Makefile [all|flash|clean|print]
#
# Board : 5" DragonEye (800x480, 24bpp)
# MCU   : STM32U5G9NJHxQ (Cortex-M33, FPU, GPU2D/NemaGFX, FreeRTOS)
# Flags mirror the STM32CubeIDE "Debug" configuration of this project.
###############################################################################

# ---------------------------------------------------------------------------
# Locations
# ---------------------------------------------------------------------------
makefile_dir := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
root         := $(patsubst %/,%,$(dir $(makefile_dir)))

project_name := DragonEyeU5_5R
touchgfx     := $(root)/TouchGFX
cubeide      := $(root)/STM32CubeIDE
freertos     := $(root)/Middlewares/Third_Party/FreeRTOS/Source
tgfx_comp    := $(root)/Middlewares/ST/touchgfx_components/gpu2d
board_dir    := $(touchgfx)/target/DragonEye

build_dir    := $(touchgfx)/build/target
obj_dir      := $(build_dir)/obj
bin_dir      := $(build_dir)/bin

elf          := $(bin_dir)/$(project_name).elf
hex          := $(bin_dir)/$(project_name).hex
map          := $(bin_dir)/$(project_name).map

linker_script := $(cubeide)/STM32U5G9NJHXQ_FLASH.ld
startup_file  := $(cubeide)/Application/User/Startup/startup_stm32u5g9njhxq.s

# ---------------------------------------------------------------------------
# Toolchain (the TouchGFX Environment ships arm-none-eabi-gcc and make).
# Override the prefix if you want to use another GCC, e.g. the one in
# STM32CubeIDE (GNU Tools for STM32 13.3), which is what the IDE build uses:
#   make -f ../gcc/Makefile GCC_PATH=/c/ST/STM32CubeIDE_1.x/.../tools/bin/
# ---------------------------------------------------------------------------
GCC_PATH ?=
PREFIX   := $(GCC_PATH)arm-none-eabi-
CC       := $(PREFIX)gcc
CXX      := $(PREFIX)g++
AS       := $(PREFIX)gcc -x assembler-with-cpp
OBJCOPY  := $(PREFIX)objcopy
SIZE     := $(PREFIX)size

# STM32CubeProgrammer CLI
# Leave empty to auto-detect (PATH, standard installs, STM32CubeIDE's bundled
# copy), or set it explicitly, e.g.
#   STM32_PROGRAMMER_CLI := C:/ST/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe
STM32_PROGRAMMER_CLI ?=

# External loader for the OCTOSPI flash (MX25UM51245G @ 0x90000000).
# Same loader the STM32CubeIDE launch configuration uses. A copy is bundled in
# STM32CubeIDE/ and used first; if it is missing, the one shipped with
# STM32CubeProgrammer (<programmer>/bin/ExternalLoader/) is used.
# Set EXTERNAL_LOADER to a full path to override both.
EXTERNAL_LOADER_NAME := MX25UM51245G_STM32U5x9J-DK.stldr
EXTERNAL_LOADER      ?=
bundled_loader       := $(cubeide)/$(EXTERNAL_LOADER_NAME)

# Candidate locations (shell globs, searched in order when not set above)
programmer_candidates := \
  "/c/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe" \
  "/c/Program Files (x86)/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe" \
  "C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe" \
  "C:/Program Files (x86)/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe" \
  /c/ST/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe \
  /c/ST/STM32CubeIDE*/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_*/tools/bin/STM32_Programmer_CLI.exe \
  "/c/Program Files/STMicroelectronics/STM32CubeIDE"*/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_*/tools/bin/STM32_Programmer_CLI.exe \
  C:/ST/STM32CubeIDE*/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.win32_*/tools/bin/STM32_Programmer_CLI.exe

# ---------------------------------------------------------------------------
# Sources
# ---------------------------------------------------------------------------
c_sources := \
  $(wildcard $(root)/Core/Src/*.c) \
  $(cubeide)/Application/User/Core/syscalls.c \
  $(cubeide)/Application/User/Core/sysmem.c \
  $(touchgfx)/App/app_touchgfx.c \
  $(wildcard $(touchgfx)/target/generated/*.c) \
  $(wildcard $(board_dir)/*.c) \
  $(wildcard $(board_dir)/Components/*.c) \
  $(addprefix $(freertos)/, \
    CMSIS_RTOS_V2/cmsis_os2.c \
    croutine.c \
    event_groups.c \
    list.c \
    queue.c \
    stream_buffer.c \
    tasks.c \
    timers.c \
    portable/GCC/ARM_CM33_NTZ/non_secure/port.c \
    portable/GCC/ARM_CM33_NTZ/non_secure/portasm.c \
    portable/MemMang/heap_4.c) \
  $(addprefix $(root)/Drivers/STM32U5xx_HAL_Driver/Src/, \
    stm32u5xx_hal.c \
    stm32u5xx_hal_cortex.c \
    stm32u5xx_hal_crc.c \
    stm32u5xx_hal_crc_ex.c \
    stm32u5xx_hal_dcache.c \
    stm32u5xx_hal_dma.c \
    stm32u5xx_hal_dma2d.c \
    stm32u5xx_hal_dma_ex.c \
    stm32u5xx_hal_dsi.c \
    stm32u5xx_hal_exti.c \
    stm32u5xx_hal_flash.c \
    stm32u5xx_hal_flash_ex.c \
    stm32u5xx_hal_gpio.c \
    stm32u5xx_hal_gpu2d.c \
    stm32u5xx_hal_gtzc.c \
    stm32u5xx_hal_i2c.c \
    stm32u5xx_hal_i2c_ex.c \
    stm32u5xx_hal_icache.c \
    stm32u5xx_hal_jpeg.c \
    stm32u5xx_hal_ltdc.c \
    stm32u5xx_hal_ltdc_ex.c \
    stm32u5xx_hal_ospi.c \
    stm32u5xx_hal_pwr.c \
    stm32u5xx_hal_pwr_ex.c \
    stm32u5xx_hal_rcc.c \
    stm32u5xx_hal_rcc_ex.c \
    stm32u5xx_hal_tim.c \
    stm32u5xx_hal_tim_ex.c \
    stm32u5xx_hal_uart.c \
    stm32u5xx_hal_uart_ex.c \
    stm32u5xx_hal_xspi.c \
    stm32u5xx_ll_dlyb.c)

# TouchGFX sources are picked up with wildcards so new screens, images,
# fonts and texts generated by the Designer are built automatically.
cpp_sources := \
  $(wildcard $(touchgfx)/target/*.cpp) \
  $(wildcard $(touchgfx)/target/generated/*.cpp) \
  $(wildcard $(board_dir)/*.cpp) \
  $(wildcard $(touchgfx)/gui/src/*/*.cpp) \
  $(wildcard $(touchgfx)/generated/gui_generated/src/*/*.cpp) \
  $(wildcard $(touchgfx)/generated/fonts/src/*.cpp) \
  $(wildcard $(touchgfx)/generated/images/src/*.cpp) \
  $(wildcard $(touchgfx)/generated/images/src/*/*.cpp) \
  $(wildcard $(touchgfx)/generated/texts/src/*.cpp) \
  $(wildcard $(touchgfx)/generated/videos/src/*.cpp)

asm_sources := $(startup_file)

# Videos: TouchGFX Designer converts each video into a ready-made object file
# (generated/videos/obj/*.bin.o, placed in ExtFlashSection). Link them as-is.
video_objs := $(wildcard $(touchgfx)/generated/videos/obj/*.o)

# ---------------------------------------------------------------------------
# Includes / defines (same order as the STM32CubeIDE project)
# ---------------------------------------------------------------------------
include_paths := \
  $(root)/Core/Inc \
  $(touchgfx)/App \
  $(touchgfx)/target/generated \
  $(touchgfx)/target \
  $(root)/Drivers/STM32U5xx_HAL_Driver/Inc \
  $(root)/Drivers/STM32U5xx_HAL_Driver/Inc/Legacy \
  $(root)/Drivers/CMSIS/Device/ST/STM32U5xx/Include \
  $(root)/Drivers/CMSIS/Include \
  $(freertos)/include \
  $(freertos)/portable/GCC/ARM_CM33_NTZ/non_secure \
  $(freertos)/CMSIS_RTOS_V2 \
  $(root)/Middlewares/Third_Party/CMSIS/RTOS2/Include \
  $(root)/Middlewares/ST/touchgfx/framework/include \
  $(touchgfx)/generated/fonts/include \
  $(touchgfx)/generated/gui_generated/include \
  $(touchgfx)/generated/images/include \
  $(touchgfx)/generated/texts/include \
  $(touchgfx)/generated/videos/include \
  $(touchgfx)/gui/include \
  $(board_dir) \
  $(board_dir)/Components \
  $(tgfx_comp)/NemaGFX/include \
  $(tgfx_comp)/TouchGFXNema/include

# Optimisation: STM32CubeIDE "Debug" builds everything at -O0 -g3.
#   make -f ../gcc/Makefile OPT=-Os   -> smaller/faster image (e.g. for demos)
#   make -f ../gcc/Makefile RELEASE=1 -> NDEBUG instead of DEBUG define
OPT     ?= -O0
RELEASE ?= 0
ifeq ($(RELEASE),1)
  defines := -DNDEBUG -DUSE_HAL_DRIVER -DSTM32U5G9xx
else
  defines := -DDEBUG -DUSE_HAL_DRIVER -DSTM32U5G9xx
endif

# ---------------------------------------------------------------------------
# Flags
# ---------------------------------------------------------------------------
mcu_flags := -mcpu=cortex-m33 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard

common_flags := $(mcu_flags) $(defines) $(addprefix -I,$(include_paths)) \
  -ffunction-sections -fdata-sections -Wall -MMD -MP --specs=nano.specs

CFLAGS   := $(common_flags) $(OPT) -g3 -std=gnu11
CXXFLAGS := $(common_flags) $(OPT) -g3 -std=gnu++14 -fno-exceptions -fno-rtti \
            -fno-use-cxa-atexit -femit-class-debug-always
ASFLAGS  := $(mcu_flags) $(defines) -g3 --specs=nano.specs

lib_dirs := \
  $(root)/Middlewares/ST/touchgfx/lib/core/cortex_m33/gcc \
  $(tgfx_comp)/NemaGFX/lib/core/cortex_m33/gcc \
  $(tgfx_comp)/TouchGFXNema/lib/cortex_m33/gcc

LDFLAGS  := $(mcu_flags) -T$(linker_script) --specs=nosys.specs --specs=nano.specs \
  -Wl,-Map=$(map) -Wl,--gc-sections -static $(addprefix -L,$(lib_dirs))
# TouchGFX + NemaGFX libraries are inside the group so their mutual
# references resolve regardless of order.
LDLIBS   := -Wl,--start-group \
  -l:libtouchgfx-float-abi-hard.a \
  -l:libnemagfx-float-abi-hard.a \
  -l:libtouchgfxnema-float-abi-hard.a \
  -lc -lm -lstdc++ -lsupc++ -Wl,--end-group

# ---------------------------------------------------------------------------
# Objects: mirror the source tree under obj_dir (avoids name clashes)
# ---------------------------------------------------------------------------
to_obj = $(patsubst $(root)/%,$(obj_dir)/%.o,$(1))
objects := $(call to_obj,$(c_sources)) $(call to_obj,$(cpp_sources)) $(call to_obj,$(asm_sources))

# Rebuild everything automatically when the compiler flags change
# (e.g. switching OPT or RELEASE).
flags_file := $(build_dir)/compiler_flags.txt
flags_now  := $(CFLAGS) | $(CXXFLAGS) | $(ASFLAGS)
$(shell mkdir -p $(build_dir) && (echo '$(flags_now)' | cmp -s - $(flags_file) || echo '$(flags_now)' > $(flags_file)))

# ---------------------------------------------------------------------------
# Rules
# ---------------------------------------------------------------------------
# Recipe lines start with ">" instead of a Tab, so editors that turn tabs
# into spaces cannot break this file ("missing separator"). Needs make 3.82+.
.RECIPEPREFIX := >

.PHONY: all flash clean print
.DEFAULT_GOAL := all

all: $(hex)
>@$(SIZE) $(elf)

# Objects are passed to the linker through a response file (objects.list) with
# paths relative to obj_dir. Passing them on the command line with absolute
# paths exceeds the Windows 32767-character limit on larger GUIs, which shows
# up as "cannot execute ... collect2.exe: CreateProcess: No such file".
rel_objects := $(patsubst $(obj_dir)/%,%,$(objects))

# The list is written 20 names per shell command: a single printf with all
# ~300 names is too long for the TouchGFX MSYS shell, which then drops the
# "> objects.list" redirect (names get printed, linker can't find the file).
# Each line of the expansion below runs as its own short recipe command.
define append_chunk
@cd $(obj_dir) && echo $(1) >> objects.list

endef
write_chunks = $(if $(strip $(1)),$(call append_chunk,$(wordlist 1,20,$(1)))$(call write_chunks,$(wordlist 21,$(words $(1)),$(1))))

$(elf): $(objects) $(video_objs) $(linker_script)
>@mkdir -p $(@D)
>@echo Linking $(notdir $@)
>@rm -f $(obj_dir)/objects.list
>$(call write_chunks,$(rel_objects))
>@cd $(obj_dir) && $(CXX) @objects.list $(video_objs) $(LDFLAGS) $(LDLIBS) -o $@

$(hex): $(elf)
>@$(OBJCOPY) -O ihex $< $@
>@echo Created $(notdir $@)

$(obj_dir)/%.c.o: $(root)/%.c $(flags_file)
>@mkdir -p $(@D)
>@echo Compiling $(notdir $<)
>@$(CC) $(CFLAGS) -c $< -o $@

$(obj_dir)/%.cpp.o: $(root)/%.cpp $(flags_file)
>@mkdir -p $(@D)
>@echo Compiling $(notdir $<)
>@$(CXX) $(CXXFLAGS) -c $< -o $@

$(obj_dir)/%.s.o: $(root)/%.s $(flags_file)
>@mkdir -p $(@D)
>@echo Assembling $(notdir $<)
>@$(AS) $(ASFLAGS) -c $< -o $@

# Program internal flash (0x08000000) + OCTOSPI flash (0x90000000) and reset.
# Connect-under-reset, same as the STM32CubeIDE launch configuration.
flash: all
>@cli="$(STM32_PROGRAMMER_CLI)"; \
>if [ -z "$$cli" ]; then cli=`command -v STM32_Programmer_CLI.exe 2>/dev/null || command -v STM32_Programmer_CLI 2>/dev/null`; fi; \
>if [ -z "$$cli" ]; then for c in $(programmer_candidates); do if [ -f "$$c" ]; then cli="$$c"; fi; done; fi; \
>if [ -z "$$cli" ]; then \
>  echo "ERROR: STM32_Programmer_CLI.exe not found."; \
>  echo "Set STM32_PROGRAMMER_CLI at the top of gcc/Makefile to its full path."; \
>  exit 1; \
>fi; \
>loader="$(EXTERNAL_LOADER)"; \
>if [ -z "$$loader" ] && [ -f "$(bundled_loader)" ]; then loader="$(bundled_loader)"; fi; \
>if [ -z "$$loader" ]; then loader="`dirname "$$cli"`/ExternalLoader/$(EXTERNAL_LOADER_NAME)"; fi; \
>if [ ! -f "$$loader" ]; then \
>  echo "ERROR: external loader not found: $$loader"; \
>  echo "Set EXTERNAL_LOADER at the top of gcc/Makefile to the full path of $(EXTERNAL_LOADER_NAME)."; \
>  exit 1; \
>fi; \
>echo "Using $$cli"; \
>echo "Flashing $(notdir $(hex)) with external loader `basename "$$loader"`"; \
>"$$cli" -c port=SWD mode=UR reset=HWrst -el "$$loader" -d "$(hex)" -v -rst

clean:
>@rm -rf $(build_dir)

print:
>@echo root     = $(root)
>@echo objects  = $(words $(objects))
>@echo elf      = $(elf)

-include $(objects:.o=.d)
