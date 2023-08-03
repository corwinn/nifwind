# BSD 3-Clause License
#
# Copyright (c) 2023, the wind.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# **** END LICENCE BLOCK ****/

# Requirements: grep
# Please setup your QTDIR prior issuing this makefile.
# Please setup FFDIR prior issuing this makefile.
# TODO CMakeLists.txt

MAKEFLAGS += rR

APP  = nifwind
MOC = $(QTDIR)/bin/moc
CC  = ccache clang
LD  = clang++
FFD_CFLAGS = -I${FFDIR} -I${FFDIR}/stl
FFD_LDFLAGS = -L${FFDIR} -Wl,-rpath -Wl,${FFDIR}
CXXFLAGS = -std=c++14 -march=core2 -mtune=core2 \
 -Wall -Wextra -Wshadow -Werror=shadow -Wno-deprecated-copy \
 -fvisibility=hidden -frtti -fno-exceptions -pipe -O0 -g
SAN = -fsanitize=undefined,leak,address
CXXFLAGS += $(SAN) -std=c++14 \
 -DNIFWIND_VERSION="\"1.0\"" \
 -I. -Isrc -Isrc/services $(FFD_CFLAGS) \
 -I${QTDIR}/include -I${QTDIR}/include/QtCore -I${QTDIR}/include/QtWidgets \
 -I${QTDIR}/include/QtOpenGL -I${QTDIR}/include/QtGui \
 -DQT_OPENGL_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB \
 -DQT_CORE_LIB -UQT_NO_CAST_FROM_ASCII -UQT_RESTRICTED_CAST_FROM_ASCII \
 -DGL_GLEXT_PROTOTYPES -DGL_2_0=1
LDFLAGS = $(SAN) -Wl,--as-needed -L${QTDIR}/lib $(FFD_LDFLAGS) \
 -lQt5Core -lQt5Widgets -lQt5OpenGL -lQt5Gui -lGL -lpthread -lwind-ffd

SRC := $(patsubst %.h,%.moc.cpp,$(shell grep --exclude=*~ -rl '\(signal\|slot\)s:'))
SRC += $(shell grep -rvl --include=*.cpp --exclude=*.moc.cpp '\(signal\|slot\)s:')
SRC := $(filter-out c++/%, $(SRC))

OBJ := $(patsubst %.cpp, %.o, $(SRC))

.PHONY: clean test cake

%.o: %.cpp
	@echo " <> CC "$<
	@$(CC) $(CXXFLAGS) $< -c -o $@
%.moc.cpp: %.h
	@echo " <> MOC "$@
	@$(MOC) $< -o $@

$(APP): config.h $(OBJ)
	@echo " <> LD "$@
	$(LD) $(OBJ) -o $(APP) $(LDFLAGS)

config.h:
	@echo "#error Implement me" > config.h

clean:
	@find -type f -iname "*.o" -delete
	@find -type f -iname "*~" -delete
	@find -type f -iname "*.moc.cpp" -delete
	@rm -f $(APP)

test:
	@echo $(SRC)
	@echo $(OBJ)

cake: universe
