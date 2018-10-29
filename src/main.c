#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ERROR 0

/* exponation fraction bit_width */
typedef struct exp_frac_width{
  int exp;
  int frac;
  int width;  //exp + frac + sign
} _efw_t;

typedef _efw_t efw_t[1];

/* Flag */
typedef struct flags{
  int wrapper_flag;
  int module_flag;
  int stdout_flag;
  int filename_flag;
  int non_DSP_flag;
  int pipeline_flag;
} _flags_t;

typedef _flags_t flags_t[1];

#define P_IF if(flag->pipeline_flag == 1){
#define P_ELSE }else{
#define P_END }

/*************************************************/

void init_flags(flags_t flag){
  flag->wrapper_flag = 0;
  flag->module_flag = 0;
  flag->stdout_flag = 0;
  flag->filename_flag = 0;
  flag->non_DSP_flag = 0;
  flag->pipeline_flag = 0;
}

void start_message(){
  fprintf(stdout,
	  "FPMulti Generator\n"
	  "  ./gen fpmulti exp=<n> frac=<m>\n"
	  "------->FPMulti_<n>_<m>_<n+m>.sfl\n"
	  "\n"
	  "Option\n"
	  " DSP=no : Using Mutiplier Module insted of Operator.\n"
	  " pipeline : Output Pipeline Version\n"
	  "\n"
	  );
}

FILE *arg_check(int argc, char **argv, efw_t f, flags_t flag, char *module_name){
  FILE *fp;
  int i;
  char output[64];
  int exp = 0, frac = 0;

  if (argc == 1) {
    start_message();
    //printf("argument taritenai\n");
    return ERROR;
  }

  /* 端末の引数をチェック */
  for(i=0 ; i<argc ; i++){
    //乗算モジュール
    if(!strncmp(argv[i], "FPMult", 6)) flag->module_flag = 1;
    if(!strncmp(argv[i], "fpmult", 6)) flag->module_flag = 1;
    
    //exp & frac substitution
    if(!strncmp(argv[i], "exp=", 4)) exp = atoi(argv[i]+4);
    if(!strncmp(argv[i], "frac=", 5)) frac = atoi(argv[i]+5);    

    //filename command
    if (!strncmp(argv[i], "filename=", 9)){
      strcpy(output, argv[i]+9);
      flag->filename_flag = 1;
    }

    //Wrapper command
    if(!strncmp(argv[i], "Wrapper", 7)) flag->wrapper_flag = 1;
    if(!strncmp(argv[i], "wrapper", 7)) flag->wrapper_flag = 1;

    //stdout command
    if(!strncmp(argv[i], "stdout", 6)) flag->stdout_flag = 1;

    //Use DSP commnad 
    if(!strncmp(argv[i], "DSP=no", 6)) flag->non_DSP_flag = 1;

    //Use pipeline command
    if(!strncmp(argv[i], "pipeline", 8)) flag->pipeline_flag = 1;

  }//end for

  //Bit width substitution
  if((exp<3) | (frac<4)){
    fprintf(stderr, "Exponation or Fraction are irregularity.\n");
    return ERROR;
  }else{    
    f->exp = exp;
    f->frac = frac;
    f->width = f->exp + f->frac + 1;
  }

  //Module command
  if(flag->module_flag == 1){
    sprintf(module_name, "FPMulti_%d_%d_%d", f->exp, f->frac, f->width);
  }else{
    fprintf(stderr, "Please input module type\n");
    return ERROR;
  }

  //When there is stdout command
  if(flag->stdout_flag == 1) return stdout;
  
  //When there is no filename command
  if(flag->filename_flag == 0){
    sprintf(output, "%s.sfl", module_name);
    if ((fp = fopen(output, "w")) == NULL){
      fprintf(stderr, "File open error.\n");
      return ERROR;
    }
  }else{
    if ((fp = fopen(output, "w")) == NULL){
      fprintf(stderr, "File open error.\n");
      return ERROR;
    }
  }
  
  return fp;
} //end arg_check


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
	  "module %s{\n"
	  "input cin;\n"
	  "input in<%d>;\n"
	  "output out<%d>;\n"
	  "output p;\n"
	  "instrin do;\n"
	  "sel cry<%d>;\n\n"
	  "instruct do par{\n"
	  "p = /&(in) & cin;\n",
	  name,
	  frac, frac, frac
	  );

  int i;
  for(i=0;i<frac;i++){
    if(i==0) fprintf(fp, "cry = (/&(in<%d:0>) & cin)\n", frac-2);
    else if(i==frac-1) fprintf(fp, "|| cin;\n");
    else fprintf(fp, "|| (/&(in<%d:0>) & cin)\n",frac-i-2);
  }
  
  fprintf(fp,
	  "out = in @ cry;\n"
	  "}\n}\n\n");
  
  return 1;
}//end IncFrac_Gnerator

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

  /* Input, Output, Submodule */
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
  /*
    fprintf(fp,
    "sel sign;\n"
    "sel exp<%d>;\n"
    "sel out_frac<%d>;\n"
    "sel rounded_frac<%d>;\n"
    "sel z<%d>;\n\n"
    "sel xmsb, ymsb;\n\n",
    exp+1,
    (frac+1)*2,
    frac,
    width
    );
  */
  fprintf(fp,
	  "sel rounded_frac<%d>;\n"
	  "sel xmsb, ymsb;\n\n"
	  "%s As, Aexp<%d>, Am1<%d>, Am2<%d>;\n"
	  "%s Bs, Bexp<%d>, Bm<%d>;\n"
	  "%s out<%d>;\n\n",
	  frac,
	  sel_or_reg, exp+1, frac+1, frac+1,
	  sel_or_reg, exp+1, (frac+1)*2,
	  sel_or_reg, width
	  );

