$VAR1 = {
          'rdb_sha1' => 'PAKj79e2gZ2CPhnjnkJ44gxx4Q0',
          'fail' => 0,
          'date' => 'Tue Mar 28 14:00:39 PDT 2017',
          'rdb_version' => 'rdb-v2-55-gc04e3f6',
          'rdb_dir' => '/projects/stbgit/stblinux/git/clkgen/7278a0/current',
          'clkgen_version' => 'clkgen-v4-307-g9726c68',
          'pm_ver' => '2016_11_01.14_30',
          'chip' => '7278a0',
          'aliases' => {},
          'unhandled_linux_funcs' => 'CPU, CPU_SysIF, HIF, MEMSYS0, MEMSYS1, MPI',
          'invocation' => 'clkgen.pl --sw_nodes -v -g -r -P -c 7278a0',
          'num_clks' => 104,
          'clks' => '	brcmstb-clks {
		#address-cells = <1>;
		#size-cells = <1>;
		reg = <0x08160000 0x101c 0x08151800 0xb0 0x08410000 0x600>;
		ranges;

		fixed1 : fixed1 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <0x337f980>;
		};

		hvd_pwrdn_req : hvd_pwrdn_req@8160518 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160518 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&fixed1>; 
			clock-names = "fixed1"; 
		};

		hvd_pdiv : hvd_pdiv@8160054 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0x8160054 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&hvd_pwrdn_req>; 
			clock-names = "hvd_pwrdn_req"; 
		};

		hvd_ndiv_int : hvd_ndiv_int {
			compatible = "multiplier-clock", "fixed-factor-clock";
			#clock-cells = <0>;
			reg = <0x8160054 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3ff>;
			index-max-mult-at-zero;
			clock-div = <1>;
			clock-mult = <200>;
			clocks = <&hvd_pdiv>; 
			clock-names = "hvd_pdiv"; 
		};

		hvd_mdiv_ch4 : hvd_mdiv_ch4@816004c {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0x816004c 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&hvd_ndiv_int>; 
			clock-names = "hvd_ndiv_int"; 
		};

		hvd_dis_ch4 : hvd_dis_ch4@816004c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x816004c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&hvd_mdiv_ch4>; 
			clock-names = "hvd_mdiv_ch4"; 
		};

		hvd_pdh_ch4 : hvd_pdh_ch4@816004c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x816004c 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&hvd_dis_ch4>; 
			clock-names = "hvd_dis_ch4"; 
		};

		hif_sdio_card : hif_sdio_card@81602f4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x81602f4 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			clocks = <&hvd_pdh_ch4>; 
			clock-names = "hvd_pdh_ch4"; 
		};

		hif_sdio_emmc : hif_sdio_emmc@81602f4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x81602f4 0x4>;
			bit-shift = <2>;
			set-bit-to-disable;
			clocks = <&hvd_pdh_ch4>; 
			clock-names = "hvd_pdh_ch4"; 
		};

		hvd_mdiv_ch3 : hvd_mdiv_ch3@8160048 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0x8160048 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&hvd_ndiv_int>; 
			clock-names = "hvd_ndiv_int"; 
		};

		hvd_dis_ch3 : hvd_dis_ch3@8160048 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160048 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&hvd_mdiv_ch3>; 
			clock-names = "hvd_mdiv_ch3"; 
		};

		hvd_pdh_ch3 : hvd_pdh_ch3@8160048 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160048 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&hvd_dis_ch3>; 
			clock-names = "hvd_dis_ch3"; 
		};

		hif_spi : sw_spi : hif_spi@81602f4 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x81602f4 0x4>;
			bit-shift = <3>;
			set-bit-to-disable;
			clocks = <&hvd_pdh_ch3>; 
			clock-names = "hvd_pdh_ch3"; 
		};

		fixed0 : fixed0 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <0x337f980>;
		};

		cpu_pdiv : cpu_pdiv@816000c {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0x816000c 0x4>;
			bit-shift = <0>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&fixed0>; 
			clock-names = "fixed0"; 
		};

		cpu_ndiv_int : cpu_ndiv_int {
			compatible = "multiplier-clock";
			#clock-cells = <0>;
			reg = <0x8161018 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3ff>;
			index-max-mult-at-zero;
			clock-div = <1>;
			clock-mult = <200>;
			clocks = <&cpu_pdiv>; 
			clock-names = "cpu_pdiv"; 
		};

		cpu_mdiv_ch0 : cpu_mdiv_ch0@8161010 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0x8161010 0x4>;
			bit-shift = <0>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&cpu_ndiv_int>; 
			clock-names = "cpu_ndiv_int"; 
		};

		cpu_mdiv_ch1 : sys_if_clk : cpu_mdiv_ch1@8161014 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0x8161014 0x4>;
			bit-shift = <0>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&cpu_ndiv_int>; 
			clock-names = "cpu_ndiv_int"; 
		};

		pcie0_54_pcie : pcie0_54_pcie@8160414 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160414 0x4>;
			bit-shift = <0>;
		};

		pcie0_gisb_pcie : pcie0_gisb_pcie@8160414 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160414 0x4>;
			bit-shift = <1>;
		};

		pcie0_scb_pcie : pcie0_scb_pcie@8160414 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160414 0x4>;
			bit-shift = <2>;
		};

		pcie0_108_pcie : pcie0_108_pcie@8160414 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160414 0x4>;
			bit-shift = <3>;
		};

		pcie1_54_pcie : pcie1_54_pcie@8160418 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160418 0x4>;
			bit-shift = <0>;
		};

		pcie1_gisb_pcie : pcie1_gisb_pcie@8160418 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160418 0x4>;
			bit-shift = <1>;
		};

		pcie1_scb_pcie : pcie1_scb_pcie@8160418 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160418 0x4>;
			bit-shift = <2>;
		};

		pcie1_108_pcie : pcie1_108_pcie@8160418 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160418 0x4>;
			bit-shift = <3>;
		};

		sata3_54 : sata3_54@8160424 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160424 0x4>;
			bit-shift = <0>;
		};

		sata3_gisb : sata3_gisb@8160424 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160424 0x4>;
			bit-shift = <1>;
		};

		sata3_scb : sata3_scb@8160424 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160424 0x4>;
			bit-shift = <2>;
		};

		fixed2 : fixed2 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <0x337f980>;
		};

		net_pwrdn_req : net_pwrdn_req@816051c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x816051c 0x4>;
			bit-shift = <1>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&fixed2>; 
			clock-names = "fixed2"; 
		};

		net_pdiv : net_pdiv@816009c {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0x816009c 0x4>;
			bit-shift = <10>;
			bit-mask = <0xf>;
			index-starts-at-one;
			clocks = <&net_pwrdn_req>; 
			clock-names = "net_pwrdn_req"; 
		};

		net_ndiv_int : net_ndiv_int {
			compatible = "multiplier-clock", "fixed-factor-clock";
			#clock-cells = <0>;
			reg = <0x816009c 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3ff>;
			index-max-mult-at-zero;
			clock-div = <1>;
			clock-mult = <125>;
			clocks = <&net_pdiv>; 
			clock-names = "net_pdiv"; 
		};

		net_mdiv_ch0 : net_mdiv_ch0@8160084 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0x8160084 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch0 : net_dis_ch0@8160084 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160084 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch0>; 
			clock-names = "net_mdiv_ch0"; 
		};

		net_pdh_ch0 : net_pdh_ch0@8160084 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160084 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch0>; 
			clock-names = "net_dis_ch0"; 
		};

		sw_25 : sw_25@8160448 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160448 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		swi_54 : swi_54@8160450 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160450 0x4>;
			bit-shift = <0>;
		};

		swi_gisb : swi_gisb@8160450 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160450 0x4>;
			bit-shift = <1>;
		};

		sprt0_gmii_tx_sprt : sprt0_gmii_tx_sprt@8160458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160458 0x4>;
			bit-shift = <0>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		sprt0_rx_scb_sprt : sprt0_rx_scb_sprt@8160458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160458 0x4>;
			bit-shift = <1>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		sprt0_rx_sys_sprt : sprt0_rx_sys_sprt@8160458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160458 0x4>;
			bit-shift = <2>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		sprt0_scb_sprt : sprt0_scb_sprt@8160458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160458 0x4>;
			bit-shift = <3>;
		};

		sprt0_tx_scb_sprt : sprt0_tx_scb_sprt@8160458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160458 0x4>;
			bit-shift = <4>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		sprt0_gisb_sprt : sprt0_gisb_sprt@8160458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160458 0x4>;
			bit-shift = <5>;
		};

		sprt0_tx_sys_sprt : sprt0_tx_sys_sprt@8160458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160458 0x4>;
			bit-shift = <6>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		sprt1_gmii_tx_sprt : sprt1_gmii_tx_sprt@8160458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160458 0x4>;
			bit-shift = <7>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		sprt1_rx_scb_sprt : sprt1_rx_scb_sprt@8160458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160458 0x4>;
			bit-shift = <8>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		sprt1_rx_sys_sprt : sprt1_rx_sys_sprt@8160458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160458 0x4>;
			bit-shift = <9>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		sprt1_scb_sprt : sprt1_scb_sprt@8160458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160458 0x4>;
			bit-shift = <10>;
		};

		sprt1_tx_scb_sprt : sprt1_tx_scb_sprt@8160458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160458 0x4>;
			bit-shift = <11>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		sprt1_gisb_sprt : sprt1_gisb_sprt@8160458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160458 0x4>;
			bit-shift = <12>;
		};

		sprt1_tx_sys_sprt : sprt1_tx_sys_sprt@8160458 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160458 0x4>;
			bit-shift = <13>;
			clocks = <&net_pdh_ch0>; 
			clock-names = "net_pdh_ch0"; 
		};

		fixed_syn0 : fixed_syn0 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <0x269fb200>;
		};

		fixed_factor0 : fixed_factor0 {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <8>;
			clock-mult = <1>;
			clocks = <&fixed_syn0>; 
			clock-names = "fixed_syn0"; 
		};

		fixed_factor1 : fixed_factor1 {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <6>;
			clock-mult = <1>;
			clocks = <&fixed_syn0>; 
			clock-names = "fixed_syn0"; 
		};

		net_mdiv_ch3 : net_mdiv_ch3@8160090 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0x8160090 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch3 : net_dis_ch3@8160090 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160090 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&net_mdiv_ch3>; 
			clock-names = "net_mdiv_ch3"; 
		};

		net_pdh_ch3 : net_pdh_ch3@8160090 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160090 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&net_dis_ch3>; 
			clock-names = "net_dis_ch3"; 
		};

		fixed_syn1 : fixed_syn1 {
			compatible = "fixed-clock";
			#clock-cells = <0>;
			clock-frequency = <0xe7be2c00>;
		};

		sys0_mdiv_ch3 : sys0_mdiv_ch3@8160140 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0x8160140 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&fixed_syn1>; 
			clock-names = "fixed_syn1"; 
		};

		sys0_dis_ch3 : sys0_dis_ch3@8160140 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160140 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&sys0_mdiv_ch3>; 
			clock-names = "sys0_mdiv_ch3"; 
		};

		sys0_pdh_ch3 : sys0_pdh_ch3@8160140 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160140 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,read-only;
			clocks = <&sys0_dis_ch3>; 
			clock-names = "sys0_dis_ch3"; 
		};

		uart_sysctrl_uart_0 : uart_sysctrl_uart_0@8160480 {
			compatible = "brcm,mux-clock", "mux-clock";
			#clock-cells = <0>;
			reg = <0x8160480 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3>;
			clocks = <&fixed_factor0>, <&fixed_factor1>, 
			  <&net_pdh_ch3>, <&sys0_pdh_ch3>; 
			clock-names = "fixed_factor0", "fixed_factor1", 
			  "net_pdh_ch3", "sys0_pdh_ch3"; 
		};

		sys_sysctrl_uart_0 : sw_uart0 : sys_sysctrl_uart_0@8160470 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160470 0x4>;
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
			clocks = <&fixed_syn0>; 
			clock-names = "fixed_syn0"; 
		};

		fixed_factor3 : fixed_factor3 {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <6>;
			clock-mult = <1>;
			clocks = <&fixed_syn0>; 
			clock-names = "fixed_syn0"; 
		};

		uart_sysctrl_uart_1 : uart_sysctrl_uart_1@8160484 {
			compatible = "brcm,mux-clock", "mux-clock";
			#clock-cells = <0>;
			reg = <0x8160484 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3>;
			clocks = <&fixed_factor2>, <&fixed_factor3>, 
			  <&net_pdh_ch3>, <&sys0_pdh_ch3>; 
			clock-names = "fixed_factor2", "fixed_factor3", 
			  "net_pdh_ch3", "sys0_pdh_ch3"; 
		};

		sys_sysctrl_uart_1 : sw_uart1 : sys_sysctrl_uart_1@8160470 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160470 0x4>;
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
			clocks = <&fixed_syn0>; 
			clock-names = "fixed_syn0"; 
		};

		fixed_factor5 : fixed_factor5 {
			compatible = "fixed-factor-clock";
			#clock-cells = <0>;
			clock-div = <6>;
			clock-mult = <1>;
			clocks = <&fixed_syn0>; 
			clock-names = "fixed_syn0"; 
		};

		uart_sysctrl_uart_2 : uart_sysctrl_uart_2@8160488 {
			compatible = "brcm,mux-clock", "mux-clock";
			#clock-cells = <0>;
			reg = <0x8160488 0x4>;
			bit-shift = <0>;
			bit-mask = <0x3>;
			clocks = <&fixed_factor4>, <&fixed_factor5>, 
			  <&net_pdh_ch3>, <&sys0_pdh_ch3>; 
			clock-names = "fixed_factor4", "fixed_factor5", 
			  "net_pdh_ch3", "sys0_pdh_ch3"; 
		};

		sys_sysctrl_uart_2 : sw_uart2 : sys_sysctrl_uart_2@8160470 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160470 0x4>;
			bit-shift = <6>;
			set-bit-to-disable;
			brcm,inhibit-disable;
			brcm,set-rate-parent;
			brcm,read-only;
			clocks = <&uart_sysctrl_uart_2>; 
			clock-names = "uart_sysctrl_uart_2"; 
		};

		usb0_54 : usb0_54@8160490 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160490 0x4>;
			bit-shift = <0>;
		};

		usb0_gisb : usb0_gisb@8160490 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160490 0x4>;
			bit-shift = <1>;
		};

		usb0_scb_1st : usb0_scb_1st@8160490 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160490 0x4>;
			bit-shift = <2>;
		};

		usb0_scb_2nd : usb0_scb_2nd@8160490 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160490 0x4>;
			bit-shift = <3>;
		};

		usb0_scb_3rd : usb0_scb_3rd@8160490 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160490 0x4>;
			bit-shift = <4>;
		};

		usb0_108_ahb : usb0_108_ahb@8160494 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160494 0x4>;
			bit-shift = <0>;
		};

		usb0_108_axi : usb0_108_axi@816049c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x816049c 0x4>;
			bit-shift = <0>;
		};

		net_mdiv_ch1 : net_mdiv_ch1@8160088 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0x8160088 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch1 : net_dis_ch1@8160088 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160088 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch1>; 
			clock-names = "net_mdiv_ch1"; 
		};

		net_pdh_ch1 : net_pdh_ch1@8160088 {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x8160088 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch1>; 
			clock-names = "net_dis_ch1"; 
		};

		net_mdiv_ch2 : net_mdiv_ch2@816008c {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0x816008c 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&net_ndiv_int>; 
			clock-names = "net_ndiv_int"; 
		};

		net_dis_ch2 : net_dis_ch2@816008c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x816008c 0x4>;
			bit-shift = <0>;
			set-bit-to-disable;
			clocks = <&net_mdiv_ch2>; 
			clock-names = "net_mdiv_ch2"; 
		};

		net_pdh_ch2 : net_pdh_ch2@816008c {
			compatible = "brcm,brcmstb-gate-clk";
			#clock-cells = <0>;
			reg = <0x816008c 0x4>;
			bit-shift = <10>;
			set-bit-to-disable;
			clocks = <&net_dis_ch2>; 
			clock-names = "net_dis_ch2"; 
		};

		sys0_mdiv_ch1 : sys0_mdiv_ch1@8160138 {
			compatible = "divider-clock";
			#clock-cells = <0>;
			reg = <0x8160138 0x4>;
			bit-shift = <1>;
			bit-mask = <0xff>;
			index-starts-at-one;
			clocks = <&fixed_syn1>; 
			clock-names = "fixed_syn1"; 
		};

		sw_cpu : sw_cpu {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&cpu_mdiv_ch0>, <&cpu_mdiv_ch1>, 
			  <&sys0_mdiv_ch1>, <&fixed_syn0>; 
			clock-names = "cpu_mdiv_ch0", "cpu_mdiv_ch1", 
			  "sys0_mdiv_ch1", "fixed_syn0"; 
		};

		sw_cpu_sysif : sw_cpu_sysif {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&fixed_syn0>, <&fixed_syn1>; 
			clock-names = "fixed_syn0", "fixed_syn1"; 
		};

		sw_hif : sw_hif {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&sys0_mdiv_ch1>, <&fixed_syn0>; 
			clock-names = "sys0_mdiv_ch1", "fixed_syn0"; 
		};

		sw_memsys0 : sw_memsys0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&sys0_mdiv_ch1>, <&fixed_syn0>; 
			clock-names = "sys0_mdiv_ch1", "fixed_syn0"; 
		};

		sw_memsys1 : sw_memsys1 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&sys0_mdiv_ch1>, <&fixed_syn0>; 
			clock-names = "sys0_mdiv_ch1", "fixed_syn0"; 
		};

		sw_mpi : sw_mpi {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&fixed_syn0>, <&fixed_syn1>; 
			clock-names = "fixed_syn0", "fixed_syn1"; 
		};

		sw_pcie0 : sw_pcie0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&pcie0_54_pcie>, <&pcie0_gisb_pcie>, 
			  <&pcie0_scb_pcie>, <&pcie0_108_pcie>, 
			  <&sys0_mdiv_ch1>, <&fixed_syn0>; 
			clock-names = "pcie0_54_pcie", "pcie0_gisb_pcie", 
			  "pcie0_scb_pcie", "pcie0_108_pcie", "sys0_mdiv_ch1", 
			  "fixed_syn0"; 
		};

		sw_pcie1 : sw_pcie1 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&pcie1_54_pcie>, <&pcie1_gisb_pcie>, 
			  <&pcie1_scb_pcie>, <&pcie1_108_pcie>, 
			  <&sys0_mdiv_ch1>, <&fixed_syn0>; 
			clock-names = "pcie1_54_pcie", "pcie1_gisb_pcie", 
			  "pcie1_scb_pcie", "pcie1_108_pcie", "sys0_mdiv_ch1", 
			  "fixed_syn0"; 
		};

		sw_sata3 : sw_sata3 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&sata3_54>, <&sata3_gisb>, <&sata3_scb>, 
			  <&sys0_mdiv_ch1>, <&fixed_syn0>; 
			clock-names = "sata3_54", "sata3_gisb", "sata3_scb", 
			  "sys0_mdiv_ch1", "fixed_syn0"; 
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
			clocks = <&sw_25>, <&swi_54>, <&swi_gisb>, 
			  <&net_pdh_ch1>, <&net_pdh_ch2>, <&sys0_mdiv_ch1>, 
			  <&fixed_syn0>; 
			clock-names = "sw_25", "swi_54", "swi_gisb", 
			  "net_pdh_ch1", "net_pdh_ch2", "sys0_mdiv_ch1", 
			  "fixed_syn0"; 
		};

		sw_sysport0 : sw_sysport0 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&sprt0_gmii_tx_sprt>, <&sprt0_rx_scb_sprt>, 
			  <&sprt0_rx_sys_sprt>, <&sprt0_scb_sprt>, 
			  <&sprt0_tx_scb_sprt>, <&sprt0_gisb_sprt>, 
			  <&sprt0_tx_sys_sprt>, <&sys0_mdiv_ch1>, 
			  <&fixed_syn0>; 
			clock-names = "sprt0_gmii_tx_sprt", 
			  "sprt0_rx_scb_sprt", "sprt0_rx_sys_sprt", 
			  "sprt0_scb_sprt", "sprt0_tx_scb_sprt", 
			  "sprt0_gisb_sprt", "sprt0_tx_sys_sprt", 
			  "sys0_mdiv_ch1", "fixed_syn0"; 
		};

		sw_sysport0wol : sw_sysport0wol {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&sprt0_rx_sys_sprt>, <&sprt0_tx_sys_sprt>; 
			clock-names = "sprt0_rx_sys_sprt", 
			  "sprt0_tx_sys_sprt"; 
		};

		sw_sysport1 : sw_sysport1 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&sprt1_gmii_tx_sprt>, <&sprt1_rx_scb_sprt>, 
			  <&sprt1_rx_sys_sprt>, <&sprt1_scb_sprt>, 
			  <&sprt1_tx_scb_sprt>, <&sprt1_gisb_sprt>, 
			  <&sprt1_tx_sys_sprt>, <&sys0_mdiv_ch1>, 
			  <&fixed_syn0>; 
			clock-names = "sprt1_gmii_tx_sprt", 
			  "sprt1_rx_scb_sprt", "sprt1_rx_sys_sprt", 
			  "sprt1_scb_sprt", "sprt1_tx_scb_sprt", 
			  "sprt1_gisb_sprt", "sprt1_tx_sys_sprt", 
			  "sys0_mdiv_ch1", "fixed_syn0"; 
		};

		sw_sysport1wol : sw_sysport1wol {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&sprt1_rx_sys_sprt>, <&sprt1_tx_sys_sprt>; 
			clock-names = "sprt1_rx_sys_sprt", 
			  "sprt1_tx_sys_sprt"; 
		};

		sw_usb20 : sw_usb20 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&usb0_54>, <&usb0_gisb>, <&usb0_scb_1st>, 
			  <&usb0_scb_3rd>, <&usb0_108_ahb>, <&usb0_108_axi>, 
			  <&sys0_mdiv_ch1>, <&fixed_syn0>; 
			clock-names = "usb0_54", "usb0_gisb", "usb0_scb_1st", 
			  "usb0_scb_3rd", "usb0_108_ahb", "usb0_108_axi", 
			  "sys0_mdiv_ch1", "fixed_syn0"; 
		};

		sw_usb30 : sw_usb30 {
			compatible = "brcm,brcmstb-sw-clk";
			#clock-cells = <0>;
			clocks = <&usb0_54>, <&usb0_gisb>, <&usb0_scb_2nd>, 
			  <&usb0_108_ahb>, <&usb0_108_axi>, <&sys0_mdiv_ch1>, 
			  <&fixed_syn0>; 
			clock-names = "usb0_54", "usb0_gisb", "usb0_scb_2nd", 
			  "usb0_108_ahb", "usb0_108_axi", "sys0_mdiv_ch1", 
			  "fixed_syn0"; 
		};

	};
