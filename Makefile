# a minimal application Makefile
APPLICATION = app_ctrl
BOARD ?= arduino-nano-33-iot
RIOTBASE ?= $(CURDIR)/../RIOT

# we want to use SAUL:
USEMODULE += saul_default
USEMODULE += auto_init_saul
USEMODULE += xtimer
# include the shell:
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += periph_spi
USEMODULE += periph_uart
USEMODULE += isrpipe_read_timeout
USEMODULE += periph_uart_hw_fc
USEMODULE += periph_dma
# USEMODULE += periph_uart_nonblocking
# USEMODULE += stdio_cdc_acm
DISABLE_MODULE += auto_init_usbus


#USB
USB_VID=${USB_VID_TESTING}
USB_PID=${USB_PID_TESTING}

#include networking

# additional modules for debugging:
USEMODULE += ps

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

include $(RIOTBASE)/Makefile.include
