
//`#start header` -- edit after this line, do not edit this line
`include "cypress.v"
`include "../BasicCounter_v1_0/BasicCounter_v1_0.v"
//`include "BasicCounter_v1_0.v"

//`#end` -- edit above this line, do not edit this line
// Generated on 12/11/2019 at 21:18
// Component: Sampler
module Sampler (
	output reg [7:0] opcode,
	output  req,
	input   clock,
	input   index,
	input   rdata,
	input   reset,
	input   sampleclock
);

//`#start body` -- edit after this line, do not edit this line

/* FIXME: Known limitation: sampleclock must be exactly 1/3 of clock. */

localparam STATE_WAITING = 0;
localparam STATE_OPCODE = 1;

reg state;
reg sampleclked;
wire [5:0] counter;
wire counterreset;
wire counterenable;

assign counterreset = reset || (state == STATE_OPCODE);
assign counterenable = sampleclked;
assign req = (state == STATE_OPCODE);

BasicCounter_v1_0 #(.Width(6)) Counter
(
	/* input */ .clock(clock),
	/* input */ .reset(counterreset),
	/* input */ .en(counterenable),
	/* output */ .cnt(counter)
);

always @(posedge clock)
begin
	if (reset)
	begin
		state <= STATE_WAITING;
		opcode <= 0;
		sampleclked <= 0;
	end
	else
	begin
		if (sampleclock && !sampleclked)
			sampleclked <= 1;
		else
			sampleclked <= 0;
		case (state)
		STATE_WAITING:
		begin
			if (sampleclked && (rdata || index || counter == 6'h3f))
			begin
				opcode[7] <= index;
				opcode[6] <= rdata;
				opcode[5:0] <= counter;
				state <= STATE_OPCODE;
			end
		end

		STATE_OPCODE: /* opcode byte sent here */
		begin
			/* Set opcode output to dummy value 0xaa when idle.
			* If you get a lot of 0xaa in your data, you'll know
			* something is wrong here.
			*/
			opcode <= 8'b10101010;
			state <= STATE_WAITING;
		end
		endcase
	end
end

//`#end` -- edit above this line, do not edit this line
endmodule
//`#start footer` -- edit after this line, do not edit this line
//`#end` -- edit above this line, do not edit this line
