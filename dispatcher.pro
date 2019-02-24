TEMPLATE = subdirs

SUBDIRS += \
          gui \
          schedulers \
          generator \
          tests

gui.depends = schedulers generator
generator.depends = schedulers
tests.depends = schedulers
