$VAR1 = {
          'rdb_sha1' => 'f2sLZjoAkYIhogP66iVbGN5oHVw',
          'fail' => 0,
          'date' => 'Tue May 26 16:05:10 PDT 2015',
          'rdb_version' => 'rdb-v2-14-gc3dd139',
          'rdb_dir' => '/home/stblinux/clkgen/7439b0/current',
          'clkgen_version' => 'clkgen-v4-57-ga51e06a',
          'chip' => '7439b0',
          'aliases' => {
                         'sw_gphy' => 'lc_pdh_ch0'
                       },
          'unhandled_linux_funcs' => 'CPU, HIF, MEMSYS0, MEMSYS1, MPI',
          'invocation' => 'clkgen.pl --sw_nodes -v -g -r -P -c 7439b0',
          'num_clks' => 144,
          'clks' => '	brcmstb-clks {
		#address-cells = <1>;
		#size-cells = <1>;
		reg = <0xf04e0000 0x684 0xf04d1800 0xdc 0xf0410000 0x600>;
		ranges;

		osc_pcie0 : osc_pcie0@f0410074 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf0410074 0x4>;
			bit-shift = <6>;
			set-bit-to-disable;
		};

		osc_mocaphy : osc_mocaphy@f0410074 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf0410074 0x4>;
			bit-shift = <5>;
			set-bit-to-disable;
		};

		osc_usb0 : osc_usb0@f0410074 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf0410074 0x4>;
			bit-shift = <4>;
			set-bit-to-disable;
		};

		osc_sata_pcie1 : osc_sata_pcie1@f0410074 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf0410074 0x4>;
			bit-shift = <3>;
			set-bit-to-disable;
		};

		fixed1 : fixed1 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		hvd_pwrdn_req : hvd_pwrdn_req@f04e0654 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0654 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&fixed1>; 
			clock-names = "fixed1"; 
		};

		hvd_pdiv : hvd_pdiv@f04e0050 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0050 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&hvd_pwrdn_req>; 
			clock-names = "hvd_pwrdn_req"; 
		};

		hvd_ndiv_int : hvd_ndiv_int {
			compatible = "multiplier-clock", "fixed-factor-clock";
			#clock-cells = <0>;
			reg = <0xf04e0050 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3ff>;
			index-max-mult-at-zero;
			clock-div = <1>;
			clock-mult = <200>;
			clocks = <&hvd_pdiv>; 
			clock-names = "hvd_pdiv"; 
		};

		hvd_mdiv_ch4 : hvd_mdiv_ch4@f04e0048 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0048 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&hvd_ndiv_int>; 
			clock-names = "hvd_ndiv_int"; 
		};

		hvd_dis_ch4 : hvd_dis_ch4@f04e0048 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0048 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&hvd_mdiv_ch4>; 
			clock-names = "hvd_mdiv_ch4"; 
		};

		hvd_pdh_ch4 : hvd_pdh_ch4@f04e0048 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0048 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&hvd_dis_ch4>; 
			clock-names = "hvd_dis_ch4"; 
		};

		hif_sdio_card : hif_sdio_card@f04e038c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e038c 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&hvd_pdh_ch4>; 
			clock-names = "hvd_pdh_ch4"; 
		};

		hif_sdio_emmc : hif_sdio_emmc@f04e038c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e038c 0x4>;
			bit-shift = <2>;
			set-bit-to-disable;
			clocks = <&hvd_pdh_ch4>; 
			clock-names = "hvd_pdh_ch4"; 
		};

		hvd_mdiv_ch3 : hvd_mdiv_ch3@f04e0044 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0044 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&hvd_ndiv_int>; 
			clock-names = "hvd_ndiv_int"; 
		};

		hvd_dis_ch3 : hvd_dis_ch3@f04e0044 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0044 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&hvd_mdiv_ch3>; 
			clock-names = "hvd_mdiv_ch3"; 
		};

		hvd_pdh_ch3 : hvd_pdh_ch3@f04e0044 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0044 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&hvd_dis_ch3>; 
			clock-names = "hvd_dis_ch3"; 
		};

		hif_spi : sw_spi : hif_spi@f04e038c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e038c 0x4>;
			bit-shift = <3>;
			set-bit-to-disable;
			clocks = <&hvd_pdh_ch3>; 
			clock-names = "hvd_pdh_ch3"; 
		};

		mocamac_54 : mocamac_54@f04e041c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e041c 0x4>;
			bit-shift = <0>;
		};

		mocamac_gisb : mocamac_gisb@f04e041c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e041c 0x4>;
			bit-shift = <1>;
		};

		mocamac_scb : mocamac_scb@f04e041c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e041c 0x4>;
			bit-shift = <2>;
		};

		mocaphy_54 : mocaphy_54@f04e0428 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0428 0x4>;
			bit-shift = <0>;
		};

		mocaphy_gisb : mocaphy_gisb@f04e0428 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0428 0x4>;
			bit-shift = <1>;
		};

		pcie_alwayson : pcie_alwayson@f04e0450 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0450 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		pcie_108 : pcie_108@f04e0458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0458 0x4>;
			bit-shift = <0>;
		};

		pcie_54 : pcie_54@f04e0458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0458 0x4>;
			bit-shift = <1>;
		};

		pcie_gisb : pcie_gisb@f04e0458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0458 0x4>;
			bit-shift = <2>;
		};

		pcie_scb : pcie_scb@f04e0458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0458 0x4>;
			bit-shift = <3>;
		};

		pcie1_alwayson : pcie1_alwayson@f04e04ec {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04ec 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		pcie1_54_pcie : pcie1_54_pcie@f04e04f4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04f4 0x4>;
			bit-shift = <0>;
		};

		pcie1_gisb_pcie : pcie1_gisb_pcie@f04e04f4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04f4 0x4>;
			bit-shift = <1>;
		};

		pcie1_scb_pcie : pcie1_scb_pcie@f04e04f4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04f4 0x4>;
			bit-shift = <2>;
		};

		sata3_108_pcie : sata3_108_pcie@f04e04f4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04f4 0x4>;
			bit-shift = <3>;
		};

		sata3_54 : sata3_54@f04e04fc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04fc 0x4>;
			bit-shift = <0>;
		};

		sata3_gisb : sata3_gisb@f04e04fc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04fc 0x4>;
			bit-shift = <1>;
		};

		sata3_scb : sata3_scb@f04e04fc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04fc 0x4>;
			bit-shift = <2>;
		};

		genet0_alwayson : genet0_alwayson@f04e0538 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0538 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		fixed4 : fixed4 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		net_pwrdn_req : net_pwrdn_req@f04e0658 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0658 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&fixed4>; 
			clock-names = "fixed4"; 
		};

		net_pdiv : net_pdiv@f04e010c {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e010c 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&net_pwrdn_req>; 
			clock-names = "net_pwrdn_req"; 
		};

		net_ndiv_int : net_ndiv_int {
			compatible = "multiplier-clock", "fixed-factor-clock";
			#clock-cells = <0>;
			reg = <0xf04e010c 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3ff>;
			index-max-mult-at-zero;
			clock-div = <1>;
			clock-mult = <125>;
			clocks = <&net_pdiv>; 
			clock-names = "net_pdiv"; 
		};

		net_mdiv_ch1 : net_mdiv_ch1@f04e0104 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0104 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch1 : net_dis_ch1@f04e0104 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0104 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch1>; 
			clock-names = "net_mdiv_ch1"; 
		};

		net_pdh_ch1 : net_pdh_ch1@f04e0104 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0104 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch1>; 
			clock-names = "net_dis_ch1"; 
		};

		genet0_sys_fast : genet0_sys_fast@f04e0538 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0538 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&net_pdh_ch1>; 
			clock-names = "net_pdh_ch1"; 
		};

		genet0_sys_pm : genet0_sys_pm@f04e0538 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0538 0x4>;
			bit-shift = <2>;
			set-bit-to-disable;
		};

		genet0_sys_slow : genet0_sys_slow@f04e0538 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0538 0x4>;
			bit-shift = <3>;
			set-bit-to-disable;
		};

		genet1_alwayson : genet1_alwayson@f04e0538 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0538 0x4>;
			bit-shift = <4>;
			set-bit-to-disable;
		};

		genet1_sys_fast : genet1_sys_fast@f04e0538 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0538 0x4>;
			bit-shift = <5>;
			set-bit-to-disable;
			clocks = <&net_pdh_ch1>; 
			clock-names = "net_pdh_ch1"; 
		};

		genet1_sys_pm : genet1_sys_pm@f04e0538 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0538 0x4>;
			bit-shift = <6>;
			set-bit-to-disable;
		};

		genet1_sys_slow : genet1_sys_slow@f04e0538 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0538 0x4>;
			bit-shift = <7>;
			set-bit-to-disable;
		};

		genet2_alwayson : genet2_alwayson@f04e0538 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0538 0x4>;
			bit-shift = <8>;
			set-bit-to-disable;
		};

		genet2_sys_fast : genet2_sys_fast@f04e0538 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0538 0x4>;
			bit-shift = <9>;
			set-bit-to-disable;
			clocks = <&net_pdh_ch1>; 
			clock-names = "net_pdh_ch1"; 
		};

		genet2_sys_pm : genet2_sys_pm@f04e0538 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0538 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
		};

		genet2_sys_slow : genet2_sys_slow@f04e0538 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0538 0x4>;
			bit-shift = <11>;
			set-bit-to-disable;
		};

		genet_54 : genet_54@f04e0540 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0540 0x4>;
			bit-shift = <0>;
		};

		genet_scb : genet_scb@f04e0540 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0540 0x4>;
			bit-shift = <1>;
		};

		genet0_250 : genet0_250@f04e0544 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0544 0x4>;
			bit-shift = <0>;
		};

		genet0_eee : genet0_eee@f04e0544 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0544 0x4>;
			bit-shift = <1>;
		};

		genet0_gisb : genet0_gisb@f04e0544 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0544 0x4>;
			bit-shift = <2>;
		};

		genet0_gmii : genet0_gmii@f04e0544 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0544 0x4>;
			bit-shift = <3>;
		};

		genet0_hfb : genet0_hfb@f04e0544 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0544 0x4>;
			bit-shift = <4>;
		};

		genet0_l2intr : genet0_l2intr@f04e0544 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0544 0x4>;
			bit-shift = <5>;
		};

		genet0_scb : genet0_scb@f04e0544 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0544 0x4>;
			bit-shift = <6>;
		};

		genet0_umac_rx : genet0_umac_rx@f04e0544 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0544 0x4>;
			bit-shift = <7>;
		};

		genet0_umac_tx : genet0_umac_tx@f04e0544 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0544 0x4>;
			bit-shift = <8>;
		};

		net_mdiv_ch0 : net_mdiv_ch0@f04e0100 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0100 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch0 : net_dis_ch0@f04e0100 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0100 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch0>; 
			clock-names = "net_mdiv_ch0"; 
		};

		net_pdh_ch0 : net_pdh_ch0@f04e0100 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0100 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch0>; 
			clock-names = "net_dis_ch0"; 
		};

		genet1_250 : genet1_250@f04e054c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e054c 0x4>;
			bit-shift = <0>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		genet1_eee : genet1_eee@f04e054c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e054c 0x4>;
			bit-shift = <1>;
		};

		genet1_gisb : genet1_gisb@f04e054c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e054c 0x4>;
			bit-shift = <2>;
		};

		genet1_gmii : genet1_gmii@f04e054c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e054c 0x4>;
			bit-shift = <3>;
		};

		genet1_hfb : genet1_hfb@f04e054c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e054c 0x4>;
			bit-shift = <4>;
		};

		genet1_l2intr : genet1_l2intr@f04e054c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e054c 0x4>;
			bit-shift = <5>;
		};

		genet1_scb : genet1_scb@f04e054c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e054c 0x4>;
			bit-shift = <6>;
		};

		genet1_umac_rx : genet1_umac_rx@f04e054c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e054c 0x4>;
			bit-shift = <7>;
		};

		genet1_umac_tx : genet1_umac_tx@f04e054c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e054c 0x4>;
			bit-shift = <8>;
		};

		genet2_250 : genet2_250@f04e0554 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0554 0x4>;
			bit-shift = <0>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		genet2_eee : genet2_eee@f04e0554 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0554 0x4>;
			bit-shift = <1>;
		};

		genet2_gisb : genet2_gisb@f04e0554 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0554 0x4>;
			bit-shift = <2>;
		};

		genet2_gmii : genet2_gmii@f04e0554 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0554 0x4>;
			bit-shift = <3>;
		};

		genet2_hfb : genet2_hfb@f04e0554 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0554 0x4>;
			bit-shift = <4>;
		};

		genet2_l2intr : genet2_l2intr@f04e0554 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0554 0x4>;
			bit-shift = <5>;
		};

		genet2_scb : genet2_scb@f04e0554 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0554 0x4>;
			bit-shift = <6>;
		};

		genet2_umac_rx : genet2_umac_rx@f04e0554 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0554 0x4>;
			bit-shift = <7>;
		};

		genet2_umac_tx : genet2_umac_tx@f04e0554 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0554 0x4>;
			bit-shift = <8>;
		};

		usb0_54_mdio : usb0_54_mdio@f04e057c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e057c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		usb0_54 : usb0_54@f04e0584 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0584 0x4>;
			bit-shift = <0>;
		};

		usb0_gisb : usb0_gisb@f04e0584 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0584 0x4>;
			bit-shift = <1>;
		};

		usb0_scb : usb0_scb@f04e0584 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0584 0x4>;
			bit-shift = <2>;
		};

		usb0_108_ahb : usb0_108_ahb@f04e0588 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0588 0x4>;
			bit-shift = <0>;
		};

		usb0_108_axi : usb0_108_axi@f04e0590 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0590 0x4>;
			bit-shift = <0>;
		};

		fixed3 : fixed3 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		moca_pwrdn_req : moca_pwrdn_req@f04e065c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e065c 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&fixed3>; 
			clock-names = "fixed3"; 
		};

		fixed2 : fixed2 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		lc_pwrdn_req : lc_pwrdn_req@f04e0668 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0668 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&fixed2>; 
			clock-names = "fixed2"; 
		};

		fixed0 : fixed0 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		cpu_pdiv : cpu_pdiv@f04e0008 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0008 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&fixed0>; 
			clock-names = "fixed0"; 
		};

		cpu_ndiv_int : cpu_ndiv_int {
			compatible = "multiplier-clock";
			#clock-cells = <0>;
			reg = <0xf04e0008 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3ff>;
			index-max-mult-at-zero;
			clock-div = <1>;
			clock-mult = <167>;
			clocks = <&cpu_pdiv>; 
			clock-names = "cpu_pdiv"; 
		};

		cpu_mdiv_ch0 : cpu_mdiv_ch0@f04e0000 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0000 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&cpu_ndiv_int>; 
			clock-names = "cpu_ndiv_int"; 
		};

		lc_pdiv : lc_pdiv@f04e0094 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0094 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&lc_pwrdn_req>; 
			clock-names = "lc_pwrdn_req"; 
		};

		lc_ndiv_int : lc_ndiv_int {
			compatible = "multiplier-clock", "fixed-factor-clock";
			#clock-cells = <0>;
			reg = <0xf04e0094 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3ff>;
			index-max-mult-at-zero;
			clock-div = <1>;
			clock-mult = <100>;
			clocks = <&lc_pdiv>; 
			clock-names = "lc_pdiv"; 
		};

		lc_mdiv_ch0 : lc_mdiv_ch0@f04e0080 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0080 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&lc_ndiv_int>; 
			clock-names = "lc_ndiv_int"; 
		};

		lc_dis_ch0 : lc_dis_ch0@f04e0080 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0080 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_mdiv_ch0>; 
			clock-names = "lc_mdiv_ch0"; 
		};

		lc_pdh_ch0 : sw_gphy : lc_pdh_ch0@f04e0080 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0080 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&lc_dis_ch0>; 
			clock-names = "lc_dis_ch0"; 
		};

		lc_mdiv_ch2 : lc_mdiv_ch2@f04e0084 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0084 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&lc_ndiv_int>; 
			clock-names = "lc_ndiv_int"; 
		};

		lc_dis_ch2 : lc_dis_ch2@f04e0084 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0084 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_mdiv_ch2>; 
			clock-names = "lc_mdiv_ch2"; 
		};

		lc_pdh_ch2 : lc_pdh_ch2@f04e0084 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0084 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&lc_dis_ch2>; 
			clock-names = "lc_dis_ch2"; 
		};

		lc_mdiv_ch4 : lc_mdiv_ch4@f04e0088 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0088 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&lc_ndiv_int>; 
			clock-names = "lc_ndiv_int"; 
		};

		lc_dis_ch4 : lc_dis_ch4@f04e0088 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0088 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_mdiv_ch4>; 
			clock-names = "lc_mdiv_ch4"; 
		};

		lc_pdh_ch4 : lc_pdh_ch4@f04e0088 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0088 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&lc_dis_ch4>; 
			clock-names = "lc_dis_ch4"; 
		};

		moca_pdiv : moca_pdiv@f04e00d4 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00d4 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&moca_pwrdn_req>; 
			clock-names = "moca_pwrdn_req"; 
		};

		moca_ndiv_int : moca_ndiv_int {
			compatible = "multiplier-clock", "fixed-factor-clock";
			#clock-cells = <0>;
			reg = <0xf04e00d4 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3ff>;
			index-max-mult-at-zero;
			clock-div = <1>;
			clock-mult = <200>;
			clocks = <&moca_pdiv>; 
			clock-names = "moca_pdiv"; 
		};

		moca_mdiv_ch0 : sw_moca_cpu : moca_mdiv_ch0@f04e00c8 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00c8 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&moca_ndiv_int>; 
			clock-names = "moca_ndiv_int"; 
		};

		moca_dis_ch0 : moca_dis_ch0@f04e00c8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00c8 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&moca_mdiv_ch0>; 
			clock-names = "moca_mdiv_ch0"; 
		};

		moca_pdh_ch0 : moca_pdh_ch0@f04e00c8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00c8 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&moca_dis_ch0>; 
			clock-names = "moca_dis_ch0"; 
		};

		moca_mdiv_ch1 : sw_moca_phy : moca_mdiv_ch1@f04e00cc {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00cc 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&moca_ndiv_int>; 
			clock-names = "moca_ndiv_int"; 
		};

		moca_dis_ch1 : moca_dis_ch1@f04e00cc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00cc 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&moca_mdiv_ch1>; 
			clock-names = "moca_mdiv_ch1"; 
		};

		moca_pdh_ch1 : moca_pdh_ch1@f04e00cc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00cc 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&moca_dis_ch1>; 
			clock-names = "moca_dis_ch1"; 
		};

		moca_mdiv_ch2 : moca_mdiv_ch2@f04e00d0 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00d0 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&moca_ndiv_int>; 
			clock-names = "moca_ndiv_int"; 
		};

		moca_dis_ch2 : moca_dis_ch2@f04e00d0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00d0 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&moca_mdiv_ch2>; 
			clock-names = "moca_mdiv_ch2"; 
		};

		moca_pdh_ch2 : sw_mocawol : sw_mocawom : moca_pdh_ch2@f04e00d0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00d0 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&moca_dis_ch2>; 
			clock-names = "moca_dis_ch2"; 
		};

		fixed7 : fixed7 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		sys0_pdiv : sys0_pdiv@f04e01bc {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e01bc 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&fixed7>; 
			clock-names = "fixed7"; 
		};

		sys0_ndiv_int : sys0_ndiv_int {
			compatible = "multiplier-clock", "fixed-factor-clock";
			#clock-cells = <0>;
			reg = <0xf04e01bc 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3ff>;
			index-max-mult-at-zero;
			clock-div = <1>;
			clock-mult = <72>;
			clocks = <&sys0_pdiv>; 
			clock-names = "sys0_pdiv"; 
		};

		sys0_mdiv_ch1 : sw_hif : sw_memsys0 : sw_memsys1 : sys0_mdiv_ch1@f04e01a8 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e01a8 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&sys0_ndiv_int>; 
			clock-names = "sys0_ndiv_int"; 
		};

		sys0_mdiv_ch5 : sw_mpi : sys0_mdiv_ch5@f04e01b8 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e01b8 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&sys0_ndiv_int>; 
			clock-names = "sys0_ndiv_int"; 
		};

		sw_cpu : sw_cpu {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&cpu_mdiv_ch0>, <&sys0_mdiv_ch1>; 
			clock-names = "cpu_mdiv_ch0", "sys0_mdiv_ch1"; 
		};

		sw_genet0 : sw_genet0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet0_alwayson>, <&genet0_sys_fast>, 
			  <&genet0_sys_pm>, <&genet0_sys_slow>, <&genet_54>, 
			  <&genet_scb>, <&genet0_250>, <&genet0_gisb>, 
			  <&genet0_gmii>, <&genet0_hfb>, <&genet0_l2intr>, 
			  <&genet0_scb>, <&genet0_umac_rx>, <&genet0_umac_tx>, 
			  <&net_pdh_ch0>, <&sys0_mdiv_ch1>; 
			clock-names = "genet0_alwayson", "genet0_sys_fast", 
			  "genet0_sys_pm", "genet0_sys_slow", "genet_54", 
			  "genet_scb", "genet0_250", "genet0_gisb", 
			  "genet0_gmii", "genet0_hfb", "genet0_l2intr", 
			  "genet0_scb", "genet0_umac_rx", "genet0_umac_tx", 
			  "net_pdh_ch0", "sys0_mdiv_ch1"; 
		};

		sw_genet1 : sw_genet1 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet1_alwayson>, <&genet1_sys_fast>, 
			  <&genet1_sys_pm>, <&genet1_sys_slow>, <&genet1_250>, 
			  <&genet1_gisb>, <&genet1_gmii>, <&genet1_hfb>, 
			  <&genet1_l2intr>, <&genet1_scb>, <&genet1_umac_rx>, 
			  <&genet1_umac_tx>, <&sys0_mdiv_ch1>; 
			clock-names = "genet1_alwayson", "genet1_sys_fast", 
			  "genet1_sys_pm", "genet1_sys_slow", "genet1_250", 
			  "genet1_gisb", "genet1_gmii", "genet1_hfb", 
			  "genet1_l2intr", "genet1_scb", "genet1_umac_rx", 
			  "genet1_umac_tx", "sys0_mdiv_ch1"; 
		};

		sw_genet2 : sw_genet2 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet2_alwayson>, <&genet2_sys_fast>, 
			  <&genet2_sys_pm>, <&genet2_sys_slow>, <&genet2_250>, 
			  <&genet2_gisb>, <&genet2_gmii>, <&genet2_hfb>, 
			  <&genet2_l2intr>, <&genet2_scb>, <&genet2_umac_rx>, 
			  <&genet2_umac_tx>, <&sys0_mdiv_ch1>; 
			clock-names = "genet2_alwayson", "genet2_sys_fast", 
			  "genet2_sys_pm", "genet2_sys_slow", "genet2_250", 
			  "genet2_gisb", "genet2_gmii", "genet2_hfb", 
			  "genet2_l2intr", "genet2_scb", "genet2_umac_rx", 
			  "genet2_umac_tx", "sys0_mdiv_ch1"; 
		};

		sw_geneteee0 : sw_geneteee0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet0_sys_pm>, <&genet0_eee>; 
			clock-names = "genet0_sys_pm", "genet0_eee"; 
		};

		sw_geneteee1 : sw_geneteee1 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet1_sys_pm>, <&genet1_eee>; 
			clock-names = "genet1_sys_pm", "genet1_eee"; 
		};

		sw_geneteee2 : sw_geneteee2 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet2_sys_pm>, <&genet2_eee>; 
			clock-names = "genet2_sys_pm", "genet2_eee"; 
		};

		sw_genetwol0 : sw_genetwol0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet0_alwayson>, <&genet0_sys_slow>, 
			  <&genet_54>, <&genet0_hfb>, <&genet0_l2intr>, 
			  <&genet0_umac_rx>; 
			clock-names = "genet0_alwayson", "genet0_sys_slow", 
			  "genet_54", "genet0_hfb", "genet0_l2intr", 
			  "genet0_umac_rx"; 
		};

		sw_genetwol1 : sw_genetwol1 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet1_alwayson>, <&genet1_sys_slow>, 
			  <&genet1_hfb>, <&genet1_l2intr>, <&genet1_umac_rx>; 
			clock-names = "genet1_alwayson", "genet1_sys_slow", 
			  "genet1_hfb", "genet1_l2intr", "genet1_umac_rx"; 
		};

		sw_genetwol2 : sw_genetwol2 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet2_alwayson>, <&genet2_sys_slow>, 
			  <&genet2_hfb>, <&genet2_l2intr>, <&genet2_umac_rx>; 
			clock-names = "genet2_alwayson", "genet2_sys_slow", 
			  "genet2_hfb", "genet2_l2intr", "genet2_umac_rx"; 
		};

		sw_moca : sw_moca {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&osc_mocaphy>, <&mocamac_54>, 
			  <&mocamac_gisb>, <&mocamac_scb>, <&mocaphy_54>, 
			  <&mocaphy_gisb>, <&moca_pdh_ch0>, <&moca_pdh_ch1>, 
			  <&moca_pdh_ch2>, <&sys0_mdiv_ch1>; 
			clock-names = "osc_mocaphy", "mocamac_54", 
			  "mocamac_gisb", "mocamac_scb", "mocaphy_54", 
			  "mocaphy_gisb", "moca_pdh_ch0", "moca_pdh_ch1", 
			  "moca_pdh_ch2", "sys0_mdiv_ch1"; 
		};

		sw_pcie0 : sw_pcie0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&osc_pcie0>, <&pcie_alwayson>, <&pcie_108>, 
			  <&pcie_54>, <&pcie_gisb>, <&pcie_scb>, 
			  <&sys0_mdiv_ch1>; 
			clock-names = "osc_pcie0", "pcie_alwayson", 
			  "pcie_108", "pcie_54", "pcie_gisb", "pcie_scb", 
			  "sys0_mdiv_ch1"; 
		};

		sw_pcie1 : sw_pcie1 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&osc_sata_pcie1>, <&pcie1_alwayson>, 
			  <&pcie1_54_pcie>, <&pcie1_gisb_pcie>, 
			  <&pcie1_scb_pcie>, <&sata3_108_pcie>, 
			  <&sys0_mdiv_ch1>; 
			clock-names = "osc_sata_pcie1", "pcie1_alwayson", 
			  "pcie1_54_pcie", "pcie1_gisb_pcie", 
			  "pcie1_scb_pcie", "sata3_108_pcie", "sys0_mdiv_ch1"; 
		};

		sw_sata3 : sw_sata3 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&osc_sata_pcie1>, <&sata3_54>, 
			  <&sata3_gisb>, <&sata3_scb>, <&sys0_mdiv_ch1>; 
			clock-names = "osc_sata_pcie1", "sata3_54", 
			  "sata3_gisb", "sata3_scb", "sys0_mdiv_ch1"; 
		};

		sw_sdio : sw_sdio {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&hif_sdio_card>, <&hif_sdio_emmc>; 
			clock-names = "hif_sdio_card", "hif_sdio_emmc"; 
		};

		sw_usb20 : sw_usb20 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&usb0_54_mdio>, <&usb0_54>, <&usb0_gisb>, 
			  <&usb0_scb>, <&usb0_108_ahb>, <&usb0_108_axi>, 
			  <&lc_pdh_ch2>, <&lc_pdh_ch4>, <&sys0_mdiv_ch1>; 
			clock-names = "usb0_54_mdio", "usb0_54", "usb0_gisb", 
			  "usb0_scb", "usb0_108_ahb", "usb0_108_axi", 
			  "lc_pdh_ch2", "lc_pdh_ch4", "sys0_mdiv_ch1"; 
		};

		sw_usb30 : sw_usb30 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&osc_usb0>, <&usb0_54_mdio>, <&usb0_54>, 
			  <&usb0_gisb>, <&usb0_scb>, <&usb0_108_ahb>, 
			  <&usb0_108_axi>, <&lc_pdh_ch2>, <&lc_pdh_ch4>, 
			  <&sys0_mdiv_ch1>; 
			clock-names = "osc_usb0", "usb0_54_mdio", "usb0_54", 
			  "usb0_gisb", "usb0_scb", "usb0_108_ahb", 
			  "usb0_108_axi", "lc_pdh_ch2", "lc_pdh_ch4", 
			  "sys0_mdiv_ch1"; 
		};

	};
