=========
OSAL API
=========

OSAL API covers the various host API constructs (structures, functions, Ops etc) which would be necessary to interface with different OSes.


Structures
==========

This section covers the various structures needed for:

        a. Maintaining OSAL layer context information.
        b. Returning status of various operations carried out in the OSAL layer.
        c. Any other supporting functionality.

.. kernel-doc:: drivers/net/wireless/img/os_if/osal/inc/osal_structs.h


Functions
=========

This section covers the various functions needed for:

        a. Initializing/Deinitializing the OSAL layer.

These functions are expected to be called by any software which is going to use the OSAL layer.

.. kernel-doc:: drivers/net/wireless/img/os_if/osal/inc/osal_api.h



Ops
===

This section covers the various Ops that need to be implemented for:

        a. Porting the driver to a OS different than the reference implementation (Linux).

For porting the driver to a specific OS, a shim layer for that OS will need to be implemented, which will consist of functions that will provide equivalent functionality and plug into the Ops specified here.


.. kernel-doc:: drivers/net/wireless/img/os_if/osal/inc/osal_ops.h
