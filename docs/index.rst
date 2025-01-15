Motis utl module documentation
==============================

MOTIS is an open-source software platform for efficient planning and routing in multi-modal transportation systems.
GitHub main repository: https://github.com/motis-project/motis

This is the documentation for the `utl` (utility) module.

..
   Table of contents
   -----------------

   .. toctree::
      :maxdepth: 2
      :caption: Contents:

Logging
-------

Logs can be produced using the `log()` `struct`::

  utl::log() << "[" << utl::log::str["info"] << "]"        \
             << "[" << utl::time() << "]"                  \
             << "[" << FILE_NAME << ":" << __LINE__ << "]" \
             << " Some message"

.. doxygenstruct:: utl::log
   :members:
