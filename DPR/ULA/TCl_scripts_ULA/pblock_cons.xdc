create_pblock pblock_inst
add_cells_to_pblock [get_pblocks pblock_inst] [get_cells -quiet [list design1_PR_i/ULA_Reconfig_0/inst]]
resize_pblock [get_pblocks pblock_inst] -add {SLICE_X50Y50:SLICE_X113Y99}
resize_pblock [get_pblocks pblock_inst] -add {DSP48_X3Y20:DSP48_X4Y39}
resize_pblock [get_pblocks pblock_inst] -add {RAMB18_X3Y20:RAMB18_X5Y39}
resize_pblock [get_pblocks pblock_inst] -add {RAMB36_X3Y10:RAMB36_X5Y19}
set_property RESET_AFTER_RECONFIG true [get_pblocks pblock_inst]
set_property SNAPPING_MODE ON [get_pblocks pblock_inst]
set_property HD.RECONFIGURABLE true [get_cells design1_PR_i/ULA_Reconfig_0/inst]
