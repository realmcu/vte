#
# Copyright (C) 2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

CC = $(CROSS_COMPILE)gcc
CFLAGS += -I../../../include/ -I$(KLINUX_SRCDIR)/include -trigraphs
LDFLAGS += -L../../../lib -lltp -lpthread

TARGET = epdc_test
TARGET_SCRIPT = epdc_full.sh epdc_REAGL_full.sh
SRC = epdc_main.c epdc_test.c diasy.c
HEADS = epdc_test.h
OBJ=$(patsubst %.c,%.o,$(SRC))

app-epd_DIR = apps-epd/linux/demo

demoColorEpd_SRS_PATH = $(app-epd_DIR)/democolorepd
CFLAGS_demoColorEpd = $(CFLAGS)
CFLAGS_demoColorEpd +=  -I../../../openlibs/include -I$(demoColorEpd_SRS_PATH)
LDFLAGS_demoColorEpd += -L../../../openlibs/lib -static -lpng -ljpeg -lz -lm

demoColorEpd_SRS = $(wildcard $(demoColorEpd_SRS_PATH)/*.c)
demoColorEpd_SRS_nodir = $(notdir $(demoColorEpd_SRS))
demoColorEpd_OBJS = $(patsubst %.c,%.o,$(demoColorEpd_SRS_nodir))

demomx6slepdc_path = $(app-epd_DIR)/demomx6slepdc
CFLAGS_demomx6slepdc = $(CFLAGS)
CFLAGS_demomx6slepdc += -I$(demomx6slepdc_path) -I./include
LDFLAGS_demomx6slepdc =

demomx6slepdc_SRS = $(wildcard $(demomx6slepdc_path)/*.c)
demomx6slepdc_SRS_nodir = $(notdir $(demomx6slepdc_SRS))
demomx6slepdc_OBJS = $(patsubst %.c,%.o,$(demomx6slepdc_SRS_nodir))

fastpageturn_path = $(app-epd_DIR)/fastpageturn
CFLAGS_fastpageturn = $(CFLAGS)
CFLAGS_fastpageturn += -I$(fastpageturn_path) -I./include
LDFLAGS_fastpageturn =

fastpageturn_SRS = $(wildcard $(fastpageturn_path)/fastpageturn.c)
fastpageturn_SRS_nodir = $(notdir $(fastpageturn_SRS))
fastpageturn_OBJS = $(patsubst %.c,%.o,$(fastpageturn_SRS_nodir))

handwriting_path = $(app-epd_DIR)/handwriting
CFLAGS_handwriting = $(CFLAGS)
CFLAGS_handwriting += -I$(handwriting_path)
LDFLAGS_handwriting += -lpthread

handwriting_SRS = $(wildcard $(handwriting_path)/*.c)
handwriting_SRS_nodir = $(notdir $(handwriting_SRS))
handwriting_OBJS = $(patsubst %.c,%.o,$(handwriting_SRS_nodir))

highlight_path = $(app-epd_DIR)/highlight
CFLAGS_highlight = $(CFLAGS)
CFLAGS_highlight += -I$(highlight_path)
LDFLAGS_highlight += -lpthread

highlight_SRS = $(wildcard $(highlight_path)/*.c)
highlight_SRS_nodir = $(notdir $(highlight_SRS))
highlight_OBJS = $(patsubst %.c,%.o,$(highlight_SRS_nodir))

ifeq (apps-epd/linux, $(wildcard apps-epd/linux))
all: epdc
else
all: epdc
endif

epdc: $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS)

install:
	-ln -f $(TARGET) ../../bin/
	-ln -f $(TARGET_SCRIPT) ../../bin/
	-ln -f democolorepd demomx6slepdc fastpageturn handwriting highlight  ../../bin/

clean: clean_democolorepd clean_demomx6slepdc \
clean_fastpageturn clean_handwriting clean_highlight
	rm -f *.o
	rm -f $(TARGET)

$(OBJ): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

apps-epd: democolorepd demomx6slepdc fastpageturn handwriting highlight
	
democolorepd: $(demoColorEpd_OBJS) hook.o
	$(CC) -o $@ $^ $(LDFLAGS_demoColorEpd)

hook.o: hook.c
	$(CC) $(CFLAGS_demoColorEpd) -c $< -o $@

$(demoColorEpd_OBJS):  %.o: $(demoColorEpd_SRS_PATH)/%.c
	$(CC) $(CFLAGS_demoColorEpd) -c $< -o $@

clean_democolorepd:
	-rm -rf democolorepd $(demoColorEpd_OBJS) hook.o

demomx6slepdc: $(demomx6slepdc_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS_demomx6slepdc)

$(demomx6slepdc_OBJS):  %.o: $(demomx6slepdc_path)/%.c
	$(CC) $(CFLAGS_demomx6slepdc) -c $< -o $@

clean_demomx6slepdc:
	-rm -rf demomx6slepdc $(demomx6slepdc_OBJS)

fastpageturn: $(fastpageturn_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS_fastpageturn)

$(fastpageturn_OBJS):  %.o: $(fastpageturn_path)/%.c
	$(CC) $(CFLAGS_fastpageturn) -c $< -o $@

clean_fastpageturn:
	 -rm -rf fastpageturn $(fastpageturn_OBJS)

handwriting: $(handwriting_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS_handwriting)

$(handwriting_OBJS):  %.o: $(handwriting_path)/%.c
	$(CC) $(CFLAGS_handwriting) -c $< -o $@

clean_handwriting:
	 -rm -rf handwriting $(handwriting_OBJS)

highlight: $(highlight_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS_highlight)

$(highlight_OBJS):  %.o: $(highlight_path)/%.c
	$(CC) $(CFLAGS_highlight) -c $< -o $@

clean_highlight:
	 -rm -rf highlight $(highlight_OBJS)

