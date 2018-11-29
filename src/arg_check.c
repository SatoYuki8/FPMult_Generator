
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "mul_gen.h"

FILE *arg_check(int argc, char **argv, efw_t f, flags_t flag, char *module_name){
  FILE *fp;
  int i;
  char output[64];
  int exp = 0, frac = 0;

  if (argc == 1) {
    help_message();
    //printf("argument taritenai\n");
    return ERROR;
  }

  /* 端末の引数をチェック */
  for(i=0 ; i<argc ; i++){
    //乗算モジュール
    if(!strncmp(argv[i], "FPMult", 6)) flag->fpmulti_flag = 1;
    if(!strncmp(argv[i], "fpmult", 6)) flag->fpmulti_flag = 1;

    //step measurement
    if(!strncmp(argv[i], "step1", 5)) flag->step1_flag = 1;
    if(!strncmp(argv[i], "step2", 5)) flag->step2_flag = 1;
    if(!strncmp(argv[i], "step3", 5)) flag->step3_flag = 1;
    if(!strncmp(argv[i], "step4", 5)) flag->step4_flag = 1;
    
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
    fprintf(stderr,
	    "Exponation or Fraction are illegal.\n"
	    "(Please input 2<exp<21, 3<frac)\n");
    return ERROR;
  }else if(exp>20){
    fprintf(stderr,
	    "Exponation is too large.\n"
	    "(Please input smaller than 21)\n");
    return ERROR; 
  }else{    
    f->exp = exp;
    f->frac = frac;
    f->width = f->exp + f->frac + 1;
  }

  //Module command
  if(flag->fpmulti_flag == 1){
    if(flag->pipeline_flag == 1){
      sprintf(module_name, "FPMulti_%d_%d_%d", f->exp, f->frac, f->width);
    }else{
      sprintf(module_name, "FPMulti_%d_%d_%d_comb", f->exp, f->frac, f->width);
    }
  }else if (flag->step1_flag == 1){
    sprintf(module_name, "step1_%d_%d_%d", f->exp, f->frac, f->width);
  }else if (flag->step2_flag == 1){
    sprintf(module_name, "step2_%d_%d_%d", f->exp, f->frac, f->width);
  }else if (flag->step3_flag == 1){
    sprintf(module_name, "step3_%d_%d_%d", f->exp, f->frac, f->width);
  }else if (flag->step4_flag == 1){
    sprintf(module_name, "step4_%d_%d_%d", f->exp, f->frac, f->width);
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
