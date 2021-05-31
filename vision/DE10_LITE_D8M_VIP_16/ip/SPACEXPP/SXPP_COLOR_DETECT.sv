module SXPP_COLOR_DETECT(
    clk,
    reset_n,

    r, g, b,
    x_in, y_in,

    is_red, is_yellow, is_green, is_blue, is_pink,
    x_out, y_out
);

input logic clk, reset_n;
input logic [7:0] r, g, b;
input logic [10:0] x_in, y_in;

output logic is_red, is_yellow, is_green, is_blue, is_pink;
output logic [10:0] x_out, y_out;

// color calculations
logic [10:0] pr, pg, pb;
logic [10:0] ppr, ppg, ppb;
logic [31:0] distance_r,distance_y,distance_g,distance_b,distance_p;

// x and y propagation
logic [10:0] x1, y1, x2, y2;

// store colours #one delay
always_ff @(posedge clk) begin
    pr <= r;
    pg <= g;
    pb <= b;
    
    x1 <= x_in;
    y1 <= y_in;
end

// do distance calculation #one delay
always_ff @(posedge clk) begin
    distance_r <= (pr-255)*(pr-255)+pg*pg+pb*pb;
    distance_y <= (pr-255)*(pr-255)+(pg-255)*(pg-255)+pb*pb;
    distance_g <= pr*pr+(pg-255)*(pg-255)+pb*pb;
    distance_b <= pr*pr+pg*pg+(pb-255)*(pb-255);
    distance_p <= (pr-255)*(pr-255)+pg*pg+(pb-170)*(pb-170);
    
    x2 <= x1;
    y2 <= y1;
end

// compute is_color
always_comb begin
    is_red = (distance_r < 50000) & (pr > (pb + pg));
    is_yellow = (distance_y < 20000) & (pg > pr-50);
    is_green = (distance_g < 35000) & (pg > pb) & (pg > pr) & (pb-10 > pr);
    is_blue = (distance_b < 44000) & (pb > pg) & (pg > pr);
    is_pink = (distance_p < 35000) & (pr > pb) & (pr < (pg+pb));
    
    x_out = x2;
    y_out = y2;
end

endmodule
