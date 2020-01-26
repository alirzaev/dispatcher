TEMPLATE = subdirs

SUBDIRS += \
          qtutils \
          widgets \
          schedulers \
          generator \
          dispatcher \
          tests

dispatcher.depends = schedulers generator qtutils widgets
generator.depends = schedulers
tests.depends = schedulers
widgets.depends = schedulers qtutils
