write_checkpoint -force ./Netlists/Config1/static1.dcp

opt_design
place_design
route_design

write_checkpoint ./Netlists/Config1/static1_routed.dcp
