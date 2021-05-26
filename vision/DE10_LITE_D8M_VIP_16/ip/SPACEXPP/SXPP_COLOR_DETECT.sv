module SXPP_COLOR_DETECT(
	input clk,
	input reset_n,
	
	input[7:0] r,
    input[7:0] g,
    input[7:0] b,
	
	output is_red,
    output is_blue,
    output is_green
);

parameter max_dist = 40000;

logic [31:0] red_dist, green_dist, blue_dist;

always_comb begin
    red_dist =   (r - 255)*(r-255) + g*g + b*b;
    green_dist = (g - 255)*(g-255) + b*b + r*r;
    blue_dist =  (b - 255)*(b-255) + r*r + g*g;
    
    is_red   = red_dist < max_dist;
    is_green = green_dist < max_dist;
    is_blue  = blue_dist < max_dist;
end

endmodule
