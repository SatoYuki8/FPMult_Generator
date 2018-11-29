#include "mul_gen.h"

int fpmulti_sel_declaration(FILE *fp, int exp, int frac, int width, flags_t flag, char *sel_or_reg){
  fprintf(fp,
	  "sel rounded_frac<%d>;\n"
	  "sel xmsb, ymsb;\n\n"
	  "sel added_exp<%d>;\n"
	  "sel sticky;\n"
	  "sel normalized_frac<%d>;\n"
	  "%s As, Aexp<%d>, Am1<%d>, Am2<%d>;\n"
	  "%s Bs, Bexp<%d>, Bm<%d>;\n"
	  "%s Cs, Cexp<%d>, Cm<%d>;\n"
	  "%s out<%d>;\n\n",
	  frac,
	  exp+1,
	  (frac+1)*2,
	  sel_or_reg, exp, frac+1, frac+1,
	  sel_or_reg, exp, (frac+1)*2,
	  sel_or_reg, exp, frac+3,
	  sel_or_reg, width
	  );

P_IF
  /* stage_name declaration */
  fprintf(fp,
	  "stage_name A {task frac_mult(As, Aexp, Am1, Am2);}\n"
	  "stage_name B {task normalize(Bs, Bexp, Bm);}\n"
	  "stage_name C {task round(Cs, Cexp, Cm);}\n"
	  "stage_name D {task result(out);}\n\n"
	  );
P_END
  
  return 1;
}//end fpmulti_sel_declaration

int fpmulti_signxor_expadd(FILE *fp, int exp, int frac, int width, flags_t flag){
  fprintf(fp,
	  "alt{\n"
	  "(a<%d:0> == %d#(0b0)): xmsb = 0b0;\n"
	  "else: xmsb = 0b1;\n"
	  "}\n\n",
	  width-2, width-1
	  );
  fprintf(fp,
	  "alt{\n"
	  "(b<%d:0> == %d#(0b0)): ymsb = 0b0;\n"
	  "else: ymsb = 0b1;\n"
	  "}\n\n",
	  width-2, width-1
	  );
  fprintf(fp,
	  "added_exp = (0b0||a<%d:%d>) + (0b0||b<%d:%d>) + (^(0b0||(%d#0b1)) + 0b1);\n",
	  width-2, frac, width-2, frac, exp-1
	  );
  
P_IF
  fprintf(fp,
	  "generate A.frac_mult(a<%d>@b<%d>, \n"
	  "added_exp<%d:0>,\n"
	  "xmsb||a<%d:0>,\n"
	  "ymsb||b<%d:0>\n"
	  ");\n"
	  "}\n"
	  "\n",
	  width-1, width-1,
	  exp-1,
	  frac-1, frac-1
	  );
P_ELSE
  fprintf(fp, "As = a<%d>@b<%d>;\n", width-1, width-1);
  fprintf(fp, "Aexp = added_exp<%d:0>;\n",
	  exp-1);
  fprintf(fp,
	  "Am1 = xmsb||a<%d:0>;\n"
	  "Am2 = ymsb||b<%d:0>;\n"
	  "\n",
	  frac-1, frac-1
	  );
P_END
  
  return 1;
}// end fpmulti_signxor_expadd


int fpmulti_fracmult(FILE *fp, flags_t flag){
P_IF
  fprintf(fp,
	  "stage A{\n"
	  "par {\n"
	  "relay B.normalize(As,\n"
	  "Aexp,\n"
	  "multi.do(Am1, Am2).out\n"
	  ");\n"
	  "}\n"
	  "}\n"
	  "\n"
	  );
P_ELSE
  fprintf(fp,
	  "Bs = As;\n"
	  "Bexp = Aexp;\n"
	  "Bm = multi.do(Am1, Am2).out;\n"
	  "\n"
	  );
P_END
  
  return 1;
}

