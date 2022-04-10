#
# Rules for building application
#

# ----------------------------------------------------------------------------
# project local config
# ----------------------------------------------------------------------------
include localconfig.mk

# ----------------------------------------------------------------------------
# common rules
# ----------------------------------------------------------------------------
ROOT_PATH := ../../..

include $(ROOT_PATH)/gcc.mk

# ----------------------------------------------------------------------------
# project and objects
# ----------------------------------------------------------------------------
PROJECT := oxr_sharedApp

INCLUDE_PATHS += -I$(ROOT_PATH)/project/$(PROJECT)

DIRS_IGNORE := ../gcc% ../image% ../shared% $(ROOT_PATH)/project/common/board/%
DIRS_ALL := $(shell find .. $(ROOT_PATH)/project/common -type d)
DIRS := $(filter-out $(DIRS_IGNORE),$(DIRS_ALL))
DIRS += $(ROOT_PATH)/project/common/board/$(__PRJ_CONFIG_BOARD)

SRCS := $(basename $(foreach dir,$(DIRS),$(wildcard $(dir)/*.[csS])))
SRCS += ../shared/src/new_cfg
SRCS += ../shared/src/new_pins
SRCS += ../shared/src/new_common
SRCS += ../shared/src/new_builtin_devices
SRCS += ../shared/src/httpserver/new_http
SRCS += ../shared/src/httpserver/http_tcp_server
SRCS += ../shared/src/httpserver/http_fns
SRCS += ../shared/src/httpserver/rest_interface
SRCS += ../shared/src/logging/logging
SRCS += ../shared/src/jsmn/jsmn
SRCS += ../shared/src/mqtt/new_mqtt
SRCS += ../shared/src/tiny_crc8
SRCS += ../shared/src/new_cmd
SRCS += ../shared/src/drv_tuyaMCU
SRCS += ../shared/src/new_tokenizer


$(info    Now will print SCRS list)
$(info    SRCS is $(SRCS))
$(info    SCRs list printed)

OBJS := $(addsuffix .o,$(SRCS))

# extra libs
PRJ_EXTRA_LIBS := -latcmd

# extra symbols (macros)
# PRJ_EXTRA_CONFIG_SYMBOLS :=

# ----------------------------------------------------------------------------
# override project variables
# ----------------------------------------------------------------------------
# linker script path/file
#   - relative to "./"
#   - define your own "LINKER_SCRIPT_PATH" and/or "LINKER_SCRIPT" to override
#     the default one
# LINKER_SCRIPT_PATH := .
LINKER_SCRIPT := ./xr809_appos.ld

# image config path/file
#   - relative to "../image/xxxxx/", eg. "../image/xr871/"
#   - define your own "IMAGE_CFG_PATH" and/or "IMAGE_CFG" to override the
#     default one
IMAGE_CFG_PATH := ./image/xr809/
IMAGE_CFG := image_auto_cal.cfg

# image name, default to xr_system
# IMAGE_NAME :=

# project make rules
include $(PRJ_MAKE_RULES)
