module SXPP_BLOB_DETECT(
	// global clock & reset
	clk,
	reset_n,
	
	// setup
	reset,
	mode_in,
	
	// input pixel data
	x_in, y_in,
	r, g, b,

	// output data
	valid,
	x_out, y_out, r_out
);

// inputs
input clk;
input reset_n;

input reset;
input mode_in;

input [10:0] x_in, y_in;
input [7:0] r, g, b;

// outputs
output valid;
output [10:0] x_out, y_out, r_out;

// constants
integer min_dist = 2;

// local variables
wire is_valid_color;
wire is_valid_pos;
wire dist;

reg validp;
reg just_reset;
reg [2:0] mode;
reg [10:0] xp, yp, rp;

// link outputs to local vairables
assign valid = validp;
assign x_out = xp;
assign y_out = yp;
assign r_out = rp;

// check if current pixel lies in valid color range
assign is_valid_color = (mode == 1)? ~r[7] & ~g[7] & b[7] : // if red mode
                        0;                                  // otherwise

// check current pixels distance
assign dist = (xp-x_in)*(xp-x_in) + (yp-y_in)*(yp-y_in) - rp*rp;
assign is_valid_pos = dist < min_dist;

// adjust blob at each clock edge with new data
always @(posedge clk) begin
	 if(!reset_n | reset) begin
        just_reset <= 1;
        mode <= mode_in;
		  xp <= 0;
		  yp <= 0;
		  rp <= 0;
		  validp <= 0;
	 end
	 else if(is_valid_color) begin
        if (just_reset) begin
            xp <= x_in;
            yp <= y_in;
            rp <= 1;
            just_reset <= 0;
				validp <= 1;
        end
		  else if(is_valid_pos) begin
            if(dist > 0) begin
                rp <= dist;
            end
    			validp <= 0;
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
