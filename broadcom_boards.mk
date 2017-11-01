
#########################################################################
## Broadcom - support for incremental compiles
#########################################################################

fpga911140_config: $(obj)fpga911140_config 
$(obj)fpga911140_config: $(TOPDIR)/include/configs/fpga911140.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm911160_tablet_config: $(obj)bcm911160_tablet_config 
$(obj)bcm911160_tablet_config: $(TOPDIR)/include/configs/bcm911160_tablet.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm91116z_tablet_config: $(obj)bcm91116z_tablet_config 
$(obj)bcm91116z_tablet_config: $(TOPDIR)/include/configs/bcm91116z_tablet.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm91116o_tablet_config: $(obj)bcm91116o_tablet_config 
$(obj)bcm91116o_tablet_config: $(TOPDIR)/include/configs/bcm91116o_tablet.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm911160sv_config: $(obj)bcm911160sv_config 
$(obj)bcm911160sv_config: $(TOPDIR)/include/configs/bcm911160sv.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm911160sv_mpg_config: $(obj)bcm911160sv_mpg_config 
$(obj)bcm911160sv_mpg_config: $(TOPDIR)/include/configs/bcm911160sv_mpg.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm91116osv_config: $(obj)bcm91116osv_config 
$(obj)bcm91116osv_config: $(TOPDIR)/include/configs/bcm91116osv.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm911160sv_li_config: $(obj)bcm911160sv_li_config 
$(obj)bcm911160sv_li_config: $(TOPDIR)/include/configs/bcm911160sv_li.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

cp_tablet_config: $(obj)cp_tablet_config 
$(obj)cp_tablet_config: $(TOPDIR)/include/configs/cp_tablet.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm91007as_hr_config: $(obj)bcm91007as_hr_config 
$(obj)bcm91007as_hr_config: $(TOPDIR)/include/configs/bcm91007as_hr.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

iproc_em_config: $(obj)iproc_em_config 
$(obj)iproc_em_config: $(TOPDIR)/include/configs/iproc_em.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_config: $(obj)bcm95301x_svk_config 
$(obj)bcm95301x_svk_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_iproc_config: $(obj)bcm95301x_svk_iproc_config 
$(obj)bcm95301x_svk_iproc_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_iproc_dual_core_config: $(obj)bcm95301x_svk_iproc_dual_core_config 
$(obj)bcm95301x_svk_iproc_dual_core_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_iproc_perf_config: $(obj)bcm95301x_svk_iproc_perf_config 
$(obj)bcm95301x_svk_iproc_perf_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_iproc_diag_config: $(obj)bcm95301x_svk_iproc_diag_config 
$(obj)bcm95301x_svk_iproc_diag_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_iproc_nand_config: $(obj)bcm95301x_svk_iproc_nand_config 
$(obj)bcm95301x_svk_iproc_nand_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

venom_nand_config:
$(obj)venom_nand_config: $(TOPDIR)/include/configs/venom.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

venom_nand_backup_config:
$(obj)venom_nand_backup_config: $(TOPDIR)/include/configs/venom.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

venom_ram_config:
$(obj)venom_ram_config: $(TOPDIR)/include/configs/venom.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_iproc_ddr775_config: $(obj)bcm95301x_svk_iproc_ddr775_config 
$(obj)bcm95301x_svk_iproc_ddr775_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@
#DDR-EYE-SHMOO: added
bcm95301x_svk_iproc_ddr775_rw_eye_config: $(obj)bcm95301x_svk_iproc_ddr775_rw_eye_config 
$(obj)bcm95301x_svk_iproc_ddr775_rw_eye_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_iproc_ddr775_rw_eye_pwm_config: $(obj)bcm95301x_svk_iproc_ddr775_rw_eye_pwm_config 
$(obj)bcm95301x_svk_iproc_ddr775_rw_eye_pwm_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_l2cram_config: $(obj)bcm95301x_svk_l2cram_config 
$(obj)bcm95301x_svk_l2cram_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

helix4_emul_config: $(obj)helix4_emul_config 
$(obj)helix4_emul_config: $(TOPDIR)/include/configs/helix4_emul.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

helix4_config: $(obj)helix4_config 
$(obj)helix4_config: $(TOPDIR)/include/configs/helix4.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

helix4_nand_config: $(obj)helix4_nand_config 
$(obj)helix4_nand_config: $(TOPDIR)/include/configs/helix4.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

northstar_plus_emul_config: $(obj)northstar_plus_emul_config 
$(obj)northstar_plus_emul_config: $(TOPDIR)/include/configs/northstar_plus_emul.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

northstar_plus_config: $(obj)northstar_plus_config 
$(obj)northstar_plus_config: $(TOPDIR)/include/configs/northstar_plus.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

northstar_plus_32_config: $(obj)northstar_plus_32_config 
$(obj)northstar_plus_32_config: $(TOPDIR)/include/configs/northstar_plus.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

northstar_plus_stdk_config: $(obj)northstar_plus_stdk_config 
$(obj)northstar_plus_stdk_config: $(TOPDIR)/include/configs/northstar_plus.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

northstar_plus_nand_config: $(obj)northstar_plus_nand_config 
$(obj)northstar_plus_nand_config: $(TOPDIR)/include/configs/northstar_plus.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@
	
northstar_plus_diag_config: $(obj)northstar_plus_diag_config 
$(obj)northstar_plus_diag_config: $(TOPDIR)/include/configs/northstar_plus.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

northstar_plus_diag_svk_mfg_config: $(obj)northstar_plus_diag_svk_mfg_config 
$(obj)northstar_plus_diag_svk_mfg_config: $(TOPDIR)/include/configs/northstar_plus.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

northstar_plus_mdk_config: $(obj)northstar_plus_mdk_config 
$(obj)northstar_plus_mdk_config: $(TOPDIR)/include/configs/northstar_plus.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

hurricane2_emul_config: $(obj)hurricane2_emul_config 
$(obj)hurricane2_emul_config: $(TOPDIR)/include/configs/hurricane2.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

hurricane2_config: $(obj)hurricane2_config 
$(obj)hurricane2_config: $(TOPDIR)/include/configs/hurricane2.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

hurricane2_nand_config: $(obj)hurricane2_nand_config 
$(obj)hurricane2_nand_config: $(TOPDIR)/include/configs/hurricane2.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

hurricane2_nor_config: $(obj)hurricane2_nor_config
$(obj)hurricane2_nor_config: $(TOPDIR)/include/configs/hurricane2.h
	@$(MKCONFIG) -A $(@F:_config=); touch $@

hurricane2_noshmoo_config: $(obj)hurricane2_noshmoo_config 
$(obj)hurricane2_noshmoo_config: $(TOPDIR)/include/configs/hurricane2.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

hurricane2_rom_emul_config: $(obj)hurricane2_rom_emul_config 
$(obj)hurricane2_rom_emul_config: $(TOPDIR)/include/configs/hurricane2_rom_emul.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

katana2_emul_config: $(obj)katana2_emul_config 
$(obj)katana2_emul_config: $(TOPDIR)/include/configs/katana2.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

katana2_config: $(obj)katana2_config 
$(obj)katana2_config: $(TOPDIR)/include/configs/katana2.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

katana2_nand_config: $(obj)katana2_nand_config 
$(obj)katana2_nand_config: $(TOPDIR)/include/configs/katana2.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@
