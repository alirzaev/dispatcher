TEMPLATE = subdirs

SUBDIRS += \
          qtutils \
          widgets \
          schedulers \
          generator \
          dispatcher \
          tests \
          taskbuilder

dispatcher.depends = schedulers generator qtutils widgets
generator.depends = schedulers
tests.depends = schedulers
widgets.depends = schedulers qtutils
taskbuilder.depends = schedulers qtutils widgets
