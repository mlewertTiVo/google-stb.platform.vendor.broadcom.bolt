$VAR1 = {
          'rdb_sha1' => 'eAqvncI1p9UcUvyWUBRVOBxc0wI',
          'fail' => 0,
          'date' => 'Thu Feb  8 13:06:04 PST 2018',
          'rdb_version' => 'rdb-v2-66-g0cc583f',
          'rdb_dir' => '/projects/stbgit/stblinux/git/clkgen/7260b0',
          'clkgen_version' => 'clkgen-v4-473-g0f9bfe0-dirty',
          'pm_ver' => '2017_12_06.15_08',
          'chip' => '7260b0',
          'aliases' => {},
          'unhandled_linux_funcs' => 'CPU, MEMSYS0, MPI',
          'invocation' => 'clkgen.pl --sw_nodes -v -g -r -p -c 7260b0 -D',
          'num_clks' => 106,
          'clks' => '	brcmstb-clks {
		#address-cells = <1>;
		#size-cells = <1>;
		reg = <0xf04e0000 0x101c 0xf04d1800 0xb0 0xf0410000 0x600>;
		ranges;

		osc_sata_pcie : osc_sata_pcie@f0410074 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf0410074 0x4>;
			bit-shift = <4>;
			set-bit-to-disable;
		};

		orion_cpu_c_54 : orion_cpu_c_54@f04e02fc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e02fc 0x4>;
			bit-shift = <0>;
			brcm,inhibit-disable;
			brcm,read-only;
		};

		orion_cpu_secure : orion_cpu_secure@f04e02fc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e02fc 0x4>;
			bit-shift = <1>;
			brcm,inhibit-disable;
			brcm,read-only;
		};

		orion_gisb : orion_gisb@f04e02fc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e02fc 0x4>;
			bit-shift = <2>;
			brcm,inhibit-disable;
			brcm,read-only;
		};

		fixed_syn0 : fixed_syn0 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <0xe7be2c00>;
		};

		sys0_mdiv_ch2 : sys0_mdiv_ch2@f04e017c {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e017c 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&fixed_syn0>;
			clock-names = "fixed_syn0";
		};

		sys0_dis_ch2 : sys0_dis_ch2@f04e017c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e017c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&sys0_mdiv_ch2>;
			clock-names = "sys0_mdiv_ch2";
		};

		sys0_pdh_ch2 : sys0_pdh_ch2@f04e017c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e017c 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&sys0_dis_ch2>;
			clock-names = "sys0_dis_ch2";
		};

		orion_scb : orion_scb@f04e02fc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e02fc 0x4>;
			bit-shift = <3>;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&sys0_pdh_ch2>;
			clock-names = "sys0_pdh_ch2";
		};

		fixed1 : fixed1 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <0x337f980>;
		};

		cpu_pdiv : cpu_pdiv@f04e0060 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0060 0x4>;
			bit-shift = <0>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&fixed1>;
			clock-names = "fixed1";
		};

		cpu_ndiv_int : cpu_ndiv_int@f04e1018 {
			compatible = "multiplier-clock";
			#clock-cells = <0>;
			reg = <0xf04e1018 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3ff>;
			index-max-mult-at-zero;
			clock-div = <1>;
			clock-mult = <167>;
			clocks = <&cpu_pdiv>;
			clock-names = "cpu_pdiv";
		};

		cpu_mdiv_ch0 : cpu_mdiv_ch0@f04e1010 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e1010 0x4>;
			bit-shift = <0>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&cpu_ndiv_int>;
			clock-names = "cpu_ndiv_int";
		};

		cpu_mdiv_ch1 : sys_if_clk : cpu_mdiv_ch1@f04e1014 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e1014 0x4>;
			bit-shift = <0>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&cpu_ndiv_int>;
			clock-names = "cpu_ndiv_int";
		};

		fixed2 : fixed2 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <0x337f980>;
		};

		net_pwrdn_req : net_pwrdn_req@f04e0378 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0378 0x4>;
			bit-shift = <18>;
			set-bit-to-disable;
			brcm,delay = <30 0>;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&fixed2>;
			clock-names = "fixed2";
		};

		hif_ebi : hif_ebi@f04e038c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e038c 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
		};

		sys0_mdiv_ch5 : sys0_mdiv_ch5@f04e0188 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0188 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&fixed_syn0>;
			clock-names = "fixed_syn0";
		};

		sys0_dis_ch5 : sys0_dis_ch5@f04e0188 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0188 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&sys0_mdiv_ch5>;
			clock-names = "sys0_mdiv_ch5";
		};

		sys0_pdh_ch5 : sys0_pdh_ch5@f04e0188 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0188 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&sys0_dis_ch5>;
			clock-names = "sys0_dis_ch5";
		};

		hif_sdio_card : hif_sdio_card@f04e038c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e038c 0x4>;
			bit-shift = <2>;
			set-bit-to-disable;
			clocks = <&sys0_pdh_ch5>;
			clock-names = "sys0_pdh_ch5";
		};

		hif_sdio_emmc : hif_sdio_emmc@f04e038c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e038c 0x4>;
			bit-shift = <3>;
			set-bit-to-disable;
			clocks = <&sys0_pdh_ch5>;
			clock-names = "sys0_pdh_ch5";
		};

		sys0_mdiv_ch3 : sys0_mdiv_ch3@f04e0180 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0180 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&fixed_syn0>;
			clock-names = "fixed_syn0";
		};

		sys0_dis_ch3 : sys0_dis_ch3@f04e0180 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0180 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&sys0_mdiv_ch3>;
			clock-names = "sys0_mdiv_ch3";
		};

		sys0_pdh_ch3 : sys0_pdh_ch3@f04e0180 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0180 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&sys0_dis_ch3>;
			clock-names = "sys0_dis_ch3";
		};

		hif_spi : sw_spi : hif_spi@f04e038c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e038c 0x4>;
			bit-shift = <4>;
			set-bit-to-disable;
			clocks = <&sys0_pdh_ch3>;
			clock-names = "sys0_pdh_ch3";
		};

		nand_ddr : nand_ddr@f04e038c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e038c 0x4>;
			bit-shift = <5>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&sys0_pdh_ch5>;
			clock-names = "sys0_pdh_ch5";
		};

		xpt_hif_gisb : xpt_hif_gisb@f04e0394 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0394 0x4>;
			bit-shift = <3>;
			brcm,inhibit-disable;
			brcm,read-only;
		};

		genet0_alwayson : genet0_alwayson@f04e03d8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03d8 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		net_pdiv : net_pdiv@f04e00a8 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e00a8 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&net_pwrdn_req>;
			clock-names = "net_pwrdn_req";
		};

		net_ndiv_int : net_ndiv_int@f04e00a8 {
			compatible = "multiplier-clock", "fixed-factor-clock";
			#clock-cells = <0>;
			reg = <0xf04e00a8 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3ff>;
			index-max-mult-at-zero;
			clock-div = <1>;
			clock-mult = <125>;
			clocks = <&net_pdiv>;
			clock-names = "net_pdiv";
		};

		net_mdiv_ch0 : net_mdiv_ch0@f04e0090 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0090 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>;
			clock-names = "net_ndiv_int";
		};

		net_dis_ch0 : net_dis_ch0@f04e0090 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0090 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch0>;
			clock-names = "net_mdiv_ch0";
		};

		net_pdh_ch0 : net_pdh_ch0@f04e0090 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0090 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch0>;
			clock-names = "net_dis_ch0";
		};

		genet0_sys_fast : genet0_sys_fast@f04e03d8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03d8 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&net_pdh_ch0>;
			clock-names = "net_pdh_ch0";
		};

		genet0_sys_pm : genet0_sys_pm@f04e03d8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03d8 0x4>;
			bit-shift = <2>;
			set-bit-to-disable;
		};

		genet0_sys_slow : genet0_sys_slow@f04e03d8 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03d8 0x4>;
			bit-shift = <3>;
			set-bit-to-disable;
		};

		net_mdiv_ch2 : net_mdiv_ch2@f04e0098 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0098 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>;
			clock-names = "net_ndiv_int";
		};

		net_dis_ch2 : net_dis_ch2@f04e0098 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0098 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch2>;
			clock-names = "net_mdiv_ch2";
		};

		net_pdh_ch2 : net_pdh_ch2@f04e0098 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0098 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch2>;
			clock-names = "net_dis_ch2";
		};

		genet0_250 : genet0_250@f04e03e0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03e0 0x4>;
			bit-shift = <0>;
			clocks = <&net_pdh_ch2>;
			clock-names = "net_pdh_ch2";
		};

		genet0_eee : genet0_eee@f04e03e0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03e0 0x4>;
			bit-shift = <1>;
		};

		genet0_gisb : genet0_gisb@f04e03e0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03e0 0x4>;
			bit-shift = <2>;
		};

		genet0_gmii : genet0_gmii@f04e03e0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03e0 0x4>;
			bit-shift = <3>;
		};

		genet0_hfb : genet0_hfb@f04e03e0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03e0 0x4>;
			bit-shift = <4>;
		};

		genet0_l2intr : genet0_l2intr@f04e03e0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03e0 0x4>;
			bit-shift = <5>;
		};

		genet0_scb : genet0_scb@f04e03e0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03e0 0x4>;
			bit-shift = <6>;
			clocks = <&sys0_pdh_ch2>;
			clock-names = "sys0_pdh_ch2";
		};

		genet0_umac_sys_rx : genet0_umac_sys_rx@f04e03e0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03e0 0x4>;
			bit-shift = <7>;
		};

		genet0_umac_sys_tx : genet0_umac_sys_tx@f04e03e0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03e0 0x4>;
			bit-shift = <8>;
		};

		genet_54 : genet_54@f04e03e0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03e0 0x4>;
			bit-shift = <18>;
		};

		genet_scb : genet_scb@f04e03e0 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e03e0 0x4>;
			bit-shift = <19>;
			clocks = <&sys0_pdh_ch2>;
			clock-names = "sys0_pdh_ch2";
		};

		genet0_select : sw_genetmux0 : genet0_select@f04e03e8 {
			compatible = "brcm,mux-clock", "mux-clock";
			#clock-cells = <0>;
			reg = <0xf04e03e8 0x4>;
			bit-shift = <0>;
			bit-mask = <0x1>;
			clocks = <&genet0_sys_fast>, <&genet0_sys_slow>;
			clock-names = "genet0_sys_fast", "genet0_sys_slow";
		};

		memsys0_54 : memsys0_54@f04e0418 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0418 0x4>;
			bit-shift = <0>;
			brcm,inhibit-disable;
			brcm,read-only;
		};

		memsys0_gisb : memsys0_gisb@f04e0418 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0418 0x4>;
			bit-shift = <1>;
			brcm,inhibit-disable;
			brcm,read-only;
		};

		memsys0_scb : memsys0_scb@f04e0418 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0418 0x4>;
			bit-shift = <2>;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&sys0_pdh_ch2>;
			clock-names = "sys0_pdh_ch2";
		};

		pcie0_alwayson : pcie0_alwayson@f04e0450 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0450 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		sys0_mdiv_ch0 : sys0_mdiv_ch0@f04e0174 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0174 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&fixed_syn0>;
			clock-names = "fixed_syn0";
		};

		sys0_dis_ch0 : sys0_dis_ch0@f04e0174 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0174 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&sys0_mdiv_ch0>;
			clock-names = "sys0_mdiv_ch0";
		};

		sys0_pdh_ch0 : sys0_pdh_ch0@f04e0174 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0174 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&sys0_dis_ch0>;
			clock-names = "sys0_dis_ch0";
		};

		sys_108_pcie0 : sys_108_pcie0@f04e0458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0458 0x4>;
			bit-shift = <0>;
			clocks = <&sys0_pdh_ch0>;
			clock-names = "sys0_pdh_ch0";
		};

		sys_54_pcie0 : sys_54_pcie0@f04e0458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0458 0x4>;
			bit-shift = <1>;
		};

		sys_gisb_pcie0 : sys_gisb_pcie0@f04e0458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0458 0x4>;
			bit-shift = <2>;
		};

		sys_scb_pcie0 : sys_scb_pcie0@f04e0458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0458 0x4>;
			bit-shift = <3>;
			clocks = <&sys0_pdh_ch2>;
			clock-names = "sys0_pdh_ch2";
		};

		sys_54_sata30 : sys_54_sata30@f04e0460 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0460 0x4>;
			bit-shift = <0>;
		};

		sys_gisb_sata30 : sys_gisb_sata30@f04e0460 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0460 0x4>;
			bit-shift = <1>;
		};

		sys_scb_sata30 : sys_scb_sata30@f04e0460 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0460 0x4>;
			bit-shift = <2>;
			clocks = <&sys0_pdh_ch2>;
			clock-names = "sys0_pdh_ch2";
		};

		fixed_factor0 : fixed_factor0 {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <8>;
			clock-mult = <1>;
			clocks = <&sys0_pdh_ch0>;
			clock-names = "sys0_pdh_ch0";
		};

		fixed_factor1 : fixed_factor1 {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <6>;
			clock-mult = <1>;
			clocks = <&sys0_pdh_ch0>;
			clock-names = "sys0_pdh_ch0";
		};

		net_mdiv_ch1 : net_mdiv_ch1@f04e0094 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0094 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>;
			clock-names = "net_ndiv_int";
		};

		net_dis_ch1 : net_dis_ch1@f04e0094 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0094 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&net_mdiv_ch1>;
			clock-names = "net_mdiv_ch1";
		};

		net_pdh_ch1 : net_pdh_ch1@f04e0094 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0094 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&net_dis_ch1>;
			clock-names = "net_dis_ch1";
		};

		sys0_mdiv_ch1 : sys0_mdiv_ch1@f04e0178 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0xf04e0178 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&fixed_syn0>;
			clock-names = "fixed_syn0";
		};

		sys0_dis_ch1 : sys0_dis_ch1@f04e0178 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0178 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&sys0_mdiv_ch1>;
			clock-names = "sys0_mdiv_ch1";
		};

		sys0_pdh_ch1 : sys0_pdh_ch1@f04e0178 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0178 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&sys0_dis_ch1>;
			clock-names = "sys0_dis_ch1";
		};

		uart_sysctrl_uart_0 : uart_sysctrl_uart_0@f04e051c {
			compatible = "brcm,mux-clock", "mux-clock";
			#clock-cells = <0>;
			reg = <0xf04e051c 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3>;
			clocks = <&fixed_factor0>, <&fixed_factor1>,
			  <&net_pdh_ch1>, <&sys0_pdh_ch1>;
			clock-names = "fixed_factor0", "fixed_factor1",
			  "net_pdh_ch1", "sys0_pdh_ch1";
		};

		stb_sysctrl_uart_0 : sw_uart0 : stb_sysctrl_uart_0@f04e0488 {
			compatible = "brcm,brcmstb-gate-clk", "fixed-clock";
			clock-frequency = <0x4d3f640>;
			#clock-cells = <0>;
			reg = <0xf04e0488 0x4>;
			bit-shift = <4>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,set-rate-parent;
			brcm,read-only;
			clocks = <&uart_sysctrl_uart_0>;
			clock-names = "uart_sysctrl_uart_0";
		};

		fixed_factor2 : fixed_factor2 {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <8>;
			clock-mult = <1>;
			clocks = <&sys0_pdh_ch0>;
			clock-names = "sys0_pdh_ch0";
		};

		fixed_factor3 : fixed_factor3 {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <6>;
			clock-mult = <1>;
			clocks = <&sys0_pdh_ch0>;
			clock-names = "sys0_pdh_ch0";
		};

		uart_sysctrl_uart_1 : uart_sysctrl_uart_1@f04e0520 {
			compatible = "brcm,mux-clock", "mux-clock";
			#clock-cells = <0>;
			reg = <0xf04e0520 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3>;
			clocks = <&fixed_factor2>, <&fixed_factor3>,
			  <&net_pdh_ch1>, <&sys0_pdh_ch1>;
			clock-names = "fixed_factor2", "fixed_factor3",
			  "net_pdh_ch1", "sys0_pdh_ch1";
		};

		stb_sysctrl_uart_1 : sw_uart1 : stb_sysctrl_uart_1@f04e0488 {
			compatible = "brcm,brcmstb-gate-clk", "fixed-clock";
			clock-frequency = <0x4d3f640>;
			#clock-cells = <0>;
			reg = <0xf04e0488 0x4>;
			bit-shift = <5>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,set-rate-parent;
			brcm,read-only;
			clocks = <&uart_sysctrl_uart_1>;
			clock-names = "uart_sysctrl_uart_1";
		};

		fixed_factor4 : fixed_factor4 {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <8>;
			clock-mult = <1>;
			clocks = <&sys0_pdh_ch0>;
			clock-names = "sys0_pdh_ch0";
		};

		fixed_factor5 : fixed_factor5 {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <6>;
			clock-mult = <1>;
			clocks = <&sys0_pdh_ch0>;
			clock-names = "sys0_pdh_ch0";
		};

		uart_sysctrl_uart_2 : uart_sysctrl_uart_2@f04e0524 {
			compatible = "brcm,mux-clock", "mux-clock";
			#clock-cells = <0>;
			reg = <0xf04e0524 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3>;
			clocks = <&fixed_factor4>, <&fixed_factor5>,
			  <&net_pdh_ch1>, <&sys0_pdh_ch1>;
			clock-names = "fixed_factor4", "fixed_factor5",
			  "net_pdh_ch1", "sys0_pdh_ch1";
		};

		stb_sysctrl_uart_2 : sw_uart2 : stb_sysctrl_uart_2@f04e0488 {
			compatible = "brcm,brcmstb-gate-clk", "fixed-clock";
			clock-frequency = <0x4d3f640>;
			#clock-cells = <0>;
			reg = <0xf04e0488 0x4>;
			bit-shift = <6>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,set-rate-parent;
			brcm,read-only;
			clocks = <&uart_sysctrl_uart_2>;
			clock-names = "uart_sysctrl_uart_2";
		};

		usb0_freerun : usb0_freerun@f04e049c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e049c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
		};

		usb0_gisb : usb0_gisb@f04e04a4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04a4 0x4>;
			bit-shift = <0>;
		};

		sys_108_usb20 : sys_108_usb20@f04e04ac {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04ac 0x4>;
			bit-shift = <0>;
			clocks = <&sys0_pdh_ch0>;
			clock-names = "sys0_pdh_ch0";
		};

		sys_54_usb20 : sys_54_usb20@f04e04ac {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04ac 0x4>;
			bit-shift = <1>;
		};

		sys_scb_usb20 : sys_scb_usb20@f04e04ac {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04ac 0x4>;
			bit-shift = <2>;
			clocks = <&sys0_pdh_ch2>;
			clock-names = "sys0_pdh_ch2";
		};

		sys_108_usbd : sys_108_usbd@f04e04bc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04bc 0x4>;
			bit-shift = <0>;
			clocks = <&sys0_pdh_ch0>;
			clock-names = "sys0_pdh_ch0";
		};

		sys_scb_usbd : sys_scb_usbd@f04e04bc {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e04bc 0x4>;
			bit-shift = <1>;
			clocks = <&sys0_pdh_ch2>;
			clock-names = "sys0_pdh_ch2";
		};

		cpu_dis_ch0 : cpu_dis_ch0@f04e0048 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0048 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&cpu_mdiv_ch0>;
			clock-names = "cpu_mdiv_ch0";
		};

		cpu_pdh_ch0 : cpu_pdh_ch0@f04e0048 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e0048 0x4>;
			bit-shift = <2>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&cpu_dis_ch0>;
			clock-names = "cpu_dis_ch0";
		};

		cpu_dis_ch1 : cpu_dis_ch1@f04e004c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e004c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&cpu_mdiv_ch1>;
			clock-names = "cpu_mdiv_ch1";
		};

		cpu_pdh_ch1 : cpu_pdh_ch1@f04e004c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0xf04e004c 0x4>;
			bit-shift = <2>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&cpu_dis_ch1>;
			clock-names = "cpu_dis_ch1";
		};

		sw_cpu : sw_cpu {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&orion_cpu_c_54>, <&orion_cpu_secure>,
			  <&orion_gisb>, <&orion_scb>, <&cpu_pdh_ch0>,
			  <&cpu_pdh_ch1>, <&sys0_pdh_ch0>;
			clock-names = "orion_cpu_c_54", "orion_cpu_secure",
			  "orion_gisb", "orion_scb", "cpu_pdh_ch0",
			  "cpu_pdh_ch1", "sys0_pdh_ch0";
		};

		sw_genet0 : sw_genet0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet0_alwayson>, <&genet0_sys_fast>,
			  <&genet0_sys_pm>, <&genet0_sys_slow>, <&genet0_250>,
			  <&genet0_gisb>, <&genet0_gmii>, <&genet0_hfb>,
			  <&genet0_l2intr>, <&genet0_scb>,
			  <&genet0_umac_sys_rx>, <&genet0_umac_sys_tx>,
			  <&genet_54>, <&genet_scb>;
			clock-names = "genet0_alwayson", "genet0_sys_fast",
			  "genet0_sys_pm", "genet0_sys_slow", "genet0_250",
			  "genet0_gisb", "genet0_gmii", "genet0_hfb",
			  "genet0_l2intr", "genet0_scb", "genet0_umac_sys_rx",
			  "genet0_umac_sys_tx", "genet_54", "genet_scb";
		};

		sw_geneteee0 : sw_geneteee0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet0_sys_pm>, <&genet0_eee>;
			clock-names = "genet0_sys_pm", "genet0_eee";
		};

		sw_genetwol0 : sw_genetwol0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&genet0_alwayson>, <&genet0_sys_slow>,
			  <&genet0_hfb>, <&genet0_l2intr>,
			  <&genet0_umac_sys_rx>, <&genet_54>;
			clock-names = "genet0_alwayson", "genet0_sys_slow",
			  "genet0_hfb", "genet0_l2intr", "genet0_umac_sys_rx",
			  "genet_54";
		};

		sw_memsys0 : sw_memsys0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&memsys0_54>, <&memsys0_gisb>,
			  <&memsys0_scb>, <&sys0_pdh_ch0>;
			clock-names = "memsys0_54", "memsys0_gisb",
			  "memsys0_scb", "sys0_pdh_ch0";
		};

		sw_mpi : sw_mpi {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&hif_ebi>, <&nand_ddr>, <&xpt_hif_gisb>;
			clock-names = "hif_ebi", "nand_ddr", "xpt_hif_gisb";
		};

		sw_pcie0 : sw_pcie0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&osc_sata_pcie>, <&pcie0_alwayson>,
			  <&sys_108_pcie0>, <&sys_54_pcie0>,
			  <&sys_gisb_pcie0>, <&sys_scb_pcie0>;
			clock-names = "osc_sata_pcie", "pcie0_alwayson",
			  "sys_108_pcie0", "sys_54_pcie0", "sys_gisb_pcie0",
			  "sys_scb_pcie0";
		};

		sw_sata30 : sw_sata30 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&osc_sata_pcie>, <&sys_54_sata30>,
			  <&sys_gisb_sata30>, <&sys_scb_sata30>,
			  <&sys0_pdh_ch0>;
			clock-names = "osc_sata_pcie", "sys_54_sata30",
			  "sys_gisb_sata30", "sys_scb_sata30", "sys0_pdh_ch0";
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
			clocks = <&usb0_freerun>, <&usb0_gisb>,
			  <&sys_108_usb20>, <&sys_54_usb20>, <&sys_scb_usb20>;
			clock-names = "usb0_freerun", "usb0_gisb",
			  "sys_108_usb20", "sys_54_usb20", "sys_scb_usb20";
		};

		sw_usbd : sw_usbd {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&usb0_gisb>, <&sys_108_usbd>,
			  <&sys_scb_usbd>;
			clock-names = "usb0_gisb", "sys_108_usbd",
			  "sys_scb_usbd";
		};

	};
