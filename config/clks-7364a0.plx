$VAR1 = {
          'rdb_sha1' => '1G+EphOTrX7qDW/0VyDo52vYpHo',
          'fail' => 0,
          'date' => 'Wed Apr 29 12:24:39 PDT 2015',
          'rdb_version' => 'rdb-v2-7-gd0ce537',
          'rdb_dir' => '/home/stblinux/clkgen/7364a0/current',
          'clkgen_version' => 'clkgen-v4-41-gdba370b',
          'chip' => '7364a0',
          'aliases' => {
                         'sw_gphy' => 'lc_pdh_ch2'
                       },
          'unhandled_linux_funcs' => 'CPU, MEMSYS, MPI',
          'invocation' => 'clkgen.pl --sw_nodes -v -g -r -P -c 7364a0',
          'num_clks' => 119,
          'clks' => '	brcmstb-clks {
		#address-cells = <1>;
		#size-cells = <1>;
		reg = <0xf04e0000 0x6bc 0xf04d1800 0x118 0xf0410000 0x400>;
		ranges;

		genet0_alwayson : genet0_alwayson@f04e0488 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0488 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		fixed5 : fixed5 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		net_ldo_pwron : net_ldo_pwron@f04e0178 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0178 0x4>;
			bit-shift = <0>;
			brcm,delay = <0 2>;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&fixed5>; 
			clock-names = "fixed5"; 
		};

		net_pwron : net_pwron@f04e0188 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0188 0x4>;
			bit-shift = <0>;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&net_ldo_pwron>; 
			clock-names = "net_ldo_pwron"; 
		};

		iso_net : iso_net@f04e06ac {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e06ac 0x4>;
			bit-shift = <4>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&net_pwron>; 
			clock-names = "net_pwron"; 
		};

		net_pll_rsta : net_pll_rsta@f04e018c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e018c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			brcm,delay = <30 0>;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&iso_net>; 
			clock-names = "iso_net"; 
		};

		net_pll_rstd : net_pll_rstd@f04e018c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e018c 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&net_pll_rsta>; 
			clock-names = "net_pll_rsta"; 
		};

		net_pdiv : net_pdiv@f04e0170 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0170 0x4>;
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

		net_mdiv_ch0 : net_mdiv_ch0@f04e015c {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e015c 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch0 : net_dis_ch0@f04e015c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e015c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch0>; 
			clock-names = "net_mdiv_ch0"; 
		};

		net_pdh_ch0 : net_pdh_ch0@f04e015c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e015c 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch0>; 
			clock-names = "net_dis_ch0"; 
		};

		genet0_sys_fast : genet0_sys_fast@f04e0488 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0488 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		genet0_sys_pm : genet0_sys_pm@f04e0488 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0488 0x4>;
			bit-shift = <2>;
			set-bit-to-disable;
		};

		genet0_sys_slow : genet0_sys_slow@f04e0488 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0488 0x4>;
			bit-shift = <3>;
			set-bit-to-disable;
		};

		genet1_alwayson : genet1_alwayson@f04e0488 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0488 0x4>;
			bit-shift = <4>;
			set-bit-to-disable;
		};

		genet1_sys_fast : genet1_sys_fast@f04e0488 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0488 0x4>;
			bit-shift = <5>;
			set-bit-to-disable;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		genet1_sys_pm : genet1_sys_pm@f04e0488 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0488 0x4>;
			bit-shift = <6>;
			set-bit-to-disable;
		};

		genet1_sys_slow : genet1_sys_slow@f04e0488 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0488 0x4>;
			bit-shift = <7>;
			set-bit-to-disable;
		};

		genet_54 : genet_54@f04e0490 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0490 0x4>;
			bit-shift = <0>;
		};

		genet0_250 : genet0_250@f04e0494 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0494 0x4>;
			bit-shift = <0>;
		};

		genet0_eee : genet0_eee@f04e0494 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0494 0x4>;
			bit-shift = <1>;
		};

		genet0_gisb : genet0_gisb@f04e0494 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0494 0x4>;
			bit-shift = <2>;
		};

		genet0_gmii : genet0_gmii@f04e0494 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0494 0x4>;
			bit-shift = <3>;
		};

		genet0_hfb : genet0_hfb@f04e0494 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0494 0x4>;
			bit-shift = <4>;
		};

		genet0_l2intr : genet0_l2intr@f04e0494 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0494 0x4>;
			bit-shift = <5>;
		};

		genet0_scb : genet0_scb@f04e0494 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0494 0x4>;
			bit-shift = <6>;
		};

		genet0_umac_rx : genet0_umac_rx@f04e0494 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0494 0x4>;
			bit-shift = <7>;
		};

		genet0_umac_tx : genet0_umac_tx@f04e0494 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0494 0x4>;
			bit-shift = <8>;
		};

		genet1_eee : genet1_eee@f04e049c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e049c 0x4>;
			bit-shift = <0>;
		};

		genet1_gisb : genet1_gisb@f04e049c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e049c 0x4>;
			bit-shift = <1>;
		};

		genet1_gmii : genet1_gmii@f04e049c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e049c 0x4>;
			bit-shift = <2>;
		};

		genet1_hfb : genet1_hfb@f04e049c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e049c 0x4>;
			bit-shift = <3>;
		};

		genet1_l2intr : genet1_l2intr@f04e049c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e049c 0x4>;
			bit-shift = <4>;
		};

		genet1_scb : genet1_scb@f04e049c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e049c 0x4>;
			bit-shift = <5>;
		};

		genet1_umac_rx : genet1_umac_rx@f04e049c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e049c 0x4>;
			bit-shift = <6>;
		};

		genet1_umac_tx : genet1_umac_tx@f04e049c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e049c 0x4>;
			bit-shift = <7>;
		};

		fixed3 : fixed3 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		lc_pwron : lc_pwron@f04e0110 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0110 0x4>;
			bit-shift = <0>;
			brcm,delay = <0 40>;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&fixed3>; 
			clock-names = "fixed3"; 
		};

		lc_pll_rsta : lc_pll_rsta@f04e0114 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0114 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			brcm,delay = <100 0>;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&lc_pwron>; 
			clock-names = "lc_pwron"; 
		};

		lc_pll_rstd : lc_pll_rstd@f04e0114 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0114 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&lc_pll_rsta>; 
			clock-names = "lc_pll_rsta"; 
		};

		lc_pdiv : lc_pdiv@f04e00ec {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00ec 0x4>;
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
			clock-mult = <50>;
			clocks = <&lc_pdiv>; 
			clock-names = "lc_pdiv"; 
		};

		lc_mdiv_ch5 : lc_mdiv_ch5@f04e00e4 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00e4 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&lc_ndiv_int>; 
			clock-names = "lc_ndiv_int"; 
		};

		lc_dis_ch5 : lc_dis_ch5@f04e00e4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00e4 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_mdiv_ch5>; 
			clock-names = "lc_mdiv_ch5"; 
		};

		lc_pdh_ch5 : lc_pdh_ch5@f04e00e4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00e4 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&lc_dis_ch5>; 
			clock-names = "lc_dis_ch5"; 
		};

		hif_sdio_card : hif_sdio_card@f04e04bc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04bc 0x4>;
			bit-shift = <2>;
			set-bit-to-disable;
			clocks = <&lc_pdh_ch5>; 
			clock-names = "lc_pdh_ch5"; 
		};

		hif_sdio_emmc : hif_sdio_emmc@f04e04bc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04bc 0x4>;
			bit-shift = <3>;
			set-bit-to-disable;
		};

		hif_spi : hif_spi@f04e04bc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04bc 0x4>;
			bit-shift = <4>;
			set-bit-to-disable;
		};

		mocamac_54 : mocamac_54@f04e0508 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0508 0x4>;
			bit-shift = <0>;
		};

		mocamac_gisb : mocamac_gisb@f04e0508 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0508 0x4>;
			bit-shift = <1>;
		};

		mocamac_scb : mocamac_scb@f04e0508 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0508 0x4>;
			bit-shift = <2>;
		};

		mocaphy_54 : mocaphy_54@f04e0514 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0514 0x4>;
			bit-shift = <0>;
		};

		usb0_54_mdio : usb0_54_mdio@f04e05b8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05b8 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		usb0_54 : usb0_54@f04e05c0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05c0 0x4>;
			bit-shift = <0>;
		};

		usb0_gisb : usb0_gisb@f04e05c0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05c0 0x4>;
			bit-shift = <1>;
		};

		usb0_scb : usb0_scb@f04e05c0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05c0 0x4>;
			bit-shift = <2>;
		};

		usb0_108_ahb : usb0_108_ahb@f04e05c4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05c4 0x4>;
			bit-shift = <0>;
		};

		usb0_108_axi : usb0_108_axi@f04e05cc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05cc 0x4>;
			bit-shift = <0>;
		};

		fixed4 : fixed4 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		moca_ldo_pwron : moca_ldo_pwron@f04e0138 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0138 0x4>;
			bit-shift = <0>;
			brcm,delay = <0 2>;
			clocks = <&fixed4>; 
			clock-names = "fixed4"; 
		};

		moca_pwron : moca_pwron@f04e0148 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0148 0x4>;
			bit-shift = <0>;
			clocks = <&moca_ldo_pwron>; 
			clock-names = "moca_ldo_pwron"; 
		};

		iso_moca : iso_moca@f04e06ac {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e06ac 0x4>;
			bit-shift = <3>;
			set-bit-to-disable;
			clocks = <&moca_pwron>; 
			clock-names = "moca_pwron"; 
		};

		fixed1 : fixed1 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		cpu_pdiv : cpu_pdiv@f04e0058 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0058 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&fixed1>; 
			clock-names = "fixed1"; 
		};

		cpu_ndiv_int : cpu_ndiv_int {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <1>;
			clock-mult = <167>;
			clocks = <&cpu_pdiv>; 
			clock-names = "cpu_pdiv"; 
		};

		cpu_mdiv_ch0 : cpu_mdiv_ch0@f04e0040 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0040 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&cpu_ndiv_int>; 
			clock-names = "cpu_ndiv_int"; 
		};

		lc_mdiv_ch0 : lc_mdiv_ch0@f04e00d0 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00d0 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&lc_ndiv_int>; 
			clock-names = "lc_ndiv_int"; 
		};

		lc_dis_ch0 : lc_dis_ch0@f04e00d0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00d0 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_mdiv_ch0>; 
			clock-names = "lc_mdiv_ch0"; 
		};

		lc_pdh_ch0 : lc_pdh_ch0@f04e00d0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00d0 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&lc_dis_ch0>; 
			clock-names = "lc_dis_ch0"; 
		};

		lc_mdiv_ch1 : lc_mdiv_ch1@f04e00d4 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00d4 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&lc_ndiv_int>; 
			clock-names = "lc_ndiv_int"; 
		};

		lc_dis_ch1 : lc_dis_ch1@f04e00d4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00d4 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_mdiv_ch1>; 
			clock-names = "lc_mdiv_ch1"; 
		};

		lc_pdh_ch1 : lc_pdh_ch1@f04e00d4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00d4 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&lc_dis_ch1>; 
			clock-names = "lc_dis_ch1"; 
		};

		lc_mdiv_ch2 : lc_mdiv_ch2@f04e00d8 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00d8 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&lc_ndiv_int>; 
			clock-names = "lc_ndiv_int"; 
		};

		lc_dis_ch2 : lc_dis_ch2@f04e00d8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00d8 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_mdiv_ch2>; 
			clock-names = "lc_mdiv_ch2"; 
		};

		lc_pdh_ch2 : sw_gphy : lc_pdh_ch2@f04e00d8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e00d8 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&lc_dis_ch2>; 
			clock-names = "lc_dis_ch2"; 
		};

		moca_pll_rsta : moca_pll_rsta@f04e014c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e014c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			brcm,delay = <30 0>;
			clocks = <&iso_moca>; 
			clock-names = "iso_moca"; 
		};

		moca_pll_rstd : moca_pll_rstd@f04e014c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e014c 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&moca_pll_rsta>; 
			clock-names = "moca_pll_rsta"; 
		};

		moca_pdiv : moca_pdiv@f04e0130 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0130 0x4>;
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

		moca_mdiv_ch0 : sw_moca_cpu : moca_mdiv_ch0@f04e0120 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0120 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&moca_ndiv_int>; 
			clock-names = "moca_ndiv_int"; 
		};

		moca_dis_ch0 : moca_dis_ch0@f04e0120 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0120 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&moca_mdiv_ch0>; 
			clock-names = "moca_mdiv_ch0"; 
		};

		moca_pdh_ch0 : moca_pdh_ch0@f04e0120 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0120 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&moca_dis_ch0>; 
			clock-names = "moca_dis_ch0"; 
		};

		moca_mdiv_ch1 : sw_moca_phy : moca_mdiv_ch1@f04e0124 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0124 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&moca_ndiv_int>; 
			clock-names = "moca_ndiv_int"; 
		};

		moca_dis_ch1 : moca_dis_ch1@f04e0124 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0124 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&moca_mdiv_ch1>; 
			clock-names = "moca_mdiv_ch1"; 
		};

		moca_pdh_ch1 : moca_pdh_ch1@f04e0124 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0124 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&moca_dis_ch1>; 
			clock-names = "moca_dis_ch1"; 
		};

		moca_mdiv_ch2 : moca_mdiv_ch2@f04e0128 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0128 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&moca_ndiv_int>; 
			clock-names = "moca_ndiv_int"; 
		};

		moca_dis_ch2 : moca_dis_ch2@f04e0128 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0128 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&moca_mdiv_ch2>; 
			clock-names = "moca_mdiv_ch2"; 
		};

		moca_pdh_ch2 : sw_mocawol : sw_mocawom : moca_pdh_ch2@f04e0128 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0128 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&moca_dis_ch2>; 
			clock-names = "moca_dis_ch2"; 
		};

		net_mdiv_ch2 : net_mdiv_ch2@f04e0164 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0164 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch2 : net_dis_ch2@f04e0164 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0164 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch2>; 
			clock-names = "net_mdiv_ch2"; 
		};

		net_pdh_ch2 : net_pdh_ch2@f04e0164 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0164 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch2>; 
			clock-names = "net_dis_ch2"; 
		};

		fixed9 : fixed9 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		sys0_pdiv : sys0_pdiv@f04e0260 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0260 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&fixed9>; 
			clock-names = "fixed9"; 
		};

		sys0_ndiv_int : sys0_ndiv_int {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <1>;
			clock-mult = <144>;
			clocks = <&sys0_pdiv>; 
			clock-names = "sys0_pdiv"; 
		};

		sys0_mdiv_ch1 : sys0_mdiv_ch1@f04e024c {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e024c 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&sys0_ndiv_int>; 
			clock-names = "sys0_ndiv_int"; 
		};

		sys0_mdiv_ch2 : sys0_mdiv_ch2@f04e0250 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0250 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&sys0_ndiv_int>; 
			clock-names = "sys0_ndiv_int"; 
		};

		sys0_mdiv_ch3 : sys0_mdiv_ch3@f04e0254 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0254 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&sys0_ndiv_int>; 
			clock-names = "sys0_ndiv_int"; 
		};

		sys0_dis_ch3 : sys0_dis_ch3@f04e0254 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0254 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&sys0_mdiv_ch3>; 
			clock-names = "sys0_mdiv_ch3"; 
		};

		sys0_pdh_ch3 : sys0_pdh_ch3@f04e0254 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0254 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&sys0_dis_ch3>; 
			clock-names = "sys0_dis_ch3"; 
		};

		sys0_mdiv_ch5 : sys0_mdiv_ch5@f04e025c {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e025c 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&sys0_ndiv_int>; 
			clock-names = "sys0_ndiv_int"; 
		};

		fixed10 : fixed10 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		sys1_pdiv : sys1_pdiv@f04e02b0 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e02b0 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&fixed10>; 
			clock-names = "fixed10"; 
		};

		sys1_ndiv_int : sys1_ndiv_int {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <1>;
			clock-mult = <120>;
			clocks = <&sys1_pdiv>; 
			clock-names = "sys1_pdiv"; 
		};

		sys1_mdiv_ch2 : sys1_mdiv_ch2@f04e02a0 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e02a0 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&sys1_ndiv_int>; 
			clock-names = "sys1_ndiv_int"; 
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
			  <&genet0_250>, <&genet0_gisb>, <&genet0_gmii>, 
			  <&genet0_hfb>, <&genet0_l2intr>, <&genet0_scb>, 
			  <&genet0_umac_rx>, <&genet0_umac_tx>, 
			  <&net_pdh_ch2>, <&sys0_mdiv_ch2>; 
			clock-names = "genet0_alwayson", "genet0_sys_fast", 
			  "genet0_sys_pm", "genet0_sys_slow", "genet_54", 
			  "genet0_250", "genet0_gisb", "genet0_gmii", 
			  "genet0_hfb", "genet0_l2intr", "genet0_scb", 
			  "genet0_umac_rx", "genet0_umac_tx", "net_pdh_ch2", 
			  "sys0_mdiv_ch2"; 
		};

		sw_genet1 : sw_genet1 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet1_alwayson>, <&genet1_sys_fast>, 
			  <&genet1_sys_pm>, <&genet1_sys_slow>, <&genet_54>, 
			  <&genet0_250>, <&genet1_gisb>, <&genet1_gmii>, 
			  <&genet1_hfb>, <&genet1_l2intr>, <&genet1_scb>, 
			  <&genet1_umac_rx>, <&genet1_umac_tx>, 
			  <&net_pdh_ch2>, <&sys0_mdiv_ch2>; 
			clock-names = "genet1_alwayson", "genet1_sys_fast", 
			  "genet1_sys_pm", "genet1_sys_slow", "genet_54", 
			  "genet0_250", "genet1_gisb", "genet1_gmii", 
			  "genet1_hfb", "genet1_l2intr", "genet1_scb", 
			  "genet1_umac_rx", "genet1_umac_tx", "net_pdh_ch2", 
			  "sys0_mdiv_ch2"; 
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
			  <&genet_54>, <&genet1_hfb>, <&genet1_l2intr>, 
			  <&genet1_umac_rx>; 
			clock-names = "genet1_alwayson", "genet1_sys_slow", 
			  "genet_54", "genet1_hfb", "genet1_l2intr", 
			  "genet1_umac_rx"; 
		};

		sw_memsys : sw_memsys {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&sys0_mdiv_ch1>, <&sys0_mdiv_ch2>; 
			clock-names = "sys0_mdiv_ch1", "sys0_mdiv_ch2"; 
		};

		sw_moca : sw_moca {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&mocamac_54>, <&mocamac_gisb>, 
			  <&mocamac_scb>, <&mocaphy_54>, <&moca_pdh_ch0>, 
			  <&moca_pdh_ch1>, <&moca_pdh_ch2>, <&sys0_mdiv_ch2>; 
			clock-names = "mocamac_54", "mocamac_gisb", 
			  "mocamac_scb", "mocaphy_54", "moca_pdh_ch0", 
			  "moca_pdh_ch1", "moca_pdh_ch2", "sys0_mdiv_ch2"; 
		};

		sw_pcie : sw_pcie {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&sys0_mdiv_ch1>, <&sys0_mdiv_ch2>; 
			clock-names = "sys0_mdiv_ch1", "sys0_mdiv_ch2"; 
		};

		sw_sdio : sw_sdio {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&hif_sdio_card>, <&hif_sdio_emmc>, 
			  <&sys0_mdiv_ch5>; 
			clock-names = "hif_sdio_card", "hif_sdio_emmc", 
			  "sys0_mdiv_ch5"; 
		};

		sw_spi : sw_spi {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&hif_spi>, <&sys0_pdh_ch3>, 
			  <&sys1_mdiv_ch2>; 
			clock-names = "hif_spi", "sys0_pdh_ch3", 
			  "sys1_mdiv_ch2"; 
		};

		sw_usb20 : sw_usb20 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&usb0_54>, <&usb0_gisb>, <&usb0_scb>, 
			  <&usb0_108_ahb>, <&usb0_108_axi>, <&lc_pdh_ch0>, 
			  <&sys0_mdiv_ch1>, <&sys0_mdiv_ch2>; 
			clock-names = "usb0_54", "usb0_gisb", "usb0_scb", 
			  "usb0_108_ahb", "usb0_108_axi", "lc_pdh_ch0", 
			  "sys0_mdiv_ch1", "sys0_mdiv_ch2"; 
		};

		sw_usb30 : sw_usb30 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&usb0_54_mdio>, <&usb0_54>, <&usb0_gisb>, 
			  <&usb0_scb>, <&usb0_108_ahb>, <&usb0_108_axi>, 
			  <&lc_pdh_ch1>, <&sys0_mdiv_ch1>, <&sys0_mdiv_ch2>; 
			clock-names = "usb0_54_mdio", "usb0_54", "usb0_gisb", 
			  "usb0_scb", "usb0_108_ahb", "usb0_108_axi", 
			  "lc_pdh_ch1", "sys0_mdiv_ch1", "sys0_mdiv_ch2"; 
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
                       'PCIE' => [
                                   'sw_pcie'
                                 ],
                       'USB30' => [
                                    'sw_usb30'
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
                       'GPHY' => [
                                   'sw_gphy'
                                 ],
                       'CPU' => [
                                  'sw_cpu'
                                ],
                       'GENET0' => [
                                     'sw_genet0'
                                   ],
                       'MEMSYS' => [
                                     'sw_memsys'
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
# [---] R fixed1 => fixed1
# [---] R fixed10 => fixed10
# [---] R fixed3 => fixed3
# [---] R fixed4 => fixed4
# [---] R fixed5 => fixed5
# [---] R fixed9 => fixed9
# [---] R genet0_250 => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_CLK_250_CLOCK_ENABLE_GENET0
# [---] R genet0_alwayson => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET0_ALWAYSON_CLOCK
# [---] R genet0_eee => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_EEE_CLOCK_ENABLE_GENET0
# [---] R genet0_gisb => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_GISB_CLOCK_ENABLE_GENET0
# [---] R genet0_gmii => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_GMII_CLOCK_ENABLE_GENET0
# [---] R genet0_hfb => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_HFB_CLOCK_ENABLE_GENET0
# [---] R genet0_l2intr => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_L2INTR_CLOCK_ENABLE_GENET0
# [---] R genet0_scb => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_SCB_CLOCK_ENABLE_GENET0
# [---]   genet0_sys_fast => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET0_SYSTEM_FAST_CLOCK
# [---] R genet0_sys_pm => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET0_SYSTEM_PM_CLOCK
# [---] R genet0_sys_slow => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET0_SYSTEM_SLOW_CLOCK
# [---] R genet0_umac_rx => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_UNIMAC_SYS_RX_CLOCK_ENABLE_GENET0
# [---] R genet0_umac_tx => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET0
#                                    /GENET0_UNIMAC_SYS_TX_CLOCK_ENABLE_GENET0
# [---] R genet1_alwayson => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET1_ALWAYSON_CLOCK
# [---] R genet1_eee => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_EEE_CLOCK_ENABLE_GENET1
# [---] R genet1_gisb => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_GISB_CLOCK_ENABLE_GENET1
# [---] R genet1_gmii => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_GMII_CLOCK_ENABLE_GENET1
# [---] R genet1_hfb => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_HFB_CLOCK_ENABLE_GENET1
# [---] R genet1_l2intr => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_L2INTR_CLOCK_ENABLE_GENET1
# [---] R genet1_scb => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_SCB_CLOCK_ENABLE_GENET1
# [---]   genet1_sys_fast => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET1_SYSTEM_FAST_CLOCK
# [---] R genet1_sys_pm => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET1_SYSTEM_PM_CLOCK
# [---] R genet1_sys_slow => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET1_SYSTEM_SLOW_CLOCK
# [---] R genet1_umac_rx => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_UNIMAC_SYS_RX_CLOCK_ENABLE_GENET1
# [---] R genet1_umac_tx => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE_GENET1
#                                    /GENET1_UNIMAC_SYS_TX_CLOCK_ENABLE_GENET1
# [---] R genet_54 => CLKGEN_ONOFF_GENET_TOP_INST_CLOCK_ENABLE
#                                    /GENET_54_CLOCK_ENABLE
# [---]   hif_sdio_card => CLKGEN_ONOFF_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_HIF_SDIO_CARD_CLOCK
# [---] R hif_sdio_emmc => CLKGEN_ONOFF_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_HIF_SDIO_EMMC_CLOCK
# [---] R hif_spi => CLKGEN_ONOFF_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_HIF_SPI_CLOCK
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
# [---]   lc_dis_ch5 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_5
#                                    /CLOCK_DIS_CH5
# [---]   lc_mdiv_ch0 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_0
#                                    /MDIV_CH0
# [---]   lc_mdiv_ch1 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_1
#                                    /MDIV_CH1
# [---]   lc_mdiv_ch2 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_2
#                                    /MDIV_CH2
# [---]   lc_mdiv_ch5 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_5
#                                    /MDIV_CH5
# [---]   lc_ndiv_int => CLKGEN_PLL_LC_PLL_DIV
#                                    /NDIV_INT
# [---]   lc_pdh_ch0 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_0
#                                    /POST_DIVIDER_HOLD_CH0
# [---]   lc_pdh_ch1 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_1
#                                    /POST_DIVIDER_HOLD_CH1
# [---]   lc_pdh_ch2 aka sw_gphy => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_2
#                                    /POST_DIVIDER_HOLD_CH2
# [---]   lc_pdh_ch5 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_5
#                                    /POST_DIVIDER_HOLD_CH5
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
# [---] R mocamac_54 => CLKGEN_ONOFF_MOCAMAC_TOP_INST_CLOCK_ENABLE
#                                    /MOCAMAC_54_CLOCK_ENABLE
# [---] R mocamac_gisb => CLKGEN_ONOFF_MOCAMAC_TOP_INST_CLOCK_ENABLE
#                                    /MOCAMAC_GISB_CLOCK_ENABLE
# [---] R mocamac_scb => CLKGEN_ONOFF_MOCAMAC_TOP_INST_CLOCK_ENABLE
#                                    /MOCAMAC_SCB_CLOCK_ENABLE
# [---] R mocaphy_54 => CLKGEN_ONOFF_MOCAPHY_TOP_INST_CLOCK_ENABLE
#                                    /MOCAPHY_54_CLOCK_ENABLE
# [---]   net_dis_ch0 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_0
#                                    /CLOCK_DIS_CH0
# [---]   net_dis_ch2 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_2
#                                    /CLOCK_DIS_CH2
# [---]   net_ldo_pwron => CLKGEN_PLL_NETWORK_PLL_LDO_PWRON
#                                    /LDO_PWRON_PLL
# [---]   net_mdiv_ch0 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_0
#                                    /MDIV_CH0
# [---]   net_mdiv_ch2 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_2
#                                    /MDIV_CH2
# [---]   net_ndiv_int => CLKGEN_PLL_NETWORK_PLL_DIV
#                                    /NDIV_INT
# [---]   net_pdh_ch0 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_0
#                                    /POST_DIVIDER_HOLD_CH0
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
# [---]   sw_cpu => sw_cpu
# [---]   sw_genet0 => sw_genet0
# [---]   sw_genet1 => sw_genet1
# [---]   sw_geneteee0 => sw_geneteee0
# [---]   sw_geneteee1 => sw_geneteee1
# [---]   sw_genetwol0 => sw_genetwol0
# [---]   sw_genetwol1 => sw_genetwol1
# [---]   sw_memsys => sw_memsys
# [---]   sw_moca => sw_moca
# [---]   sw_pcie => sw_pcie
# [---]   sw_sdio => sw_sdio
# [---]   sw_spi => sw_spi
# [---]   sw_usb20 => sw_usb20
# [---]   sw_usb30 => sw_usb30
# [---]   sys0_dis_ch3 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3
#                                    /CLOCK_DIS_CH3
# [---]   sys0_mdiv_ch1 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1
#                                    /MDIV_CH1
# [---]   sys0_mdiv_ch2 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_2
#                                    /MDIV_CH2
# [---]   sys0_mdiv_ch3 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3
#                                    /MDIV_CH3
# [---]   sys0_mdiv_ch5 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_5
#                                    /MDIV_CH5
# [---]   sys0_ndiv_int => CLKGEN_PLL_SYS0_PLL_DIV
#                                    /NDIV_INT
# [---]   sys0_pdh_ch3 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3
#                                    /POST_DIVIDER_HOLD_CH3
# [---]   sys0_pdiv => CLKGEN_PLL_SYS0_PLL_DIV
#                                    /PDIV
# [---]   sys1_mdiv_ch2 => CLKGEN_PLL_SYS1_PLL_CHANNEL_CTRL_CH_2
#                                    /MDIV_CH2
# [---]   sys1_ndiv_int => CLKGEN_PLL_SYS1_PLL_DIV
#                                    /NDIV_INT
# [---]   sys1_pdiv => CLKGEN_PLL_SYS1_PLL_DIV
#                                    /PDIV
# [---] R usb0_108_ahb => CLKGEN_ONOFF_USB0_TOP_INST_CLOCK_ENABLE_AHB
#                                    /USB0_108_CLOCK_ENABLE_AHB
# [---] R usb0_108_axi => CLKGEN_ONOFF_USB0_TOP_INST_CLOCK_ENABLE_AXI
#                                    /USB0_108_CLOCK_ENABLE_AXI
# [---] R usb0_54 => CLKGEN_ONOFF_USB0_TOP_INST_CLOCK_ENABLE
#                                    /USB0_54_CLOCK_ENABLE
# [---] R usb0_54_mdio => CLKGEN_ONOFF_USB0_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_USB0_54_MDIO_CLOCK
# [---] R usb0_gisb => CLKGEN_ONOFF_USB0_TOP_INST_CLOCK_ENABLE
#                                    /USB0_GISB_CLOCK_ENABLE
# [---] R usb0_scb => CLKGEN_ONOFF_USB0_TOP_INST_CLOCK_ENABLE
#                                    /USB0_SCB_CLOCK_ENABLE
