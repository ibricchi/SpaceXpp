module SXPP_COLOR_DETECT(
	clk,
	reset_n,
	
	r,g, b,
	
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

SXPP_RGB_TO_HSV(r,g,b,h,s,v);

always_comb begin
    is_red = 0;
    is_yellow = 0;
    is_green = 0;
    is_blue = 0;
    is_pink = 0;
    if(s+v>85) begin
        if(330 <= h | h <= 35) begin
            is_red = 1;
        end
        if(30 <= h & h <= 70) begin
            is_yellow = 1;
        end
        if(65 <= h & h <= 160) begin
            is_green = 1;
        end
        if(200 <= h & h <= 260) begin
            is_blue = 1;
        end
        if(250 <= h & h <= 350) begin
            is_pink = 1;
        end
    end
end

endmodule
