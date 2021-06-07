//module EEE_IMG_COMPRESSOR(
//    clk,
//    reset_n,
//    
//    x_in, y_in,
//    r, g, b,
//    
//    read,
//    store,
//    
//    vald,
//    message
//);
//
//input logic clk, reset_n;
//
//input logic[10:0] x_in, y_in;
//input logic read, save;
//
//output logic valid;
//output logic[31:0] message;
//
//parameter IMG_W = 640, IMG_H = 480;
//parameter CR = 2;
//parameter BUFFER_W = IMG_W>>CR, BUFFER_H = IMG_H>>CR;
//
//// internal state
//logic keep_saving;
//
//logic [10:0] last_sent_x, last_sent_y;
//logic [23:0] buffer [BUFFER_W-1:0][BUFFER_H-1:0];
//
//initial begin
//    keep_saving <= 1;
//end
//
//always_ff @(posedge clk) begin
//    if(keep_saving) begin
//        if(x[CR:0] == 0 & y[CR:0] == 0) begin
//            buffer[x>>CR][y>>CR] <= {r, g, b};
//        end
//    end
//    else begin
//        if(last_sent_x < BUFFER_W & last_sent_y < BUFFER_H) begin
//            
//        end
//    end
//end
//
//endmodule
