update_design -cell [get_cells bd_dpr_i/SAA_Recon_0/inst] -black_box
lock_design -level routing

write_checkpoint ./Netlists/Static/static_routed.dcp