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
    
    rad,
    minx, maxx, miny, maxy,
    
    fx, fy
);

// parameters
parameter X_POS=0,
          Y_POS=0,
          MINX_INIT = 0,
          MAXX_INIT = 100,
          MINY_INIT = 0,
          MAXY_INIT = 100,
          min_dist = 1024;

// inputs
input logic clk;
input logic reset_n;

input logic reset;

input logic[10:0] x_in, y_in;
input logic is_valid_color;

// outputs
output logic valid;

output logic[31:0] rad;
output logic[10:0] minx, maxx, miny, maxy;

output logic[10:0] fx, fy;

// blob state
logic just_reset;

// process blob information
// box center
logic[10:0] cx, cy;
always_comb begin
    cx = (minx + maxx) >> 1; // center x
    cy = (miny + maxy) >> 1; // center y
end
// box size
logic[10:0] bw, bh, br;
always_comb begin
    bw = maxx - minx; // box height
    bh = maxy - miny; // box width
    br = (bw > bh) ? (bw>>1) : (bh>>1); // average box distance
end
// min-max of new point compared to box range
logic[10:0] new_minx, new_maxx, new_miny, new_maxy;
always_comb begin
    new_minx = (minx < x_in)?minx:x_in;
    new_maxx = (maxx < x_in)?x_in:maxx;
    new_miny = (miny < y_in)?miny:y_in;
    new_maxy = (maxy < y_in)?y_in:maxy;
end

// check current pixels distance
logic[31:0] distance;
logic is_valid_pos;
logic is_valid_init;
always_comb begin
    distance = (cx-x_in)*(cx-x_in) + (cy-y_in)*(cy-y_in) - br * br;
    is_valid_pos = (distance < min_dist | distance[31]) & !just_reset & (x_in > 30) & (y_in > 30) & (x_in < 610) & (y_in < 450);
    is_valid_init = MINX_INIT <= x_in & x_in <= MAXX_INIT &
                    MINY_INIT <= y_in & y_in <= MAXY_INIT &
                    (x_in > 30) & (y_in > 30) & (x_in < 610) & (y_in < 450) &
                    just_reset;
end

// adjust blob at each clock edge with new data
always_ff @(posedge clk) begin
    if(!reset_n | reset) begin
        valid <= 0;
        just_reset <= 1;

        minx <= 0;
        maxx <= 0;
        miny <= 0;
        maxy <= 0;
        rad <= 0;
        
        fx <= 0;
        fy <= 0;
    end
    else if(is_valid_color) begin
        if (is_valid_init) begin
            valid <= 1;
            just_reset <= 0;
            minx <= x_in;
            maxx <= x_in;
            miny <= y_in;
            maxy <= y_in;
            rad <= br;
            
            fx <= x_in;
            fy <= y_in;
        end
        else if(is_valid_pos) begin
            valid <= 1;
            minx <= new_minx;
            maxx <= new_maxx;
            miny <= new_miny;
            maxy <= new_maxy;
            rad <= br;
        end
        else begin
            valid <= 0;
        end
    end
    else begin
        valid <= 0;
    end
end

endmodule
