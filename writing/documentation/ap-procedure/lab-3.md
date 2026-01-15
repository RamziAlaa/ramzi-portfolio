# Lab: Setup multiple SSIDs on different VLANs <!-- (will need to configure trunk ports on cisco switch) -->

## Objective

To learn how to simultaneously configure multiple networking devices at once in a practical scenario. To become accustomed to configuring Cisco switches and VLANs.

## Expected Time to Complete

<!-- 40-75 minutes. Setting up VLANs and configuring trunk ports can be finicky and mistakes are very likely to happen during the lab. Multiple sanity checks (checking if your networking works) will be made. -->

40-75 minutes. Figuring out how to set up multiple SSIDs within different VLANs can be finicky and mistakes are very likely to happen during the lab. Multiple sanity checks (checking if your networking works) will be made.

*However,* there should be a familiarity with setting up SSID's now. That part should not have too many issues.

If you come across a hiccup, don't hesitate to go to the professor or any assistant **as soon as possible** in order to save time.

## Definitions

### VLAN

A VLAN is a **Virtual Local Area Network** that divides multiple devices into separate *smaller* networks within a singular network.

### Trunk Port

<!-- Plug your cables in adjecent ports VERTICALLY. -->
<!-- Ports 1-12 are trunk ports -->

"A trunk port can have two or more VLANs configured on the interface; it can carry traffic for several VLANs simultaneously." 
(As described by [Cisco](https://www.cisco.com/c/en/us/td/docs/switches/datacenter/nexus5000/sw/layer2/503_n2_1/503_n2_1nw/Cisco_n5k_layer2_config_gd_rel_503_N2_1_chapter6.html#d5220e81_navtitle))