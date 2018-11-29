
#include "mul_gen.h"

int IncFrac_Generator(FILE *fp, const int frac, char *name){
  sprintf(name, "IncreaseFrac_%d", frac);
  //Header
  fprintf(fp,
	  "declare %s{\n"
	  "input cin;\n"
	  "input in<%d>;\n"
	  "output out<%d>;\n"
	  "output p;\n"
	  "instrin do;\n"
	  "instr_arg do(cin, in);\n"
	  "}\n\n",
	  name,
	  frac, frac
	  );

  //Module
  fprintf(fp,
	  "circuit %s{\n"
	  "input cin;\n"
	  "input in<%d>;\n"
	  "output out<%d>;\n"
	  "output p;\n"
	  "instrin do;\n"
	  "sel tmp<%d>;\n\n"
	  "instruct do par{\n"
	  "tmp = (0b0||in) + cin;\n"
	  "out = tmp<%d:0>;\n"
	  "p = tmp<%d>;\n"
	  "}\n}\n\n",
	  name,
	  frac, frac, frac+1,
	  frac-1, frac
	  );
  
  return 1;
}//end IncFrac_Gnerator

/* 筆算アルゴリズムによる乗算    */
/* SECONDSでのシュミレーション用 */
int Long_Division(FILE *fp, const int frac, char *name){
  int mul_frac = frac + 1;

  fprintf(fp,
	  "circuit %s{\n"
	  "input       in1<%d> ;\n"
	  "input       in2<%d> ;\n"
	  "output      out<%d> ;\n"
	  "instrin     do;",
	  name,
	  mul_frac, mul_frac, mul_frac*2
	  );

  //sel declaration
  int i;
  for(i=0;i<mul_frac;i++){
    fprintf(fp, "sel tmp%d<%d>;\n", i, mul_frac*2);
  }

  fprintf(fp, "\ninstruct do par{\n");
  //long division
  fprintf(fp, "tmp0 = (%3d#(0b0)||in1           ) & (%d # in2<  0>);\n",mul_frac, mul_frac*2);
  for(i=1;i<mul_frac;i++){
    fprintf(fp,
	    "tmp%d = (%3d#(0b0)||in1||%3d#(0b0)) & (%d # in2<%3d>);\n", i, mul_frac-i, i, mul_frac*2, i);
  }

  fprintf(fp, "out = tmp0");
  for(i=1;i<mul_frac;i++){
    fprintf(fp, " + tmp%d", i);
  }
  fprintf(fp, ";\n}\n}\n\n");
  
  return 1;
}//end Long_Division

int Multiplier_Generator(FILE *fp, const int frac, flags_t flag, char *name){
  int mul_frac = frac + 1;

  sprintf(name, "Multi_%d", mul_frac);

  //Header
  fprintf(fp,
	  "declare %s{\n"
	  "input in1<%d>;\n"
	  "input in2<%d> ;\n"
	  "output out<%d> ;\n"
	  "instrin do ;\n"
	  "instr_arg do(in1,in2) ;\n"
	  "}\n\n",
	  name,
	  mul_frac,
	  mul_frac,
	  mul_frac*2
	  );

  //Multiplier module
  if(flag->non_DSP_flag == 1){
    Long_Division(fp, frac, name);  //筆算アルゴリズムで乗算
    fprintf(stdout, "Not Using Operator\n");
    return 1;
  }
  
  fprintf(fp,
	  "circuit %s{\n"
	  "input in1<%d>;\n"
	  "input in2<%d> ;\n"
	  "output out<%d> ;\n"
	  "instrin do ;\n\n"
	  "instruct do out = in1 * in2;\n"
	  "}\n\n",
	  name,
	  mul_frac,
	  mul_frac,
	  mul_frac*2
	  );

  fprintf(stdout, "Using Operator\n");
  return 1;
}//end Multiplier_Generator

