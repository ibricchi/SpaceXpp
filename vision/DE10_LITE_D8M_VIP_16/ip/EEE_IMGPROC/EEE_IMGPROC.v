module EEE_IMGPROC(
    // global clock & reset
    clk,
    reset_n,
    
    // mm slave
    s_chipselect,
    s_read,
    s_write,
    s_readdata,
    s_writedata,
    s_address,

    // stream sink
    sink_data,
    sink_valid,
    sink_ready,
    sink_sop,
    sink_eop,
    
    // streaming source
    source_data,
    source_valid,
    source_ready,
    source_sop,
    source_eop,
    
    // conduit
    sw,
    led
);


// global clock & reset
input    clk;
input    reset_n;

// mm slave
input                            s_chipselect;
input                            s_read;
input                            s_write;
output    reg    [31:0]          s_readdata;
input      [31:0]                s_writedata;
input      [2:0]                 s_address;


// streaming sink
input      [23:0]                sink_data;
input                            sink_valid;
output                           sink_ready;
input                            sink_sop;
input                            sink_eop;

// streaming source
output     [23:0]                source_data;
output                           source_valid;
input                            source_ready;
output                           source_sop;
output                           source_eop;

// conduit export
input      [9:0]                 sw;
output     [9:0]                 led;

wire mode;
assign mode = sw[0];

////////////////////////////////////////////////////////////////////////
parameter IMAGE_W = 11'd640;
parameter IMAGE_H = 11'd480;
parameter MESSAGE_BUF_MAX = 256;
parameter MSG_INTERVAL = 6;
parameter BB_COL_DEFAULT = 24'h00ff00;


wire [7:0]   red, green, blue, grey;
wire [7:0]   red_out, green_out, blue_out;

wire         sop, eop, in_valid, out_ready;
////////////////////////////////////////////////////////////////////////

// setup blob detector params
parameter gx_min = 100, gy_min = 40, gx_max = 540, gy_max = 440;
parameter grids_x = 3;
parameter grids_y = 3;
parameter grid_w = (gx_max-gx_min)/grids_x, grid_h = (gy_max-gy_min)/grids_y;
parameter color_count = 6;

// setup color detector vairables
wire [10:0] cd_x, cd_y;

wire [color_count-1:0] bd_color_high;
SXPP_COLOR_DETECT cd1 (
    clk,
    reset_n,
    
    red, green, blue,
    
    x, y,
    
    bd_color_high[0], //red
    bd_color_high[1], //yellow
    bd_color_high[2], //green
    bd_color_high[3], //blue
    bd_color_high[4], //pink
    bd_color_high[5], //unkown
    
    cd_x, cd_y
);

// blob detector variables global
wire bds_valid[color_count-1:0][grids_x-1:0][grids_y-1:0];

wire [10:0] bds_rad[color_count-1:0][grids_x-1:0][grids_y-1:0];
wire [10:0] bds_pix[color_count-1:0][grids_x-1:0][grids_y-1:0];
wire [10:0] bds_x_min[color_count-1:0][grids_x-1:0][grids_y-1:0],
            bds_x_max[color_count-1:0][grids_x-1:0][grids_y-1:0],
            bds_y_min[color_count-1:0][grids_x-1:0][grids_y-1:0],
            bds_y_max[color_count-1:0][grids_x-1:0][grids_y-1:0];
                
// blob detector variables local
wire bd_reset;
wire bd_valid;

wire [10:0] bd_rad, obs_rad;
reg [10:0] bd_pix, obs_pix;
reg [10:0] bd_which_c, bd_which_x, bd_which_y, obs_which_x, obs_which_y;
wire [10:0] x_min, y_min, x_max, y_max;

assign bd_reset = sop;

genvar c, i, j;
generate
    for(c = 0; c < color_count; c = c + 1) begin : generate_color_loop
        for(i = 0; i < grids_x; i = i + 1) begin : generate_x_loop
            for(j = 0; j < grids_y; j = j + 1) begin : generate_y_loop
                SXPP_BLOB_DETECT
                    #(
                        i, j,
                        gx_min + i * grid_w,
                        gx_min + (i + 1) * grid_w,
                        gy_min + j * grid_h,
                        gy_min + (j + 1) * grid_h
                    )
                    bd(
                        .clk(clk),
                        .reset_n(reset_n),

                        .reset(bd_reset),

                        .x_in(cd_x), .y_in(cd_y),
                        .is_valid_color(bd_color_high[c] & in_valid),

                        .valid(bds_valid[c][i][j]),

                        .rad(bds_rad[c][i][j]),
                        .pixel_count(bds_pix[c][i][j]),
                        
                        .minx(bds_x_min[c][i][j]),
                        .maxx(bds_x_max[c][i][j]),
                        .miny(bds_y_min[c][i][j]),
                        .maxy(bds_y_max[c][i][j])
                );
            end
        end
    end
endgenerate

