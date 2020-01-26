TEMPLATE = subdirs

SUBDIRS += \
          widgets \
          schedulers \
          generator \
          dispatcher \
          tests \
          taskbuilder

dispatcher.depends = schedulers generator widgets
generator.depends = schedulers
tests.depends = schedulers
widgets.depends = schedulers
taskbuilder.depends = schedulers widgets