',
          'funcs' => {
                       'MOCAWOM' => [
                                      'sw_mocawom'
                                    ],
                       'MEMSYS1' => [
                                      'sw_memsys1'
                                    ],
                       'GENET2' => [
                                     'sw_genet2'
                                   ],
                       'USB20' => [
                                    'sw_usb20'
                                  ],
                       'MOCA_CTRL_DIV' => [
                                            'sw_moca_cpu',
                                            'sw_moca_phy'
                                          ],
                       'SDIO' => [
                                   'sw_sdio'
                                 ],
                       'GENETWOL0' => [
                                        'sw_genetwol0'
                                      ],
                       'USB30' => [
                                    'sw_usb30'
                                  ],
                       'MEMSYS0' => [
                                      'sw_memsys0'
                                    ],
                       'GENET1' => [
                                     'sw_genet1'
                                   ],
                       'GENETWOL1' => [
                                        'sw_genetwol1'
                                      ],
                       'SPI' => [
                                  'sw_spi'
                                ],
                       'HIF' => [
                                  'sw_hif'
                                ],
                       'PCIE1' => [
                                    'sw_pcie1'
                                  ],
                       'GENETWOL2' => [
                                        'sw_genetwol2'
                                      ],
                       'PCIE0' => [
                                    'sw_pcie0'
                                  ],
                       'GENETEEE2' => [
                                        'sw_geneteee2'
                                      ],
                       'SATA3' => [
                                    'sw_sata3'
                                  ],
                       'MPI' => [
                                  'sw_mpi'
                                ],
                       'GPHY' => [
                                   'sw_gphy'
                                 ],
                       'CPU' => [
                                  'sw_cpu'
                                ],
                       'GENET0' => [
                                     'sw_genet0'
                                   ],
                       'MOCAWOL' => [
                                      'sw_mocawol'
                                    ],
                       'GENETEEE0' => [
                                        'sw_geneteee0'
                                      ],
                       'MOCA' => [
                                   'sw_moca'
                                 ],
                       'GENETEEE1' => [
                                        'sw_geneteee1'
                                      ]
                     }
        };


