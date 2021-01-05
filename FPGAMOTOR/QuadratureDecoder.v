
module QuadratureDecoder (clk,rst,wen,ch_A,ch_B,in, out);

	input wire clk;
	input wire rst;
	input wire wen;
	input wire ch_A;
	input wire ch_B;
	input wire [15:0] in;
    output [15:0] out;
	
	
	reg[2:0] delayedCh_A,delayedCh_B;
	
	always @(posedge clk)
	begin
		delayedCh_A <= {delayedCh_A[1:0],ch_A};
	end
	
	always @(posedge clk)
	begin
		delayedCh_B <= {delayedCh_B[1:0],ch_B};
	end
	
	
	wire count_enable = delayedCh_A[1] ^ delayedCh_A[2] ^ delayedCh_B[1] ^ delayedCh_B[2] ;
	wire count_dir = delayedCh_A[1] ^ delayedCh_B[2];
	
	
	reg [15:0] Quad;
	
	always @(posedge clk)
	begin
	    if (rst == 0) 
		begin
			Quad <= 15'd0;
	    end
		else 
		if (wen==1)
		begin
			Quad <= in; 
	    end
		else 
		if (count_enable)
		begin
			if (count_dir)
			begin
				Quad <= Quad + 1'b1;
			end
			else
			begin
				Quad <= Quad - 1'b1;
			end
		end
	end
	
	assign out = Quad;
	
endmodule







