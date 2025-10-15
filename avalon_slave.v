module avalon_slave( // Lado escravo da ula, registadores e comando/status(so recebe ordens)
    input               clk,
    input               reset_n,
    input      [2:0]    address,
    input               chipselect,
    input               write,
    input      [31:0]   writedata, 
    input               read,
    output reg [31:0]   readdata, 

	 
    output     [31:0]   comando_nucleo,
    output              buffer_selec,
    input               status_in      
);
	 
	 
	 // Registradores internos que armazenam o estado do nosso periférico
	reg [31:0] comando_reg;       // Armazena o último comando enviado pelo HPS
	reg        buffer_selecao; // Armazena o bit de seleção de buffer
	 
    always @(posedge clk or negedge reset_n) begin
	        if (!reset_n) begin
			     comando_reg <= 32'b0;
				  buffer_selecao <= 1'b0;
			  end
			  else begin
			    if (chipselect && write) begin
			       case (address)
				       3'b000: comando_reg <= writedata;
						 3'b010: buffer_selecao <= writedata[0]; // pega o bit na posicao 0, ou seja, o LSB
                   default;
                 endcase
			     end
				end
	  end


    always @(*) begin
         readdata = 32'h0;
			if (chipselect && read) begin
            case (address)
                3'b001: readdata = {31'b0, status_in};
                default: readdata = 32'h0;
            endcase
        end
    end

    assign comando_nucleo = command_reg;
    assign buffer_selec   = buffer_select_reg;

endmodule
	      		