always @(posedge clk) begin
    if(sop) begin
        bd_which_c <= -1;
        bd_which_x <= -1;
        bd_which_y <= -1;
        obs_which_x <= -1;
        obs_which_y <= -1;
        bd_pix <= 0;
        obs_pix <= 0;
    end
    else if (in_valid) begin
        integer d, k, l;
        for(d = 0; d < color_count-1; d = d + 1) begin
            for(k = 0; k < grids_x; k = k + 1) begin
                for(l = 0; l < grids_y; l = l + 1) begin
                    if(bds_pix[d][k][l] > 50 & bd_pix < bds_pix[d][k][l]) begin
                        bd_which_c <= d;
                        bd_which_x <= k;
                        bd_which_y <= l;
                        bd_pix <= bds_pix[d][k][l];
                    end
                end
            end
        end
        for(k = 0; k < grids_x; k = k + 1) begin
            for(l = 0; l < grids_y; l = l + 1) begin
                if(bds_pix[color_count-1][k][l] > 50 & bd_pix < bds_pix[color_count-1][k][l]) begin
                    obs_which_x <= k;
                    obs_which_y <= l;
                    obs_pix <= bds_pix[color_count-1][k][l];
                end
            end
        end
    end
end

assign bd_valid = (sw[9]?(bds_valid[which_c][which_x][which_y]&(which_c[10]==0)):bd_color_high[sw[4:1]]);
assign bd_rad = bds_rad[which_c][which_x][which_y];
assign obs_rad = bds_rad[color_count-1][obs_which_x][obs_which_y];
assign x_min = bds_x_min[which_c][which_x][which_y];
assign x_max = bds_x_max[which_c][which_x][which_y];
assign y_min = bds_y_min[which_c][which_x][which_y];
assign y_max = bds_y_max[which_c][which_x][which_y];

// display sensor grids
reg grid_active;
integer m, n;
always @(*) begin
    grid_active <= 0;
    if(gx_min <= x & x <= gx_max & gy_min <= y & y <= gy_max) begin
        for(m = 0; m < grids_x+1; m = m + 1) begin
            if(x == gx_min + m * grid_w) begin
                grid_active <= 1;
            end
        end
        for(n = 0; n < grids_y+1; n = n + 1) begin
            if(y == gy_min + n * grid_h | y == IMAGE_H - 1) begin
                grid_active <= 1;
            end
        end
    end
end

wire [23:0] highlight_color[color_count-1:0];
assign highlight_color[0] = 24'hff0000;
assign highlight_color[1] = 24'hffff00;
assign highlight_color[2] = 24'h00ff00;
assign highlight_color[3] = 24'h0000ff;
assign highlight_color[4] = 24'hff00ff;
assign highlight_color[5] = 24'h000000;

