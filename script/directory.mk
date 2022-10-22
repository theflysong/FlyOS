#
# SPDX-License-Identifier: LGPL-2.1-only
# -------------------------------*-TayhuangOS-*-----------------------------------
# 
#    Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
# 
# --------------------------------------------------------------------------------
# 
# 作者: theflysong 383494
# 
# directory.mk
# 
# 目录
# 
#

BUILDDIR := $(ROOTDIR)/build/$(MODE)/
BINDIR := $(BUILDDIR)/bin/
OBJECTSDIR := $(BUILDDIR)/objects/
TAYHUANGOS_MOUNT_DIR := /mnt/tayhuangOS
TAYHUANGOS_IMG := tayhuangOS.img

export BUILDDIR OBJECTSDIR TAYHUANGOS_MOUNT_DIR TAYHUANGOS_IMG