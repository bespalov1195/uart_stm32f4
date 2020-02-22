BUILD_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))


SPL_PATH = $(BUILD_ROOT)/stm_spl/
COMMON_PATH = $(BUILD_ROOT)/common
TASK_DIR = $(BUILD_ROOT)/projects/Task_1

.PHONY: task spl common clean

all: task

spl:
	make -C $(SPL_PATH)

common:
	make -C $(COMMON_PATH)

task: spl common
	make -C $(TASK_DIR)

clean:
	make -C $(SPL_PATH) clean
	make -C $(COMMON_PATH) clean
	make -C $(TASK_DIR) clean
