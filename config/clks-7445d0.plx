$VAR1 = {
          'rdb_sha1' => 'yADw3G5N3kc/VF2pB+sBdx2TyzE',
          'fail' => 0,
          'date' => 'Tue Apr 21 14:35:27 PDT 2015',
          'rdb_version' => 'rdb-v2-4-g6e897f8',
          'rdb_dir' => '/home/stblinux/clkgen/7445d0/current',
          'clkgen_version' => 'clkgen-v4-37-g3fb836d',
          'chip' => '7445d0',
          'aliases' => {
                         'sw_gphy' => 'lc_pdh_ch0'
                       },
          'unhandled_linux_funcs' => 'CPU, MEMSYS0, MEMSYS1, MEMSYS2, MPI',
          'invocation' => 'clkgen.pl --sw_nodes -v -g -r -P -D -c 7445d0',
          'num_clks' => 97,
          'clks' => '	brcmstb-clks {
		#address-cells = <1>;
		#size-cells = <1>;
		reg = <0xf04e0000 0x6fc 0xf04d1800 0xd8 0xf0410000 0x600>;
		ranges;

		osc_pcie0 : osc_pcie0@f0410074 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf0410074 0x4>;
			bit-shift = <6>;
			set-bit-to-disable;
		};

		osc_moca : osc_moca@f0410074 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf0410074 0x4>;
			bit-shift = <5>;
			set-bit-to-disable;
		};

		osc_usb30 : osc_usb30@f0410074 {
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

		moca_pwrdn_req : moca_pwrdn_req@f04e06c0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e06c0 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&fixed1>; 
			clock-names = "fixed1"; 
		};

		moca_pdiv : moca_pdiv@f04e00e0 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00e0 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&moca_pwrdn_req>; 
			clock-names = "moca_pwrdn_req"; 
		};

		moca_ndiv_int : moca_ndiv_int {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <1>;
			clock-mult = <200>;
			clocks = <&moca_pdiv>; 
			clock-names = "moca_pdiv"; 
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

		hif_sdio_card : hif_sdio_card@f04e0438 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0438 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&moca_pdh_ch2>; 
			clock-names = "moca_pdh_ch2"; 
		};

		hif_sdio_emmc : hif_sdio_emmc@f04e0438 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0438 0x4>;
			bit-shift = <2>;
			set-bit-to-disable;
			clocks = <&moca_pdh_ch2>; 
			clock-names = "moca_pdh_ch2"; 
		};

		fixed2 : fixed2 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		net_pwrdn_req : net_pwrdn_req@f04e06c4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e06c4 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&fixed2>; 
			clock-names = "fixed2"; 
		};

		net_pdiv : net_pdiv@f04e011c {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e011c 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&net_pwrdn_req>; 
			clock-names = "net_pwrdn_req"; 
		};

		net_ndiv_int : net_ndiv_int {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <1>;
			clock-mult = <125>;
			clocks = <&net_pdiv>; 
			clock-names = "net_pdiv"; 
		};

		net_mdiv_ch2 : net_mdiv_ch2@f04e0114 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0114 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch2 : net_dis_ch2@f04e0114 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0114 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch2>; 
			clock-names = "net_mdiv_ch2"; 
		};

		net_pdh_ch2 : net_pdh_ch2@f04e0114 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0114 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch2>; 
			clock-names = "net_dis_ch2"; 
		};

		hif_spi : sw_spi : hif_spi@f04e0438 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0438 0x4>;
			bit-shift = <3>;
			set-bit-to-disable;
			clocks = <&net_pdh_ch2>; 
			clock-names = "net_pdh_ch2"; 
		};

		mocamac_54 : mocamac_54@f04e04b0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04b0 0x4>;
			bit-shift = <0>;
		};

		mocamac_gisb : mocamac_gisb@f04e04b0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04b0 0x4>;
			bit-shift = <1>;
		};

		mocamac_scb : mocamac_scb@f04e04b0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04b0 0x4>;
			bit-shift = <2>;
		};

		mocaphy_54 : mocaphy_54@f04e04bc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04bc 0x4>;
			bit-shift = <0>;
		};

		mocaphy_gisb : mocaphy_gisb@f04e04bc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04bc 0x4>;
			bit-shift = <1>;
		};

		pcie_alwayson : pcie_alwayson@f04e04e0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04e0 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		pcie_54 : pcie_54@f04e04e8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04e8 0x4>;
			bit-shift = <0>;
		};

		pcie_gisb : pcie_gisb@f04e04e8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04e8 0x4>;
			bit-shift = <1>;
		};

		pcie_scb : pcie_scb@f04e04e8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04e8 0x4>;
			bit-shift = <2>;
		};

		pcie1_gisb_pcie : pcie1_gisb_pcie@f04e0598 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0598 0x4>;
			bit-shift = <1>;
		};

		pcie1_scb_pcie : pcie1_scb_pcie@f04e0598 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0598 0x4>;
			bit-shift = <2>;
		};

		sata3_108_pcie : sata3_108_pcie@f04e0598 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0598 0x4>;
			bit-shift = <3>;
		};

		sata3_54 : sata3_54@f04e05a0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05a0 0x4>;
			bit-shift = <0>;
		};

		sata3_gisb : sata3_gisb@f04e05a0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05a0 0x4>;
			bit-shift = <1>;
		};

		sata3_scb : sata3_scb@f04e05a0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05a0 0x4>;
			bit-shift = <2>;
		};

		swi_54 : swi_54@f04e05c4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05c4 0x4>;
			bit-shift = <0>;
		};

		swi_gisb : swi_gisb@f04e05c4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05c4 0x4>;
			bit-shift = <1>;
		};

		swi_scb : swi_scb@f04e05c4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05c4 0x4>;
			bit-shift = <2>;
		};

		usb0_54_mdio : usb0_54_mdio@f04e05e8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05e8 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		usb0_54 : usb0_54@f04e05f0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05f0 0x4>;
			bit-shift = <0>;
		};

		usb0_gisb : usb0_gisb@f04e05f0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05f0 0x4>;
			bit-shift = <1>;
		};

		usb0_scb : usb0_scb@f04e05f0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05f0 0x4>;
			bit-shift = <2>;
		};

		usb0_108_ahb : usb0_108_ahb@f04e05f4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05f4 0x4>;
			bit-shift = <0>;
		};

		usb0_108_axi : usb0_108_axi@f04e05fc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05fc 0x4>;
			bit-shift = <0>;
		};

		usb1_54_mdio : usb1_54_mdio@f04e060c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e060c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		usb1_54 : usb1_54@f04e0614 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0614 0x4>;
			bit-shift = <0>;
		};

		usb1_gisb : usb1_gisb@f04e0614 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0614 0x4>;
			bit-shift = <1>;
		};

		usb1_scb : usb1_scb@f04e0614 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0614 0x4>;
			bit-shift = <2>;
		};

		usb1_108_ahb : usb1_108_ahb@f04e0618 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0618 0x4>;
			bit-shift = <0>;
		};

		usb1_108_axi : usb1_108_axi@f04e0620 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0620 0x4>;
			bit-shift = <0>;
		};

		net_mdiv_ch0 : net_mdiv_ch0@f04e010c {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e010c 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch0 : net_dis_ch0@f04e010c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e010c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch0>; 
			clock-names = "net_mdiv_ch0"; 
		};

		net_pdh_ch0 : net_pdh_ch0@f04e010c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e010c 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch0>; 
			clock-names = "net_dis_ch0"; 
		};

		sprt_tx_syssprt : sprt_tx_syssprt@f04e064c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e064c 0x4>;
			bit-shift = <0>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		sprt_gisb : sprt_gisb@f04e0670 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0670 0x4>;
			bit-shift = <0>;
		};

		sprt_gmii_tx : sprt_gmii_tx@f04e0670 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0670 0x4>;
			bit-shift = <1>;
		};

		sprt_rx_scb : sprt_rx_scb@f04e0670 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0670 0x4>;
			bit-shift = <2>;
		};

		sprt_rx_sys : sprt_rx_sys@f04e0670 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0670 0x4>;
			bit-shift = <3>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		sprt_scb : sprt_scb@f04e0670 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0670 0x4>;
			bit-shift = <4>;
		};

		sprt_tx_scb : sprt_tx_scb@f04e0670 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0670 0x4>;
			bit-shift = <5>;
		};

		lc_pwrdn_req : lc_pwrdn_req@f04e06d8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e06d8 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			brcm,delay = <0 21>;
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
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <1>;
			clock-mult = <167>;
			clocks = <&cpu_pdiv>; 
			clock-names = "cpu_pdiv"; 
		};

		cpu_mdiv_ch0 : sw_cpu : cpu_mdiv_ch0@f04e0000 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0000 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&cpu_ndiv_int>; 
			clock-names = "cpu_ndiv_int"; 
		};

		lc_dis_ch0 : lc_dis_ch0@f04e007c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e007c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_pwrdn_req>; 
			clock-names = "lc_pwrdn_req"; 
		};

		lc_pdh_ch0 : sw_gphy : lc_pdh_ch0@f04e007c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e007c 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&lc_dis_ch0>; 
			clock-names = "lc_dis_ch0"; 
		};

		lc_dis_ch1 : lc_dis_ch1@f04e0080 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0080 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_pwrdn_req>; 
			clock-names = "lc_pwrdn_req"; 
		};

		lc_pdh_ch1 : lc_pdh_ch1@f04e0080 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0080 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&lc_dis_ch1>; 
			clock-names = "lc_dis_ch1"; 
		};

		lc_dis_ch2 : lc_dis_ch2@f04e0084 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0084 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_pwrdn_req>; 
			clock-names = "lc_pwrdn_req"; 
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

		lc_dis_ch4 : lc_dis_ch4@f04e0088 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0088 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_pwrdn_req>; 
			clock-names = "lc_pwrdn_req"; 
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

		net_mdiv_ch1 : net_mdiv_ch1@f04e0110 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0110 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch1 : net_dis_ch1@f04e0110 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0110 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch1>; 
			clock-names = "net_mdiv_ch1"; 
		};

		net_pdh_ch1 : net_pdh_ch1@f04e0110 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0110 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch1>; 
			clock-names = "net_dis_ch1"; 
		};

		net_mdiv_ch3 : sw_switch_mdiv : net_mdiv_ch3@f04e0118 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0118 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch3 : net_dis_ch3@f04e0118 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0118 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch3>; 
			clock-names = "net_mdiv_ch3"; 
		};

		net_pdh_ch3 : net_pdh_ch3@f04e0118 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0118 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch3>; 
			clock-names = "net_dis_ch3"; 
		};

		sw_moca : sw_moca {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&osc_moca>, <&mocamac_54>, <&mocamac_gisb>, 
			  <&mocamac_scb>, <&mocaphy_54>, <&mocaphy_gisb>, 
			  <&moca_pdh_ch0>, <&moca_pdh_ch1>, <&moca_pdh_ch2>; 
			clock-names = "osc_moca", "mocamac_54", 
			  "mocamac_gisb", "mocamac_scb", "mocaphy_54", 
			  "mocaphy_gisb", "moca_pdh_ch0", "moca_pdh_ch1", 
			  "moca_pdh_ch2"; 
		};

		sw_pcie0 : sw_pcie0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&osc_pcie0>, <&pcie_alwayson>, <&pcie_54>, 
			  <&pcie_gisb>, <&pcie_scb>; 
			clock-names = "osc_pcie0", "pcie_alwayson", "pcie_54", 
			  "pcie_gisb", "pcie_scb"; 
		};

		sw_pcie1 : sw_pcie1 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&osc_sata_pcie1>, <&pcie_alwayson>, 
			  <&pcie1_gisb_pcie>, <&pcie1_scb_pcie>, 
			  <&sata3_108_pcie>; 
			clock-names = "osc_sata_pcie1", "pcie_alwayson", 
			  "pcie1_gisb_pcie", "pcie1_scb_pcie", 
			  "sata3_108_pcie"; 
		};

		sw_sata3 : sw_sata3 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&osc_sata_pcie1>, <&sata3_54>, 
			  <&sata3_gisb>, <&sata3_scb>; 
			clock-names = "osc_sata_pcie1", "sata3_54", 
			  "sata3_gisb", "sata3_scb"; 
		};

		sw_sdio : sw_sdio {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&hif_sdio_card>, <&hif_sdio_emmc>; 
			clock-names = "hif_sdio_card", "hif_sdio_emmc"; 
		};

		sw_switch : sw_switch {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&swi_54>, <&swi_gisb>, <&swi_scb>, 
			  <&net_pdh_ch0>, <&net_pdh_ch1>, <&net_pdh_ch3>; 
			clock-names = "swi_54", "swi_gisb", "swi_scb", 
			  "net_pdh_ch0", "net_pdh_ch1", "net_pdh_ch3"; 
		};

		sw_sysport : sw_sysport {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&sprt_tx_syssprt>, <&sprt_gisb>, 
			  <&sprt_gmii_tx>, <&sprt_rx_scb>, <&sprt_rx_sys>, 
			  <&sprt_scb>, <&sprt_tx_scb>, <&net_pdh_ch3>; 
			clock-names = "sprt_tx_syssprt", "sprt_gisb", 
			  "sprt_gmii_tx", "sprt_rx_scb", "sprt_rx_sys", 
			  "sprt_scb", "sprt_tx_scb", "net_pdh_ch3"; 
		};

		sw_sysportwol : sw_sysportwol {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&sprt_tx_syssprt>, <&sprt_rx_sys>, 
			  <&net_pdh_ch3>; 
			clock-names = "sprt_tx_syssprt", "sprt_rx_sys", 
			  "net_pdh_ch3"; 
		};

		sw_usb20 : sw_usb20 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&usb0_54_mdio>, <&usb0_54>, <&usb0_gisb>, 
			  <&usb0_scb>, <&usb0_108_ahb>, <&usb0_108_axi>, 
			  <&lc_pdh_ch2>; 
			clock-names = "usb0_54_mdio", "usb0_54", "usb0_gisb", 
			  "usb0_scb", "usb0_108_ahb", "usb0_108_axi", 
			  "lc_pdh_ch2"; 
		};

		sw_usb21 : sw_usb21 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&usb1_54_mdio>, <&usb1_54>, <&usb1_gisb>, 
			  <&usb1_scb>, <&usb1_108_ahb>, <&usb1_108_axi>, 
			  <&lc_pdh_ch1>; 
			clock-names = "usb1_54_mdio", "usb1_54", "usb1_gisb", 
			  "usb1_scb", "usb1_108_ahb", "usb1_108_axi", 
			  "lc_pdh_ch1"; 
		};

		sw_usb30 : sw_usb30 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&osc_usb30>, <&usb0_54_mdio>, <&usb0_54>, 
			  <&usb0_gisb>, <&usb0_scb>, <&usb0_108_ahb>, 
			  <&usb0_108_axi>, <&lc_pdh_ch4>; 
			clock-names = "osc_usb30", "usb0_54_mdio", "usb0_54", 
			  "usb0_gisb", "usb0_scb", "usb0_108_ahb", 
			  "usb0_108_axi", "lc_pdh_ch4"; 
		};

	};
