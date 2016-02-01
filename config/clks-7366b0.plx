$VAR1 = {
          'rdb_sha1' => 'b56wD5pno7WvCLQ7rNygMpxGomM',
          'fail' => 0,
          'date' => 'Tue Oct 14 12:13:57 PDT 2014',
          'rdb_version' => 'rdb-v1-3-g076fd2b',
          'rdb_dir' => '/home/stblinux/clkgen/7366b0/current',
          'clkgen_version' => 'clkgen-v3-1-g5687c32',
          'chip' => '7366b0',
          'aliases' => {
                         'sw_gphy' => 'lc_pdh_ch0'
                       },
          'unhandled_linux_funcs' => 'CPU, HIF, MEMSYS0, MEMSYS1, MPI',
          'invocation' => 'clkgen.pl --sw_nodes -v -g -r -P -c 7366b0 -D',
          'num_clks' => 136,
          'clks' => '	brcmstb-clks {
		#address-cells = <1>;
		#size-cells = <1>;
		reg = <0xf04e0000 0x870 0xf04d1800 0x12c 0xf0410000 0x600>;
		ranges;

		mocaphy : mocaphy@f0410074 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf0410074 0x4>;
			bit-shift = <14>;
			set-bit-to-disable;
		};

		sata_usb3_pcie1 : sata_usb3_pcie1@f0410074 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf0410074 0x4>;
			bit-shift = <12>;
			set-bit-to-disable;
		};

		fixed3 : fixed3 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		moca_pwrdn_req : moca_pwrdn_req@f04e0830 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0830 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&fixed3>; 
			clock-names = "fixed3"; 
		};

		moca_pdiv : moca_pdiv@f04e00dc {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00dc 0x4>;
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

		hif_sdio_card : hif_sdio_card@f04e052c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e052c 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&moca_pdh_ch2>; 
			clock-names = "moca_pdh_ch2"; 
		};

		hif_sdio_emmc : hif_sdio_emmc@f04e052c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e052c 0x4>;
			bit-shift = <2>;
			set-bit-to-disable;
			clocks = <&moca_pdh_ch2>; 
			clock-names = "moca_pdh_ch2"; 
		};

		fixed4 : fixed4 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		net_pwrdn_req : net_pwrdn_req@f04e0834 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0834 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&fixed4>; 
			clock-names = "fixed4"; 
		};

		net_pdiv : net_pdiv@f04e0114 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0114 0x4>;
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

		net_mdiv_ch2 : net_mdiv_ch2@f04e0110 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0110 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch2 : net_dis_ch2@f04e0110 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0110 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch2>; 
			clock-names = "net_mdiv_ch2"; 
		};

		net_pdh_ch2 : net_pdh_ch2@f04e0110 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0110 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch2>; 
			clock-names = "net_dis_ch2"; 
		};

		hif_spi : sw_spi : hif_spi@f04e052c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e052c 0x4>;
			bit-shift = <3>;
			set-bit-to-disable;
			clocks = <&net_pdh_ch2>; 
			clock-names = "net_pdh_ch2"; 
		};

		mocamac_54 : mocamac_54@f04e05a0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05a0 0x4>;
			bit-shift = <0>;
		};

		mocamac_gisb : mocamac_gisb@f04e05a0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05a0 0x4>;
			bit-shift = <1>;
		};

		mocamac_scb : mocamac_scb@f04e05a0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05a0 0x4>;
			bit-shift = <2>;
		};

		mocaphy_54 : mocaphy_54@f04e05ac {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05ac 0x4>;
			bit-shift = <0>;
		};

		mocaphy_gisb : mocaphy_gisb@f04e05ac {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05ac 0x4>;
			bit-shift = <1>;
		};

		pcie_alwayson : pcie_alwayson@f04e05d4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05d4 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		pcie_108 : pcie_108@f04e05dc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05dc 0x4>;
			bit-shift = <0>;
		};

		pcie_54 : pcie_54@f04e05dc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05dc 0x4>;
			bit-shift = <1>;
		};

		pcie_gisb : pcie_gisb@f04e05dc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05dc 0x4>;
			bit-shift = <2>;
		};

		pcie_scb : pcie_scb@f04e05dc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e05dc 0x4>;
			bit-shift = <3>;
		};

		pcie1_54_pcie : pcie1_54_pcie@f04e069c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e069c 0x4>;
			bit-shift = <0>;
		};

		pcie1_gisb_pcie : pcie1_gisb_pcie@f04e069c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e069c 0x4>;
			bit-shift = <1>;
		};

		pcie1_scb_pcie : pcie1_scb_pcie@f04e069c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e069c 0x4>;
			bit-shift = <2>;
		};

		sata3_108_pcie : sata3_108_pcie@f04e069c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e069c 0x4>;
			bit-shift = <3>;
		};

		sata3_54 : sata3_54@f04e06a4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e06a4 0x4>;
			bit-shift = <0>;
		};

		sata3_gisb : sata3_gisb@f04e06a4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e06a4 0x4>;
			bit-shift = <1>;
		};

		sata3_scb : sata3_scb@f04e06a4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e06a4 0x4>;
			bit-shift = <2>;
		};

		genet0_alwayson : genet0_alwayson@f04e073c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e073c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		net_mdiv_ch1 : net_mdiv_ch1@f04e010c {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e010c 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch1 : net_dis_ch1@f04e010c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e010c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch1>; 
			clock-names = "net_mdiv_ch1"; 
		};

		net_pdh_ch1 : net_pdh_ch1@f04e010c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e010c 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch1>; 
			clock-names = "net_dis_ch1"; 
		};

		genet0_sys_fast : genet0_sys_fast@f04e073c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e073c 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&net_pdh_ch1>; 
			clock-names = "net_pdh_ch1"; 
		};

		genet0_sys_pm : genet0_sys_pm@f04e073c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e073c 0x4>;
			bit-shift = <2>;
			set-bit-to-disable;
		};

		genet0_sys_slow : genet0_sys_slow@f04e073c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e073c 0x4>;
			bit-shift = <3>;
			set-bit-to-disable;
		};

		genet1_alwayson : genet1_alwayson@f04e073c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e073c 0x4>;
			bit-shift = <4>;
			set-bit-to-disable;
		};

		genet1_sys_fast : genet1_sys_fast@f04e073c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e073c 0x4>;
			bit-shift = <5>;
			set-bit-to-disable;
			clocks = <&net_pdh_ch1>; 
			clock-names = "net_pdh_ch1"; 
		};

		genet1_sys_pm : genet1_sys_pm@f04e073c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e073c 0x4>;
			bit-shift = <6>;
			set-bit-to-disable;
		};

		genet1_sys_slow : genet1_sys_slow@f04e073c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e073c 0x4>;
			bit-shift = <7>;
			set-bit-to-disable;
		};

		genet2_alwayson : genet2_alwayson@f04e073c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e073c 0x4>;
			bit-shift = <8>;
			set-bit-to-disable;
		};

		genet2_sys_fast : genet2_sys_fast@f04e073c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e073c 0x4>;
			bit-shift = <9>;
			set-bit-to-disable;
			clocks = <&net_pdh_ch1>; 
			clock-names = "net_pdh_ch1"; 
		};

		genet2_sys_pm : genet2_sys_pm@f04e073c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e073c 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
		};

		genet2_sys_slow : genet2_sys_slow@f04e073c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e073c 0x4>;
			bit-shift = <11>;
			set-bit-to-disable;
		};

		genet_54 : genet_54@f04e0744 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0744 0x4>;
			bit-shift = <0>;
		};

		genet_scb : genet_scb@f04e0744 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0744 0x4>;
			bit-shift = <1>;
		};

		genet0_eee : genet0_eee@f04e0748 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0748 0x4>;
			bit-shift = <0>;
		};

		genet0_gisb : genet0_gisb@f04e0748 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0748 0x4>;
			bit-shift = <1>;
		};

		genet0_gmii : genet0_gmii@f04e0748 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0748 0x4>;
			bit-shift = <2>;
		};

		genet0_hfb : genet0_hfb@f04e0748 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0748 0x4>;
			bit-shift = <3>;
		};

		genet0_l2intr : genet0_l2intr@f04e0748 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0748 0x4>;
			bit-shift = <4>;
		};

		genet0_scb : genet0_scb@f04e0748 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0748 0x4>;
			bit-shift = <5>;
		};

		genet0_umac_rx : genet0_umac_rx@f04e0748 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0748 0x4>;
			bit-shift = <6>;
		};

		genet0_umac_tx : genet0_umac_tx@f04e0748 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0748 0x4>;
			bit-shift = <7>;
		};

		net_mdiv_ch0 : net_mdiv_ch0@f04e0108 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0108 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch0 : net_dis_ch0@f04e0108 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0108 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch0>; 
			clock-names = "net_mdiv_ch0"; 
		};

		net_pdh_ch0 : net_pdh_ch0@f04e0108 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0108 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch0>; 
			clock-names = "net_dis_ch0"; 
		};

		genet1_250 : genet1_250@f04e0750 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0750 0x4>;
			bit-shift = <0>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		genet1_eee : genet1_eee@f04e0750 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0750 0x4>;
			bit-shift = <1>;
		};

		genet1_gisb : genet1_gisb@f04e0750 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0750 0x4>;
			bit-shift = <2>;
		};

		genet1_gmii : genet1_gmii@f04e0750 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0750 0x4>;
			bit-shift = <3>;
		};

		genet1_hfb : genet1_hfb@f04e0750 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0750 0x4>;
			bit-shift = <4>;
		};

		genet1_l2intr : genet1_l2intr@f04e0750 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0750 0x4>;
			bit-shift = <5>;
		};

		genet1_scb : genet1_scb@f04e0750 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0750 0x4>;
			bit-shift = <6>;
		};

		genet1_umac_rx : genet1_umac_rx@f04e0750 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0750 0x4>;
			bit-shift = <7>;
		};

		genet1_umac_tx : genet1_umac_tx@f04e0750 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0750 0x4>;
			bit-shift = <8>;
		};

		genet2_250 : genet2_250@f04e0758 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0758 0x4>;
			bit-shift = <0>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		genet2_eee : genet2_eee@f04e0758 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0758 0x4>;
			bit-shift = <1>;
		};

		genet2_gisb : genet2_gisb@f04e0758 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0758 0x4>;
			bit-shift = <2>;
		};

		genet2_gmii : genet2_gmii@f04e0758 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0758 0x4>;
			bit-shift = <3>;
		};

		genet2_hfb : genet2_hfb@f04e0758 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0758 0x4>;
			bit-shift = <4>;
		};

		genet2_l2intr : genet2_l2intr@f04e0758 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0758 0x4>;
			bit-shift = <5>;
		};

		genet2_scb : genet2_scb@f04e0758 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0758 0x4>;
			bit-shift = <6>;
		};

		genet2_umac_rx : genet2_umac_rx@f04e0758 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0758 0x4>;
			bit-shift = <7>;
		};

		genet2_umac_tx : genet2_umac_tx@f04e0758 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0758 0x4>;
			bit-shift = <8>;
		};

		usb0_54 : usb0_54@f04e0778 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0778 0x4>;
			bit-shift = <0>;
		};

		usb0_gisb : usb0_gisb@f04e0778 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0778 0x4>;
			bit-shift = <1>;
		};

		usb0_scb : usb0_scb@f04e0778 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0778 0x4>;
			bit-shift = <2>;
		};

		usb0_108_ahb : usb0_108_ahb@f04e077c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e077c 0x4>;
			bit-shift = <0>;
		};

		usb0_108_axi : usb0_108_axi@f04e0784 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0784 0x4>;
			bit-shift = <0>;
		};

		usb1_54 : usb1_54@f04e0798 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0798 0x4>;
			bit-shift = <0>;
		};

		usb1_gisb : usb1_gisb@f04e0798 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0798 0x4>;
			bit-shift = <1>;
		};

		usb1_scb : usb1_scb@f04e0798 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0798 0x4>;
			bit-shift = <2>;
		};

		usb1_108_ahb : usb1_108_ahb@f04e079c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e079c 0x4>;
			bit-shift = <0>;
		};

		usb1_108_axi : usb1_108_axi@f04e07a4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e07a4 0x4>;
			bit-shift = <0>;
		};

		fixed2 : fixed2 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <54000000>;
		};

		lc_pwrdn_req : lc_pwrdn_req@f04e0848 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0848 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			brcm,delay = <0 21>;
			brcm,inhibit-disable;
			brcm,read-only;
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
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <1>;
			clock-mult = <100>;
			clocks = <&lc_pdiv>; 
			clock-names = "lc_pdiv"; 
		};

		lc_mdiv_ch0 : lc_mdiv_ch0@f04e0078 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0078 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&lc_ndiv_int>; 
			clock-names = "lc_ndiv_int"; 
		};

		lc_dis_ch0 : lc_dis_ch0@f04e0078 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0078 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_mdiv_ch0>; 
			clock-names = "lc_mdiv_ch0"; 
		};

		lc_pdh_ch0 : sw_gphy : lc_pdh_ch0@f04e0078 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0078 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&lc_dis_ch0>; 
			clock-names = "lc_dis_ch0"; 
		};

		lc_mdiv_ch1 : lc_mdiv_ch1@f04e007c {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e007c 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&lc_ndiv_int>; 
			clock-names = "lc_ndiv_int"; 
		};

		lc_dis_ch1 : lc_dis_ch1@f04e007c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e007c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_mdiv_ch1>; 
			clock-names = "lc_mdiv_ch1"; 
		};

		lc_pdh_ch1 : lc_pdh_ch1@f04e007c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e007c 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&lc_dis_ch1>; 
			clock-names = "lc_dis_ch1"; 
		};

		lc_mdiv_ch2 : lc_mdiv_ch2@f04e0080 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0080 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&lc_ndiv_int>; 
			clock-names = "lc_ndiv_int"; 
		};

		lc_dis_ch2 : lc_dis_ch2@f04e0080 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0080 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&lc_mdiv_ch2>; 
			clock-names = "lc_mdiv_ch2"; 
		};

		lc_pdh_ch2 : lc_pdh_ch2@f04e0080 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0080 0x4>;
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

		moca_mdiv_ch5 : sw_mpi : moca_mdiv_ch5@f04e00d8 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00d8 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&moca_ndiv_int>; 
			clock-names = "moca_ndiv_int"; 
		};

		sw_genet0 : sw_genet0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet0_alwayson>, <&genet0_sys_fast>, 
			  <&genet0_sys_pm>, <&genet0_sys_slow>, <&genet_54>, 
			  <&genet_scb>, <&genet0_gisb>, <&genet0_gmii>, 
			  <&genet0_hfb>, <&genet0_l2intr>, <&genet0_scb>, 
			  <&genet0_umac_rx>, <&genet0_umac_tx>, <&genet1_250>; 
			clock-names = "genet0_alwayson", "genet0_sys_fast", 
			  "genet0_sys_pm", "genet0_sys_slow", "genet_54", 
			  "genet_scb", "genet0_gisb", "genet0_gmii", 
			  "genet0_hfb", "genet0_l2intr", "genet0_scb", 
			  "genet0_umac_rx", "genet0_umac_tx", "genet1_250"; 
		};

		sw_genet1 : sw_genet1 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet1_alwayson>, <&genet1_sys_fast>, 
			  <&genet1_sys_pm>, <&genet1_sys_slow>, <&genet1_250>, 
			  <&genet1_gisb>, <&genet1_gmii>, <&genet1_hfb>, 
			  <&genet1_l2intr>, <&genet1_scb>, <&genet1_umac_rx>, 
			  <&genet1_umac_tx>; 
			clock-names = "genet1_alwayson", "genet1_sys_fast", 
			  "genet1_sys_pm", "genet1_sys_slow", "genet1_250", 
			  "genet1_gisb", "genet1_gmii", "genet1_hfb", 
			  "genet1_l2intr", "genet1_scb", "genet1_umac_rx", 
			  "genet1_umac_tx"; 
		};

		sw_genet2 : sw_genet2 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet2_alwayson>, <&genet2_sys_fast>, 
			  <&genet2_sys_pm>, <&genet2_sys_slow>, <&genet2_250>, 
			  <&genet2_gisb>, <&genet2_gmii>, <&genet2_hfb>, 
			  <&genet2_l2intr>, <&genet2_scb>, <&genet2_umac_rx>, 
			  <&genet2_umac_tx>; 
			clock-names = "genet2_alwayson", "genet2_sys_fast", 
			  "genet2_sys_pm", "genet2_sys_slow", "genet2_250", 
			  "genet2_gisb", "genet2_gmii", "genet2_hfb", 
			  "genet2_l2intr", "genet2_scb", "genet2_umac_rx", 
			  "genet2_umac_tx"; 
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
			clocks = <&mocaphy>, <&mocamac_54>, <&mocamac_gisb>, 
			  <&mocamac_scb>, <&mocaphy_54>, <&mocaphy_gisb>, 
			  <&moca_pdh_ch0>, <&moca_pdh_ch1>, <&moca_pdh_ch2>; 
			clock-names = "mocaphy", "mocamac_54", "mocamac_gisb", 
			  "mocamac_scb", "mocaphy_54", "mocaphy_gisb", 
			  "moca_pdh_ch0", "moca_pdh_ch1", "moca_pdh_ch2"; 
		};

		sw_pcie0 : sw_pcie0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&pcie_alwayson>, <&pcie_108>, <&pcie_54>, 
			  <&pcie_gisb>, <&pcie_scb>; 
			clock-names = "pcie_alwayson", "pcie_108", "pcie_54", 
			  "pcie_gisb", "pcie_scb"; 
		};

		sw_pcie1 : sw_pcie1 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&sata_usb3_pcie1>, <&pcie_alwayson>, 
			  <&pcie1_54_pcie>, <&pcie1_gisb_pcie>, 
			  <&pcie1_scb_pcie>, <&sata3_108_pcie>; 
			clock-names = "sata_usb3_pcie1", "pcie_alwayson", 
			  "pcie1_54_pcie", "pcie1_gisb_pcie", 
			  "pcie1_scb_pcie", "sata3_108_pcie"; 
		};

		sw_sata3 : sw_sata3 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&sata_usb3_pcie1>, <&sata3_54>, 
			  <&sata3_gisb>, <&sata3_scb>; 
			clock-names = "sata_usb3_pcie1", "sata3_54", 
			  "sata3_gisb", "sata3_scb"; 
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
			clocks = <&usb0_54>, <&usb0_gisb>, <&usb0_scb>, 
			  <&usb0_108_ahb>, <&usb0_108_axi>, <&lc_pdh_ch2>; 
			clock-names = "usb0_54", "usb0_gisb", "usb0_scb", 
			  "usb0_108_ahb", "usb0_108_axi", "lc_pdh_ch2"; 
		};

		sw_usb21 : sw_usb21 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&usb1_54>, <&usb1_gisb>, <&usb1_scb>, 
			  <&usb1_108_ahb>, <&usb1_108_axi>, <&lc_pdh_ch1>; 
			clock-names = "usb1_54", "usb1_gisb", "usb1_scb", 
			  "usb1_108_ahb", "usb1_108_axi", "lc_pdh_ch1"; 
		};

		sw_usb30 : sw_usb30 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&sata_usb3_pcie1>, <&usb0_54>, <&usb0_gisb>, 
			  <&usb0_scb>, <&usb0_108_ahb>, <&usb0_108_axi>, 
			  <&lc_pdh_ch4>; 
			clock-names = "sata_usb3_pcie1", "usb0_54", 
			  "usb0_gisb", "usb0_scb", "usb0_108_ahb", 
			  "usb0_108_axi", "lc_pdh_ch4"; 
		};

	};
