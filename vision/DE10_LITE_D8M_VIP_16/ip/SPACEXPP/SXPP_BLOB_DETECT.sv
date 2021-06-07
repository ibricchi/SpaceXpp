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
    pixel_count,
    
    minx, maxx, miny, maxy
);

// parameters
parameter X_POS=0,
          Y_POS=0,
          MINX_INIT = 0,
          MAXX_INIT = 100,
          MINY_INIT = 0,
          MAXY_INIT = 100,
          min_dist = 128;

// inputs
input logic clk;
input logic reset_n;

input logic reset;

input logic[10:0] x_in, y_in;
input logic is_valid_color;

// outputs
output logic valid;

output logic[31:0] rad;
output logic [31:0] pixel_count;

output logic[10:0] minx, maxx, miny, maxy;

// blob state
logic just_reset;

// process blob information
// box center
logic[10:0] cx, cy;
// box size
logic[10:0] bw, bh, br;
// min-max of new point compared to box range
logic[10:0] new_minx, new_maxx, new_miny, new_maxy;
always_comb begin
    cx = (minx + maxx) >> 1; // center x
    cy = (miny + maxy) >> 1; // center y
end
always_comb begin
    bw = maxx - minx; // box height
    bh = maxy - miny; // box width
    br = (bw+bh)>>2; // average box radius
end

// check current pixels distance
logic[31:0] distance;
logic is_valid_pos;
logic is_valid_init;
logic ivc1, ivc2;
logic[10:0] x1,y1,x2,y2;


always_ff @(posedge clk) begin
    if(!reset_n | reset) begin
        distance <= 0;
    
        ivc1 <= 0;
        x1 <= 0;
        y1 <= 0;
    end
    else begin
        distance <= (cx-x_in)*(cx-x_in) + (cy-y_in)*(cy-y_in) - bw*bh;
        
        ivc1 <= is_valid_color;
        x1 <= x_in;
        y1 <= y_in;
    end
end

always_ff @(posedge clk) begin
    if(!reset_n | reset) begin
        is_valid_pos <= 0;
        is_valid_init <= 0;
        new_minx <= 0;
        new_maxx <= 0;
        new_miny <= 0;
        new_maxy <= 0;
        ivc2 <= 0;
        x2 <= 0;
        y2 <= 0;
    end
    else begin
        is_valid_pos <=  (distance < min_dist | distance[31]) &
                         (10 < x1) & (MINX_INIT - 10 < x1) &
                         (10 < y1) & (MINY_INIT - 10 < y1) &
                         !just_reset;
                         
        is_valid_init <= MINX_INIT <= x1 & x1 <= MAXX_INIT &
                         MINY_INIT <= y_in & y_in <= MAXY_INIT &
                         (10 < x1) & (MINX_INIT - 10 < x1) &
                         (10 < y1) & (MINY_INIT - 10 < y1) &
                         just_reset;
                         
        new_minx <= (minx < x1)?minx:x1;
        new_maxx <= (maxx < x1)?x1:maxx;
        new_miny <= (miny < y1)?miny:y1;
        new_maxy <= (maxy < y1)?y1:maxy;
        
        ivc2 <= ivc1;
        x2 <= x1;
        y2 <= y1;
    end
end

// adjust blob at each clock edge with new data
always_ff @(posedge clk) begin
    if(!reset_n | reset) begin
        valid <= 0;
        just_reset <= 1;
        pixel_count <= 0;

        minx <= 0;
        maxx <= 0;
        miny <= 0;
        maxy <= 0;
        rad <= 0;
    end
    else if(ivc2) begin
        if (is_valid_init) begin
            valid <= 1;
            just_reset <= 0;
            pixel_count <= 1;
            minx <= x2;
            maxx <= x2;
            miny <= y2;
            maxy <= y2;
            rad <= br;
        end
        else if(is_valid_pos) begin
            valid <= 1;
            pixel_count <= pixel_count + 1;
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
