module QP_Lab8 (CLOCK_50, KEY, GPIO);
  input [3:0] KEY;
  input CLOCK_50;
  output [3:1] GPIO;
  
	Lab8HWConfig u0 (
		.clk_clk        (CLOCK_50),
		.pushbutton_port_export (KEY),
		.spi_port_MISO          (null),
		.spi_port_MOSI          (GPIO[1]),
		.spi_port_SCLK          (GPIO[2]),
		.spi_port_SS_n          (GPIO[3])
	);

endmodule