int fpmulti_normalize(FILE *fp, int exp, int frac, flags_t flag){
 
P_IF
  fprintf(fp,
	  "stage B{\n"
	  "par{\n"
	  "alt{\n"
	  "(^(/|Bm<%d:%d>)): par{\n"
	  "relay C. round(0b0, %d#0b0, %d#0b0);\n"
	  "}\n",
	  (frac+1)*2 - 1, (frac+1)*2 - 2,
	  exp, frac+3
	  );
 fprintf(fp,
	 "else: par{\n"
	 "normalized_frac = Bm >> Bm<%d>;\n"
	 "sticky = /|Bm<%d:0> | (Bm<%d>&Bm<%d>);\n"
	 "relay C.round(Bs, Bexp + Bm<%d>, normalized_frac<%d:%d> || sticky);\n"
	 "}\n"
	 "}\n"
	 "}\n"
	 "}\n"
	 "\n",
	 (frac+1)*2 - 1,
	 frac-3, (frac+1)*2 - 1, frac-2,
	 (frac+1)*2 - 1, (frac+1)*2 - 3, frac-2
	 );
P_ELSE
  fprintf(fp,
	  "alt{\n"
	  "(^(/|Bm<%d:%d>)): par{\n"
	  "Cs = 0b0; Cexp =  %d#0b0; Cm = %d#0b0;\n"
	  "}\n",
	  (frac+1)*2 - 1, (frac+1)*2 - 2,
	  exp, frac+3
	  );
 fprintf(fp,
	 "else: par{\n"
	 "normalized_frac = Bm >> Bm<%d>;\n"
	 "sticky = /|Bm<%d:0> | (Bm<%d>&Bm<%d>);\n"
	 "Cs = Bs; Cexp = Bexp + Bm<%d>; Cm = normalized_frac<%d:%d> || sticky;\n"
	 "}\n"
	 "}\n"
	 "\n",
	 (frac+1)*2 - 1,
	 frac-3, (frac+1)*2 - 1, frac-2,
	 (frac+1)*2 - 1, (frac+1)*2 - 3, frac-2
	 );
P_END

  return 1;
}

int fpmulti_round(FILE *fp, int frac, flags_t flag){
  char tmp[16];
P_IF
  strcpy(tmp, "relay D.result");
 fprintf(fp,
	 "stage C{\n"
	 "par{\n"
	 );
P_ELSE
  strcpy(tmp, "out = ");
P_END
  

  fprintf(fp,
	  "rounded_frac = incfrac.do((Cm<2>&(Cm<3>|Cm<1>|Cm<0>)), Cm<%d:3>).out;\n",
	  frac+2
	  );
  fprintf(fp,
	  "%s(Cs || (Cexp + incfrac.p) || rounded_frac);\n",
	  tmp 
	  );  
  
  return 1;
}

int FPMultiplier_Generator(FILE *fp, const efw_t f, flags_t flag, char *module_name){
  int exp = f->exp;
  int frac= f->frac;
  int width=f->width;
  char incfrac_module[64];
  char multi_module[64];
  char sel_or_reg[8] = "sel";
  
  /* Sub module */
  IncFrac_Generator(fp, frac, incfrac_module);
  Multiplier_Generator(fp, frac, flag, multi_module);
  /**************/

  /************ Input, Output, Submodule **************/
  fprintf(fp,
	  "circuit %s{\n"
	  "input a<%d>, b<%d>;\n"
	  "output result<%d>;\n"
	  "instrin do;\n\n"
	  "%s multi;\n"
	  "%s incfrac;\n\n",
	  module_name,
	  width, width,
	  width,
	  multi_module, incfrac_module
	  );

P_IF
  strcpy(sel_or_reg, "reg_wr");
P_END
  
  /* Sel declaration */
  fpmulti_sel_declaration(fp, exp, frac, width, flag, sel_or_reg);
  
  /*********************************************************************/
  /************************* step1 *****************************/
 fprintf(fp, "instruct do par{\n");
 
 fpmulti_signxor_expadd(fp, exp, frac, width, flag);
 
 /******************** Multiplication *********************/
 fpmulti_fracmult(fp, flag);
 
 /******************Normalize*********************/
 fpmulti_normalize(fp, exp, frac, flag);
 
 /********************Round**********************/
 fpmulti_round(fp, frac, flag);
 
 /*********************** Result Output **************************/
P_IF
  fprintf(fp,
	  "}\n"
	  "}\n"
	  "\n"
	  "stage D{\n"
	  );
P_END
  
  fprintf(fp,
	  "result = out;\n"
	  "}\n"
	  "}\n\n"
	  );

P_IF
  fprintf(stdout, "Generate %s Module (Pipeline Version)\n", module_name);
P_ELSE
  fprintf(stdout, "Generate %s Module\n", module_name);
P_END
  
  return 1;
}//end FPMultiplier_Generator
