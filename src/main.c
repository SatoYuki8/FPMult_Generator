#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "mul_gen.h"

/*************************************************/

void init_flags(flags_t flag){
  flag->wrapper_flag = 0;
  flag->fpmulti_flag = 0;
  flag->stdout_flag = 0;
  flag->filename_flag = 0;
  flag->non_DSP_flag = 0;
  flag->pipeline_flag = 0;
  flag->step1_flag = 0;
  flag->step2_flag = 0;
  flag->step3_flag = 0;
  flag->step4_flag = 0;
}

void help_message(){
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



int Wrapper_Generator(FILE *fp, const efw_t f, char *module_name){
  int width = f->width;

  //FPMultiplier header
  fprintf(fp,
	  "declare %s{\n"
	  "input a<%d>, b<%d>;\n"
	  "output result<%d>;\n"
	  "instrin do_in;\n"
	  "instr_arg do_in(a, b);\n"
	  "}\n\n",
	  module_name,
	  width, width,
	  width
	  );
	  
  //Wrapper module
  fprintf(fp,
	  //"module %s_Wrapper{\n"
	  "module wrapper{\n"
	  "input a<%d>, b<%d>;\n"
	  "output result<%d>;\n"
	  "instrin do_in;\n"
	  "%s fmulti;\n"
	  "reg_wr ina<%d>;\n"
	  "reg_wr inb<%d>;\n"
	  "reg_wr out<%d>;\n"
	  "instruct do_in par{\n"
	  "ina:=a;\n"
	  "inb:=b;\n"
	  "out:=fmulti.do_in(ina, inb).result;\n"
	  "result = out;\n"
	  "}\n"
	  "}\n\n",
	  //module_name,
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
  if (flag->fpmulti_flag == 1) FPMultiplier_Generator(fp, f, flag, module_name);

  /* Step measurement */
  if (flag->step1_flag == 1) step_measurement(fp, f, flag, module_name);
  else if (flag->step2_flag == 1) step_measurement(fp, f, flag, module_name);
  else if (flag->step3_flag == 1) step_measurement(fp, f, flag, module_name);
  else if (flag->step4_flag == 1) step_measurement(fp, f, flag, module_name);
  
  /* Wrapper Generator */
  if(flag->wrapper_flag == 1) Wrapper_Generator(fp, f, module_name);
  
  fclose(fp);
  return 1;
}

int main(int argc, char **argv){

  Manage_FPMult_Generator(argc, argv);

  return 1;
}
