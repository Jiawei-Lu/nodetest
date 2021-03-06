BOARD ?= iotlab-m3

include ../Makefile.tests_common

# list of arm boards that provide at86rf2xx radios, can't require it so
# add whitelist
BOARD_WHITELIST = \
  fox \
  iotlab-m3 \
  iotlab-a8-m3 \
  samr21-xpro \
  samr30-xpro \
  #

## OpenWSN Modules
USEPKG += openwsn
USEMODULE += openwsn_openstack
USEMODULE += openwsn_scheduler

# Optional Module required for root nodes
# USEMODULE += openwsn_serial

# Optional Modules
USEMODULE += openwsn_leds
USEMODULE += openwsn_debugpins

ifneq (,$(filter openwsn_serial,$(USEMODULE)))
# Uncomment to use STDIO_UART_DEV as the uart for OpenWSN openserial
# USEMODULE += stdio_null
  ifneq (,$(filter iotlab-m3 iotlab-a8-m3,$(BOARD)))
    USEMODULE += stdio_null
  endif
  # OpenWSN serial module can't handle data at more than 115200 bauds/s,
  # depending on the platform this might be even lower (e.g. 57600 bauds
  # for iotlab-m3, 19200 for samr21-xpro).
  # NOTE: baudrate can'y be changed when testing over IotLab.
  OPENSERIAL_BAUD ?=
  ifneq (,$(OPENSERIAL_BAUD))
    CFLAGS += -DOPENWSN_UART_BAUDRATE=$(OPENSERIAL_BAUD)
    ifneq (,$(filter stdio_null,$(USEMODULE)))
      BAUD ?= $(OPENSERIAL_BAUD)
    endif
  endif
endif

# To enable debugging prints on OpenWSN code
# export OPENWSN_LOG_LEVEL ?= LOG_ERROR

## Test application Modules
USEMODULE += ipv6_addr
USEMODULE += ps
USEMODULE += od_string
USEMODULE += shell
USEMODULE += shell_commands
# ztimer is used instead of xtimer because it's a dependency of some
# OpenWSN modules.
USEMODULE += ztimer_usec

include $(RIOTBASE)/Makefile.include

# We want the highest possible frequency set for periph_rtt, but not all
# platforms can configure this value. use highest possible RTT_FREQUENCY
# for platforms that allow it
ifneq (,$(filter stm32,$(CPU)))
  RTT_FREQUENCY ?= RTT_MAX_FREQUENCY
  CFLAGS += -DRTT_FREQUENCY=$(RTT_FREQUENCY)
endif
