TEMPLATE = subdirs

SUBDIRS += \
          widgets \
          schedulers \
          generator \
          dispatcher \
          tests

dispatcher.depends = schedulers generator widgets
generator.depends = schedulers
tests.depends = schedulers
widgets.depends = schedulers