P_IF
  /* stage_name declaration */
  fprintf(fp,
	  "stage_name A {task frac_mult(As, Aexp, Am1, Am2);}\n"
	  "stage_name B {task normalize(Bs, Bexp, Bm);}\n"
	  "stage_name C {task result(out);}\n\n"
	  );
P_END

  /*********************************************************************/
  /* instruct do par... */
  fprintf(fp,
	  "instruct do par{\n"
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
P_IF
  fprintf(fp,
	  "generate A.frac_mult(a<%d>@b<%d>, \n"
	  "(0b0||a<%d:%d>) + (0b0||b<%d:%d>) + (^(0b0||(%d#0b1)) + 0b1),\n"
	  "xmsb||a<%d:0>,\n"
	  "ymsb||b<%d:0>\n"
	  ");\n"
	  "}\n"
	  "\n",
	  width-1, width-1,
	  width-2, frac, width-2, frac, exp-1,
	  frac-1, frac-1
	  );
P_ELSE
  fprintf(fp, "As = a<%d>@b<%d>;\n", width-1, width-1);
  fprintf(fp, "Aexp = (0b0||a<%d:%d>) + (0b0||b<%d:%d>) + (^(0b0||(%d#0b1)) + 0b1);\n",
	  width-2, frac, width-2, frac, exp-1);
  fprintf(fp,
	  "Am1 = xmsb||a<%d:0>;\n"
	  "Am2 = ymsb||b<%d:0>;\n"
	  "\n",
	  frac-1, frac-1
	  );
P_END
  
  /* Multiplication */
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
  
  //Normalize, Round
P_IF
  fprintf(fp,
	  "stage B{\n"
	  "par{\n"
	  );
P_END

  char tmp[16];
P_IF
  strcpy(tmp, "relay C.result");
P_ELSE
  strcpy(tmp, "out = ");
P_END
  
  fprintf(fp,
	  "alt{\n"
	  "(^(/|Bm)): %s(%d#(0b0));\n"
	  "else: par{\n"
	  "alt{\n",
	  tmp, width
	  );
  fprintf(fp,
	  "(Bm<%d>): rounded_frac = incfrac.do((Bm<%d>&(Bm<%d>|Bm<%d>|(/|Bm<%d:0>))), Bm<%d:%d>).out;\n",
	  ((frac+1)*2-1), frac, frac+1, frac-1, frac-2, ((frac+1)*2-2), frac+1
	  );
  fprintf(fp,
	  "else: rounded_frac = incfrac.do((Bm<%d>&(Bm<%d>|Bm<%d>|(/|Bm<%d:0>))), Bm<%d:%d>).out;\n",
	  frac-1, frac, frac-2, frac-3, ((frac+1)*2-3), frac
	  );
  fprintf(fp,
	  "}\n"
	  "%s(Bs || (Bexp<%d:0> + Bm<%d> + incfrac.p) || rounded_frac);\n"
	  "}\n"
	  "}\n",
	  tmp, exp-1, ((frac+1)*2-1)
	  );
P_IF
  fprintf(fp,
	  "}\n"
	  "}\n"
	  "\n"
	  "stage C{\n"
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

int Wrapper_Generator(FILE *fp, const efw_t f, char *module_name){
  int width = f->width;

  //FPMultiplier header
  fprintf(fp,
	  "declare %s{\n"
	  "input a<%d>, b<%d>;\n"
	  "output result<%d>;\n"
	  "instrin do;\n"
	  "instr_arg do(a, b);\n"
	  "}\n\n",
	  module_name,
	  width, width,
	  width
	  );
	  
  //Wrapper module
  fprintf(fp,
	  "module %s_Wrapper{\n"
	  "input a<%d>, b<%d>;\n"
	  "output result<%d>;\n"
	  "instrin do;\n"
	  "%s fmulti;\n"
	  "reg_wr ina<%d>;\n"
	  "reg_wr inb<%d>;\n"
	  "reg_wr out<%d>;\n"
	  "instruct do par{\n"
	  "ina:=a;\n"
	  "inb:=b;\n"
	  "out:=fmulti.do(ina, inb).result;\n"
	  "result = out;\n"
	  "}\n"
	  "}\n\n",
	  module_name,
	  width, width,
	  width,
	  module_name,
	  width,
	  width,
	  width
	  );

  fprintf(stdout,
	  "Generate Wrapper Module\n"
	  );
  
  return 1;
}//end Wrapper_Generator

int Manage_FPMult_Generator(int argc, char **argv){
  FILE *fp;
  efw_t f;
  flags_t flag;
  char module_name[64];

  /* efw_t & flags_t init */
  f->exp = 0; f->frac = 0; f->width = 0;
  init_flags(flag);

  /* Argument check */
  fp = arg_check(argc, argv, f, flag, module_name);
  if (fp == ERROR) return ERROR;

  /* FPMultiplier_generator */
  FPMultiplier_Generator(fp, f, flag, module_name);

  /* Wrapper Generator */
  if(flag->wrapper_flag == 1) Wrapper_Generator(fp, f, module_name);
  
  fclose(fp);
  return 1;
}

int main(int argc, char **argv){

  Manage_FPMult_Generator(argc, argv);

  return 1;
}
