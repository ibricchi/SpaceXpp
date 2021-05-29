module SXPP_COLOR_DETECT_2(
	clk,
	reset_n,
	
	r,g,b,
	
	is_red,
	is_yellow,
    is_green,
    is_blue,
    is_pink
);

input logic clk, reset_n;
input logic [7:0] r, g, b;

output logic is_red, is_yellow, is_green, is_blue, is_pink;

logic[8:0] h;
logic[6:0] s,v;
logic[7:0] sv;

SXPP_RGB_TO_HSV_2(r,g,b,h,s,v);

always_comb begin
    sv = s + v;

    is_red = 0;
    is_yellow = 0;
    is_green = 0;
    is_blue = 0;
    is_pink = 0;

    if(90 <= h & h <= 155 & sv > 75 & s > 60) begin // red
        is_red = 1;
    end
    if(30 <= h & h <= 90 & sv > 95) begin // yellow
        is_yellow = 1;
    end
    if(130<= h & h <= 160 & sv > 60) begin // green
        is_green = 1;
    end
    if(170 <= h & h <= 260 & sv > 60) begin // blue
        is_blue = 1;
    end
    if(250 <= h | h <= 35 & 60 < sv & sv < 90) begin // pink
        is_pink = 1;
    end
end

endmodule
