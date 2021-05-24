create_pblock pblock_inst
add_cells_to_pblock [get_pblocks pblock_inst] [get_cells -quiet [list bd_saa_i/SAA_Recon_0/inst]]
resize_pblock [get_pblocks pblock_inst] -add {SLICE_X50Y0:SLICE_X113Y74}
resize_pblock [get_pblocks pblock_inst] -add {DSP48_X3Y0:DSP48_X4Y29}
resize_pblock [get_pblocks pblock_inst] -add {RAMB18_X3Y0:RAMB18_X5Y29}
resize_pblock [get_pblocks pblock_inst] -add {RAMB36_X3Y0:RAMB36_X5Y14}
set_property RESET_AFTER_RECONFIG true [get_pblocks pblock_inst]
set_property SNAPPING_MODE ON [get_pblocks pblock_inst]
set_property HD.RECONFIGURABLE true [get_cells bd_saa_i/SAA_Recon_0/inst]
