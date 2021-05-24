module SXPP_COLOR_DETECT(
	clk,
	reset_n,
	
	r, g, b,
	
	is_red, is_blue, is_green
);

input clk, reset_n;
input [7:0] r, g, b;
output is_red, is_blue, is_green;

integer max_dist = 40000;

wire [31:0] red_dist, green_dist, blue_dist;
assign red_dist =   (r - 255)*(r-255) + g*g + b*b;
assign green_dist = (g - 255)*(g-255) + b*b + r*r;
assign blue_dist =  (b - 255)*(b-255) + r*r + g*g;

assign is_red   = red_dist < max_dist;
assign is_green = green_dist < max_dist;
assign is_blue  = blue_dist < max_dist;

endmodule
