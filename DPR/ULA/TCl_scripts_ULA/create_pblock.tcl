startgroup
create_pblock pblock_inst
resize_pblock pblock_inst -add {SLICE_X50Y50:SLICE_X113Y99 DSP48_X3Y20:DSP48_X4Y39 RAMB18_X3Y20:RAMB18_X5Y39 RAMB36_X3Y10:RAMB36_X5Y19}
add_cells_to_pblock pblock_inst [get_cells [list bd_dpr_i/ULA_Recon_0/inst]] -clear_locs
endgroup

set_property RESET_AFTER_RECONFIG 1 [get_pblocks pblock_inst]
set_property SNAPPING_MODE ON [get_pblocks pblock_inst]
file mkdir C:/DPR_MUSIC/Tutorial/ULA/DPR_proj/DPR_proj.srcs/constrs_1/new
close [ open C:/DPR_MUSIC/Tutorial/ULA/DPR_proj/DPR_proj.srcs/constrs_1/new/pblock_cons.xdc w ]
add_files -fileset constrs_1 C:/DPR_MUSIC/Tutorial/ULA/DPR_proj/DPR_proj.srcs/constrs_1/new/pblock_cons.xdc
set_property target_constrs_file C:/DPR_MUSIC/Tutorial/ULA/DPR_proj/DPR_proj.srcs/constrs_1/new/pblock_cons.xdc [current_fileset -constrset]
save_constraints -force