',
          'funcs' => {
                       'MOCAWOM' => [
                                      'sw_mocawom'
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


# [---]   cpu_mdiv_ch0 aka sw_cpu => CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_0
#                                    /MDIV_CH0
# [---]   cpu_ndiv_int => CLKGEN_PLL_CPU_PLL_DIV
#                                    /NDIV_INT
# [---]   cpu_pdiv => CLKGEN_PLL_CPU_PLL_DIV
#                                    /PDIV
# [---] R fixed0 => fixed0
# [---] R fixed2 => fixed2
# [---] R fixed3 => fixed3
# [---] R fixed4 => fixed4
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
# [---]   lc_dis_ch0 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_0
#                                    /CLOCK_DIS_CH0
# [---]   lc_dis_ch1 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_1
#                                    /CLOCK_DIS_CH1
# [---]   lc_dis_ch2 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_2
#                                    /CLOCK_DIS_CH2
# [---]   lc_dis_ch4 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_4
#                                    /CLOCK_DIS_CH4
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
# [---]   lc_pdh_ch0 aka sw_gphy => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_0
#                                    /POST_DIVIDER_HOLD_CH0
# [---]   lc_pdh_ch1 => CLKGEN_PLL_LC_PLL_CHANNEL_CTRL_CH_1
#                                    /POST_DIVIDER_HOLD_CH1
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
# [---]   moca_pwrdn_req => CLKGEN_MOCA_PLL_CTRL_WRAPPER_CONTROL
#                                    /PWRDN_PLL_REQ
# [---] R mocamac_54 => CLKGEN_MOCAMAC_TOP_INST_CLOCK_ENABLE
#                                    /MOCAMAC_54_CLOCK_ENABLE
# [---] R mocamac_gisb => CLKGEN_MOCAMAC_TOP_INST_CLOCK_ENABLE
#                                    /MOCAMAC_GISB_CLOCK_ENABLE
# [---] R mocamac_scb => CLKGEN_MOCAMAC_TOP_INST_CLOCK_ENABLE
#                                    /MOCAMAC_SCB_CLOCK_ENABLE
# [---] R mocaphy => AON_CTRL_ANA_XTAL_CONTROL
#                                    /en_osc_cml_in_s3_mocaphy
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
# [---]   net_pwrdn_req => CLKGEN_NETWORK_PLL_CTRL_WRAPPER_CONTROL
#                                    /PWRDN_PLL_REQ
# [---] R pcie1_54_pcie => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE
#                                    /PCIE1_54_CLOCK_ENABLE_PCIE
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
# [---] R sata_usb3_pcie1 => AON_CTRL_ANA_XTAL_CONTROL
#                                    /en_osc_cml_in_s3_sata_usb3_pcie1
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
# [---]   sw_usb21 => sw_usb21
# [---]   sw_usb30 => sw_usb30
# [---] R usb0_108_ahb => CLKGEN_USB0_TOP_INST_CLOCK_ENABLE_AHB
#                                    /USB0_108_CLOCK_ENABLE_AHB
# [---] R usb0_108_axi => CLKGEN_USB0_TOP_INST_CLOCK_ENABLE_AXI
#                                    /USB0_108_CLOCK_ENABLE_AXI
# [---] R usb0_54 => CLKGEN_USB0_TOP_INST_CLOCK_ENABLE
#                                    /USB0_54_CLOCK_ENABLE
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
# [---] R usb1_gisb => CLKGEN_USB1_TOP_INST_CLOCK_ENABLE
#                                    /USB1_GISB_CLOCK_ENABLE
# [---] R usb1_scb => CLKGEN_USB1_TOP_INST_CLOCK_ENABLE
#                                    /USB1_SCB_CLOCK_ENABLE
