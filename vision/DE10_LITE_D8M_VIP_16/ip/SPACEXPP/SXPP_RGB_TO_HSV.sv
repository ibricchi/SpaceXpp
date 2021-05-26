module SXPP_RGB_TO_HSV(
    r,g,b,

    h,s,v
);

input logic[31:0] r,g,b;

output logic[31:0] h,s,v;

logic[31:0] max, min, dc;
logic[31:0] h_diff, h_d_a, h_div, h_d_s, h_sum, h_s_a, h_mult, h_m_s;

always_comb begin
    // find max
    if(r >= g & r >= b) begin
        max = r;
    end
    else if(g >= b & g >= r) begin
        max = g;
    end
    else begin
        max = b;
    end

    // find min
    if(r <= g & r <= b) begin
        min = r;
    end
    else if(g <= b & g <= r) begin
        min = g;
    end
    else begin
        min = b;
    end

    // find delta
    dc = max - min;

    v = max;

    if(dc == 0) begin
        h = 0;
    end
    else begin
        $display("rgb: %d, %d, %d", r, g, b);
        $display("max: %d, min: %d, dc: %d", max, min, dc);

        // find difference
        if(max == r) begin
            h_diff = g - b;
        end
        else if (max == g) begin
            h_diff = b - r;
        end
        else begin
            h_diff = r - g;
        end
        $display("diff: pos: %d, neg: %d", h_diff, -h_diff);

        // remove sign
        if(h_diff[31]) begin
            h_d_a = -h_diff;
        end
        else begin
            h_d_a = h_diff;
        end
        $display("da: pos: %d, neg: %d", h_d_a, -h_d_a);

        // divide
        h_div = (h_d_a<<8)/dc;

        $display("div: pos: %d, neg: %d", h_div, -h_div);

        // add sign
        if(h_diff[31]) begin
            h_d_s = -h_div;
        end
        else begin
            h_d_s = h_div;
        end
        $display("ds: pos: %d, neg: %d", h_d_s, -h_d_s);

        // add offset
        if(max == r) begin
            h_sum = h_d_s;
        end
        else if (max == g) begin
            h_sum = h_d_s + (2<<8);
        end
        else begin
            h_sum = h_d_s + (4<<8);
        end
        $display("sum: pos: %d, neg: %d", h_sum, -h_sum);

        // remove sign
        if(h_sum[31]) begin
            h_s_a = -h_sum;
        end
        else begin
            h_s_a = h_sum;
        end
        $display("sa: pos: %d, neg: %d", h_s_a, -h_s_a);

        // multiply
        h_mult = (60 * h_s_a)>>8;
        $display("mult: pos: %d, neg: %d", h_mult, -h_mult);

        // add sign
        if(h_sum[31]) begin
            h_m_s = -h_mult;
        end
        else begin
            h_m_s = h_mult;
        end
        $display("ma: pos: %d, neg: %d", h_m_s, -h_m_s);

        if($signed(h_m_s) < 0) begin
            h = h_m_s + 360;
        end
        else begin
            h = h_m_s;
        end
        $display("h: pos: %d, neg: %d",h,-h);
        $display("--------------------------");
    end

    s = (max == 0) ? 0 : 100*dc/max;
    
    v = max*100>>8+1;
end

endmodule

