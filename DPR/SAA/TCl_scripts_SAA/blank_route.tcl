open_checkpoint ./Netlists/Static/static_routed.dcp
update_design -buffer_ports -cell bd_dpr_i/SAA_Recon_0/inst
place_design
route_design

mkdir ./Netlists/Blank/
write_checkpoint -force ./Netlists/Blank/blank_routed.dcp
