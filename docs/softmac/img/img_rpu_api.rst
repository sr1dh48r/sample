RPU API Details
===============

RPU message format used for commands and events
-----------------------------------------------

All RPU messages are encapsulated in the following format:

.. flat-table:: Host-RPU message format
   :widths: 3 1 1 1 1 1 1

   * - struct host_rpu_msg
     - cmd
     - cmd
     - cmd
     -  .
     -  .
     -  .

Based on the type the control messages are categorized mainly into two types:

a. Data path and System messages, and
b. Control path messages





RPU API Flow
------------





RPU API reference
-----------------

.. kernel-doc:: drivers/net/wireless/img/inc/rpu_if.h
.. kernel-doc:: drivers/net/wireless/img/shared/inc/descriptors.h
.. kernel-doc:: drivers/net/wireless/img/shared/inc/lmac_if.h
.. kernel-doc:: drivers/net/wireless/img/shared/inc/hpqm_if.h
.. kernel-doc:: drivers/net/wireless/img/shared/inc/lmac_measurement.h
