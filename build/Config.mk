# --- User Defined Configurations ----

# Kernel Configuration

USE_FRAMEBUFFER ?= 1
SMALL_PAGES ?= 0

# PIN_LOGO set to 0 if we want the logo to be scrolled.
PIN_LOGO ?= 1

# Build Configuration

TOOLCHAIN ?= clang
BUILD_FOLDER ?= dist
FONT_FOLDER ?= fonts

# Image Base Name

IMAGE_BASE_NAME ?= DreamOs64
