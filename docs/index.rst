MOTIS utl module documentation
==============================

MOTIS is an open-source software platform for efficient planning and routing in multi-modal transportation systems.
GitHub main repository: https://github.com/motis-project/motis

This is the documentation for the **utl** (utility) module.

..
   Table of contents
   -----------------

   .. toctree::
      :maxdepth: 2
      :caption: Contents:

Logging
-------

The simplest way to produce log lines is to use the ``logF()`` macro::

  logF(info, "Simple message");

The following log levels are supported:

debug
  Messages that contain information only useful when debugging MOTIS

info
  Important information about a normal behavior of the program

error
  Details on an abnormal behavior of the application

Advanced usage
^^^^^^^^^^^^^^

By default, ``logF()`` inserts the current filename & line number in the log line.
However, you can use ``log()`` to specify your own **context** ::

  log(info, "http.get.resource", "Details");

You can also insert variables in the message by using ``{}`` and passing them as extra arguments::

  logF(info, "String={} Int={}", "Hello", 42);

API details
^^^^^^^^^^^
Under the hood, the ``log()`` & ``logF()`` macros use the ``utl::log()`` function:

.. doxygenfunction:: utl::log