',
          'funcs' => {
                       'MEMSYS1' => [
                                      'sw_memsys1'
                                    ],
                       'CPU_SysIF' => [
                                        'sw_cpu_sysif'
                                      ],
                       'USB20' => [
                                    'sw_usb20'
                                  ],
                       'SDIO' => [
                                   'sw_sdio'
                                 ],
                       'SYSPORT1WOL' => [
                                          'sw_sysport1wol'
                                        ],
                       'USB30' => [
                                    'sw_usb30'
                                  ],
                       'MEMSYS0' => [
                                      'sw_memsys0'
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
                       'UART1' => [
                                    'sw_uart1'
                                  ],
                       'PCIE0' => [
                                    'sw_pcie0'
                                  ],
                       'UART2' => [
                                    'sw_uart2'
                                  ],
                       'SATA3' => [
                                    'sw_sata3'
                                  ],
                       'MPI' => [
                                  'sw_mpi'
                                ],
                       'CPU' => [
                                  'sw_cpu'
                                ],
                       'SYSPORT0' => [
                                       'sw_sysport0'
                                     ],
                       'SYSPORT0WOL' => [
                                          'sw_sysport0wol'
                                        ],
                       'SWITCH' => [
                                     'sw_switch'
                                   ],
                       'SYSPORT1' => [
                                       'sw_sysport1'
                                     ],
                       'UART0' => [
                                    'sw_uart0'
                                  ]
                     }
        };


