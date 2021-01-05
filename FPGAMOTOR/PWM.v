module PWM (clk,wen,rst,in,out);
	
	input wire clk;
	input wire rst;
	input wire wen;
	input wire [15:0] in;
	output reg out;
 
 
	reg [16:0] periodCounter;
	reg [1:0] clkCount;
	reg [15:0] PWMPulseWidthCount;
	
	
	
	always @(posedge clk)
	begin 
		if (rst==0)
			begin 
				periodCounter <= 17'd0;
				PWMPulseWidthCount <= 16'd0;
				out <= 1'b0;
				clkCount <= 2'b00;
			end	
		else	
		if (wen==1'b1)
		
			PWMPulseWidthCount <= in;
			//PWMPulseWidthCount
		
		else 
		if (clkCount == 2'b10)
             begin
				clkCount <= 0;	
				if 	(periodCounter == 17'd79999)
				begin
					out <= 1'b1;
					periodCounter <= 17'd0;
				end
				else
				begin
					periodCounter <= periodCounter + 1'b1;
					if ((periodCounter[15:0] == PWMPulseWidthCount) && (periodCounter[16]==1'b0))
					begin
						out <= 1'b0;
					end	
				end
			end	
		else
        begin		
		    clkCount <= clkCount + 2'b01;
		end
		
	end 

endmodule