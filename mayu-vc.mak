############################################################## -*- Makefile -*-
#
# Makefile for mayu (Visual C++ 6.0)
#
#	make release version: nmake nodebug=1
#	make debug version: nmake
#
###############################################################################


!include <vc.mak>
!include <mayu-common.mak>

DEFINES		= -DSTRICT -DVERSION=""""$(VERSION)""""

LDFLAGS_1	= $(guilflags)
LDFLAGS_2	= $(dlllflags)

$(TARGET_1):	$(OBJS_1) $(RES_1) $(EXTRADEP_1)
	$(link) -out:$@ $(ldebug) $(LDFLAGS_1) $(OBJS_1) $(LIBS_1) $(RES_1)

$(TARGET_2):	$(OBJS_2) $(RES_2) $(EXTRADEP_2)
	$(link) -out:$@ $(ldebug) $(LDFLAGS_2) $(OBJS_2) $(LIBS_2) $(RES_2)

$(TARGET_3):	$(DLL_3)