# [---]   cpu_mdiv_ch0 => CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_0_4K
#                                    /MDIV_CH0
# [---]   cpu_mdiv_ch1 aka sys_if_clk => CLKGEN_PLL_CPU_PLL_CHANNEL_CTRL_CH_1_4K
#                                    /MDIV_CH1
# [---]   cpu_ndiv_int => CLKGEN_PLL_CPU_PLL_DIV_4K
#                                    /NDIV_INT
# [---]   cpu_pdiv => CLKGEN_PLL_CPU_PLL_DIV
#                                    /PDIV
# [---] R fixed0 => fixed0
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
# [---] R fixed_syn1 => fixed_syn1
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
# [---]   net_mdiv_ch3 => CLKGEN_PLL_NETWORK_PLL_CHANNEL_CTRL_CH_3
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
# [---] R pcie0_108_pcie => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE
#                                    /PCIE1_108_CLOCK_ENABLE_PCIE
# [---] R pcie0_54_pcie => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE
#                                    /PCIE1_54_CLOCK_ENABLE_PCIE
# [---] R pcie0_gisb_pcie => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE
#                                    /PCIE1_GISB_CLOCK_ENABLE_PCIE
# [---] R pcie0_scb_pcie => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE
#                                    /PCIE1_SCB_CLOCK_ENABLE_PCIE
# [---] R pcie1_108_pcie => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE1
#                                    /PCIE1_108_CLOCK_ENABLE_PCIE1
# [---] R pcie1_54_pcie => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE1
#                                    /PCIE1_54_CLOCK_ENABLE_PCIE1
# [---] R pcie1_gisb_pcie => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE1
#                                    /PCIE1_GISB_CLOCK_ENABLE_PCIE1
# [---] R pcie1_scb_pcie => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_PCIE1
#                                    /PCIE1_SCB_CLOCK_ENABLE_PCIE1
# [---] R sata3_54 => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_SATA3
#                                    /SATA3_54_CLOCK_ENABLE_SATA3
# [---] R sata3_gisb => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_SATA3
#                                    /SATA3_GISB_CLOCK_ENABLE_SATA3
# [---] R sata3_scb => CLKGEN_SATA3_PCIE_TOP_INST_CLOCK_ENABLE_SATA3
#                                    /SATA3_SCB_CLOCK_ENABLE_SATA3
# [---] R sprt0_gisb_sprt => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT0_GISB_CLOCK_ENABLE_SYSTEMPORT
# [---]   sprt0_gmii_tx_sprt => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT0_GMII_TX_CLOCK_ENABLE_SYSTEMPORT
# [---]   sprt0_rx_scb_sprt => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT0_RX_SCB_CLOCK_ENABLE_SYSTEMPORT
# [---]   sprt0_rx_sys_sprt => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT0_RX_SYS_CLOCK_ENABLE_SYSTEMPORT
# [---] R sprt0_scb_sprt => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT0_SCB_CLOCK_ENABLE_SYSTEMPORT
# [---]   sprt0_tx_scb_sprt => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT0_TX_SCB_CLOCK_ENABLE_SYSTEMPORT
# [---]   sprt0_tx_sys_sprt => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT0_TX_SYS_CLOCK_ENABLE_SYSTEMPORT
# [---] R sprt1_gisb_sprt => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT1_GISB_CLOCK_ENABLE_SYSTEMPORT
# [---]   sprt1_gmii_tx_sprt => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT1_GMII_TX_CLOCK_ENABLE_SYSTEMPORT
# [---]   sprt1_rx_scb_sprt => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT1_RX_SCB_CLOCK_ENABLE_SYSTEMPORT
# [---]   sprt1_rx_sys_sprt => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT1_RX_SYS_CLOCK_ENABLE_SYSTEMPORT
# [---] R sprt1_scb_sprt => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT1_SCB_CLOCK_ENABLE_SYSTEMPORT
# [---]   sprt1_tx_scb_sprt => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT1_TX_SCB_CLOCK_ENABLE_SYSTEMPORT
# [---]   sprt1_tx_sys_sprt => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_ENABLE_SYSTEMPORT
#                                    /SYSTEMPORT1_TX_SYS_CLOCK_ENABLE_SYSTEMPORT
# [---]   sw_25 => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_DISABLE
#                                    /DISABLE_SW_25_CLOCK
# [---]   sw_cpu => sw_cpu
# [---]   sw_cpu_sysif => sw_cpu_sysif
# [---]   sw_hif => sw_hif
# [---]   sw_memsys0 => sw_memsys0
# [---]   sw_memsys1 => sw_memsys1
# [---]   sw_mpi => sw_mpi
# [---]   sw_pcie0 => sw_pcie0
# [---]   sw_pcie1 => sw_pcie1
# [---]   sw_sata3 => sw_sata3
# [---]   sw_sdio => sw_sdio
# [---]   sw_switch => sw_switch
# [---]   sw_sysport0 => sw_sysport0
# [---]   sw_sysport0wol => sw_sysport0wol
# [---]   sw_sysport1 => sw_sysport1
# [---]   sw_sysport1wol => sw_sysport1wol
# [---]   sw_usb20 => sw_usb20
# [---]   sw_usb30 => sw_usb30
# [---] R swi_54 => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_ENABLE
#                                    /SWITCH_54_CLOCK_ENABLE
# [---] R swi_gisb => CLKGEN_SWITCH_TOP_WRAPPER_INST_CLOCK_ENABLE
#                                    /SWITCH_GISB_CLOCK_ENABLE
# [---]   sys0_dis_ch3 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3
#                                    /CLOCK_DIS_CH3
# [---]   sys0_mdiv_ch1 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_1
#                                    /MDIV_CH1
# [---]   sys0_mdiv_ch3 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3
#                                    /MDIV_CH3
# [---]   sys0_pdh_ch3 => CLKGEN_PLL_SYS0_PLL_CHANNEL_CTRL_CH_3
#                                    /POST_DIVIDER_HOLD_CH3
# [---]   sys_sysctrl_uart_0 aka sw_uart0 => CLKGEN_SYS_CTRL_INST_CLOCK_DISABLE
#                                    /DISABLE_SYSCTRL_UART_0_CLOCK
# [---]   sys_sysctrl_uart_1 aka sw_uart1 => CLKGEN_SYS_CTRL_INST_CLOCK_DISABLE
#                                    /DISABLE_SYSCTRL_UART_1_CLOCK
# [---]   sys_sysctrl_uart_2 aka sw_uart2 => CLKGEN_SYS_CTRL_INST_CLOCK_DISABLE
#                                    /DISABLE_SYSCTRL_UART_2_CLOCK
# [---]   uart_sysctrl_uart_0 => CLKGEN_UART_0_CLOCK_MUX_SELECT
#                                    /SYSCTRL_UART_0_CLOCK
# [---]   uart_sysctrl_uart_1 => CLKGEN_UART_1_CLOCK_MUX_SELECT
#                                    /SYSCTRL_UART_1_CLOCK
# [---]   uart_sysctrl_uart_2 => CLKGEN_UART_2_CLOCK_MUX_SELECT
#                                    /SYSCTRL_UART_2_CLOCK
# [---] R usb0_108_ahb => CLKGEN_USB0_TOP_INST_CLOCK_ENABLE_AHB
#                                    /USB0_108_CLOCK_ENABLE_AHB
# [---] R usb0_108_axi => CLKGEN_USB0_TOP_INST_CLOCK_ENABLE_AXI
#                                    /USB0_108_CLOCK_ENABLE_AXI
# [---] R usb0_54 => CLKGEN_USB0_TOP_INST_CLOCK_ENABLE
#                                    /USB0_54_CLOCK_ENABLE
# [---] R usb0_gisb => CLKGEN_USB0_TOP_INST_CLOCK_ENABLE
#                                    /USB0_GISB_CLOCK_ENABLE
# [---] R usb0_scb_1st => CLKGEN_USB0_TOP_INST_CLOCK_ENABLE
#                                    /USB0_SCB_1ST_CLOCK_ENABLE
# [---] R usb0_scb_2nd => CLKGEN_USB0_TOP_INST_CLOCK_ENABLE
#                                    /USB0_SCB_2ND_CLOCK_ENABLE
# [---] R usb0_scb_3rd => CLKGEN_USB0_TOP_INST_CLOCK_ENABLE
#                                    /USB0_SCB_3RD_CLOCK_ENABLE
