==========================
Host - RPU Interface API's
==========================


RPU API's
----------

The RPU API's provide an interface for the upper edge of the host driver to
communicate with the RPU.

Communication with the RPU takes place mainly using messages:
a. From the Host to RPU called "Commands", and
b. From the RPU to Host called "Events"

These messages also form the basis for signaling data packet transfer.


.. toctree::
   :maxdepth: 2

   img/img_rpu_api.rst

Hardware Abstraction Layer (HAL) API's
--------------------------------------

The HAL layer is a layer within the host driver which is responsible for
transporting control messages (i.e. commands and events) between the host and
the LMAC firmware.

The flow of messages between the Host and the LMAC are managed by a
hardware construct called Host Port Queue Manager (HPQM).

The HAL layer provides an interface which allows the lower edge of the host
driver to communicate with the LMAC.

The transport method used by the HAL is known as the “hostport” method. This
method uses an area of GRAM as a shared memory mailbox for storing messages
as they are transferred between host and firmware. The LMAC hardware provides
registers for facilitating communication between host and firmware.


.. toctree::
   :maxdepth: 2

   img/img_hal_api.rst

