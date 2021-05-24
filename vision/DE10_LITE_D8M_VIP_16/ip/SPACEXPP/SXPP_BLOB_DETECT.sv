module SXPP_BLOB_DETECT (
	// global clock & reset
	clk,
	reset_n,
	
	// setup
	reset,
	
	// input pixel data
	x_in, y_in,
	is_valid_color,

	// output data
	valid,
	
	rad_out,
	minx_out, maxx_out, miny_out, maxy_out
);

// parameters
parameter MINX_INIT = 0,
          MAXX_INIT = 100,
          MINY_INIT = 0,
		  MAXY_INIT = 100;

// inputs
input clk;
input reset_n;

input reset;

input [10:0] x_in, y_in;
input is_valid_color;

// outputs
output valid;

output [10:0] minx_out, maxx_out, miny_out, maxy_out;
output [10:0] rad_out;
	
// constants
integer min_dist = 1024;

// blob state
reg validp;
reg just_reset;
reg [10:0] minx, miny, maxx, maxy;
reg [10:0] box_rad;

// link outputs to local vairables
assign valid = validp;
assign minx_out = minx;
assign maxx_out = maxx;
assign miny_out = miny;
assign maxy_out = maxy;
assign rad_out = box_rad;

// process blob information
// box center
wire [10:0] cx, cy;
assign cx = (minx + maxx) >> 1; // center x
assign cy = (miny + maxy) >> 1; // center y
// box size
wire [10:0] bw, bh, br;
assign bw = maxx - minx; // box height
assign bh = maxy - miny; // box width
assign br = (bw + bh) >> 2; // average box distance
// min-max of new point compared to box range
wire [10:0] new_minx, new_maxx, new_miny, new_maxy;
assign new_minx = (minx < x_in)?minx:x_in;
assign new_maxx = (maxx < x_in)?x_in:maxx;
assign new_miny = (miny < y_in)?miny:y_in;
assign new_maxy = (maxy < y_in)?y_in:maxy;

// check current pixels distance
wire [31:0] distance;
wire is_valid_pos;
wire is_valid_init;
assign distance = (cx-x_in)*(cx-x_in) + (cy-y_in)*(cy-y_in) - br * br;
assign is_valid_pos = (distance < min_dist | distance[31]) & !just_reset;
assign is_valid_init = MINX_INIT <= x_in & x_in <= MAXX_INIT &
                       MINY_INIT <= y_in & y_in <= MAXY_INIT &
                       just_reset;
							  
// adjust blob at each clock edge with new data
always @(posedge clk) begin
	 if(!reset_n | reset) begin
		  validp <= 0;
        just_reset <= 1;
		  minx <= 0;
		  maxx <= 0;
		  miny <= 0;
		  maxy <= 0;
		  box_rad <= 0;
	 end
	 else if(is_valid_color) begin
        if (is_valid_init) begin
				validp <= 1;
            just_reset <= 0;
            minx <= x_in;
            maxx <= x_in;
            miny <= y_in;
            maxy <= y_in;
				box_rad <= br;
        end
		  else if(is_valid_pos) begin
    			validp <= 1;
            minx <= new_minx;
            maxx <= new_maxx;
            miny <= new_miny;
            maxy <= new_maxy;
				box_rad <= br;
        end
		  else begin
				validp <= 0;
		  end
    end
	 else begin
		  validp <= 0;
    end
end

endmodule
