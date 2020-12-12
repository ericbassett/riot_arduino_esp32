# a minimal application Makefile
APPLICATION = myapp
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
USEMODULE += nina_w102

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