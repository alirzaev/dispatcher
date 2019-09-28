TEMPLATE = subdirs

SUBDIRS += \
          dispatcher \
          schedulers \
          generator \
          tests

dispatcher.depends = schedulers generator
generator.depends = schedulers
tests.depends = schedulers
