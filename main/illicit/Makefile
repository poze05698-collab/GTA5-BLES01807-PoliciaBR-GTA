# POLICIA BR - GTA V BLES01807
# Open PS3DEV/PSL1GHT build path. This does not include any proprietary Sony SDK.

TARGET := PoliciaBR
OFILES := menu.o police.o printf.o prx.o

include $(PSL1GHT)/ppu_rules

CXXFLAGS += -std=gnu++98 -fno-exceptions -fno-rtti -D__CELL_ASSERT__ -DPSL1GHT_BUILD
CXXFLAGS += -I. -I$(PSL1GHT)/ppu/include -I$(PSL1GHT)/ppu/include/simdmath
LDFLAGS += -Wl,--gc-sections

.PHONY: all clean sprx self

all: $(TARGET).self

$(TARGET).self: $(TARGET).elf
	@mkdir -p build
	@$(STRIP) $< -o build/$(TARGET).elf
	@$(SPRX) build/$(TARGET).elf
	@$(SELF) build/$(TARGET).elf $@
	@cp build/$(TARGET).elf $(TARGET).sprx
	@echo "Generated $(TARGET).sprx and $(TARGET).self"

$(TARGET).elf: $(OFILES)
	@$(LD) $(OFILES) $(LDFLAGS) $(LIBPATHS) $(LIBS) -o $@

clean:
	rm -rf *.o $(TARGET).elf $(TARGET).sprx $(TARGET).self build
