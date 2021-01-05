module SPI(ch0_A,ch0_B,ch1_A,ch1_B,ch2_A,ch2_B,ch3_A,ch3_B,ch4_A,ch4_B,ch5_A,ch5_B,ch6_A,ch6_B,ch7_A,ch7_B,RST,CLK,SCK,SSEL,MOSI,MISO,PWM0,PWM1,PWM2,PWM3,PWM4,PWM5,PWM6,PWM7);

	input wire ch0_A;
	input wire ch0_B;
	input wire ch1_A;
	input wire ch1_B;
	input wire ch2_A;
	input wire ch2_B;
	input wire ch3_A;
	input wire ch3_B;
	input wire ch4_A;
	input wire ch4_B;
	input wire ch5_A;
	input wire ch5_B;
	input wire ch6_A;
	input wire ch6_B;
	input wire ch7_A;
	input wire ch7_B;
	
	input wire RST;
	input wire CLK;
	input wire SCK;
	input wire SSEL;
	input wire MOSI;
	
	output wire MISO;
	
	output wire PWM0;
	output wire PWM1;
	output wire PWM2;
	output wire PWM3;
	output wire PWM4;
	output wire PWM5;
	output wire PWM6;
	output wire PWM7;
	
	wire myclk;
	
	SB_GB My_Global_Buffer_i (.USER_SIGNAL_TO_GLOBAL_BUFFER (CLK), .GLOBAL_BUFFER_OUTPUT (myclk) );

    

    reg [15:0] SerialNo = 16'b1010101010111011; 
	wire [15:0] dataRead0;
	wire [15:0] dataRead1;
	wire [15:0] dataRead2;
	wire [15:0] dataRead3;
	wire [15:0] dataRead4;
	wire [15:0] dataRead5;
	wire [15:0] dataRead6;
	wire [15:0] dataRead7;
	
    reg [15:0] dataWrite;
	//reg [15:0] dataWrite= 16'b0001011101110000;
	//re

    reg [7:0] pwmWrite;
    reg [7:0]quadWrite;	//write=1, read=0
	//reg pwmWrite;
    //reg quadWrite;
	
	//read from or write to Quad reg (QuadWrite=0- read, QuadWrite=1 write )
    QuadratureDecoder QuadInstance0(.clk(myclk),.rst(RST),.wen(quadWrite[0]),.ch_A(ch0_A),.ch_B(ch0_B),.in(dataWrite),.out(dataRead0));
	QuadratureDecoder QuadInstance1(.clk(myclk),.rst(RST),.wen(quadWrite[1]),.ch_A(ch1_A),.ch_B(ch1_B),.in(dataWrite),.out(dataRead1));
	QuadratureDecoder QuadInstance2(.clk(myclk),.rst(RST),.wen(quadWrite[2]),.ch_A(ch2_A),.ch_B(ch2_B),.in(dataWrite),.out(dataRead2));
	QuadratureDecoder QuadInstance3(.clk(myclk),.rst(RST),.wen(quadWrite[3]),.ch_A(ch3_A),.ch_B(ch3_B),.in(dataWrite),.out(dataRead3));
	QuadratureDecoder QuadInstance4(.clk(myclk),.rst(RST),.wen(quadWrite[4]),.ch_A(ch4_A),.ch_B(ch4_B),.in(dataWrite),.out(dataRead4));
	QuadratureDecoder QuadInstance5(.clk(myclk),.rst(RST),.wen(quadWrite[5]),.ch_A(ch5_A),.ch_B(ch5_B),.in(dataWrite),.out(dataRead5));
	QuadratureDecoder QuadInstance6(.clk(myclk),.rst(RST),.wen(quadWrite[6]),.ch_A(ch6_A),.ch_B(ch6_B),.in(dataWrite),.out(dataRead6));
	QuadratureDecoder QuadInstance7(.clk(myclk),.rst(RST),.wen(quadWrite[7]),.ch_A(ch7_A),.ch_B(ch7_B),.in(dataWrite),.out(dataRead7));
	
	
	
	
	
	//write to Pulsewidthcounter
	PWM PWMInstance0(.clk(myclk),.wen(pwmWrite[0]),.rst(RST),.in(dataWrite),.out(PWM0));
	PWM PWMInstance1(.clk(myclk),.wen(pwmWrite[1]),.rst(RST),.in(dataWrite),.out(PWM1));
	PWM PWMInstance2(.clk(myclk),.wen(pwmWrite[2]),.rst(RST),.in(dataWrite),.out(PWM2));
	PWM PWMInstance3(.clk(myclk),.wen(pwmWrite[3]),.rst(RST),.in(dataWrite),.out(PWM3));
	PWM PWMInstance4(.clk(myclk),.wen(pwmWrite[4]),.rst(RST),.in(dataWrite),.out(PWM4));
	PWM PWMInstance5(.clk(myclk),.wen(pwmWrite[5]),.rst(RST),.in(dataWrite),.out(PWM5));
	PWM PWMInstance6(.clk(myclk),.wen(pwmWrite[6]),.rst(RST),.in(dataWrite),.out(PWM6));
	PWM PWMInstance7(.clk(myclk),.wen(pwmWrite[7]),.rst(RST),.in(dataWrite),.out(PWM7));
	
	
	
	reg [2:0] SCKr; 
	
	//Receiving
	always @(posedge myclk)
	begin
		SCKr <= {SCKr[1:0],SCK};
	end
	wire SCK_risingEdge = (SCKr[2:1]==2'b01);
	wire SCK_fallingEdge = (SCKr[2:1]==2'b10);
	
	reg	[2:0] SSELr;
	always @(posedge myclk)
	begin 
		SSELr<={SSELr[1:0],SSEL};	
	end 
	wire SSEL_active= ~SSELr[1];
	wire SSEL_startMessage  = (SSELr[2:1]==2'b10);	
	wire SSEL_endMessage = (SSELr[2:1]==2'b01);
	wire SSEL_risingEdge=(SSELr[2:1]==2'b01);
	
	
	reg [1:0] MOSIr;	
	always @(posedge myclk)
	begin
		MOSIr <= {MOSIr[0],MOSI};
	end
	
	wire MOSI_data = MOSIr[1];



	
	//Receiving SPI data
	reg test;
	reg byte_received;
	reg[4:0] bit_count;
	reg[23:0] data_received;

	always @(posedge myclk)
	begin
		if (~SSEL_active)
		begin
			bit_count <= 5'b00000;	
			data_received <= 24'b000000000000000000000000;
		end
		else
		if (SCK_fallingEdge)
		begin
			bit_count<=bit_count+5'b00001;
		end
		if(SCK_risingEdge)
		begin
				data_received <= {data_received[22:0],MOSI_data};
		end
	end
	
	
	reg dataOut;
	reg [15:0] OutReg;
	
	
	//sending data out through SPI
	always @(posedge myclk)
	begin
	    //decide what data to send out and store it in out reg--(reading from MC)
		if ((data_received[4]==1'b0) && (bit_count== 5'b00110) && SCK_risingEdge)
		begin
			if(data_received[3:0]==4'b0000)
				OutReg <= dataRead0;
			if(data_received[3:0]==4'b0001)
				OutReg <= dataRead1;
			if(data_received[3:0]==4'b0010)
				OutReg <= dataRead2;
			if(data_received[3:0]==4'b0011)
				OutReg <= dataRead3;
			if(data_received[3:0]==4'b0100)
				OutReg <= dataRead4;
			if(data_received[3:0]==4'b0101)
				OutReg <= dataRead5;
			if(data_received[3:0]==4'b0110)
				OutReg <= dataRead6;
			if(data_received[3:0]==4'b0111)
				OutReg <= dataRead7;
				
			if(data_received[3:0]==4'b1111)
				OutReg <= SerialNo;	
		end
        // after receiving the 1st byte, sending the rest of the 2 byte data from the dataout on MISO 
		else if (SSEL_active && SCK_fallingEdge && (bit_count > 3'b111))
		begin 
			dataOut <= OutReg[15];
			OutReg <= {OutReg[14:0],1'b0};
		end
		else if (~SSEL_active)
		begin
			dataOut <= 1'b0;
		//	data_received <= 24'b000000000000000000000000;
		end
	end
	
	assign MISO = dataOut;
	
	//Writing to PWM Pulsewidthcounter and Quad(writing to Servo and Quad)
	always @(posedge myclk)
	begin
		if (SSEL_risingEdge && (data_received[23]==1'b1))//check the bit count if sent 23 bits ????????????????????????????????????????????
		begin
		    dataWrite <= data_received[15:0];
			
			//Deciding PWM no. to write to
			if(data_received[22:19]==4'b0000)
			begin
				pwmWrite[7:0]  <= 8'b00000001;
			    quadWrite[7:0] <= 8'b00000000;
			end	
			if(data_received[22:19]==4'b0001)
			begin
				pwmWrite[7:0]  <= 8'b00000010;
			    quadWrite[7:0] <= 8'b00000000;
			end
			if(data_received[22:19]==4'b0010)
			begin
				pwmWrite[7:0]  <= 8'b00000100;
			    quadWrite[7:0] <= 8'b00000000;
			end	

			if(data_received[22:19]==4'b0011)
			begin
				pwmWrite[7:0]  <= 8'b00001000;
			    quadWrite[7:0] <= 8'b00000000;
			end	
			
			if(data_received[22:19]==4'b0100)
			begin
				pwmWrite[7:0]  <= 8'b00010000;
			    quadWrite[7:0] <= 8'b00000000;
			end	
			
			if(data_received[22:19]==4'b0101)
			begin
				pwmWrite[7:0]  <= 8'b00100000;
			    quadWrite[7:0] <= 8'b00000000;
			end	
			
			if(data_received[22:19]==4'b0110)
			begin
				pwmWrite[7:0]  <= 8'b01000000;
			    quadWrite[7:0] <= 8'b00000000;
			end	
			
			if(data_received[22:19]==4'b0111)
			begin
				pwmWrite[7:0]  <= 8'b10000000;
			    quadWrite[7:0] <= 8'b00000000;
			end	


			
			
			//writing to QuadratureDecoder
		    if(data_received[22:19]==4'b1000)
            begin			
			    pwmWrite[7:0] <= 8'b00000000;
			    quadWrite[7:0] <=8'b00000001;		
			end	
			if(data_received[22:19]==4'b1001)
            begin			
			    pwmWrite[7:0] <= 16'b00000000;
			    quadWrite[7:0] <=16'b00000010;		
			end
			if(data_received[22:19]==4'b1010)
            begin			
			    pwmWrite[7:0] <= 16'b00000000;
			    quadWrite[7:0] <=16'b00000100;		
			end
			
			if(data_received[22:19]==4'b1011)
            begin			
			    pwmWrite[7:0] <= 16'b00000000;
			    quadWrite[7:0] <=16'b00001000;		
			end
			
			if(data_received[22:19]==4'b1100)
            begin			
			    pwmWrite[7:0] <= 16'b00000000;
			    quadWrite[7:0] <=16'b00010000;		
			end
			
			if(data_received[22:19]==4'b1101)
            begin			
			    pwmWrite[7:0] <= 16'b00000000;
			    quadWrite[7:0] <=16'b00100000;		
			end
			
			if(data_received[22:19]==4'b1110)
            begin			
			    pwmWrite[7:0] <= 16'b00000000;
			    quadWrite[7:0] <=16'b01000000;		
			end

			if(data_received[22:19]==4'b1111)
            begin			
			    pwmWrite[7:0] <= 16'b00000000;
			    quadWrite[7:0] <=16'b10000000;		
			end


			
		end	
		else
		begin
			//pwmWrite <= 1'b0;
			//quadWrite <= 1'b0;
			pwmWrite[7:0] <= 8'b00000000;
			quadWrite[7:0] <= 8'b00000000;
		end
	end
	
	

endmodule