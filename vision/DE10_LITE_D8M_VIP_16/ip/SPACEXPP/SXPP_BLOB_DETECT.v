module SXPP_BLOB_DETECT
//#(
//	parameter initalize_minx = 0, initialize_maxx = 100,
//	parameter initalize_miny = 0, initialize_maxy = 100,
//)
(
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
	
	minx_out, maxx_out, miny_out, maxy_out
);

// inputs
input clk;
input reset_n;

input reset;

input [10:0] x_in, y_in;
input is_valid_color;

// outputs
output valid;

output [10:0] minx_out, maxx_out, miny_out, maxy_out;
	
// constants
integer min_dist = 2048;

// blob state
reg validp;
reg just_reset;
reg [10:0] minx, miny, maxx, maxy;

// link outputs to local vairables
assign valid = validp;
assign minx_out = minx;
assign maxx_out = maxx;
assign miny_out = miny;
assign maxy_out = maxy;

// process blob information
// box center
wire [10:0] cx, cy;
assign cx = (minx + maxx) >> 1; // center x
assign cy = (miny + maxy) >> 1; // center y
// box size
wire [10:0] bw, bh, br;
assign bw = maxx - minx; // box height
assign bh = maxy - miny; // box width
assign br = (bw + bh) >> 2; // average box radius
// min-max of new point compared to box range
wire [10:0] new_minx, new_maxx, new_miny, new_maxy;
assign new_minx = (minx < x_in)?minx:x_in;
assign new_maxx = (maxx < x_in)?x_in:maxx;
assign new_miny = (miny < y_in)?miny:y_in;
assign new_maxy = (maxy < y_in)?y_in:maxy;

// check current pixels distance
wire [31:0] dist;
wire is_valid_pos;
assign dist = (cx-x_in)*(cx-x_in) + (cy-y_in)*(cy-y_in) - br*br;
assign is_valid_pos = dist < min_dist | dist[31];

// adjust blob at each clock edge with new data
always @(posedge clk) begin
	 if(!reset_n | reset) begin
		  validp <= 0;
        just_reset <= 1;
		  minx <= 0;
		  maxx <= 0;
		  miny <= 0;
		  maxy <= 0;
	 end
	 else if(is_valid_color) begin
        if (just_reset) begin
				validp <= 1;
            just_reset <= 0;
            minx <= x_in;
            maxx <= x_in;
            miny <= y_in;
            maxy <= y_in;
        end
		  else if(is_valid_pos) begin
    			validp <= 1;
            minx <= new_minx;
            maxx <= new_maxx;
            miny <= new_miny;
            maxy <= new_maxy;
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
