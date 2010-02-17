#!/usr/bin/env python

import sys
import PyLorcon2

def functionTest(functionName, returnValue, paramList=None):
  print " * %s" % functionName
  print "  - [Parameters  ]: %s" % repr(paramList)
  print "  - [Return Type ]: %s" % repr(type(returnValue))
  print "  - [Return Value]: %s" % repr(returnValue)

def testModuleMethods(iface, driver):
  "Test PyLorcon2 module methods."
  # lorcon_get_version
  functionTest("get_version()", PyLorcon2.get_version())
  # lorcon_list_drivers
  functionTest("list_drivers()", PyLorcon2.list_drivers())
  # lorcon_list_driver
  functionTest("find_driver()", PyLorcon2.find_driver(driver), [driver])
  # lorcon_auto_driver
  functionTest("auto_driver()", PyLorcon2.auto_driver(iface), [iface])

def simpleInjectionTest(iface, data):
  "Simple Lorcon2 Injection Test."
  c = PyLorcon2.Context(iface)
  c.open_injmon()
  c.send_bytes(data)


def simpleTimeoutTest(iface, timeout):
  "Simple Set Timeout test"
  c = PyLorcon2.Context(iface)
  c.set_timeout(timeout) 
  t = c.get_timeout()

  return (t == timeout)

def simpleVapTest(iface, vap):
  "Simple vap test"
  c = PyLorcon2.Context(iface)
  c.set_vap(vap) 
  v = c.get_vap()
  
  return (v == vap)

def simpleGetDriverNameTest(iface):
  "Test Driver name"
  c = PyLorcon2.Context(iface)
  drv = c.get_driver_name()
  return drv

def simpleSetChannelTest(iface, channel):
  "Set Channel Test"
  c = PyLorcon2.Context(iface)
  c.open_monitor()  
  c.set_channel(1)
  chan = c. get_channel()

  return (chan == channel)


if __name__ == "__main__":
  if len(sys.argv) != 2:
    print "Usage:"
    print "\t%s <iface>" % sys.argv[0]
    sys.exit(-1)

  # Tests
  iface  = sys.argv[1]
  driver = "mac80211" # only testing mac80211 driver for now

  # Test Module Methods
  print "\n== Module Tests =="
  testModuleMethods(iface, driver)

  # data is a beacon packet with bssid == 00:21:21:21:21:21
  data = "\x80\x00\x00\x00\xff\xff\xff\xff\xff\xff\x00\x21\x21" \
         "\x21\x21\x21\x00\x21\x21\x21\x21\x21\x90\x83\x50\x8c" \
         "\xf4\x38\x23\x00\x00\x00\x64\x00\x11\x04\x00\x04XXXX" \
         "\x01\x08\x82\x84\x8b\x96\x24\x30\x48\x6c\x03\x01\x01" \
         "\x32\x04\x0c\x12\x18\x60"

  print "\n== Lorcon Context Tests =="
  
  
  simpleInjectionTest(iface, data)
  print " * Injection Test PASSED (sniff to check!)"

  if simpleTimeoutTest(iface, 123):
    print " * Timeout Test OK"
  
  if simpleVapTest(iface, "wlan0" ): 
    print " * Vap Test OK"
  
  driver = simpleGetDriverNameTest(iface)
  print " * Get Driver Name Test: %s" % driver

  if not simpleSetChannelTest(iface, 1):
    print " * Set Shannel Test FAILED"
  else:
    print " * Set Shannel Test OK"