',
          'funcs' => {
                       'MOCAWOM' => [
                                      'sw_mocawom'
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
                       'USB30' => [
                                    'sw_usb30'
                                  ],
                       'SPI' => [
                                  'sw_spi'
                                ],
                       'USB21' => [
                                    'sw_usb21'
                                  ],
                       'PCIE1' => [
                                    'sw_pcie1'
                                  ],
                       'PCIE0' => [
                                    'sw_pcie0'
                                  ],
                       'SATA3' => [
                                    'sw_sata3'
                                  ],
                       'GPHY' => [
                                   'sw_gphy'
                                 ],
                       'CPU' => [
                                  'sw_cpu'
                                ],
                       'SWITCH' => [
                                     'sw_switch'
                                   ],
                       'MOCAWOL' => [
                                      'sw_mocawol'
                                    ],
                       'MOCA' => [
                                   'sw_moca'
                                 ],
                       'SYSPORT' => [
                                      'sw_sysport'
                                    ],
                       'SYSPORTWOL' => [
                                         'sw_sysportwol'
                                       ],
                       'SWITCH_CTRL_DIV' => [
                                              'sw_switch_mdiv'
                                            ]
                     }
        };


# [---]   cpu_mdiv_ch0 aka sw_cpu => CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_0
#                                    /MDIV_CH0
# [---]   cpu_ndiv_int => CLKGEN_PLL_CPU_PLL_DIV
#                                    /NDIV_INT
# [---]   cpu_pdiv => CLKGEN_PLL_CPU_PLL_DIV
#                                    /PDIV
# [---] R fixed0 => fixed0
# [---] R fixed1 => fixed1
# [---] R fixed2 => fixed2
# [---]   hif_sdio_card => CLKGEN_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_HIF_SDIO_CARD_CLOCK
# [---]   hif_sdio_emmc => CLKGEN_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_HIF_SDIO_EMMC_CLOCK
# [---]   hif_spi aka sw_spi => CLKGEN_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_HIF_SPI_CLOCK
# [---]   lc_dis_ch0 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_0
#                                    /CLOCK_DIS_CH0
# [---]   lc_dis_ch1 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_1
#                                    /CLOCK_DIS_CH1
# [---]   lc_dis_ch2 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_2
#                                    /CLOCK_DIS_CH2
# [---]   lc_dis_ch4 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_4
#                                    /CLOCK_DIS_CH4
# [---]   lc_pdh_ch0 aka sw_gphy => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_0
#                                    /POST_DIVIDER_HOLD_CH0
# [---]   lc_pdh_ch1 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_1
#                                    /POST_DIVIDER_HOLD_CH1
# [---]   lc_pdh_ch2 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_2
#                                    /POST_DIVIDER_HOLD_CH2
# [---]   lc_pdh_ch4 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_4
#                                    /POST_DIVIDER_HOLD_CH4
# [---] R lc_pwrdn_req => CLKGEN_LC_PLL_CTRL_WRAPPER_CONTROL
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
# [---]   net_dis_ch2 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_2
#                                    /CLOCK_DIS_CH2
# [---]   net_dis_ch3 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_3
#                                    /CLOCK_DIS_CH3
# [---]   net_mdiv_ch0 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_0
#                                    /MDIV_CH0
# [---]   net_mdiv_ch1 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_1
#                                    /MDIV_CH1
# [---]   net_mdiv_ch2 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_2
#                                    /MDIV_CH2
# [---]   net_mdiv_ch3 aka sw_switch_mdiv => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_3
#                                    /MDIV_CH3
# [---]   net_ndiv_int => CLKGEN_PLL_NETWORK_PLL_DIV
#                                    /NDIV_INT
# [---]   net_pdh_ch0 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_0
#                                    /POST_DIVIDER_HOLD_CH0
# [---]   net_pdh_ch1 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_1
#                                    /POST_DIVIDER_HOLD_CH1
# [---]   net_pdh_ch2 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_2
#                                    /POST_DIVIDER_HOLD_CH2
# [---]   net_pdh_ch3 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_3
#                                    /POST_DIVIDER_HOLD_CH3
# [---]   net_pdiv => CLKGEN_PLL_NETWORK_PLL_DIV
#                                    /PDIV
# [---]   net_pwrdn_req => CLKGEN_NETWORK_PLL_CTRL_WRAPPER_CONTROL
#                                    /PWRDN_PLL_REQ
# [---] R osc_moca => AON_CTRL_ANA_XTAL_CONTROL
#                                    /osc_cml_sel_pd_moca
# [---] R osc_pcie0 => AON_CTRL_ANA_XTAL_CONTROL
#                                    /osc_cml_sel_pd_pcie0
# [---] R osc_sata_pcie1 => AON_CTRL_ANA_XTAL_CONTROL
#                                    /osc_cml_sel_pd_sata_pcie1
# [---] R osc_usb30 => AON_CTRL_ANA_XTAL_CONTROL
#                                    /osc_cml_sel_pd_usb30
# [---] R pcie1_gisb_pcie => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE
#                                    /PCIE1_GISB_CLOCK_ENABLE_PCIE
# [---] R pcie1_scb_pcie => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE
#                                    /PCIE1_SCB_CLOCK_ENABLE_PCIE
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
# [---] R sprt_gisb => CLKGEN_VEC_AIO_GFX_TOP_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT_GISB_CLOCK_ENABLE_SYSTEMPORT
# [---] R sprt_gmii_tx => CLKGEN_VEC_AIO_GFX_TOP_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT_GMII_TX_CLOCK_ENABLE_SYSTEMPORT
# [---] R sprt_rx_scb => CLKGEN_VEC_AIO_GFX_TOP_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT_RX_SCB_CLOCK_ENABLE_SYSTEMPORT
# [---]   sprt_rx_sys => CLKGEN_VEC_AIO_GFX_TOP_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT_RX_SYS_CLOCK_ENABLE_SYSTEMPORT
# [---] R sprt_scb => CLKGEN_VEC_AIO_GFX_TOP_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT_SCB_CLOCK_ENABLE_SYSTEMPORT
# [---] R sprt_tx_scb => CLKGEN_VEC_AIO_GFX_TOP_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT_TX_SCB_CLOCK_ENABLE_SYSTEMPORT
# [---]   sprt_tx_syssprt => CLKGEN_VEC_AIO_GFX_TOP_INST_CLOCK_ENABLESYSTEMPORT
#                                    /SYSTEMPORT_TX_SYS_CLOCK_ENABLESYSTEMPORT
# [---]   sw_moca => sw_moca
# [---]   sw_pcie0 => sw_pcie0
# [---]   sw_pcie1 => sw_pcie1
# [---]   sw_sata3 => sw_sata3
# [---]   sw_sdio => sw_sdio
# [---]   sw_switch => sw_switch
# [---]   sw_sysport => sw_sysport
# [---]   sw_sysportwol => sw_sysportwol
# [---]   sw_usb20 => sw_usb20
# [---]   sw_usb21 => sw_usb21
# [---]   sw_usb30 => sw_usb30
# [---] R swi_54 => CLKGEN_SWITCH_TOP_INST_CLOCK_ENABLE
#                                    /SWITCH_54_CLOCK_ENABLE
# [---] R swi_gisb => CLKGEN_SWITCH_TOP_INST_CLOCK_ENABLE
#                                    /SWITCH_GISB_CLOCK_ENABLE
# [---] R swi_scb => CLKGEN_SWITCH_TOP_INST_CLOCK_ENABLE
#                                    /SWITCH_SCB_CLOCK_ENABLE
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
# [---] R usb1_108_ahb => CLKGEN_USB1_TOP_INST_CLOCK_ENABLE_AHB
#                                    /USB1_108_CLOCK_ENABLE_AHB
# [---] R usb1_108_axi => CLKGEN_USB1_TOP_INST_CLOCK_ENABLE_AXI
#                                    /USB1_108_CLOCK_ENABLE_AXI
# [---] R usb1_54 => CLKGEN_USB1_TOP_INST_CLOCK_ENABLE
#                                    /USB1_54_CLOCK_ENABLE
# [---] R usb1_54_mdio => CLKGEN_USB1_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_USB1_54_MDIO_CLOCK
# [---] R usb1_gisb => CLKGEN_USB1_TOP_INST_CLOCK_ENABLE
#                                    /USB1_GISB_CLOCK_ENABLE
# [---] R usb1_scb => CLKGEN_USB1_TOP_INST_CLOCK_ENABLE
#                                    /USB1_SCB_CLOCK_ENABLE
