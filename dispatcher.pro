TEMPLATE = subdirs

SUBDIRS += \
          dispatcher \
          schedulers \
          generator \
          tests \
          widgets

dispatcher.depends = schedulers generator widgets
generator.depends = schedulers
tests.depends = schedulers
widgets.depends = schedulers
