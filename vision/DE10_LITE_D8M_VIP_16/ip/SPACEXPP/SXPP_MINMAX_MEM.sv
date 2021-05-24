module SXPP_MINMAX_MEM(
    // global clock & reset
    clk,
    reset_n,

    en,
    rad_in,
    minx_in,
    maxx_in,
    miny_in,
    maxy_in,

    x_out, y_out,
    rad_out,
    minx_out,
    maxx_out,
    miny_out,
    maxy_out
);

parameter DATA_SIZE=8, X_SIZE=1, Y_SIZE=1;

input clk;
input reset_n;

input en;
input [DATA_SIZE-1:0][X_SIZE-1:0][Y_SIZE-1:0] rad_in;
input [DATA_SIZE-1:0][X_SIZE-1:0][Y_SIZE-1:0] minx_in;
input [DATA_SIZE-1:0][X_SIZE-1:0][Y_SIZE-1:0] maxx_in;
input [DATA_SIZE-1:0][X_SIZE-1:0][Y_SIZE-1:0] miny_in;
input [DATA_SIZE-1:0][X_SIZE-1:0][Y_SIZE-1:0] maxy_in;

output [DATA_SIZE-1:0] x_out, y_out;
output [DATA_SIZE-1:0] rad_out;
output [DATA_SIZE-1:0] minx_out;
output [DATA_SIZE-1:0] maxx_out;
output [DATA_SIZE-1:0] miny_out;
output [DATA_SIZE-1:0] maxy_out;

reg [DATA_SIZE-1:0] x, y;
reg [DATA_SIZE-1:0] rad;
reg [DATA_SIZE-1:0] minx;
reg [DATA_SIZE-1:0] maxx;
reg [DATA_SIZE-1:0] miny;
reg [DATA_SIZE-1:0] maxy;

assign x_out = x;
assign y_out = y;
assign rad_out = rad;
assign minx_out = minx;
assign maxx_out = maxx;
assign miny_out = miny;
assign maxy_out = maxy;

always @(posedge clk) begin
    if(reset_n) begin
        x <= 0;
        y <= 0;
        rad <= 0;
        minx <= 0;
        maxx <= 0;
        miny <= 0;
        maxy <= 0;
    end
end

integer i, j;
always @(posedge clk) begin
    if(en) begin
        x <= 0;
        y <= 0;
        rad <= 0;
        minx <= 0;
        maxx <= 0;
        miny <= 0;
        maxx <= 0;
        for(i = 0; i < X_SIZE; i = i + 1) begin
            for(j = 0; j < Y_SIZE; j = j + 1) begin
                if(rad < rad_in[i][j]) begin
                    x <= i; y <= j;
                    rad <= rad_in[i][j];
                    minx <= minx_in[i][j];
                    maxx <= maxx_in[i][j];
                    miny <= miny_in[i][j];
                    maxy <= maxy_in[i][j];
                end
            end
        end
    end
end

endmodule
