/*Integrantes Grupo 0xEC: Henrique Andrews Prado Marques 9771463
                          Estevam Fernandes Arantes 9763105
                          Osmar Bohr Horng Chen 9288359
                          Willian Gonzaga Leodegario 9771293
*/

/* code-c - Student's code for mmcpu

   Copyright 2017  Monaco F. J.   <monaco@icmc.usp.br>

   This file is part of Muticlycle Mips CPU (MMCPU)

   MMCPU is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


/* After you have implemented your functions, you may safely remove these lines. */
#include <stdio.h>
#include <stdlib.h>
#include "mask.h"
#include "cpu.h"
#define not_implemented() fprintf(stderr, "Not implemented\n"); exit(EXIT_FAILURE)

/* Students, you are required to implemented the functions bellow.
   Please, refere to cpu.h for further information. */



int alu( int a, int b, char alu_op, int *result_alu, char *zero, char *overflow){
  switch(alu_op){
    //ADD
    case 0b0010:
      *result_alu = a + b;
      break;
    //SUB
    case 0b0110:
      *result_alu = a - b;
      break;
    //AND
    case 0b0000:
      *result_alu = a & b;
      break;
    //OR
    case 0b0001:
      *result_alu = a | b;
      break;
    //Set on less than
    case 0b0111:
      if(a < b){
        *result_alu = 1;
      }
      else *result_alu = 0;
      break;
  }

  if(*result_alu == 0){
    *zero = 1;
  }
  else *zero = 0;
  return 0;
}


void control_unit(int IR, short int *sc){
  //Checar se está no estado 0 representado pelo IR = -1, caso sim, setar bits de controle
  if(IR == -1){
    *sc = 0b1001010000001000;
    return;
  }
  //Caso os bits de controle forem equivalentes ao do estado 0, muda-se os bits para os do estado 1
  if(*sc == (short int) 0b1001010000001000){
    *sc = 0b0000000000011000;
    return;
  }
  //Caso os bits de controle forem equivalentes ao do estado 1, muda-se os bits para escolher
  // 1 estado entre 4 possíveis
  if(*sc == (short int) 0b0000000000011000){
    switch((IR & separa_cop) >> 26){
      case 0: //Tipo R
        *sc = 0b0000000001000100;
        break;

      case 2: //Jump
        *sc = 0b0000010100000000;
        break;

      case 4: //Branch
        *sc = 0b0000001010100100;
        break;

      default: // LW ou SW
        *sc = 0b0000000000010100;
    }
    return;
  }
  //Caso os bits de controle forem equivalentes ao do estado 6 (Tipo R), muda-se os bits para os
  //do estado 7
  if(*sc == (short int) 0b0000000001000100){
    *sc = 0b0000000000000011;
    return;
  }
  //Caso os bits de controle forem equivalentes ao do estado 9 (Jump), muda-se os bits para os
  //do estado 0
  if(*sc == (short int) 0b0000010100000000){
    *sc = 0b1001010000001000;
    return;
  }
  //Caso os bits de controle forem equivalentes ao do estado 7 (Tipo R), muda-se os bits para os
  //do estado 0
  if(*sc == (short int) 0b0000000000000011){
    *sc = 0b1001010000001000;
    return;
  }
  //Caso os bits de controle forem equivalentes ao do estado 8 (Branch), muda-se os bits para os
  //do estado 0
  if(*sc == (short int) 0b0000001010100100){
    *sc = 0b1001010000001000;
    return;
  }
  //Caso os bits de controle forem equivalentes ao do estado 2, muda-se os bits para os
  //do estado 3 (LW) ou 5 (SW)
  if(*sc == (short int) 0b0000000000010100){
    if((IR & separa_cop) >> 26 == 0x23){
      *sc = 0b0001100000000000;
      return;
    }
    if((IR & separa_cop) >> 26 == 0x2b){
      *sc = 0b0010100000000000;
      return;
    }
  }
  //Caso os bits de controle forem equivalentes ao do estado 5 (SW), muda-se os bits para os
  //do estado 0
  if(*sc == (short int) 0b0010100000000000){
    *sc = 0b1001010000001000;
    return;
  }
  //Caso os bits de controle forem equivalentes ao do estado 3 (LW), muda-se os bits para os
  //do estado 4
  if(*sc == (short int) 0b0001100000000000){
    *sc = 0b0100000000000010;
    return;
  }
  //Caso os bits de controle forem equivalentes ao do estado 4, muda-se os bits para os
  //do estado 0
  if(*sc == (short int) 0b0100000000000010){
    *sc = 0b1001010000001000;
    return;
  }
}