# [---]   cpu_mdiv_ch0 => CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_0
#                                    /MDIV_CH0
# [---]   cpu_ndiv_int => CLKGEN_PLL_CPU_PLL_DIV
#                                    /NDIV_INT
# [---]   cpu_pdiv => CLKGEN_PLL_CPU_PLL_DIV
#                                    /PDIV
# [---] R fixed0 => fixed0
# [---] R fixed1 => fixed1
# [---] R fixed2 => fixed2
# [---] R fixed3 => fixed3
# [---] R fixed4 => fixed4
# [---] R fixed7 => fixed7
# [---] R genet0_250 => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_CLK_250_CLOCK_ENABLE_GENET0
# [---] R genet0_alwayson => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET0_ALWAYSON_CLOCK
# [---] R genet0_eee => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_EEE_CLOCK_ENABLE_GENET0
# [---] R genet0_gisb => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_GISB_CLOCK_ENABLE_GENET0
# [---] R genet0_gmii => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_GMII_CLOCK_ENABLE_GENET0
# [---] R genet0_hfb => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_HFB_CLOCK_ENABLE_GENET0
# [---] R genet0_l2intr => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_L2INTR_CLOCK_ENABLE_GENET0
# [---] R genet0_scb => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_SCB_CLOCK_ENABLE_GENET0
# [---]   genet0_sys_fast => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET0_SYSTEM_FAST_CLOCK
# [---] R genet0_sys_pm => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET0_SYSTEM_PM_CLOCK
# [---] R genet0_sys_slow => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET0_SYSTEM_SLOW_CLOCK
# [---] R genet0_umac_rx => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_UNIMAC_SYS_RX_CLOCK_ENABLE_GENET0
# [---] R genet0_umac_tx => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_UNIMAC_SYS_TX_CLOCK_ENABLE_GENET0
# [---]   genet1_250 => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_CLK_250_CLOCK_ENABLE_GENET1
# [---] R genet1_alwayson => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET1_ALWAYSON_CLOCK
# [---] R genet1_eee => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_EEE_CLOCK_ENABLE_GENET1
# [---] R genet1_gisb => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_GISB_CLOCK_ENABLE_GENET1
# [---] R genet1_gmii => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_GMII_CLOCK_ENABLE_GENET1
# [---] R genet1_hfb => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_HFB_CLOCK_ENABLE_GENET1
# [---] R genet1_l2intr => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_L2INTR_CLOCK_ENABLE_GENET1
# [---] R genet1_scb => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_SCB_CLOCK_ENABLE_GENET1
# [---]   genet1_sys_fast => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET1_SYSTEM_FAST_CLOCK
# [---] R genet1_sys_pm => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET1_SYSTEM_PM_CLOCK
# [---] R genet1_sys_slow => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET1_SYSTEM_SLOW_CLOCK
# [---] R genet1_umac_rx => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_UNIMAC_SYS_RX_CLOCK_ENABLE_GENET1
# [---] R genet1_umac_tx => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_UNIMAC_SYS_TX_CLOCK_ENABLE_GENET1
# [---]   genet2_250 => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET2
#                                    /GENET2_CLK_250_CLOCK_ENABLE_GENET2
# [---] R genet2_alwayson => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET2_ALWAYSON_CLOCK
# [---] R genet2_eee => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET2
#                                    /GENET2_EEE_CLOCK_ENABLE_GENET2
# [---] R genet2_gisb => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET2
#                                    /GENET2_GISB_CLOCK_ENABLE_GENET2
# [---] R genet2_gmii => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET2
#                                    /GENET2_GMII_CLOCK_ENABLE_GENET2
# [---] R genet2_hfb => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET2
#                                    /GENET2_HFB_CLOCK_ENABLE_GENET2
# [---] R genet2_l2intr => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET2
#                                    /GENET2_L2INTR_CLOCK_ENABLE_GENET2
# [---] R genet2_scb => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET2
#                                    /GENET2_SCB_CLOCK_ENABLE_GENET2
# [---]   genet2_sys_fast => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET2_SYSTEM_FAST_CLOCK
# [---] R genet2_sys_pm => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET2_SYSTEM_PM_CLOCK
# [---] R genet2_sys_slow => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET2_SYSTEM_SLOW_CLOCK
# [---] R genet2_umac_rx => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET2
#                                    /GENET2_UNIMAC_SYS_RX_CLOCK_ENABLE_GENET2
# [---] R genet2_umac_tx => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE_GENET2
#                                    /GENET2_UNIMAC_SYS_TX_CLOCK_ENABLE_GENET2
# [---] R genet_54 => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE
#                                    /GENET_54_CLOCK_ENABLE
# [---] R genet_scb => CLKGEN_TRIPLE_GENET_TOP_RGMII_INST_CLOCK_ENABLE
#                                    /GENET_SCB_CLOCK_ENABLE
# [---]   hif_sdio_card => CLKGEN_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_HIF_SDIO_CARD_CLOCK
# [---]   hif_sdio_emmc => CLKGEN_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_HIF_SDIO_EMMC_CLOCK
# [---]   hif_spi aka sw_spi => CLKGEN_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_HIF_SPI_CLOCK
# [---]   hvd_dis_ch3 => CLKGEN_PLL_HVD_PLL_CHANNEL_CTRL_CH_3
#                                    /CLOCK_DIS_CH3
# [---]   hvd_dis_ch4 => CLKGEN_PLL_HVD_PLL_CHANNEL_CTRL_CH_4
#                                    /CLOCK_DIS_CH4
# [---]   hvd_mdiv_ch3 => CLKGEN_PLL_HVD_PLL_CHANNEL_CTRL_CH_3
#                                    /MDIV_CH3
# [---]   hvd_mdiv_ch4 => CLKGEN_PLL_HVD_PLL_CHANNEL_CTRL_CH_4
#                                    /MDIV_CH4
# [---]   hvd_ndiv_int => CLKGEN_PLL_HVD_PLL_DIV
#                                    /NDIV_INT
# [---]   hvd_pdh_ch3 => CLKGEN_PLL_HVD_PLL_CHANNEL_CTRL_CH_3
#                                    /POST_DIVIDER_HOLD_CH3
# [---]   hvd_pdh_ch4 => CLKGEN_PLL_HVD_PLL_CHANNEL_CTRL_CH_4
#                                    /POST_DIVIDER_HOLD_CH4
# [---]   hvd_pdiv => CLKGEN_PLL_HVD_PLL_DIV
#                                    /PDIV
# [---]   hvd_pwrdn_req => CLKGEN_HVD_PLL_CTRL_WRAPPER_CONTROL
#                                    /PWRDN_PLL_REQ
# [---]   lc_dis_ch0 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_0
#                                    /CLOCK_DIS_CH0
# [---]   lc_dis_ch2 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_2
#                                    /CLOCK_DIS_CH2
# [---]   lc_dis_ch4 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_4
#                                    /CLOCK_DIS_CH4
# [---]   lc_mdiv_ch0 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_0
#                                    /MDIV_CH0
# [---]   lc_mdiv_ch2 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_2
#                                    /MDIV_CH2
# [---]   lc_mdiv_ch4 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_4
#                                    /MDIV_CH4
# [---]   lc_ndiv_int => CLKGEN_PLL_LC_PLL_DIV
#                                    /NDIV_INT
# [---]   lc_pdh_ch0 aka sw_gphy => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_0
#                                    /POST_DIVIDER_HOLD_CH0
# [---]   lc_pdh_ch2 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_2
#                                    /POST_DIVIDER_HOLD_CH2
# [---]   lc_pdh_ch4 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_4
#                                    /POST_DIVIDER_HOLD_CH4
# [---]   lc_pdiv => CLKGEN_PLL_LC_PLL_DIV
#                                    /PDIV
# [---]   lc_pwrdn_req => CLKGEN_LC_PLL_CTRL_WRAPPER_CONTROL
#                                    /PWRDN_PLL_REQ
# [---]   moca_dis_ch0 => CLKGEN_PLL_MOCA_PLL_CHANNEL_CTRL_CH_0
#                                    /CLOCK_DIS_CH0
# [---]   moca_dis_ch1 => CLKGEN_PLL_MOCA_PLL_CHANNEL_CTRL_CH_1
#                                    /CLOCK_DIS_CH1
# [---]   moca_dis_ch2 => CLKGEN_PLL_MOCA_PLL_CHANNEL_CTRL_CH_2
#                                    /CLOCK_DIS_CH2
# [---]   moca_mdiv_ch0 aka sw_moca_cpu => CLKGEN_PLL_MOCA_PLL_CHANNEL_CTRL_CH_0
#                                    /MDIV_CH0
# [---]   moca_mdiv_ch1 aka sw_moca_phy => CLKGEN_PLL_MOCA_PLL_CHANNEL_CTRL_CH_1
#                                    /MDIV_CH1
# [---]   moca_mdiv_ch2 => CLKGEN_PLL_MOCA_PLL_CHANNEL_CTRL_CH_2
#                                    /MDIV_CH2
# [---]   moca_ndiv_int => CLKGEN_PLL_MOCA_PLL_DIV
#                                    /NDIV_INT
# [---]   moca_pdh_ch0 => CLKGEN_PLL_MOCA_PLL_CHANNEL_CTRL_CH_0
#                                    /POST_DIVIDER_HOLD_CH0
# [---]   moca_pdh_ch1 => CLKGEN_PLL_MOCA_PLL_CHANNEL_CTRL_CH_1
#                                    /POST_DIVIDER_HOLD_CH1
# [---]   moca_pdh_ch2 aka sw_mocawol aka sw_mocawom => CLKGEN_PLL_MOCA_PLL_CHANNEL_CTRL_CH_2
#                                    /POST_DIVIDER_HOLD_CH2
# [---]   moca_pdiv => CLKGEN_PLL_MOCA_PLL_DIV
#                                    /PDIV
# [---]   moca_pwrdn_req => CLKGEN_MOCA_PLL_CTRL_WRAPPER_CONTROL
#                                    /PWRDN_PLL_REQ
# [---] R mocamac_54 => CLKGEN_MOCAMAC_TOP_INST_CLOCK_ENABLE
#                                    /MOCAMAC_54_CLOCK_ENABLE
# [---] R mocamac_gisb => CLKGEN_MOCAMAC_TOP_INST_CLOCK_ENABLE
#                                    /MOCAMAC_GISB_CLOCK_ENABLE
# [---] R mocamac_scb => CLKGEN_MOCAMAC_TOP_INST_CLOCK_ENABLE
#                                    /MOCAMAC_SCB_CLOCK_ENABLE
# [---] R mocaphy_54 => CLKGEN_MOCAPHY_TOP_INST_CLOCK_ENABLE
#                                    /MOCAPHY_54_CLOCK_ENABLE
# [---] R mocaphy_gisb => CLKGEN_MOCAPHY_TOP_INST_CLOCK_ENABLE
#                                    /MOCAPHY_GISB_CLOCK_ENABLE
# [---]   net_dis_ch0 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_0
#                                    /CLOCK_DIS_CH0
# [---]   net_dis_ch1 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_1
#                                    /CLOCK_DIS_CH1
# [---]   net_mdiv_ch0 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_0
#                                    /MDIV_CH0
# [---]   net_mdiv_ch1 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_1
#                                    /MDIV_CH1
# [---]   net_ndiv_int => CLKGEN_PLL_NETWORK_PLL_DIV
#                                    /NDIV_INT
# [---]   net_pdh_ch0 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_0
#                                    /POST_DIVIDER_HOLD_CH0
# [---]   net_pdh_ch1 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_1
#                                    /POST_DIVIDER_HOLD_CH1
# [---]   net_pdiv => CLKGEN_PLL_NETWORK_PLL_DIV
#                                    /PDIV
# [---]   net_pwrdn_req => CLKGEN_NETWORK_PLL_CTRL_WRAPPER_CONTROL
#                                    /PWRDN_PLL_REQ
# [---] R osc_mocaphy => AON_CTRL_ANA_XTAL_CONTROL
#                                    /osc_cml_sel_pd_mocaphy
# [---] R osc_pcie0 => AON_CTRL_ANA_XTAL_CONTROL
#                                    /osc_cml_sel_pd_pcie0
# [---] R osc_sata_pcie1 => AON_CTRL_ANA_XTAL_CONTROL
#                                    /osc_cml_sel_pd_sata_pcie1
# [---] R osc_usb0 => AON_CTRL_ANA_XTAL_CONTROL
#                                    /osc_cml_sel_pd_usb0
# [---] R pcie1_54_pcie => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE
#                                    /PCIE1_54_CLOCK_ENABLE_PCIE
# [---] R pcie1_alwayson => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_PCIE1_ALWAYSON_CLOCK
# [---] R pcie1_gisb_pcie => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE
#                                    /PCIE1_GISB_CLOCK_ENABLE_PCIE
# [---] R pcie1_scb_pcie => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE
#                                    /PCIE1_SCB_CLOCK_ENABLE_PCIE
# [---] R pcie_108 => CLKGEN_PCIE_X1_TOP_INST_CLOCK_ENABLE
#                                    /PCIE_108_CLOCK_ENABLE
# [---] R pcie_54 => CLKGEN_PCIE_X1_TOP_INST_CLOCK_ENABLE
#                                    /PCIE_54_CLOCK_ENABLE
# [---] R pcie_alwayson => CLKGEN_PCIE_X1_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_PCIE_ALWAYSON_CLOCK
# [---] R pcie_gisb => CLKGEN_PCIE_X1_TOP_INST_CLOCK_ENABLE
#                                    /PCIE_GISB_CLOCK_ENABLE
# [---] R pcie_scb => CLKGEN_PCIE_X1_TOP_INST_CLOCK_ENABLE
#                                    /PCIE_SCB_CLOCK_ENABLE
# [---] R sata3_108_pcie => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE
#                                    /SATA3_108_CLOCK_ENABLE_PCIE
# [---] R sata3_54 => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_SATA3
#                                    /SATA3_54_CLOCK_ENABLE_SATA3
# [---] R sata3_gisb => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_SATA3
#                                    /SATA3_GISB_CLOCK_ENABLE_SATA3
# [---] R sata3_scb => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_SATA3
#                                    /SATA3_SCB_CLOCK_ENABLE_SATA3
# [---]   sw_cpu => sw_cpu
# [---]   sw_genet0 => sw_genet0
# [---]   sw_genet1 => sw_genet1
# [---]   sw_genet2 => sw_genet2
# [---]   sw_geneteee0 => sw_geneteee0
# [---]   sw_geneteee1 => sw_geneteee1
# [---]   sw_geneteee2 => sw_geneteee2
# [---]   sw_genetwol0 => sw_genetwol0
# [---]   sw_genetwol1 => sw_genetwol1
# [---]   sw_genetwol2 => sw_genetwol2
# [---]   sw_moca => sw_moca
# [---]   sw_pcie0 => sw_pcie0
# [---]   sw_pcie1 => sw_pcie1
# [---]   sw_sata3 => sw_sata3
# [---]   sw_sdio => sw_sdio
# [---]   sw_usb20 => sw_usb20
# [---]   sw_usb30 => sw_usb30
# [---]   sys0_mdiv_ch1 aka sw_hif aka sw_memsys0 aka sw_memsys1 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1
#                                    /MDIV_CH1
# [---]   sys0_mdiv_ch5 aka sw_mpi => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_5
#                                    /MDIV_CH5
# [---]   sys0_ndiv_int => CLKGEN_PLL_SYS0_PLL_DIV
#                                    /NDIV_INT
# [---]   sys0_pdiv => CLKGEN_PLL_SYS0_PLL_DIV
#                                    /PDIV
# [---] R usb0_108_ahb => CLKGEN_USB0_TOP_INST_CLOCK_ENABLE_AHB
#                                    /USB0_108_CLOCK_ENABLE_AHB
# [---] R usb0_108_axi => CLKGEN_USB0_TOP_INST_CLOCK_ENABLE_AXI
#                                    /USB0_108_CLOCK_ENABLE_AXI
# [---] R usb0_54 => CLKGEN_USB0_TOP_INST_CLOCK_ENABLE
#                                    /USB0_54_CLOCK_ENABLE
# [---] R usb0_54_mdio => CLKGEN_USB0_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_USB0_54_MDIO_CLOCK
# [---] R usb0_gisb => CLKGEN_USB0_TOP_INST_CLOCK_ENABLE
#                                    /USB0_GISB_CLOCK_ENABLE
# [---] R usb0_scb => CLKGEN_USB0_TOP_INST_CLOCK_ENABLE
#                                    /USB0_SCB_CLOCK_ENABLE
