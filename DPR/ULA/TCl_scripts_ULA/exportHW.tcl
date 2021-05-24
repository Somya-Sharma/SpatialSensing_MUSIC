save_constraints
file mkdir C:/DPR_MUSIC/Tutorial/SAA/DPR_proj/DPR_proj.sdk
write_hwdef -force  -file C:/DPR_MUSIC/Tutorial/SAA/DPR_proj/DPR_proj.sdk/bd_dpr_wrapper.hdf

launch_sdk -workspace C:/DPR_MUSIC/Tutorial/SAA/DPR_proj/DPR_proj.sdk -hwspec C:/DPR_MUSIC/Tutorial/SAA/DPR_proj/DPR_proj.sdk/bd_dpr_wrapper.hdf
close_project