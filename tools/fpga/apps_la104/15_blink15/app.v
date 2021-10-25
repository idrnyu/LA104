module app (
    input wire clk,
    output wire miso
);

reg [24:0] counter;
assign miso = counter[15];

always @(posedge clk) begin
    counter <= counter + 1;
end

endmodule