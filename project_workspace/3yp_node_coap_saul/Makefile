# name of your application
APPLICATION = 3yp_node_coap_saul

# If no BOARD is found in the environment, use this default:
BOARD = 3yp-node
DEFAULT_CHANNEL = 0

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../RIOT/

# include to use shell and print ips
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps
USEMODULE += printf_float
USEMODULE += xtimer

# example uses saul
USEMODULE += saul_default

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Activate ICMPv6 error messages
USEMODULE += gnrc_icmpv6_error
# Specify the mandatory networking modules for IPv6 and UDP
USEMODULE += gnrc_ipv6_router_default # MUST HAVE ONE ROUTER
USEMODULE += gnrc_udp
USEMODULE += gnrc_sock_udp

# Say what PID expect to have radio on so can set power
CFLAGS += -DPHYS_PID=6

# Add rpl routing
USEMODULE += gnrc_rpl
USEMODULE += auto_init_gnrc_rpl
# CFLAGS += -DCONFIG_GNRC_RPL_DEFAULT_NETIF=6

# Additional networking modules that can be dropped if not needed
USEMODULE += gnrc_icmpv6_echo

# include nanocoap socket so can run coap server
USEMODULE += nanocoap_sock

# include this for nicely formatting the returned internal value
USEMODULE += fmt

# Comment this out to enable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP = 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include

# Set a custom channel if needed
include $(RIOTMAKE)/default-radio-settings.inc.mk