wire [23:0] blob_high;
wire [23:0] grid_high;
assign grey = green[7:1] + red[7:2] + blue[7:2]; //Grey = green/2 + red/4 + blue/4
assign blob_high = bd_valid ? highlight_color[bd_which_c] : {grey, grey, grey};
assign grid_high = grid_active ? {8'hff, 8'hff, 8'h0} : blob_high;

wire [23:0] bb_color;
assign bb_color = 24'hffffff;

// Show bounding box
wire [23:0] new_image;
wire bb_active;
assign bb_active = (x == left) | (x == right) | (y == top) | (y == bottom);
assign new_image = bb_active ? bb_color : grid_high; // used to be bb_col

// Switch output pixels depending on mode switch
// Don't modify the start-of-packet word - it's a packet discriptor
// Don't modify data in non-video packets
assign {red_out, green_out, blue_out} = (mode & ~sop & packet_video) ? new_image : {red,green,blue};

//Count valid pixels to tget the image coordinates. Reset and detect packet type on Start of Packet.
reg [10:0] x, y;
reg packet_video;
always@(posedge clk) begin
    if (sop) begin
        x <= 11'h0;
        y <= 11'h0;
        packet_video <= (blue[3:0] == 3'h0);
    end
    else if (in_valid) begin
        if (x == IMAGE_W-1) begin
            x <= 11'h0;
            y <= y + 11'h1;
        end
        else begin
            x <= x + 11'h1;
        end
    end
end


//Process bounding box at the end of the frame.
reg [2:0] msg_state;
reg [10:0] left, right, top, bottom;
reg [10:0] which_c, which_x, which_y;
reg [10:0] obs_x, obs_y;
reg [7:0] frame_count;
always@(posedge clk) begin
    if (eop & in_valid & packet_video) begin  //Ignore non-video packets
        
        //Latch edges for display overlay on next frame
        left <= x_min;
        right <= x_max;
        top <= y_min;
        bottom <= y_max;
        
        // same as above for which c, x and y
        which_c <= bd_which_c;
        which_x <= bd_which_x;
        which_y <= bd_which_y;
        obs_x <= obs_which_x;
        obs_y <= obs_which_y;
        
        //Start message writer FSM once every MSG_INTERVAL frames, if there is room in the FIFO
        frame_count <= frame_count - 1;
        
        if (frame_count == 0 && msg_buf_size < MESSAGE_BUF_MAX - 3) begin
            msg_state <= 3'b01;
            frame_count <= MSG_INTERVAL-1;
        end
    end
    
    //Cycle through message writer states once started
    if (msg_state != 3'b00) begin
        case(msg_state)
            3'b00: msg_state <= 3'b00;
            default: msg_state <= msg_state + 3'b01;
        endcase
    end

end
    
//Generate output messages for CPU
reg [31:0] msg_buf_in; 
wire [31:0] msg_buf_out;
reg msg_buf_wr;
wire msg_buf_rd, msg_buf_flush;
wire [7:0] msg_buf_size;
wire msg_buf_empty;

`define RED_BOX_MSG_ID "RBB"
`define RED_BOX_MSG_PTST "PRE"
`define RED_BOX_MSG_TST "END"

always@(*) begin    //Write words to FIFO as state machine advances
    case(msg_state)
        3'b000: begin
            msg_buf_in = 32'b0;
            msg_buf_wr = 1'b0;
        end
        3'b001: begin
            msg_buf_in = `RED_BOX_MSG_ID;    //Message ID
            msg_buf_wr = 1'b1;
        end
        3'b010: begin
            msg_buf_in = {which_x[7:0], which_y[7:0], which_c[4:0], bd_rad};    //Top left coordinate
            msg_buf_wr = 1'b1;
        end
        3'b011: begin
            msg_buf_in = {obs_x[7:0], obs_y[7:0], 5'h5, obs_rad};    //Top left coordinate
            msg_buf_wr = 1'b1;
        end
        3'b100: begin
            msg_buf_in = {`RED_BOX_MSG_TST}; //Bottom right coordinate
            msg_buf_wr = 1'b1;
        end
    endcase
end


//Output message FIFO
MSG_FIFO    MSG_FIFO_inst (
    .clock (clk),
    .data (msg_buf_in),
    .rdreq (msg_buf_rd),
    .sclr (~reset_n | msg_buf_flush),
    .wrreq (msg_buf_wr),
    .q (msg_buf_out),
    .usedw (msg_buf_size),
    .empty (msg_buf_empty)
);


//Streaming registers to buffer video signal
STREAM_REG #(.DATA_WIDTH(26)) in_reg (
    .clk(clk),
    .rst_n(reset_n),
    .ready_out(sink_ready),
    .valid_out(in_valid),
    .data_out({red,green,blue,sop,eop}),
    .ready_in(out_ready),
    .valid_in(sink_valid),
    .data_in({sink_data,sink_sop,sink_eop})
);

STREAM_REG #(.DATA_WIDTH(26)) out_reg (
    .clk(clk),
    .rst_n(reset_n),
    .ready_out(out_ready),
    .valid_out(source_valid),
    .data_out({source_data,source_sop,source_eop}),
    .ready_in(source_ready),
    .valid_in(in_valid),
    .data_in({red_out, green_out, blue_out, sop, eop})
);


/////////////////////////////////
/// Memory-mapped port      /////
/////////////////////////////////

// Addresses
`define REG_STATUS                0
`define READ_MSG                  1
`define READ_ID                   2
`define REG_BBCOL                 3

//Status register bits
// 31:16 - unimplemented
// 15:8 - number of words in message buffer (read only)
// 7:5 - unused
// 4 - flush message buffer (write only - read as 0)
// 3:0 - unused


// Process write
reg  [7:0]   reg_status;
reg    [23:0]    bb_col;

always @ (posedge clk)
begin
    if (~reset_n)
    begin
        reg_status <= 8'b0;
        bb_col <= BB_COL_DEFAULT;
    end
    else begin
        if(s_chipselect & s_write) begin
           if      (s_address == `REG_STATUS)    reg_status <= s_writedata[7:0];
           if      (s_address == `REG_BBCOL)    bb_col <= s_writedata[23:0];
        end
    end
end


//Flush the message buffer if 1 is written to status register bit 4
assign msg_buf_flush = (s_chipselect & s_write & (s_address == `REG_STATUS) & s_writedata[4]);

// Process reads
reg read_d; //Store the read signal for correct updating of the message buffer

// Copy the requested word to the output port when there is a read.
always @ (posedge clk)
begin
   if (~reset_n) begin
       s_readdata <= {32'b0};
        read_d <= 1'b0;
    end
    
    else if (s_chipselect & s_read) begin
        if   (s_address == `REG_STATUS) s_readdata <= {16'b0,msg_buf_size,reg_status};
        if   (s_address == `READ_MSG) s_readdata <= {msg_buf_out};
        if   (s_address == `READ_ID) s_readdata <= 32'h1234EEE2;
        if   (s_address == `REG_BBCOL) s_readdata <= {8'h0, bb_col};
    end
    
    read_d <= s_read;
end

//Fetch next word from message buffer after read from READ_MSG
assign msg_buf_rd = s_chipselect & s_read & ~read_d & ~msg_buf_empty & (s_address == `READ_MSG);

endmodule