void instruction_fetch(short int sc, int PC, int ALUOUT, int IR, int* PCnew, int* IRnew, int* MDRnew){
  //Checar se a IF deve ser executada ou não
  if(sc == (short int) 0b1001010000001000){
    *IRnew = memory[PC];
    
    char zero, overflow;
    alu(PC, 1, controle_da_ula(IR, sc), PCnew, &zero, &overflow);
    *MDRnew = memory[PC];
    //Para o loop
    if(*IRnew == 0){
      loop = 0;
    }
  }
  else return;
}


void decode_register(short int sc, int IR, int PC, int A, int B, int *Anew, int *Bnew, int *ALUOUTnew){
  char zero, overflow;

  //Checar se o DR deve ser executado ou não
  if(sc == (short int) 0b0000000000011000){

  	short int imediato = (IR & separa_imediato);
    //Colocar valores do rs e rt nos buffers A e B 
    *Anew = reg[(IR & separa_rs) >> 21];
    *Bnew = reg[(IR & separa_rt) >> 16];
    //Adianta o cálculo da Branch

    //Se o imediato for negativo (solução dada por Tiago Toledo)
    if(imediato & 0x00008000){
    	//Remove o bit de sinal para encontrar o imediato
    	imediato = imediato & 0x0fff;
    	imediato = (-1) * imediato;
    }
    alu(PC, (IR & separa_imediato), controle_da_ula(IR, sc), ALUOUTnew, &zero, &overflow);
  }
}


void exec_calc_end_branch(short int sc, int A, int B, int IR, int PC, int ALUOUT, int *ALUOUTnew, int *PCnew){
  char zero,overflow;
  //Estado 6 (Tipo R)
  if(sc == (short int) 0b0000000001000100){
    alu(A, B, controle_da_ula(IR, sc), ALUOUTnew, &zero, &overflow);
    return;
  }
  //Estado 9 (Jump)
  if(sc == (short int) 0b0000010100000000){
    *PCnew = ((PC & separa_4bits_PC) | (IR & separa_endereco_jump));
    return;
  }
  //Estado 8 (Branch)
  if(sc == (short int) 0b0000001010100100){
    alu(A, B, controle_da_ula(IR, sc), ALUOUTnew, &zero, &overflow);
    if(zero == 1){
      *PCnew = ALUOUT;
    }
    return;
  }
  //Estado 2 (LW ou SW)
  if(sc == (short int) 0b0000000000010100){
    alu(A, (IR & separa_imediato), controle_da_ula(IR, sc), ALUOUTnew, &zero, &overflow);
    return;
  }
}


void write_r_access_memory(short int sc, int B, int IR, int ALUOUT, int PC, int *MDRnew, int *IRnew){
  //Caso esteja no estado 7 (Tipo R)
  if(sc == (short int) 0b0000000000000011){
    reg[(IR & separa_rd) >> 11] = ALUOUT;
    return;
  }
  //Caso esteja no estado 3 (LW)
  if(sc == (short int) 0b0001100000000000){
    *MDRnew = memory[ALUOUT];
    return;
  }
  //Caso esteja no estado 5 (SW)
  if(sc == (short int) 0b0010100000000000){
    memory[ALUOUT] = reg[(IR & separa_rt) >> 16];
    return;
  }

  else return;
}


void write_ref_mem(short int sc, int IR, int MDR, int ALUOUT){
  //Checar se o WRM deve ser executado
  if(sc == (short int) 0b0100000000000010){
    //Estado 4 (LW)
    reg[(IR & separa_rt) >> 16] = MDR;
  }

  else return;
}

//Função da tabela verdade para controle da ULA
int controle_da_ula(int IR, int sc){
  switch(((sc & separa_ALUOp1) | (sc & separa_ALUOp0)) >> 5){
    //LW ou SW
    case 0b00:
      return 0b0010;
      break;
    //Branch
    case 0b01:
      return 0b0110;
      break;
    //Tipo R
    case 0b10:
      switch((IR & separa_cfuncao) & 0b001111){

        //Operação de Adição
        case 0b0000:
          return 0b0010;
          break;
        //Operação de Subtração
        case 0b0010:
          return 0b0110;
          break;
        //Operação de E lógico
        case 0b0100:
          return 0b0000;
          break;
        //Operação de OU lógico
        case 0b0101:
          return 0b0001;
          break;
        //Operação de Set on less than
        case 0b1010:
          return 0b0111;
          break;
      }

  }
}

