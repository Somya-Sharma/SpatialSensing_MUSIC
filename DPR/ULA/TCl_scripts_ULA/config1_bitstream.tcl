write_bitstream -force ./Bitstreams/Config1/config1.bit
write_cfgmem -force -format BIN -interface SMAPx32 -disablebitswap -loadbit "up 0x0 ./Bitstreams/Config1/config1_pblock_inst_partial.bit" "./Bin_files/config1.bin"
close_project