',
          'funcs' => {
                       'USB20' => [
                                    'sw_usb20'
                                  ],
                       'SDIO' => [
                                   'sw_sdio'
                                 ],
                       'GENETWOL0' => [
                                        'sw_genetwol0'
                                      ],
                       'USBD' => [
                                   'sw_usbd'
                                 ],
                       'MEMSYS0' => [
                                      'sw_memsys0'
                                    ],
                       'SPI' => [
                                  'sw_spi'
                                ],
                       'UART1' => [
                                    'sw_uart1'
                                  ],
                       'SATA30' => [
                                     'sw_sata30'
                                   ],
                       'PCIE0' => [
                                    'sw_pcie0'
                                  ],
                       'UART2' => [
                                    'sw_uart2'
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
                       'GENETMUX0' => [
                                        'sw_genetmux0'
                                      ],
                       'UART0' => [
                                    'sw_uart0'
                                  ],
                       'GENETEEE0' => [
                                        'sw_geneteee0'
                                      ]
                     }
        };


# [---]   cpu_dis_ch0 => CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_0
#                                    /CLOCK_DIS_CH0
# [---]   cpu_dis_ch1 => CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_1
#                                    /CLOCK_DIS_CH1
# [---]   cpu_mdiv_ch0 => CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_0_4K
#                                    /MDIV_CH0
# [---]   cpu_mdiv_ch1 aka sys_if_clk => CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_1_4K
#                                    /MDIV_CH1
# [---]   cpu_ndiv_int => CLKGEN_PLL_CPU_PLL_DIV_4K
#                                    /NDIV_INT
# [---]   cpu_pdh_ch0 => CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_0
#                                    /POST_DIVIDER_HOLD_CH0
# [---]   cpu_pdh_ch1 => CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_1
#                                    /POST_DIVIDER_HOLD_CH1
# [---]   cpu_pdiv => CLKGEN_PLL_CPU_PLL_DIV
#                                    /PDIV
# [---] R fixed1 => fixed1
# [---] R fixed2 => fixed2
# [---]   fixed_factor0 => fixed_factor0
#                                    /
# [---]   fixed_factor1 => fixed_factor1
#                                    /
# [---]   fixed_factor2 => fixed_factor2
#                                    /
# [---]   fixed_factor3 => fixed_factor3
#                                    /
# [---]   fixed_factor4 => fixed_factor4
#                                    /
# [---]   fixed_factor5 => fixed_factor5
#                                    /
# [---] R fixed_syn0 => fixed_syn0
# [---]   genet0_250 => CLKGEN_STB_GENET_TOP_INST_CLOCK_ENABLE
#                                    /GENET0_CLK_250_CLOCK_ENABLE
# [---] R genet0_alwayson => CLKGEN_STB_GENET_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET0_ALWAYSON_CLOCK
# [---] R genet0_eee => CLKGEN_STB_GENET_TOP_INST_CLOCK_ENABLE
#                                    /GENET0_EEE_CLOCK_ENABLE
# [---] R genet0_gisb => CLKGEN_STB_GENET_TOP_INST_CLOCK_ENABLE
#                                    /GENET0_GISB_CLOCK_ENABLE
# [---] R genet0_gmii => CLKGEN_STB_GENET_TOP_INST_CLOCK_ENABLE
#                                    /GENET0_GMII_CLOCK_ENABLE
# [---] R genet0_hfb => CLKGEN_STB_GENET_TOP_INST_CLOCK_ENABLE
#                                    /GENET0_HFB_CLOCK_ENABLE
# [---] R genet0_l2intr => CLKGEN_STB_GENET_TOP_INST_CLOCK_ENABLE
#                                    /GENET0_L2INTR_CLOCK_ENABLE
# [---]   genet0_scb => CLKGEN_STB_GENET_TOP_INST_CLOCK_ENABLE
#                                    /GENET0_SCB_CLOCK_ENABLE
# [---]   genet0_select aka sw_genetmux0 => CLKGEN_STB_GENET_TOP_INST_CLOCK_SELECT
#                                    /GENET0_CLOCK_SELECT
# [---]   genet0_sys_fast => CLKGEN_STB_GENET_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET0_SYSTEM_FAST_CLOCK
# [---] R genet0_sys_pm => CLKGEN_STB_GENET_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET0_SYSTEM_PM_CLOCK
# [---] R genet0_sys_slow => CLKGEN_STB_GENET_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_GENET0_SYSTEM_SLOW_CLOCK
# [---] R genet0_umac_sys_rx => CLKGEN_STB_GENET_TOP_INST_CLOCK_ENABLE
#                                    /GENET0_UNIMAC_SYS_RX_CLOCK_ENABLE
# [---] R genet0_umac_sys_tx => CLKGEN_STB_GENET_TOP_INST_CLOCK_ENABLE
#                                    /GENET0_UNIMAC_SYS_TX_CLOCK_ENABLE
# [---] R genet_54 => CLKGEN_STB_GENET_TOP_INST_CLOCK_ENABLE
#                                    /GENET_54_CLOCK_ENABLE
# [---]   genet_scb => CLKGEN_STB_GENET_TOP_INST_CLOCK_ENABLE
#                                    /GENET_SCB_CLOCK_ENABLE
# [---] R hif_ebi => CLKGEN_STB_CORE_XPT_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_HIF_EBI_CLOCK
# [---]   hif_sdio_card => CLKGEN_STB_CORE_XPT_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_HIF_SDIO_CARD_CLOCK
# [---]   hif_sdio_emmc => CLKGEN_STB_CORE_XPT_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_HIF_SDIO_EMMC_CLOCK
# [---]   hif_spi aka sw_spi => CLKGEN_STB_CORE_XPT_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_HIF_SPI_CLOCK
# [---] R memsys0_54 => CLKGEN_STB_MEMSYS_32_0_INST_CLOCK_ENABLE
#                                    /MEMSYS0_54_CLOCK_ENABLE
# [---] R memsys0_gisb => CLKGEN_STB_MEMSYS_32_0_INST_CLOCK_ENABLE
#                                    /MEMSYS0_GISB_CLOCK_ENABLE
# [---]   memsys0_scb => CLKGEN_STB_MEMSYS_32_0_INST_CLOCK_ENABLE
#                                    /MEMSYS0_SCB_CLOCK_ENABLE
# [---]   nand_ddr => CLKGEN_STB_CORE_XPT_HIF_INST_CLOCK_DISABLE
#                                    /DISABLE_NAND_DDR_CLOCK
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
# [---]   net_pwrdn_req => CLKGEN_SPARE
#                                    /NETWORK_PWRDN_PLL_REQ
# [---] R orion_cpu_c_54 => CLKGEN_CPU_ORION_TOP_INST_CLOCK_ENABLE
#                                    /ORION_CPU_C_54_CLOCK_ENABLE
# [---] R orion_cpu_secure => CLKGEN_CPU_ORION_TOP_INST_CLOCK_ENABLE
#                                    /ORION_CPU_SECURE_CLOCK_ENABLE
# [---] R orion_gisb => CLKGEN_CPU_ORION_TOP_INST_CLOCK_ENABLE
#                                    /ORION_GISB_CLOCK_ENABLE
# [---]   orion_scb => CLKGEN_CPU_ORION_TOP_INST_CLOCK_ENABLE
#                                    /ORION_SCB_CLOCK_ENABLE
# [---] R osc_sata_pcie => AON_CTRL_ANA_XTAL_CONTROL
#                                    /osc_cml_sel_pd_sata_pcie
# [---] R pcie0_alwayson => CLKGEN_STB_SATA3_PCIE_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_PCIE0_ALWAYSON_CLOCK
# [---]   stb_sysctrl_uart_0 aka sw_uart0 => CLKGEN_STB_SYS_CTRL_INST_CLOCK_DISABLE
#                                    /DISABLE_SYSCTRL_UART_0_CLOCK
# [---]   stb_sysctrl_uart_1 aka sw_uart1 => CLKGEN_STB_SYS_CTRL_INST_CLOCK_DISABLE
#                                    /DISABLE_SYSCTRL_UART_1_CLOCK
# [---]   stb_sysctrl_uart_2 aka sw_uart2 => CLKGEN_STB_SYS_CTRL_INST_CLOCK_DISABLE
#                                    /DISABLE_SYSCTRL_UART_2_CLOCK
# [---]   sw_cpu => sw_cpu
# [---]   sw_genet0 => sw_genet0
# [---]   sw_geneteee0 => sw_geneteee0
# [---]   sw_genetwol0 => sw_genetwol0
# [---]   sw_memsys0 => sw_memsys0
# [---]   sw_mpi => sw_mpi
# [---]   sw_pcie0 => sw_pcie0
# [---]   sw_sata30 => sw_sata30
# [---]   sw_sdio => sw_sdio
# [---]   sw_usb20 => sw_usb20
# [---]   sw_usbd => sw_usbd
# [---]   sys0_dis_ch0 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_0
#                                    /CLOCK_DIS_CH0
# [---]   sys0_dis_ch1 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1
#                                    /CLOCK_DIS_CH1
# [---]   sys0_dis_ch2 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_2
#                                    /CLOCK_DIS_CH2
# [---]   sys0_dis_ch3 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3
#                                    /CLOCK_DIS_CH3
# [---]   sys0_dis_ch5 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_5
#                                    /CLOCK_DIS_CH5
# [---]   sys0_mdiv_ch0 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_0
#                                    /MDIV_CH0
# [---]   sys0_mdiv_ch1 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1
#                                    /MDIV_CH1
# [---]   sys0_mdiv_ch2 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_2
#                                    /MDIV_CH2
# [---]   sys0_mdiv_ch3 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3
#                                    /MDIV_CH3
# [---]   sys0_mdiv_ch5 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_5
#                                    /MDIV_CH5
# [---]   sys0_pdh_ch0 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_0
#                                    /POST_DIVIDER_HOLD_CH0
# [---]   sys0_pdh_ch1 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1
#                                    /POST_DIVIDER_HOLD_CH1
# [---]   sys0_pdh_ch2 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_2
#                                    /POST_DIVIDER_HOLD_CH2
# [---]   sys0_pdh_ch3 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3
#                                    /POST_DIVIDER_HOLD_CH3
# [---]   sys0_pdh_ch5 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_5
#                                    /POST_DIVIDER_HOLD_CH5
# [---]   sys_108_pcie0 => CLKGEN_STB_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE0
#                                    /SYSTEM_108_CLOCK_ENABLE_PCIE0
# [---]   sys_108_usb20 => CLKGEN_STB_USB0_TOP_INST_CLOCK_ENABLE_USB20
#                                    /SYSTEM_108_CLOCK_ENABLE_USB20
# [---]   sys_108_usbd => CLKGEN_STB_USB0_TOP_INST_CLOCK_ENABLE_USBD
#                                    /SYSTEM_108_CLOCK_ENABLE_USBD
# [---] R sys_54_pcie0 => CLKGEN_STB_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE0
#                                    /SYSTEM_54_CLOCK_ENABLE_PCIE0
# [---] R sys_54_sata30 => CLKGEN_STB_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_SATA30
#                                    /SYSTEM_54_CLOCK_ENABLE_SATA30
# [---] R sys_54_usb20 => CLKGEN_STB_USB0_TOP_INST_CLOCK_ENABLE_USB20
#                                    /SYSTEM_54_CLOCK_ENABLE_USB20
# [---] R sys_gisb_pcie0 => CLKGEN_STB_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE0
#                                    /SYSTEM_GISB_CLOCK_ENABLE_PCIE0
# [---] R sys_gisb_sata30 => CLKGEN_STB_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_SATA30
#                                    /SYSTEM_GISB_CLOCK_ENABLE_SATA30
# [---]   sys_scb_pcie0 => CLKGEN_STB_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE0
#                                    /SYSTEM_SCB_CLOCK_ENABLE_PCIE0
# [---]   sys_scb_sata30 => CLKGEN_STB_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_SATA30
#                                    /SYSTEM_SCB_CLOCK_ENABLE_SATA30
# [---]   sys_scb_usb20 => CLKGEN_STB_USB0_TOP_INST_CLOCK_ENABLE_USB20
#                                    /SYSTEM_SCB_CLOCK_ENABLE_USB20
# [---]   sys_scb_usbd => CLKGEN_STB_USB0_TOP_INST_CLOCK_ENABLE_USBD
#                                    /SYSTEM_SCB_CLOCK_ENABLE_USBD
# [---]   uart_sysctrl_uart_0 => CLKGEN_UART_0_CLOCK_MUX_SELECT
#                                    /SYSCTRL_UART_0_CLOCK
# [---]   uart_sysctrl_uart_1 => CLKGEN_UART_1_CLOCK_MUX_SELECT
#                                    /SYSCTRL_UART_1_CLOCK
# [---]   uart_sysctrl_uart_2 => CLKGEN_UART_2_CLOCK_MUX_SELECT
#                                    /SYSCTRL_UART_2_CLOCK
# [---] R usb0_freerun => CLKGEN_STB_USB0_TOP_INST_CLOCK_DISABLE
#                                    /DISABLE_USB0_FREERUN_CLOCK
# [---] R usb0_gisb => CLKGEN_STB_USB0_TOP_INST_CLOCK_ENABLE
#                                    /USB0_GISB_CLOCK_ENABLE
# [---] R xpt_hif_gisb => CLKGEN_STB_CORE_XPT_HIF_INST_CLOCK_ENABLE
#                                    /XPT_HIF_GISB_CLOCK_ENABLE
