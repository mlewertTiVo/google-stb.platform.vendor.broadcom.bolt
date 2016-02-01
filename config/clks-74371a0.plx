$VAR1 = {
          'rdb_sha1' => 'gFMPAXOLfEdVALjA5kHOGqkflr0',
          'fail' => 0,
          'date' => 'Mon Feb 16 08:06:39 PST 2015',
          'rdb_version' => 'rdb-v2',
          'rdb_dir' => '/home/stblinux/clkgen/74371a0/current',
          'clkgen_version' => 'clkgen-v4',
          'chip' => '74371a0',
          'aliases' => {},
          'unhandled_linux_funcs' => 'CPU, HIF, MEMSYS0, MPI',
          'invocation' => 'clkgen.pl --sw_nodes -v -g -r -P -c 74371a0',
          'num_clks' => 137,
          'clks' => '	brcmstb-clks {
		#address-cells = <1>;
		#size-cells = <1>;
		reg = <0xf04e0000 0x5b4 0xf04d1500 0xbc 0xf0410000 0x400>;
		ranges;

		genet0_alwayson : genet0_alwayson@f04e034c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e034c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		fixed4 : fixed4 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		net_ldo_pwron : net_ldo_pwron@f04e0118 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0118 0x4>;
			bit-shift = <0>;
			brcm,delay = <0 2>;
			clocks = <&fixed4>; 
			clock-names = "fixed4"; 
		};

		net_pwron : net_pwron@f04e0128 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0128 0x4>;
			bit-shift = <0>;
			clocks = <&net_ldo_pwron>; 
			clock-names = "net_ldo_pwron"; 
		};

		iso_net : iso_net@f04e04b8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04b8 0x4>;
			bit-shift = <3>;
			set-bit-to-disable;
			clocks = <&net_pwron>; 
			clock-names = "net_pwron"; 
		};

		net_pll_rsta : net_pll_rsta@f04e012c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e012c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			brcm,delay = <30 0>;
			clocks = <&iso_net>; 
			clock-names = "iso_net"; 
		};

		net_pll_rstd : net_pll_rstd@f04e012c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e012c 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&net_pll_rsta>; 
			clock-names = "net_pll_rsta"; 
		};

		net_pdiv : net_pdiv@f04e0110 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0110 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&net_pll_rstd>; 
			clock-names = "net_pll_rstd"; 
		};

		net_ndiv_int : net_ndiv_int {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <1>;
			clock-mult = <125>;
			clocks = <&net_pdiv>; 
			clock-names = "net_pdiv"; 
		};

		net_mdiv_ch1 : net_mdiv_ch1@f04e0108 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0108 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch1 : net_dis_ch1@f04e0108 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0108 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch1>; 
			clock-names = "net_mdiv_ch1"; 
		};

		net_pdh_ch1 : net_pdh_ch1@f04e0108 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0108 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch1>; 
			clock-names = "net_dis_ch1"; 
		};

		genet0_sys_fast : genet0_sys_fast@f04e034c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e034c 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&net_pdh_ch1>; 
			clock-names = "net_pdh_ch1"; 
		};

		genet0_sys_pm : genet0_sys_pm@f04e034c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e034c 0x4>;
			bit-shift = <2>;
			set-bit-to-disable;
		};

		genet0_sys_slow : genet0_sys_slow@f04e034c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e034c 0x4>;
			bit-shift = <3>;
			set-bit-to-disable;
		};

		genet1_alwayson : genet1_alwayson@f04e034c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e034c 0x4>;
			bit-shift = <4>;
			set-bit-to-disable;
		};

		genet1_sys_fast : genet1_sys_fast@f04e034c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e034c 0x4>;
			bit-shift = <5>;
			set-bit-to-disable;
			clocks = <&net_pdh_ch1>; 
			clock-names = "net_pdh_ch1"; 
		};

		genet1_sys_pm : genet1_sys_pm@f04e034c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e034c 0x4>;
			bit-shift = <6>;
			set-bit-to-disable;
		};

		genet1_sys_slow : genet1_sys_slow@f04e034c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e034c 0x4>;
			bit-shift = <7>;
			set-bit-to-disable;
		};

		genet_54 : genet_54@f04e0354 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0354 0x4>;
			bit-shift = <0>;
		};

		genet_scb : genet_scb@f04e0354 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0354 0x4>;
			bit-shift = <1>;
		};

		net_mdiv_ch0 : net_mdiv_ch0@f04e0104 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0104 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch0 : net_dis_ch0@f04e0104 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0104 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch0>; 
			clock-names = "net_mdiv_ch0"; 
		};

		net_pdh_ch0 : net_pdh_ch0@f04e0104 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0104 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch0>; 
			clock-names = "net_dis_ch0"; 
		};

		genet0_250 : genet0_250@f04e0358 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0358 0x4>;
			bit-shift = <0>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		genet0_eee : genet0_eee@f04e0358 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0358 0x4>;
			bit-shift = <1>;
		};

		genet0_gisb : genet0_gisb@f04e0358 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0358 0x4>;
			bit-shift = <2>;
		};

		genet0_gmii : genet0_gmii@f04e0358 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0358 0x4>;
			bit-shift = <3>;
		};

		genet0_hfb : genet0_hfb@f04e0358 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0358 0x4>;
			bit-shift = <4>;
		};

		genet0_l2intr : genet0_l2intr@f04e0358 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0358 0x4>;
			bit-shift = <5>;
		};

		genet0_scb : genet0_scb@f04e0358 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0358 0x4>;
			bit-shift = <6>;
		};

		genet0_umac_rx : genet0_umac_rx@f04e0358 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0358 0x4>;
			bit-shift = <7>;
		};

		genet0_umac_tx : genet0_umac_tx@f04e0358 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0358 0x4>;
			bit-shift = <8>;
		};

		genet1_250 : genet1_250@f04e0360 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0360 0x4>;
			bit-shift = <0>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		genet1_eee : genet1_eee@f04e0360 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0360 0x4>;
			bit-shift = <1>;
		};

		genet1_gisb : genet1_gisb@f04e0360 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0360 0x4>;
			bit-shift = <2>;
		};

		genet1_gmii : genet1_gmii@f04e0360 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0360 0x4>;
			bit-shift = <3>;
		};

		genet1_hfb : genet1_hfb@f04e0360 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0360 0x4>;
			bit-shift = <4>;
		};

		genet1_l2intr : genet1_l2intr@f04e0360 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0360 0x4>;
			bit-shift = <5>;
		};

		genet1_scb : genet1_scb@f04e0360 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0360 0x4>;
			bit-shift = <6>;
		};

		genet1_umac_rx : genet1_umac_rx@f04e0360 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0360 0x4>;
			bit-shift = <7>;
		};

		genet1_umac_tx : genet1_umac_tx@f04e0360 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0360 0x4>;
			bit-shift = <8>;
		};

		fixed3 : fixed3 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		moca_ldo_pwron : moca_ldo_pwron@f04e00e0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00e0 0x4>;
			bit-shift = <0>;
			brcm,delay = <0 2>;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&fixed3>; 
			clock-names = "fixed3"; 
		};

		moca_pwron : moca_pwron@f04e00f0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00f0 0x4>;
			bit-shift = <0>;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&moca_ldo_pwron>; 
			clock-names = "moca_ldo_pwron"; 
		};

		iso_moca : iso_moca@f04e04b8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04b8 0x4>;
			bit-shift = <2>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&moca_pwron>; 
			clock-names = "moca_pwron"; 
		};

		moca_pll_rsta : moca_pll_rsta@f04e00f4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00f4 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			brcm,delay = <30 0>;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&iso_moca>; 
			clock-names = "iso_moca"; 
		};

		moca_pll_rstd : moca_pll_rstd@f04e00f4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00f4 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&moca_pll_rsta>; 
			clock-names = "moca_pll_rsta"; 
		};

		moca_pdiv : moca_pdiv@f04e00d8 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00d8 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&moca_pll_rstd>; 
			clock-names = "moca_pll_rstd"; 
		};

		moca_ndiv_int : moca_ndiv_int {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <1>;
			clock-mult = <200>;
			clocks = <&moca_pdiv>; 
			clock-names = "moca_pdiv"; 
		};

		moca_mdiv_ch2 : moca_mdiv_ch2@f04e00c8 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00c8 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&moca_ndiv_int>; 
			clock-names = "moca_ndiv_int"; 
		};

		moca_dis_ch2 : moca_dis_ch2@f04e00c8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00c8 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&moca_mdiv_ch2>; 
			clock-names = "moca_mdiv_ch2"; 
		};

		moca_pdh_ch2 : sw_mocawol : sw_mocawom : moca_pdh_ch2@f04e00c8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00c8 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&moca_dis_ch2>; 
			clock-names = "moca_dis_ch2"; 
		};

		hif_sdio_card : hif_sdio_card@f04e03e4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03e4 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&moca_pdh_ch2>; 
			clock-names = "moca_pdh_ch2"; 
		};

		hif_sdio_emmc : hif_sdio_emmc@f04e03e4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03e4 0x4>;
			bit-shift = <2>;
			set-bit-to-disable;
			clocks = <&moca_pdh_ch2>; 
			clock-names = "moca_pdh_ch2"; 
		};

		net_mdiv_ch2 : net_mdiv_ch2@f04e010c {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e010c 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch2 : net_dis_ch2@f04e010c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e010c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch2>; 
			clock-names = "net_mdiv_ch2"; 
		};

		net_pdh_ch2 : net_pdh_ch2@f04e010c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e010c 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch2>; 
			clock-names = "net_dis_ch2"; 
		};

		hif_spi : sw_spi : hif_spi@f04e03e4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03e4 0x4>;
			bit-shift = <3>;
			set-bit-to-disable;
			clocks = <&net_pdh_ch2>; 
			clock-names = "net_pdh_ch2"; 
		};

		mocamac_54 : mocamac_54@f04e0424 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0424 0x4>;
			bit-shift = <0>;
		};

		mocamac_gisb : mocamac_gisb@f04e0424 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0424 0x4>;
			bit-shift = <1>;
		};

		mocamac_scb : mocamac_scb@f04e0424 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0424 0x4>;
			bit-shift = <2>;
		};

		mocaphy_54 : mocaphy_54@f04e0430 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0430 0x4>;
			bit-shift = <0>;
		};

		mocaphy_gisb : mocaphy_gisb@f04e0430 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0430 0x4>;
			bit-shift = <1>;
		};

		pcie_alwayson : pcie_alwayson@f04e0454 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0454 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		pcie_108 : pcie_108@f04e045c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e045c 0x4>;
			bit-shift = <0>;
		};

		pcie_54 : pcie_54@f04e045c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e045c 0x4>;
			bit-shift = <1>;
		};

		pcie_gisb : pcie_gisb@f04e045c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e045c 0x4>;
			bit-shift = <2>;
		};

		pcie_scb : pcie_scb@f04e045c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e045c 0x4>;
			bit-shift = <3>;
		};

		fixed2 : fixed2 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		lc_ldo_pwron : lc_ldo_pwron@f04e009c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e009c 0x4>;
			bit-shift = <0>;
			brcm,delay = <0 40>;
			clocks = <&fixed2>; 
			clock-names = "fixed2"; 
		};

		lc_pwron : lc_pwron@f04e00b0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00b0 0x4>;
			bit-shift = <0>;
			clocks = <&lc_ldo_pwron>; 
			clock-names = "lc_ldo_pwron"; 
		};

		iso_lc : iso_lc@f04e04b8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04b8 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&lc_pwron>; 
			clock-names = "lc_pwron"; 
		};

		sata3_spd_scn : sata3_spd_scn@f04e04e4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04e4 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		sata3_54 : sata3_54@f04e04ec {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04ec 0x4>;
			bit-shift = <0>;
		};

		sata3_gisb : sata3_gisb@f04e04ec {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04ec 0x4>;
			bit-shift = <1>;
		};

		sata3_scb : sata3_scb@f04e04ec {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04ec 0x4>;
			bit-shift = <2>;
		};

		usb0_54_mdio : usb0_54_mdio@f04e0518 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0518 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		usb0_freerun : usb0_freerun@f04e0518 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0518 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
		};

		usb0_54 : usb0_54@f04e0520 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0520 0x4>;
			bit-shift = <0>;
		};

		usb0_gisb : usb0_gisb@f04e0520 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0520 0x4>;
			bit-shift = <1>;
		};

		usb0_scb : usb0_scb@f04e0520 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0520 0x4>;
			bit-shift = <2>;
		};

		usb0_108_ahb : usb0_108_ahb@f04e0524 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0524 0x4>;
			bit-shift = <0>;
		};

		usb0_108_axi : usb0_108_axi@f04e052c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e052c 0x4>;
			bit-shift = <0>;
		};

		usb1_54_mdio : usb1_54_mdio@f04e053c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e053c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		usb1_freerun : usb1_freerun@f04e053c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e053c 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
		};

		usb1_54 : usb1_54@f04e0544 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0544 0x4>;
			bit-shift = <0>;
		};

		usb1_gisb : usb1_gisb@f04e0544 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0544 0x4>;
			bit-shift = <1>;
		};

		usb1_scb : usb1_scb@f04e0544 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0544 0x4>;
			bit-shift = <2>;
		};

		usb1_108_ahb : usb1_108_ahb@f04e0548 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0548 0x4>;
			bit-shift = <0>;
		};

		usb1_108_axi : usb1_108_axi@f04e0550 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0550 0x4>;
			bit-shift = <0>;
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

		lc_pll_rsta : lc_pll_rsta@f04e00b4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00b4 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			brcm,delay = <100 0>;
			clocks = <&lc_pwron>; 
			clock-names = "lc_pwron"; 
		};

		lc_pll_rstd : lc_pll_rstd@f04e00b4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00b4 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&lc_pll_rsta>; 
			clock-names = "lc_pll_rsta"; 
		};

		lc_pdiv : lc_pdiv@f04e008c {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e008c 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&lc_pll_rstd>; 
			clock-names = "lc_pll_rstd"; 
		};

		lc_ndiv_int : lc_ndiv_int {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <1>;
			clock-mult = <100>;
			clocks = <&lc_pdiv>; 
			clock-names = "lc_pdiv"; 
		};

		lc_mdiv_ch0 : lc_mdiv_ch0@f04e0074 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0074 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&lc_ndiv_int>; 
			clock-names = "lc_ndiv_int"; 
		};

		lc_dis_ch0 : lc_dis_ch0@f04e0074 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0074 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_mdiv_ch0>; 
			clock-names = "lc_mdiv_ch0"; 
		};

		lc_pdh_ch0 : lc_pdh_ch0@f04e0074 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0074 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&lc_dis_ch0>; 
			clock-names = "lc_dis_ch0"; 
		};

		lc_mdiv_ch1 : lc_mdiv_ch1@f04e0078 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0078 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&lc_ndiv_int>; 
			clock-names = "lc_ndiv_int"; 
		};

		lc_dis_ch1 : lc_dis_ch1@f04e0078 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0078 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_mdiv_ch1>; 
			clock-names = "lc_mdiv_ch1"; 
		};

		lc_pdh_ch1 : lc_pdh_ch1@f04e0078 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0078 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&lc_dis_ch1>; 
			clock-names = "lc_dis_ch1"; 
		};

		lc_mdiv_ch2 : lc_mdiv_ch2@f04e007c {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e007c 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&lc_ndiv_int>; 
			clock-names = "lc_ndiv_int"; 
		};

		lc_dis_ch2 : lc_dis_ch2@f04e007c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e007c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_mdiv_ch2>; 
			clock-names = "lc_mdiv_ch2"; 
		};

		lc_pdh_ch2 : lc_pdh_ch2@f04e007c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e007c 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&lc_dis_ch2>; 
			clock-names = "lc_dis_ch2"; 
		};

		lc_mdiv_ch4 : lc_mdiv_ch4@f04e0080 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0080 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&lc_ndiv_int>; 
			clock-names = "lc_ndiv_int"; 
		};

		lc_dis_ch4 : lc_dis_ch4@f04e0080 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0080 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_mdiv_ch4>; 
			clock-names = "lc_mdiv_ch4"; 
		};

		lc_pdh_ch4 : lc_pdh_ch4@f04e0080 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0080 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&lc_dis_ch4>; 
			clock-names = "lc_dis_ch4"; 
		};

		moca_mdiv_ch0 : sw_moca_cpu : moca_mdiv_ch0@f04e00c0 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00c0 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&moca_ndiv_int>; 
			clock-names = "moca_ndiv_int"; 
		};

		moca_dis_ch0 : moca_dis_ch0@f04e00c0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00c0 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&moca_mdiv_ch0>; 
			clock-names = "moca_mdiv_ch0"; 
		};

		moca_pdh_ch0 : moca_pdh_ch0@f04e00c0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00c0 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&moca_dis_ch0>; 
			clock-names = "moca_dis_ch0"; 
		};

		moca_mdiv_ch1 : sw_moca_phy : moca_mdiv_ch1@f04e00c4 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00c4 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&moca_ndiv_int>; 
			clock-names = "moca_ndiv_int"; 
		};

		moca_dis_ch1 : moca_dis_ch1@f04e00c4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00c4 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&moca_mdiv_ch1>; 
			clock-names = "moca_mdiv_ch1"; 
		};

		moca_pdh_ch1 : moca_pdh_ch1@f04e00c4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00c4 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&moca_dis_ch1>; 
			clock-names = "moca_dis_ch1"; 
		};

		moca_mdiv_ch5 : sw_mpi : moca_mdiv_ch5@f04e00d4 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00d4 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&moca_ndiv_int>; 
			clock-names = "moca_ndiv_int"; 
		};

		fixed8 : fixed8 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		sys0_pdiv : sys0_pdiv@f04e01f0 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e01f0 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&fixed8>; 
			clock-names = "fixed8"; 
		};

		sys0_ndiv_int : sys0_ndiv_int {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <1>;
			clock-mult = <144>;
			clocks = <&sys0_pdiv>; 
			clock-names = "sys0_pdiv"; 
		};

		sys0_mdiv_ch2 : sw_hif : sw_memsys0 : sys0_mdiv_ch2@f04e01e0 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e01e0 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&sys0_ndiv_int>; 
			clock-names = "sys0_ndiv_int"; 
		};

		sw_cpu : sw_cpu {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&cpu_mdiv_ch0>, <&sys0_mdiv_ch2>; 
			clock-names = "cpu_mdiv_ch0", "sys0_mdiv_ch2"; 
		};

		sw_genet0 : sw_genet0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet0_alwayson>, <&genet0_sys_fast>, 
			  <&genet0_sys_pm>, <&genet0_sys_slow>, <&genet_54>, 
			  <&genet_scb>, <&genet0_250>, <&genet0_gisb>, 
			  <&genet0_gmii>, <&genet0_hfb>, <&genet0_l2intr>, 
			  <&genet0_scb>, <&genet0_umac_rx>, <&genet0_umac_tx>, 
			  <&sys0_mdiv_ch2>; 
			clock-names = "genet0_alwayson", "genet0_sys_fast", 
			  "genet0_sys_pm", "genet0_sys_slow", "genet_54", 
			  "genet_scb", "genet0_250", "genet0_gisb", 
			  "genet0_gmii", "genet0_hfb", "genet0_l2intr", 
			  "genet0_scb", "genet0_umac_rx", "genet0_umac_tx", 
			  "sys0_mdiv_ch2"; 
		};

		sw_genet1 : sw_genet1 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet1_alwayson>, <&genet1_sys_fast>, 
			  <&genet1_sys_pm>, <&genet1_sys_slow>, <&genet1_250>, 
			  <&genet1_gisb>, <&genet1_gmii>, <&genet1_hfb>, 
			  <&genet1_l2intr>, <&genet1_scb>, <&genet1_umac_rx>, 
			  <&genet1_umac_tx>, <&sys0_mdiv_ch2>; 
			clock-names = "genet1_alwayson", "genet1_sys_fast", 
			  "genet1_sys_pm", "genet1_sys_slow", "genet1_250", 
			  "genet1_gisb", "genet1_gmii", "genet1_hfb", 
			  "genet1_l2intr", "genet1_scb", "genet1_umac_rx", 
			  "genet1_umac_tx", "sys0_mdiv_ch2"; 
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

		sw_gphy : sw_gphy {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&iso_lc>, <&lc_pdh_ch0>; 
			clock-names = "iso_lc", "lc_pdh_ch0"; 
		};

		sw_moca : sw_moca {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&mocamac_54>, <&mocamac_gisb>, 
			  <&mocamac_scb>, <&mocaphy_54>, <&mocaphy_gisb>, 
			  <&moca_pdh_ch0>, <&moca_pdh_ch1>, <&moca_pdh_ch2>, 
			  <&sys0_mdiv_ch2>; 
			clock-names = "mocamac_54", "mocamac_gisb", 
			  "mocamac_scb", "mocaphy_54", "mocaphy_gisb", 
			  "moca_pdh_ch0", "moca_pdh_ch1", "moca_pdh_ch2", 
			  "sys0_mdiv_ch2"; 
		};

		sw_pcie0 : sw_pcie0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&pcie_alwayson>, <&pcie_108>, <&pcie_54>, 
			  <&pcie_gisb>, <&pcie_scb>, <&sys0_mdiv_ch2>; 
			clock-names = "pcie_alwayson", "pcie_108", "pcie_54", 
			  "pcie_gisb", "pcie_scb", "sys0_mdiv_ch2"; 
		};

		sw_sata3 : sw_sata3 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&sata3_spd_scn>, <&sata3_54>, <&sata3_gisb>, 
			  <&sata3_scb>, <&sys0_mdiv_ch2>; 
			clock-names = "sata3_spd_scn", "sata3_54", 
			  "sata3_gisb", "sata3_scb", "sys0_mdiv_ch2"; 
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
			clocks = <&iso_lc>, <&usb0_54_mdio>, <&usb0_freerun>, 
			  <&usb0_54>, <&usb0_gisb>, <&usb0_scb>, 
			  <&usb0_108_ahb>, <&usb0_108_axi>, <&lc_pdh_ch2>, 
			  <&sys0_mdiv_ch2>; 
			clock-names = "iso_lc", "usb0_54_mdio", 
			  "usb0_freerun", "usb0_54", "usb0_gisb", "usb0_scb", 
			  "usb0_108_ahb", "usb0_108_axi", "lc_pdh_ch2", 
			  "sys0_mdiv_ch2"; 
		};

		sw_usb21 : sw_usb21 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&iso_lc>, <&usb1_54_mdio>, <&usb1_freerun>, 
			  <&usb1_54>, <&usb1_gisb>, <&usb1_scb>, 
			  <&usb1_108_ahb>, <&usb1_108_axi>, <&lc_pdh_ch1>, 
			  <&sys0_mdiv_ch2>; 
			clock-names = "iso_lc", "usb1_54_mdio", 
			  "usb1_freerun", "usb1_54", "usb1_gisb", "usb1_scb", 
			  "usb1_108_ahb", "usb1_108_axi", "lc_pdh_ch1", 
			  "sys0_mdiv_ch2"; 
		};

		sw_usb30 : sw_usb30 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&iso_lc>, <&usb0_54_mdio>, <&usb0_freerun>, 
			  <&usb0_54>, <&usb0_gisb>, <&usb0_scb>, 
			  <&usb0_108_ahb>, <&usb0_108_axi>, <&lc_pdh_ch4>, 
			  <&sys0_mdiv_ch2>; 
			clock-names = "iso_lc", "usb0_54_mdio", 
			  "usb0_freerun", "usb0_54", "usb0_gisb", "usb0_scb", 
			  "usb0_108_ahb", "usb0_108_axi", "lc_pdh_ch4", 
			  "sys0_mdiv_ch2"; 
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
                       'USB21' => [
                                    'sw_usb21'
                                  ],
                       'HIF' => [
                                  'sw_hif'
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
                       'MPI' => [
                                  'sw_mpi'
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
# [---] R fixed2 => fixed2
# [---] R fixed3 => fixed3
# [---] R fixed4 => fixed4
# [---] R fixed8 => fixed8
# [---]   genet0_250 => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_CLK_250_CLOCK_ENABLE_GENET0
# [---] R genet0_alwayson => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET0_ALWAYSON_CLOCK
# [---] R genet0_eee => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_EEE_CLOCK_ENABLE_GENET0
# [---] R genet0_gisb => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_GISB_CLOCK_ENABLE_GENET0
# [---] R genet0_gmii => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_GMII_CLOCK_ENABLE_GENET0
# [---] R genet0_hfb => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_HFB_CLOCK_ENABLE_GENET0
# [---] R genet0_l2intr => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_L2INTR_CLOCK_ENABLE_GENET0
# [---] R genet0_scb => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_SCB_CLOCK_ENABLE_GENET0
# [---]   genet0_sys_fast => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET0_SYSTEM_FAST_CLOCK
# [---] R genet0_sys_pm => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET0_SYSTEM_PM_CLOCK
# [---] R genet0_sys_slow => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET0_SYSTEM_SLOW_CLOCK
# [---] R genet0_umac_rx => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_UNIMAC_SYS_RX_CLOCK_ENABLE_GENET0
# [---] R genet0_umac_tx => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_UNIMAC_SYS_TX_CLOCK_ENABLE_GENET0
# [---]   genet1_250 => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_CLK_250_CLOCK_ENABLE_GENET1
# [---] R genet1_alwayson => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET1_ALWAYSON_CLOCK
# [---] R genet1_eee => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_EEE_CLOCK_ENABLE_GENET1
# [---] R genet1_gisb => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_GISB_CLOCK_ENABLE_GENET1
# [---] R genet1_gmii => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_GMII_CLOCK_ENABLE_GENET1
# [---] R genet1_hfb => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_HFB_CLOCK_ENABLE_GENET1
# [---] R genet1_l2intr => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_L2INTR_CLOCK_ENABLE_GENET1
# [---] R genet1_scb => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_SCB_CLOCK_ENABLE_GENET1
# [---]   genet1_sys_fast => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET1_SYSTEM_FAST_CLOCK
# [---] R genet1_sys_pm => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET1_SYSTEM_PM_CLOCK
# [---] R genet1_sys_slow => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET1_SYSTEM_SLOW_CLOCK
# [---] R genet1_umac_rx => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_UNIMAC_SYS_RX_CLOCK_ENABLE_GENET1
# [---] R genet1_umac_tx => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_UNIMAC_SYS_TX_CLOCK_ENABLE_GENET1
# [---] R genet_54 => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE
#                                    /GENET_54_CLOCK_ENABLE
# [---] R genet_scb => CLKGEN_DUAL_GENET_TOP_DUAL_RGMII_INST_CLOCK_ENABLE
#                                    /GENET_SCB_CLOCK_ENABLE
# [---]   hif_sdio_card => CLKGEN_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_HIF_SDIO_CARD_CLOCK
# [---]   hif_sdio_emmc => CLKGEN_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_HIF_SDIO_EMMC_CLOCK
# [---]   hif_spi aka sw_spi => CLKGEN_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_HIF_SPI_CLOCK
# [---]   iso_lc => CLKGEN_PM_PLL_LDO_POWERUP
#                                    /ISO_CLOCK_PLL_LC
# [---]   iso_moca => CLKGEN_PM_PLL_LDO_POWERUP
#                                    /ISO_CLOCK_PLL_MOCA
# [---]   iso_net => CLKGEN_PM_PLL_LDO_POWERUP
#                                    /ISO_CLOCK_PLL_NETWORK
# [---]   lc_dis_ch0 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_0
#                                    /CLOCK_DIS_CH0
# [---]   lc_dis_ch1 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_1
#                                    /CLOCK_DIS_CH1
# [---]   lc_dis_ch2 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_2
#                                    /CLOCK_DIS_CH2
# [---]   lc_dis_ch4 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_4
#                                    /CLOCK_DIS_CH4
# [---]   lc_ldo_pwron => CLKGEN_PLL_LC_PLL_LDO_PWRON
#                                    /LDO_PWRON_PLL
# [---]   lc_mdiv_ch0 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_0
#                                    /MDIV_CH0
# [---]   lc_mdiv_ch1 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_1
#                                    /MDIV_CH1
# [---]   lc_mdiv_ch2 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_2
#                                    /MDIV_CH2
# [---]   lc_mdiv_ch4 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_4
#                                    /MDIV_CH4
# [---]   lc_ndiv_int => CLKGEN_PLL_LC_PLL_DIV
#                                    /NDIV_INT
# [---]   lc_pdh_ch0 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_0
#                                    /POST_DIVIDER_HOLD_CH0
# [---]   lc_pdh_ch1 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_1
#                                    /POST_DIVIDER_HOLD_CH1
# [---]   lc_pdh_ch2 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_2
#                                    /POST_DIVIDER_HOLD_CH2
# [---]   lc_pdh_ch4 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_4
#                                    /POST_DIVIDER_HOLD_CH4
# [---]   lc_pdiv => CLKGEN_PLL_LC_PLL_DIV
#                                    /PDIV
# [---]   lc_pll_rsta => CLKGEN_PLL_LC_PLL_RESET
#                                    /RESETA
# [---]   lc_pll_rstd => CLKGEN_PLL_LC_PLL_RESET
#                                    /RESETD
# [---]   lc_pwron => CLKGEN_PLL_LC_PLL_PWRON
#                                    /PWRON_PLL
# [---]   moca_dis_ch0 => CLKGEN_PLL_MOCA_PLL_CHANNEL_CTRL_CH_0
#                                    /CLOCK_DIS_CH0
# [---]   moca_dis_ch1 => CLKGEN_PLL_MOCA_PLL_CHANNEL_CTRL_CH_1
#                                    /CLOCK_DIS_CH1
# [---]   moca_dis_ch2 => CLKGEN_PLL_MOCA_PLL_CHANNEL_CTRL_CH_2
#                                    /CLOCK_DIS_CH2
# [---]   moca_ldo_pwron => CLKGEN_PLL_MOCA_PLL_LDO_PWRON
#                                    /LDO_PWRON_PLL
# [---]   moca_mdiv_ch0 aka sw_moca_cpu => CLKGEN_PLL_MOCA_PLL_CHANNEL_CTRL_CH_0
#                                    /MDIV_CH0
# [---]   moca_mdiv_ch1 aka sw_moca_phy => CLKGEN_PLL_MOCA_PLL_CHANNEL_CTRL_CH_1
#                                    /MDIV_CH1
# [---]   moca_mdiv_ch2 => CLKGEN_PLL_MOCA_PLL_CHANNEL_CTRL_CH_2
#                                    /MDIV_CH2
# [---]   moca_mdiv_ch5 aka sw_mpi => CLKGEN_PLL_MOCA_PLL_CHANNEL_CTRL_CH_5
#                                    /MDIV_CH5
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
# [---]   moca_pll_rsta => CLKGEN_PLL_MOCA_PLL_RESET
#                                    /RESETA
# [---]   moca_pll_rstd => CLKGEN_PLL_MOCA_PLL_RESET
#                                    /RESETD
# [---]   moca_pwron => CLKGEN_PLL_MOCA_PLL_PWRON
#                                    /PWRON_PLL
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
# [---]   net_ldo_pwron => CLKGEN_PLL_NETWORK_PLL_LDO_PWRON
#                                    /LDO_PWRON_PLL
# [---]   net_mdiv_ch0 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_0
#                                    /MDIV_CH0
# [---]   net_mdiv_ch1 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_1
#                                    /MDIV_CH1
# [---]   net_mdiv_ch2 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_2
#                                    /MDIV_CH2
# [---]   net_ndiv_int => CLKGEN_PLL_NETWORK_PLL_DIV
#                                    /NDIV_INT
# [---]   net_pdh_ch0 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_0
#                                    /POST_DIVIDER_HOLD_CH0
# [---]   net_pdh_ch1 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_1
#                                    /POST_DIVIDER_HOLD_CH1
# [---]   net_pdh_ch2 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_2
#                                    /POST_DIVIDER_HOLD_CH2
# [---]   net_pdiv => CLKGEN_PLL_NETWORK_PLL_DIV
#                                    /PDIV
# [---]   net_pll_rsta => CLKGEN_PLL_NETWORK_PLL_RESET
#                                    /RESETA
# [---]   net_pll_rstd => CLKGEN_PLL_NETWORK_PLL_RESET
#                                    /RESETD
# [---]   net_pwron => CLKGEN_PLL_NETWORK_PLL_PWRON
#                                    /PWRON_PLL
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
# [---] R sata3_54 => CLKGEN_SATA3_TOP_INST_CLOCK_ENABLE
#                                    /SATA3_54_CLOCK_ENABLE
# [---] R sata3_gisb => CLKGEN_SATA3_TOP_INST_CLOCK_ENABLE
#                                    /SATA3_GISB_CLOCK_ENABLE
# [---] R sata3_scb => CLKGEN_SATA3_TOP_INST_CLOCK_ENABLE
#                                    /SATA3_SCB_CLOCK_ENABLE
# [---] R sata3_spd_scn => CLKGEN_SATA3_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_SATA3_AT_SPEED_SCAN_CLOCK
# [---]   sw_cpu => sw_cpu
# [---]   sw_genet0 => sw_genet0
# [---]   sw_genet1 => sw_genet1
# [---]   sw_geneteee0 => sw_geneteee0
# [---]   sw_geneteee1 => sw_geneteee1
# [---]   sw_genetwol0 => sw_genetwol0
# [---]   sw_genetwol1 => sw_genetwol1
# [---]   sw_gphy => sw_gphy
# [---]   sw_moca => sw_moca
# [---]   sw_pcie0 => sw_pcie0
# [---]   sw_sata3 => sw_sata3
# [---]   sw_sdio => sw_sdio
# [---]   sw_usb20 => sw_usb20
# [---]   sw_usb21 => sw_usb21
# [---]   sw_usb30 => sw_usb30
# [---]   sys0_mdiv_ch2 aka sw_hif aka sw_memsys0 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_2
#                                    /MDIV_CH2
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
# [---] R usb0_freerun => CLKGEN_USB0_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_USB0_FREERUN_CLOCK
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
# [---] R usb1_freerun => CLKGEN_USB1_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_USB1_FREERUN_CLOCK
# [---] R usb1_gisb => CLKGEN_USB1_TOP_INST_CLOCK_ENABLE
#                                    /USB1_GISB_CLOCK_ENABLE
# [---] R usb1_scb => CLKGEN_USB1_TOP_INST_CLOCK_ENABLE
#                                    /USB1_SCB_CLOCK_ENABLE
