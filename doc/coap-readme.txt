:toc2:
:toclevels: 4
:max-width: 100%

SSDP and CoAP in BOLT
=====================

 Introduction
-------------

Simple Service Discovery Protocol (SSDP) and Constrained Application Protocol
(CoAP) allow controlling BOLT through Ethernet LAN without requiring serial
port.

SSDP allows STB to be discovered by control points in LAN.
CoAP lets BOLT commands get invoked through LAN.

 Build
------

Both SSDP and CoAP get compiled by default.


 Runtime
--------

Although SSDP and CoAP are built by default they are not enabled at runtime.
To turn both on, set the following environment variable and restart STB.

----
setenv -p COAP_SSDP_ENABLE 1
----

Now SSDP control points can discover STB, and CoAP clients can invoke BOLT
commands using the CoAP server running on BOLT.

NOTE: SSDP and CoAP services are available only in BOLT prompt. Once BOLT
launches kernel, these services are not available.


 Bolt-admin desktop app
-----------------------

Bolt-admin is a desktop application that can run on Windows/Linux/Mac.
It can discover and control Broadcom STBs, that have SSDP and CoAP enabled and
active in BOLT.

https://app.box.com/s/30344pe1876m554icv5jj3z9c4cr3vn5[Bolt-admin binary]


 Implementation
---------------

SSDP implementation can be found at ssbl/net/net_ssdp.c. SSDP message formats
are as defined in UPnP device architecture v1.1.

SSDP discovery is only intended to work with desktop app Bolt-admin. Other UPnP
control points may not be able to discover STB, as the complete UPnP discovery
guidelines are not implemented.

BOLT's CoAP server is implemented using libCoap v4.1.1 plus additional changes
to interface with BOLT's networking stack.


 Startup behavior
-----------------

BOLT executes STARTUP environment variable or sysinit.txt at startup.
If startup command happens to launch kernel, then CoAP clients
and SSDP control points will never get a chance to talk to BOLT.

To overcome this, BOLT allows a 100ms window (can be configured using
STARTUP_WAIT_TIME environment variable) for CoAP clients to send a CoAP halt
command. If BOLT receives the halt command during that window, startup command
will not be executed, and will remain in BOLT prompt.


 Network interface
------------------

The default network interface name to be used is "eth0". On boards that do not
have built-in Ethernet port, a USB-Ethernet dongle should be used. Because a
USB-Ethernet dongle should be enumerated before being registered, there might
be an extra delay up to 1 second if "eth0" does not exist.

NOTE: If eth0 is available but is internally disabled then network
initialization will fail. This scenario needs to be handled in future.


 Resources
----------

http://upnp.org/specs/arch/UPnP-arch-DeviceArchitecture-v1.1.pdf[UPnP Spec]

https://libcoap.net[libCoAP]


Appendix: Copyright Info
------------------------

Copyright (C) 2017, Broadcom.
