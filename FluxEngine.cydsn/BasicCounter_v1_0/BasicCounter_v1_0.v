/*******************************************************************************
* File Name: BasicCounter_v1_0.v
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
* The Basic Counter component provides an up-counter with routable count value. 
* This file describes the component functionality in Verilog.
*
********************************************************************************
* I*O Signals:
********************************************************************************
*   Name        Direction       Description
*   cnt         output          Counter value
*   clock       input           Clock signal to count
*   en          input           Enable counting
*   reset       input           Synchronous reset
*
********************************************************************************
* Copyright 2012, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

`include "cypress.v"

`ifdef BasicCounter_v1_0_V_ALREADY_INCLUDED
`else
`define BasicCounter_v1_0_V_ALREADY_INCLUDED

module BasicCounter_v1_0 (
    cnt,    /* Counter value */
    clock,  /* Clock signal to count */
    en,     /* Enable counting */
    reset   /* Synchronous reset */
);

    /***************************************************************************
    *             Parameters                                                
    ***************************************************************************/
    parameter [7:0] Width = 8'd4;
    
    /***************************************************************************
    *            Interface Definition                                                
    ***************************************************************************/
    output reg [(Width-1):0] cnt;
    input wire clock;
    input wire en;
    input wire reset;

    always@(posedge clock)
    begin
        if (reset)
        begin
            cnt <= 0;
        end
        else if (en)
        begin
            cnt <= cnt + 1;
        end
    end

endmodule

`endif /* BasicCounter_v1_0_V_ALREADY_INCLUDED */
