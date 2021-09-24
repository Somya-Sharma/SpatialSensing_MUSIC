mkdir ./Bitstreams/Blank/
write_bitstream -force ./Bitstreams/Blank/blank_routed.bit
write_cfgmem -force -format BIN -interface SMAPx32 -disablebitswap -loadbit "up 0x0 ./Bitstreams/Blank/blank_routed_pblock_inst_partial.bit" "./Bin_files/blanking.bin"
